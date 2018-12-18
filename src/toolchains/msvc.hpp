#pragma once

#include <filesystem>

#include "../console.hpp"
#include "../process.hpp"

#include "win32reg.hpp"

namespace pronto::toolchains::msvc
{
  enum class compiler { x86, x64 };  

  const char* get_compiler_name(compiler compiler_)
  {
    static const char* compiler_strings[] = { "x86", "x64" };
    return compiler_strings[(byte)compiler_];
  }

  enum class target { x86, x64, arm };

  const char* get_target_name(target target_)
  {
    static const char* target_strings[] = { "x86", "x64", "arm" };
    return target_strings[(byte)target_];
  }

  template<typename console_t=console, typename process_t=process<console_t>>
  struct toolchain
  {
    static constexpr const char* const COMPILER = "Compiler";
    static constexpr const char* const REG_COMPILER_SUBKEY = "SOFTWARE\\WOW6432Node\\Microsoft\\VisualStudio\\VC";

    console_t console_;

    process_t proc_;

    int run(const std::filesystem::path& path, compiler compiler_, target target_)
    {
      (path);

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
          compiler_regsubkey_path += get_compiler_name(compiler_);
          compiler_regsubkey_path += "\\";
          compiler_regsubkey_path += get_target_name(target_);

          auto r_compiler_key = key.get_subkey(compiler_regsubkey_path);

          if (r_compiler_key.is_ok())
          {
            auto compiler_key = r_compiler_key.value();
            auto r_compiler_dir = compiler_key.get_string(COMPILER);

            if (r_compiler_dir.is_ok())
            {
              auto compiler_dir = r_compiler_dir.value();

              return compile(path, compiler_dir, compiler_, target_);
            }
            else
              console_.err(lstatus_to_error("compiler_regkey.get_string(COMPILER)", r_compiler_dir.error()).c_str());
          }
          else
          {
            std::string err = "get_subkey(";
            err += compiler_regsubkey_path;
            err += ")";
            console_.err(lstatus_to_error(err.c_str(), r_compiler_key.error()).c_str());
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
    /*  {
        auto vcvarsall_path = compiler_path.parent_path();
        vcvarsall_path /= "vcvarsall.bat";

        auto w_vcvarsall_path = str::utf16_to_utf8(vcvarsall_path);
        console_.inf(w_vcvarsall_path.c_str());
        auto result0 = proc_.run(w_vcvarsall_path);
        if (result0 != 0)
        {
          std::string err = "error code: ";
          err += std::to_string(result0);
          console_.err(err.c_str());
          return -1;
        }
      }*/

      {
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
      }

      auto lib_path = compiler_path.parent_path().parent_path().parent_path();
      lib_path /= "lib\\amd64\\";

      //auto libcmt_path = lib_path;
      //libcmt_path /= "LIBCMT.lib";

      std::string cmd = str::utf16_to_utf8(compiler_path) + " ";
      auto parent_path = path.parent_path();
      cmd += str::utf16_to_utf8(parent_path);
      cmd += "\\main.cpp";
      cmd += " /link /LIBPATH:\"";
      cmd += str::utf16_to_utf8(lib_path);
      cmd += "\"";

      // include winapi libs from windows sdks... from registry 
      // HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Microsoft SDKs\Windows\v10.0
      // InstallationFolder
      // ProductVersion

      console_.inf(cmd.c_str());
      return proc_.run(cmd);
    }

    std::string lstatus_to_error(const char* func_name, LSTATUS status)
    {
      std::string s = func_name;
      s += " failed: ";

      switch (status)
      {
      case ERROR_FILE_NOT_FOUND:
        return s + "file not found";
      case ERROR_ACCESS_DENIED:
        return s + "access denied";
      default:
        return s + "not listed error";
      }
    }
  };
}