#pragma once

#include "base_command.hpp"

#include <vector>
#include "../utils/span.hpp"
#include "../console.hpp"
#include "../config.hpp"
#include "../toolchains/toolchain_names.hpp"
#include "../core.hpp"
#include "../env.hpp"

namespace pronto::commands
{
  template<typename env_t=env, typename console_t=console, typename config_t=config<env_t, console_t> >
  struct set_command : base_command<set_command<env_t, console_t, config_t> >
  {
    friend struct base_command<set_command>;

    constexpr static const char* const command_name = "set";

  private:
    console_t console_;

    config_t config_;

  private:
    int on_execute(utils::cspan_vec_s commandArgs)
    {
      auto iter = commandArgs.begin();
      auto end = commandArgs.end();

      if (iter == end) return 0;

      auto& first = *iter;

      if (first.compare("-tc") == 0 || first.compare("-toolchain") == 0)
      {
        iter++;
        if (iter == end) 
          return 0;

        //auto& tc_name = *iter;                

        //setenv(env::toolchain, next.c_str(), true);

        //std::string s = 

       // console_.inf()
      }

      return 0;
    }
  };
}
