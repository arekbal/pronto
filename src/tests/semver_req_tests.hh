#pragma once

#include <filesystem>

#include "../utils/semver_req.hh"
#include "../console.hh"

using namespace pronto::utils;

bool semver_req_tests(pronto::console& out)
{
  out.inf("semver_req");

  { auto _ = out.indent();
  
  semver_req req("1.x || >=2.5.0 || 5.0.0 - 7.2.3");    

  if (not req.is_satisfied_by(semver(1, 2, 3)))
    return out.err("ERR: \
'1.2.3' should satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (not req.is_satisfied_by(semver(2, 5, 0)))
    return out.err("ERR: \
'2.5.0' should satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (not req.is_satisfied_by(semver(2, 6, 0)))
    return out.err("ERR: \
'2.6.0' should satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (not req.is_satisfied_by(semver(5, 1, 0)))
    return out.err("ERR: \
'5.1.0' should satisfy requirement '1.x || >=2.5.0 || 5.0.0 - 7.2.3'");

  if (not semver_req("1.2.x").is_satisfied_by(semver(1, 2, 0)))
    return out.err("ERR: \
'1.2.0' should satisfy requirement '1.2.x'");

  if (not semver_req("1.2.x").is_satisfied_by(semver(1, 2, 9999)))
    return out.err("ERR: \
'1.2.9999' should satisfy requirement '1.2.x'");

  if (not semver_req("5.0.0 - 5.1.1").is_satisfied_by(semver(5, 0, 0)))
    return out.err("ERR: \
'5.0.0' should satisfy requirement '5.0.0 - 5.1.1'");

  if (not semver_req("5.0.0 - 5.1.1").is_satisfied_by(semver(5, 1, 1)))
    return out.err("ERR: \
'5.1.1' should satisfy requirement '5.0.0 - 5.1.1'");

  if (semver_req("5.0.0 - 5.1.1").is_satisfied_by(semver(6, 0, 0)))
    return out.err("ERR: \
'6.0.0' should not satisfy requirement '5.0.0 - 5.1.1'");

  if (not semver_req("1.2.x").is_satisfied_by(semver(1, 2, 9999)))
    return out.err("ERR: \
'1.2.9999' should satisfy requirement '1.2.x'");

  if (not semver_req("^0.2.3").is_satisfied_by(semver(0, 2, 3)))
    return out.err("ERR: \
'0.2.3' should satisfy requirement '^0.2.3'");

  if (semver_req("^0.2.3").is_satisfied_by(semver(0, 3, 0)))
    return out.err("ERR: \
'0.3.0' shouldn't satisfy requirement '^0.2.3'");

  if (not semver_req("^0.2.3").is_satisfied_by(semver(0, 2, 9999)))
    return out.err("ERR: \
'0.2.9999' should satisfy requirement '^0.2.3'");

  if (not semver_req("^1.2.3").is_satisfied_by(semver(1, 2, 3)))
    return out.err("ERR: \
'1.2.3' should satisfy requirement '^1.2.3'");

  if (semver_req("^1.2.3").is_satisfied_by(semver(1, 2, 2)))
    return out.err("ERR: \
'1.2.2' should not satisfy requirement '^1.2.3'");

  if (semver_req("^1.2.3").is_satisfied_by(semver(2, 0, 0)))
    return out.err("ERR: \
'2.0.0' should not satisfy requirement '^1.2.3'");

  if (not semver_req("^1.2.3").is_satisfied_by(semver(1, 9999, 9999)))
    return out.err("ERR: \
'1.9999.9999' should satisfy requirement '^1.2.3'");


  if (not semver_req("^0.2.3").is_satisfied_by(semver(0, 2, 3)))
    return out.err("ERR: \
'0.2.3' should satisfy requirement '^0.2.3'");

  if (semver_req("^0.2.3").is_satisfied_by(semver(0, 2, 2)))
    return out.err("ERR: \
'0.2.2' should not satisfy requirement '^0.2.3'");

  if (semver_req("^0.2.3").is_satisfied_by(semver(0, 3, 0)))
    return out.err("ERR: \
'0.3.0' should not satisfy requirement '^0.2.3'");

  if (not semver_req("^0.2.3").is_satisfied_by(semver(0, 2, 9999)))
    return out.err("ERR: \
'0.2.9999' should satisfy requirement '^0.2.3'");


  if (semver_req("^0.0.3").is_satisfied_by(semver(0, 0, 2)))
    return out.err("ERR: \
'0.0.2' should not satisfy requirement '^0.0.3'");

  if (semver_req("^0.0.3").is_satisfied_by(semver(0, 0, 4)))
    return out.err("ERR: \
'0.0.4' should not satisfy requirement '^0.0.3'");

  if (not semver_req("^0.0.3").is_satisfied_by(semver(0, 0, 3)))
    return out.err("ERR: \
'0.0.3' should satisfy requirement '^0.0.3'");


  if (not semver_req("^1.2.x").is_satisfied_by(semver(1, 2, 0)))
    return out.err("ERR: \
'1.2.0' should satisfy requirement '^1.2.x'");

  if (semver_req("^1.2.x").is_satisfied_by(semver(2, 0, 0)))
    return out.err("ERR: \
'2.0.0' should not satisfy requirement '^1.2.x'");  

  if (not semver_req("^1.2.x").is_satisfied_by(semver(1, 9999, 9999)))
    return out.err("ERR: \
'1.9999.9999' should satisfy requirement '^1.2.x'");


  if (not semver_req("^0.0").is_satisfied_by(semver(0, 0, 9999)))
    return out.err("ERR: \
'0.0.9999' should satisfy requirement '^0.0'");

  if (semver_req("^0.0.x").is_satisfied_by(semver(0, 1, 0)))
    return out.err("ERR: \
'0.1.0' should not satisfy requirement '^0.0.x'");


  if (not semver_req("~1.2.3").is_satisfied_by(semver(1, 2, 9999)))
    return out.err("ERR: \
'1.2.9999' should satisfy requirement '~1.2.3'");

  if (semver_req("~1.2.3").is_satisfied_by(semver(1, 3, 0)))
    return out.err("ERR: \
'1.3.0' should not satisfy requirement '~1.2.3'");

  if (not semver_req("~1").is_satisfied_by(semver(1, 9999, 9999)))
    return out.err("ERR: \
'1.9999.9999' should satisfy requirement '~1'");


  if (semver_req("~0.2.3").is_satisfied_by(semver(0, 2, 2)))
    return out.err("ERR: \
'0.2.2' should not satisfy requirement '~0.2.3'");

  if (not semver_req("~0.2.3").is_satisfied_by(semver(0, 2, 9999)))
    return out.err("ERR: \
'0.2.9999' should satisfy requirement '~0.2.3'");

  if (semver_req("~0").is_satisfied_by(semver(1, 0, 0)))
    return out.err("ERR: \
'1.0.0' should not satisfy requirement '~0'");

  if (not semver_req("~0").is_satisfied_by(semver(0, 9999, 9999)))
    return out.err("ERR: \
'0.9999.9999' should satisfy requirement '~0'");


//  if (!semver_req("~1.2.3").is_satisfied_by(semver(1, 2, 9999)))
//    return out.err("ERR: \
//'1.2.9999' should satisfy requirement '~1.2.3'");
   
  return out.ok();
  }
}