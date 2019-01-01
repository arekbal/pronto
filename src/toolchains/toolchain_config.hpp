#pragma once
#include <filesystem>
#include <vector>
#include <string>

#include "compiler.hpp"
#include "target.hpp"

namespace pronto::toolchains
{
  struct toolchain_config
  {
    std::filesystem::path output;
    std::filesystem::path toml_path;
    std::vector<std::string> sources;
    compiler compiler;
    target target;
  }; 
}