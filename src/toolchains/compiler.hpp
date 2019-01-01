#pragma once

#include "../core.hpp"

namespace pronto::toolchains
{
  enum class compiler { x86, x64 };

  const char* get_compiler_name(compiler compiler_)
  {
    static const char* compiler_strings[] = { "x86", "x64" };
    return compiler_strings[(u8)compiler_];
  }
}