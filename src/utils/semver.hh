#pragma once

#include <vector>
#include <string>
#include "str.hh"
#include "span.hh"

namespace pronto::utils
{
  struct semver
  {
    unsigned int major;
    unsigned int minor;
    unsigned int patch;

    std::vector<std::string> tags;
  
    semver()
      :major(0), minor(0), patch(0)
    {
    }

    semver(unsigned int major_, unsigned int minor_, unsigned int patch_)
      :major(major_), minor(minor_), patch(patch_)
    {
    }

    semver(const std::string& s)    
    {
      auto elems = str::split(s, '-');
      auto major_minor_patch = str::split(elems[0], '.');
      major = std::stoul(major_minor_patch[0]);
      minor = std::stoul(major_minor_patch[1]);
      patch = std::stoul(major_minor_patch[2]);

      tags = make_cspan(elems, 1).to_vector<std::string>();
    }

    static semver coerce(std::string s)
    {
      semver ver;

      if (str::starts_with(s, "v"))
      {
        s = s.substr(1);
      }

      auto elems = str::split(s, '-');
      auto major_minor_patch = str::split(elems[0], '.');
      ver.major = std::stoul(major_minor_patch[0]);

      auto size = major_minor_patch.size();
      if(size > 1)
        ver.minor = std::stoul(major_minor_patch[1]);

      if (size > 2)
        ver.patch = std::stoul(major_minor_patch[2]);

      ver.tags = make_cspan(elems, 1).to_vector<std::string>();

      return ver;
    }

    bool empty() const
    {
      return major == 0 && minor == 0 && patch == 0;
    }

    friend bool operator <(const semver& l, const semver& r)
    {
      return std::tie(l.major, l.minor, l.patch)
        < std::tie(l.major, l.minor, l.patch);
    }
   
    friend bool operator > (const semver& lhs, const semver& rhs) { return rhs < lhs; }
    friend bool operator <=(const semver& lhs, const semver& rhs) { return !(lhs > rhs); }
    friend bool operator >=(const semver& lhs, const semver& rhs) { return !(lhs < rhs); }

    std::string to_string() const
    {
      std::string s(std::to_string(major));
      s += ".";
      s += std::to_string(minor);
      s += ".";
      s += std::to_string(patch);

      for (auto& tag : tags)
        if(not tag.empty())
        {
          s += "-";
          s += tag;
        }

      return s;
    }
  };
}