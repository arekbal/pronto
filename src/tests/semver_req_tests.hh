#pragma once

#include <filesystem>

#include "../utils/semver_req.hh"
#include "../console.hh"

using namespace pronto::utils;

bool semver_req_tests(pronto::console& out)
{
  out.inf("semver_req");

  { auto _ = out.indent();
  
  semver_req ver("1.x || >=2.5.0 || 5.0.0 - 7.2.3");    

  if (!ver.satisfies(semver(1, 2, 3)))
    return out.err("ERR: \
'1.2.3' does not satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (!ver.satisfies(semver(2, 5, 0)))
    return out.err("ERR: \
'2.5.0' does not satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (!ver.satisfies(semver(2, 6, 0)))
    return out.err("ERR: \
'2.6.0' does not satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (!ver.satisfies(semver(5, 1, 0)))
    return out.err("ERR: \
'5.1.0' does not satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");
   
  return out.ok();
  }
}