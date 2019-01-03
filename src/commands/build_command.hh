#pragma once

#include "base_command.hh"


#include "../utils/span.hh"
#include "../console.hh"
#include "../config.hh"
#include "../utils/str.hh"
#include "../process.hh"

#include "../toolchains/compiler.hh"
#include "../toolchains/target.hh"
#include "../toolchains/toolchain_config.hh"

#include "../toolchains/msvc/msvc_toolchain.hh"
#include "../toolchains/clang/clang_toolchain.hh"


namespace pronto::commands
{   
  template<
    typename env_t = env, 
    typename console_t = console, 
    typename process_t = process<console_t>, 
    typename config_t = config<env_t, console_t> >
  struct build_command : base_command<build_command<env_t, console_t, process_t, config_t> >
  {
    friend struct base_command<build_command>;

    static constexpr const char* const command_name = "build";

  private:

    console_t console_;
    config_t config_;
    process_t proc_;

    int on_execute(const utils::cspan_vec_s command_args)
    {
      if (command_args.empty())
      {
        result_err<std::string> res = config_.load();
        
        if (!res.is_ok())
        {
          console_.err(res.error());
          return -1;
        }

        return run_toolchain();
      }

      // TODO: check for extra path argument before building
      auto iter = command_args.begin();
      auto end = command_args.end();

      auto& arg0 = *iter;

      if (!str::starts_with(arg0, "-"))
      {
        result_err<std::string> res = config_.load(arg0);

        if (!res.is_ok())
        {
          console_.err(res.error());
          return -1;
        }

        return run_toolchain();
      }
      
     /* if (first.compare("-p") == 0 || first.compare("-profile") == 0)
      {
        iter++;
        
        if (iter != end)
        {
          auto& config_name = *iter;
          config_name.size();
        }
      }*/

      return 0;
    }   

    int run_toolchain()
    {
      auto toolchain = config_.toolchain();

      std::vector<std::string> sources_with_wildcards = config_.sources();
      std::vector<std::string> excludes_with_wildcards = config_.excludes();

      std::vector<std::string> sources = {};

      auto root = config_.root_path();


      auto deps = config_.dependencies();

      for (const auto& entry : fs::recursive_directory_iterator(root))
      {
        if (entry.is_regular_file())
        {
          std::string path = fs::absolute(entry.path()).u8string();

          for (const auto& src_wild : sources_with_wildcards)
            if (str::wildcard_path_match(src_wild.c_str(), path))
            {
              bool exclude_it = false;
              for (const auto& excludes_wild : excludes_with_wildcards)
                if (str::wildcard_path_match(excludes_wild.c_str(), path))
                {
                  exclude_it = true;
                  break;
                }

              if (not exclude_it)
              {
                console_.inf(path);
                sources.push_back(path);
              }
            }
        }
      }

      toolchains::toolchain_config cfg;
      cfg.toml_path = config_.toml_path();
      cfg.sources = sources;
      cfg.compiler = toolchains::compiler::x64;
      cfg.target = toolchains::target::x64;

      const auto& target_dir = config_.target_dir();
           
      if (target_dir.is_absolute())
        cfg.output = target_dir;
      else
        cfg.output = cfg.toml_path.parent_path() / target_dir;

      cfg.output /= get_target_compiler_dir_name(cfg);

      std::string filename = config_.package_name();
      
      if (filename.size() == 0)
        filename += "a";

      filename += ".exe";

      std::filesystem::create_directories(cfg.output); // returns true if successfully created and false if failed (for instance they existed before...)

      cfg.output /= filename;

      switch (toolchain)
      {
      case toolchains::toolchain_names::msvc:
      {
        toolchains::msvc::msvc_toolchain<console_t, process_t, env_t> msvc_toolchain_;
        return msvc_toolchain_.run(cfg);
      }
      case toolchains::toolchain_names::clang:
      {
        toolchains::clang::clang_toolchain<console_t, process_t, env_t> clang_toolchain_;        
        return clang_toolchain_.run(cfg);
      }
      default:
        console_.err("currently unsupported toolkit");
        return -1;
      }
    }

    const std::string get_target_compiler_dir_name(pronto::toolchains::toolchain_config& cfg)
    {
      return std::string(toolchains::get_compiler_name(cfg.compiler)) + std::string("_") + toolchains::get_target_name(cfg.target);
    }
  }; 
}