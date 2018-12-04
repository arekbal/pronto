#pragma once

#include "base_command.hpp"

#include "../utils/span.hpp"

namespace pronto::commands
{
  struct help_command : base_command<help_command>
  {
    friend class base_command<help_command>;

    constexpr static const char* readonly command_name = "help";

  private:
    int on_execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}