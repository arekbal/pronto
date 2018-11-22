#pragma once

#include <vector>
#include "../utils/span.hpp"

namespace pronto::commands
{
  struct init_command
  {
    constexpr static const char* readonly command_name = "init";

    int execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}
