#pragma once

#include <vector>
#include "../utils/span.hpp"

#include "../console.hpp"
#include "../config.hpp"
#include "../compiler_toolchains.hpp"
#include "../core.hpp"
#include "../env_vars.hpp"

namespace pronto::commands
{
  struct set_command
  {
    constexpr static const char* readonly command_name = "set";

    console console_;

    config config_;

    int execute(utils::cspan_vec_s commandArgs)
    {
      auto iter = commandArgs.begin();
      auto end = commandArgs.end();

      if (iter == end) return 0;

      auto& first = *iter;

      if (first.compare("-tc") == 0 || first.compare("-toolchain") == 0)
      {
        iter++;
        if (iter == end) return 0;

        auto& next = *iter;

        setenv(env_vars::toolchain, next.c_str(), true);

        //std::string s = 

       // console_.inf()
      }

      return 0;
    }
  };
}
