#pragma once

#include "base_command.hpp"

#include "../utils/span.hpp"

namespace pronto::commands
{
  struct run_command : base_command<run_command>
  {
    friend class base_command<run_command>;

    constexpr static const char* readonly command_name = "run";

  private:
    int on_execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}
