#pragma once

#include <filesystem>

#include "../../console.hpp"
#include "../../process.hpp"
#include "../../env.hpp"

#include "win32reg.hpp"
#include "cl_args.hpp"

#include "../compiler.hpp"
#include "../target.hpp"

namespace pronto::toolchains::msvc
{
  template<typename console_t=console, typename process_t=process<console_t>, typename env_t=env>
  class msvc_toolchain
  {
    static constexpr const char* const COMPILER = "Compiler";
    static constexpr const char* const REG_COMPILER_SUBKEY = "SOFTWARE\\WOW6432Node\\Microsoft\\VisualStudio\\VC";
    static constexpr const char* const REG_WINSDK_10_SUBKEY = "SOFTWARE\\WOW6432Node\\Microsoft\\Microsoft SDKs\\Windows\\v10.0";
    static constexpr const char* const REG_WINSDK_10_INSTALLATION_FOLDER = "InstallationFolder";
    static constexpr const char* const REG_WINSDK_10_PRODUCT_VERSION = "ProductVersion";

    console_t console_;

    process_t proc_;

    env_t env_;

  public:
    int run(const toolchain_config& cfg)
    {
      auto o_key = win32reg::reg_key::factory::local_machine().get_subkey(REG_COMPILER_SUBKEY);
      
      if (o_key.is_ok())
      {
        auto key = o_key.value();
        auto subkeys = key.list_subkeys();

        std::string found = str::empty_string;
        double highest_found_version = -1.;

        for (auto& subkey : subkeys)
        {
          auto o_version = str::str_to_double(subkey);

          if (o_version.has_value())
            if (highest_found_version < o_version.value())
            {
              found = subkey;
              highest_found_version = o_version.value();
            }
        }

        if (found.length() > 0)
        {
          std::string compiler_regsubkey_path = found;

          compiler_regsubkey_path += "\\";
          compiler_regsubkey_path += get_compiler_name(cfg.compiler);
          compiler_regsubkey_path += "\\";
          compiler_regsubkey_path += get_target_name(cfg.target);

          auto r_compiler_key = key.get_subkey(compiler_regsubkey_path);

          if (r_compiler_key.is_ok())
          {
            auto compiler_key = r_compiler_key.value();
            auto r_compiler_dir = compiler_key.get_string(COMPILER);

            if (r_compiler_dir.is_ok())
            {
              auto compiler_dir = r_compiler_dir.value();

              return compile(cfg.toml_path, compiler_dir, cfg.compiler, cfg.target);
            }
            else
              console_.err(lstatus_to_error("compiler_regkey.get_string(COMPILER)", r_compiler_dir.error()).c_str());
          }
          else
          {
            std::string err = "get_subkey(";
            err += compiler_regsubkey_path;
            err += ")";
            console_.err(lstatus_to_error(err.c_str(), r_compiler_key.error()));
          }
        }

        return 0;
      }
      
      // read compiler path from registry.
      // extend toolchain params (x86 32bit x86_amd64 64 bit or arm)
      return -1;
    }

