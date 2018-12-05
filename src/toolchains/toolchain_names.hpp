#pragma once

#include <string>

#include <optional>

namespace pronto::toolchains {

  enum class toolchain_names
  {
    borland = 1,
    intel,
    clang,
    msvc,
    gcc,
  };

  std::optional<toolchain_names> toolchain_from(std::string str)
  {      
    if (str.compare("msvc") == 0) return toolchain_names::msvc;
    if (str.compare("intel") == 0) return toolchain_names::intel;
    if (str.compare("clang") == 0) return toolchain_names::clang;
    if (str.compare("gcc") == 0) return toolchain_names::gcc;
    if (str.compare("borland") == 0) return toolchain_names::borland;
    return {};
  }

  std::string toolchain_to_str(toolchain_names toolchain)
  {
    std::string s;

    if (toolchain == toolchain_names::msvc) s = "msvc";
    else if (toolchain == toolchain_names::intel) s = "intel";
    else if (toolchain == toolchain_names::clang) s = "clang";
    else if (toolchain == toolchain_names::gcc) s = "gcc";
    else s = "";

    return s;
  }
}

