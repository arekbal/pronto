#pragma once

#include "base_command.hh"

#include "../utils/span.hh"

namespace pronto::commands
{
  struct help_command : base_command<help_command>
  {
    friend struct base_command<help_command>;

    constexpr static const char* const command_name = "help";

  private:
    int on_execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}