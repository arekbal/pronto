#pragma once

#include <filesystem>

#include "../utils/str.hpp"
#include "../console.hpp"

using namespace pronto::str;

bool wildcard_path_match_tests(pronto::console& out)
{  
  out.inf("wildcard_path_match");

  { auto _ = out.indent();
    
    if (not wildcard_path_match("*c[0-9].cpp", "towerc57777.cpp"))
      return out.err("ERROR: '*c[0-9].cpp' should match 'towerc57777.cpp'");

    if (wildcard_path_match("*c[0-9].cpp", "towerc577x77.cpp"))
      return out.err("ERROR: '*c[0-9].cpp' should not match 'towerc577x77.cpp'");

    if (not wildcard_path_match("*c[5-7][A-z][7]//g.cpp", "towerc577x77//g.cpp"))
      return out.err("ERROR: '**c[5-7][A-z][7]//.cpp' should match 'towerc577x77/.cpp'");

    if (not wildcard_path_match("*c[5-7][A-z][7]\\[[3]\\].cpp", "towerc577x77[3].cpp"))
      return out.err("ERROR: '*c[5-7][A-z][7].cpp' should match 'towerc577x77.cpp'");

    if (wildcard_path_match("*g[0-9].cpp", "towerc57777.cpp"))
      return out.err("ERROR: '*g[0-9].cpp' should not match 'towerc57777.cpp'");

    if (wildcard_path_match("*/*.cpp", "tower.cpp"))
      return out.err("ERROR: '*/*.cpp' should not match 'tower.cpp'");

    if (not wildcard_path_match("*.cpp", "tower.cpp"))
      return out.err("ERROR: '*.cpp' should match 'tower.cpp'");

    if (not wildcard_path_match("*.cpp", "C:\\help_me\\tower.cpp"))
      return out.err("ERROR: '*.cpp' should match 'C:\\help_me\\tower.cpp'");

    if (not wildcard_path_match("*/*.cpp", "C:\\help_me\\tower.cpp"))
      return out.err("ERROR: '*/*.cpp' should match 'C:\\help_me\\tower.cpp'");

    if (not wildcard_path_match("*.cpp", "C:\\help_me\\tower.cpp"))
      return out.err("ERROR: '*.cpp' should match 'C:\\help_me\\tower.cpp'");

    if (wildcard_path_match(".cpp", "C:\\help_me\\tower.cpp"))
      return out.err("ERROR: '.cpp' should not match 'C:\\help_me\\tower.cpp'");

    if (wildcard_path_match("*/*.cfg", "C:\\help_me\\tower.cpp"))
      return out.err("ERROR: '*/*.cfg' should not match 'C:\\help_me\\tower.cpp'");

    if (wildcard_path_match("*/*/*/*.cfg", "C:\\help_me\\tower.cpp"))
      return out.err("ERROR: '*/*/*/*.cfg' should not match 'C:\\help_me\\tower.cpp'");

    out.ok("OK");
  }

  return true;
}
