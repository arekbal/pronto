#pragma once

#include <filesystem>

#include "../../console.hh"
#include "../../process.hh"
#include "../../env.hh"

#include "../../utils/str.hh"

#include "../compiler.hh"
#include "../target.hh"

#include "../toolchain_config.hh"

namespace pronto::toolchains::clang
{
  template<typename console_t = console, typename process_t = process<console_t>, typename env_t = env>
  class clang_toolchain
  {
    console_t console_;

    process_t proc_;

    env_t env_;

  public:
    int run(const toolchain_config& cfg)
    {
      auto dir = cfg.toml_path.parent_path();

      auto curr_path = std::filesystem::current_path();

      std::string s = "clang -x c++ -Wall -v -std=c++17";
      
      s += " -working-directory=";
      s += dir.u8string();

      s += " --output=";
      s += cfg.output.u8string();

      for (auto& src_file : cfg.sources)
      {
        s += " \"";
        console_.inf(src_file);
        s += src_file;
        s += "\"";
      }

      console_.inf(s.c_str());
      proc_.run(s);

      return 0;
    }
  };
}