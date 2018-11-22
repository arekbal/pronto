#pragma once

#include "../utils/span.hpp"

namespace pronto::commands
{
  struct help_command
  {
    constexpr static const char* readonly command_name = "help";

    int execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}