#pragma once
#define TESTS

#include "../console.hh"

#include "semver_tests.hh"
#include "semver_req_tests.hh"
#include "str_tests.hh"

int main(int argc, char **argv) 
{ 
  pronto::console console_;
  console_.no_color = false;
  console_.inf("TESTS:");

  auto _ = console_.indent();

  return     
    wildcard_path_match_tests(console_) &&
    semver_tests(console_) &&
    semver_req_tests(console_);
}