  private:
    int compile(const std::filesystem::path& path, const std::filesystem::path& compiler_path, compiler compiler_, target target_)
    {
      std::filesystem::path cd("F:\\PROJECTS\\crisp");
      std::filesystem::current_path(cd);
      
      auto x = std::getenv("PATH");
      // F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\lib\\x64
      // C:\\Program Files(x86)\\Windows Kits\\10\\lib\\10.0.17763.0\\ucrt\\x64

      auto cd1 = std::filesystem::current_path();
     // proc_.run("cd F:\\PROJECTS\\crisp");
      return proc_.run("\"\
F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\bin\\Hostx64\\x64\\cl.exe\" main.cpp /link \
/libpath:\"F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\lib\\x64\" \
/libpath:\"C:\\Program Files\\Windows Kits\\10\\Lib\\10.0.17763.0\\um\\x64\" \
/libpath:\"C:\\Program Files\\Windows Kits\\10\\Lib\\10.0.17763.0\\ucrt\\x64\"");

      (compiler_);

      auto compiler_path2 = std::filesystem::path("F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\bin\\Hostx64\\x64\\cl.exe");

      {
        auto vcvarsall_path = compiler_path2.parent_path().parent_path().parent_path().parent_path();
        vcvarsall_path /= "vcvarsall.bat x64";

        auto w_vcvarsall_path = str::utf16_to_utf8(vcvarsall_path);
        console_.inf(w_vcvarsall_path);
        auto result0 = proc_.run(w_vcvarsall_path);
        if (result0 != 0)
        {
          std::string err = "error code: ";
          err += std::to_string(result0);
          console_.err(err);
          return -1;
        }
      }

     /* {
        auto vcvars64_path = compiler_path.parent_path();
        vcvars64_path /= "vcvars64.bat";

        auto w_vcvars64_path = str::utf16_to_utf8(vcvars64_path);
        console_.inf(w_vcvars64_path.c_str());
        auto result0 = proc_.run(w_vcvars64_path);
        if (result0 != 0)
        {
          std::string err = "error code: ";
          err += std::to_string(result0);
          console_.err(err.c_str());
          return -1;
        }
      }*/

      auto r_latest_win_sdk_lib_path = get_latest_win_sdk_lib_path(target_);

      if (!r_latest_win_sdk_lib_path.is_ok())
      {
        console_.err(r_latest_win_sdk_lib_path.error());
        return -1;
      }

      auto latest_win_sdk_lib_path = r_latest_win_sdk_lib_path.value();

      auto lib_path = compiler_path2.parent_path().parent_path().parent_path().parent_path();
      lib_path /= "lib\\x64";

      //auto libcmt_path = lib_path;
      //libcmt_path /= "LIBCMT.lib";

      std::string cmd = compiler_path2.u8string() + " /nologo /c /E ";
     
      auto parent_path = path.parent_path();

      cmd += "/Fo\"";
      cmd += parent_path.u8string();
      cmd += "\\target\\x64\\Debug\\\"";

      cmd += " ";
      cmd += parent_path.u8string();
      cmd += "\\main.cpp";
     /* cmd += " /link";
      cmd += " /LIBPATH:\"";
      cmd += str::utf16_to_utf8(lib_path);   
      cmd += "\"";
      cmd += " /LIBPATH:\"";
      cmd += str::utf16_to_utf8(latest_win_sdk_lib_path);
      cmd += "\"";*/

      cl_args args(compiler_path2);

      args.link = true;

      args.lib_paths.push_back(latest_win_sdk_lib_path.u8string());
      args.lib_paths.push_back(lib_path.u8string());

      // TODO: where to get this path from? vswhere.exe
     /* args.lib_paths.push_back("F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\lib\\x64");
      args.lib_paths.push_back("F:\\VisualStudio2017Community\\VC\\Auxiliary\\VS\\lib\\x64");
      args.lib_paths.push_back("F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\lib\\x64");
      args.lib_paths.push_back("F:\\VisualStudio2017Community\\VC\\Tools\\MSVC\\14.16.27023\\atlmfc\\lib\\x64");
      args.lib_paths.push_back("F:\\VisualStudio2017Community\\VC\\Auxiliary\\VS\\lib\\x64");
      args.lib_paths.push_back("C:\\Program Files(x86)\\Windows Kits\\10\\lib\\10.0.17763.0\\ucrt\\x64");
      args.lib_paths.push_back("C:\\Program Files(x86)\\Windows Kits\\10\\lib\\10.0.17763.0\\um\\x64");
      args.lib_paths.push_back("C:\\Program Files(x86)\\Windows Kits\\NETFXSDK\\4.6.1\\lib\\um\\x64");
      args.lib_paths.push_back("C:\\Program Files(x86)\\Windows Kits\\NETFXSDK\\4.6.1\\Lib\\um\\x64");*/

      args.file_or_lib_paths.push_back("..\\..\\crisp\\main.cpp");

      auto kernel32_path = latest_win_sdk_lib_path;
      kernel32_path /= "kernel32.lib";

      //args.file_or_lib_paths.push_back(str::utf16_to_utf8(kernel32_path));

      //args.file_or_lib_paths.push_back("kernel32.lib");

      std::filesystem::create_directory("..\\..\\crisp\\obj");
      std::filesystem::create_directory("..\\..\\crisp\\bin");

      args.object_file_name = "..\\..\\crisp\\obj\\main";
      args.exe_file_name = "..\\..\\crisp\\bin\\main";

      auto cmd_line = args.cmd_line();

      console_.inf(cmd_line);
      auto result = proc_.run(cmd_line);

     /* auto link_path = compiler_path.parent_path();
      link_path /= "link.exe";
           
      std::string link_cmd_line(str::utf16_to_utf8(link_path));

      link_cmd_line += " \"";
      link_cmd_line += args.object_file_name;
      link_cmd_line += "\"";

      link_cmd_line += " /LIBPATH:";
      link_cmd_line += "\"";
      link_cmd_line += str::utf16_to_utf8(latest_win_sdk_lib_path);
      link_cmd_line += "\"";

      link_cmd_line += " /LIBPATH:";
      link_cmd_line += "\"";
      link_cmd_line += str::utf16_to_utf8(lib_path);
      link_cmd_line += "\"";


      console_.inf(link_cmd_line.c_str());
      result =  proc_.run(link_cmd_line);*/
      //console_.inf(cmd.c_str());
      //return proc_.run(cmd);

      return result;
    }

    result<std::filesystem::path, std::string> get_latest_win_sdk_lib_path(target target_)
    {
      auto r_key = win32reg::reg_key::factory::local_machine()
        .get_subkey(REG_WINSDK_10_SUBKEY);

      if (r_key.is_ok())
      {
        auto key = r_key.value();

        auto r_win_sdk_install_path = key.get_string(REG_WINSDK_10_INSTALLATION_FOLDER);
        auto r_prod_ver = key.get_string(REG_WINSDK_10_PRODUCT_VERSION);

        if (r_win_sdk_install_path.is_ok() && r_prod_ver.is_ok())
        {
          auto win_sdk_install_path = r_win_sdk_install_path.value();
          auto prod_ver = r_prod_ver.value();

          std::filesystem::path latest_win_sdk_install_path = win_sdk_install_path;
          latest_win_sdk_install_path /= "Lib";
          latest_win_sdk_install_path /= prod_ver;
          latest_win_sdk_install_path /= "um";
          //latest_win_sdk_install_path /= get_target_name(target_);

          return latest_win_sdk_install_path;
        }
      }

      return fail<std::string>("win sdk not found");
    }

    std::string lstatus_to_error(const char* func_name, LSTATUS status)
    {
      std::string s = func_name;
      s += " failed: ";

      switch (status)
      {
      case ERROR_FILE_NOT_FOUND: return s + "file not found";
      case ERROR_ACCESS_DENIED: return s + "access denied";
      default: return s + "not listed error";
      }
    }
  };
}