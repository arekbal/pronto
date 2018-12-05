#pragma once

#include <filesystem>
#include <windows.h>

#include "../console.hpp"
#include "../process.hpp"


namespace pronto::toolchains
{
  template<typename console_t=console, typename process_t=process<console_t>>
  struct msvc_toolchain
  {
    console_t console_;

    process_t proc_;

    int run(const std::filesystem::path& path)
    {
      (path);
      // read compiler path from registry.
      // extend toolchain params (x86 32bit x86_amd64 64 bit or arm)
      return proc_.run("cargo");
    }
  };
}