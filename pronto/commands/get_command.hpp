#pragma once

#include <vector>
#include "../utils/span.hpp"

#include "../console.hpp"
#include "../config.hpp"
#include "../compiler_toolchains.hpp"

namespace pronto::commands
{
  struct get_command
  {
    constexpr static const char* readonly command_name = "get";

    console console_;

    config config_;

    int execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}
