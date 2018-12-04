#pragma once

#include "base_command.hpp"

#include "../utils/span.hpp"
#include "../console.hpp"
#include "../config.hpp"
#include "../utils/str.hpp"
#include "../process.hpp"

namespace pronto::commands
{   
  template<typename env_t = env, typename console_t=console, typename process_t=process<console_t>, typename config_t=config<env_t, console_t> >
  struct build_command : base_command<build_command<env_t, console_t, process_t, config_t> >
  {
    friend class base_command<build_command>;

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
        // TODO: just build from config

        auto res = config_.load();

        return 0;
      }

      // TODO: check for extra path argument before building
      auto iter = command_args.begin();
      auto end = command_args.end();

      auto& first = *iter;

      if (!str::starts_with(first, "-"))
      {
        auto res = config_.load(first);

        auto result = proc_.run("cargo");

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