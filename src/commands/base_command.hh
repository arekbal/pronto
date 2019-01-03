#pragma once

#include <vector>
#include "../utils/span.hh"

namespace pronto::commands
{
  template<typename t>
  struct base_command
  {
    int execute(utils::cspan_vec_s command_args)
    {
      return static_cast<t*>(this)->on_execute(command_args);
    }
  };
}