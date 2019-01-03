#pragma once

#include <filesystem>

#include "../utils/semver.hh"
#include "../console.hh"

using namespace pronto::utils;

bool semver_tests(pronto::console& out)
{
  out.inf("semver");

  { auto _ = out.indent();
  
    semver v("49.112.37-hello-world-that-is-a-bunch-of-tags");

    if (v.major != 49)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' major does not equal 49");

    if (v.minor != 112)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' minor does not equal 112");

    if (v.patch != 37)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' patch does not equal 37");

    if (semver("1.0.0") > semver("11.0.0"))
      return out.err("ERR: \
'1.0.0' should be smaller than '11.0.0' instead it is bigger?!?");

    if (v.tags.empty() || (v.tags[0]).compare(std::string("hello")) != 0)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' 1st tag does not equal 'hello'");

    if (v.tags.size() < 2 || (v.tags[1]).compare(std::string("world")) != 0)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' 2nd tag does not equal 'world'");

    if (v.tags.size() < 3 || (v.tags[2]).compare(std::string("that")) != 0)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' 3rd tag does not equal 'that'");

    if (v.tags.size() < 8 || (v.tags[7]).compare(std::string("tags")) != 0)
      return out.err("ERR: \
'49.112.37-hello-world-that-is-a-bunch-of-tags' 8th tag does not equal 'tags'");

    {
      auto s = v.to_string();
      if (s.compare("49.112.37-hello-world-that-is-a-bunch-of-tags") != 0)
      {
        std::string err_msg = "ERR: ";
        err_msg += '\'';
        err_msg += s;
        err_msg += '\'';
        err_msg += " is different than expected '49.112.37-hello-world-that-is-a-bunch-of-tags'";
        return out.err(err_msg);
      }
    }

    return out.ok();
  }
}