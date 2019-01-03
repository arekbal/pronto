#pragma once

#include "utils/semver.hh"

namespace pronto
{
  struct dependency
  {
    std::string name;
    utils::semver version;
    std::string git;
  };
}
