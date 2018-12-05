#pragma once

#include <vector>
#include "../utils/span.hpp"

namespace pronto::commands
{
  template<typename t>
  struct base_command
  {
    int execute(utils::cspan_vec_s commandArgs)
    {
      return static_cast<t*>(this)->on_execute(commandArgs);
    }
  };
}