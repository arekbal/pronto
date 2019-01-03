#pragma once

#include "base_command.hh"

#include <vector>
#include "../utils/span.hh"
#include "../console.hh"
#include "../config.hh"
#include "../toolchains/toolchain_names.hh"

namespace pronto::commands
{
  template<typename env_t=env, typename console_t=console, typename config_t=config<env_t, console_t> >
  struct get_command : base_command<get_command<env_t, console_t, config_t> >
  {
    friend struct base_command<get_command>;

    static constexpr const char* const command_name = "get";

    console_t console_;

    config_t config_;

  private:
    int on_execute(utils::cspan_vec_s commandArgs)
    {
      return 0;
    }
  };
}
