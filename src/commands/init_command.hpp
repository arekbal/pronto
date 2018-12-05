#pragma once

#include <vector>
#include "../utils/span.hpp"

#include "base_command.hpp"

namespace pronto::commands
{
  struct init_command : base_command<init_command>
  {
    friend struct base_command<init_command>;

    constexpr static const char* readonly command_name = "init";

  private:
    int on_execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}
