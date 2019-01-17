#pragma once

#include "utils/semver.hh"
#include "utils/semver_req.hh"

namespace pronto
{
  struct dependency
  {
    std::string name;
    utils::semver_req req_version;
    utils::semver curr_version;
    std::string git;
    std::string git_branch;

    dependency()
      :name(), req_version(), curr_version(), git(), git_branch()
    {
    }

    dependency(std::string name_, utils::semver_req req_version_)
      :name(name_), req_version(req_version_), curr_version(), git(), git_branch()
    {
    }
  };
}
