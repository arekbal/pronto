#pragma once

#include "../core.hpp"

namespace pronto::toolchains
{
  enum class target { x86, x64, arm };

  const char* get_target_name(target target_)
  {
    static const char* target_strings[] = { "x86", "x64", "arm" };
    return target_strings[(u8)target_];
  }
}