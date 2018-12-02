#pragma once

#include <optional>

namespace pronto {

  enum class compiler_toolchains
  {
    borland = 1,
    intel,
    clang,
    msvc,
    gcc,
  };

  std::optional<compiler_toolchains> compiler_toolchain_from(std::string str)
  {      
    if (str.compare("msvc") == 0) return compiler_toolchains::msvc;
    if (str.compare("intel") == 0) return compiler_toolchains::intel;
    if (str.compare("clang") == 0) return compiler_toolchains::clang;
    if (str.compare("gcc") == 0) return compiler_toolchains::gcc;
    if (str.compare("borland") == 0) return compiler_toolchains::borland;
    return {};
  }

  std::string compiler_toolchain_to_str(compiler_toolchains toolchain)
  {
    std::string s;

    if (toolchain == compiler_toolchains::msvc) s = "msvc";
    else if (toolchain == compiler_toolchains::intel) s = "intel";
    else if (toolchain == compiler_toolchains::clang) s = "clang";
    else if (toolchain == compiler_toolchains::gcc) s = "gcc";
    else s = "";

    return s;
  }
}

