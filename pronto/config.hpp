#pragma once

#include <filesystem>
#include <optional>
#include <stdlib.h>

#include "deps/cpptoml.hpp"

#include "compiler_toolchains.hpp"

#include "utils/result.hpp"
#include "env_vars.hpp"

namespace pronto
{
  namespace fs = std::filesystem;

  struct config
  {
    result_fail<std::string> open()
    {
      return result_fail<std::string>::fail("hello World");
    }

    compiler_toolchains toolchain() {

      std::string toolchain_l(::std::getenv(env_vars::toolchain));

      if (toolchain_l.compare("msvc") == 0) return compiler_toolchains::msvc;
      if (toolchain_l.compare("intel") == 0) return compiler_toolchains::intel;
      if (toolchain_l.compare("clang") == 0) return compiler_toolchains::clang;
      if (toolchain_l.compare("gcc") == 0) return compiler_toolchains::gcc;
      return compiler_toolchains::unknown;
    }

    void set_toolchain(compiler_toolchains toolchain)
    {
      std::string s;

      if (toolchain == compiler_toolchains::msvc) s = "msvc";
      else if (toolchain == compiler_toolchains::intel) s = "intel";
      else if (toolchain == compiler_toolchains::clang) s = "clang";
      else if (toolchain == compiler_toolchains::gcc) s = "gcc";
      else s = "";

      ::setenv(env_vars::toolchain, s.c_str(), true);
    }

    std::optional<fs::path> find_toml()
    {
      auto path = fs::current_path();

      do
      {
        auto filepath = path;
        filepath /= "_pronto.toml";

        if (fs::exists(filepath))
        {
          return filepath;
        }

        if (!path.has_parent_path())
        {
          return {};
        }

        path = path.parent_path();

      } loop;
    }
  };
}