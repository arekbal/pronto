#pragma once

#include "../utils/span.hpp"

namespace pronto::commands
{
  struct run_command
  {
    constexpr static const char* readonly command_name = "run";

    int execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}
