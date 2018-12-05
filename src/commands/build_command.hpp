#pragma once

#include "base_command.hpp"

#include "../utils/span.hpp"
#include "../console.hpp"
#include "../config.hpp"
#include "../utils/str.hpp"
#include "../process.hpp"

#include "../toolchains/msvc_toolchain.hpp"

namespace pronto::commands
{   
  template<typename env_t = env, typename console_t=console, typename process_t=process<console_t>, typename config_t=config<env_t, console_t> >
  struct build_command : base_command<build_command<env_t, console_t, process_t, config_t> >
  {
    friend struct base_command<build_command>;

    static constexpr cstr const command_name = "build";

  private:   

    console_t console_;
    config_t config_;
    process_t proc_;

  private:
    int on_execute(const utils::cspan_vec_s command_args)
    {
      if (command_args.empty())
      {
        result_err<std::string> res = config_.load();
        
        if (!res.is_ok())
        {
          console_.err(res.error().c_str());
          return -1;
        }

        auto toolchain = config_.toolchain();

        switch (toolchain)
        {
          case toolchains::toolchain_names::msvc:
          {
            toolchains::msvc_toolchain<console_t, process_t> msvc;
            return msvc.run(config_.toml_path());
          }
          default:
            console_.err("currently unsupported toolkit");
            return -1;
        }

        return 0;
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
          console_.err(res.error().c_str());
          return -1;
        }

        auto toolchain = config_.toolchain();

        switch (toolchain)
        {
          case toolchains::toolchain_names::msvc:
            toolchains::msvc_toolchain<console_t, process_t> msvc;
            return msvc.run(config_.toml_path());
          default:
            console_.err("currently unsupported toolkit");
            return -1;
        }

        return 0;
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
  }; 
}