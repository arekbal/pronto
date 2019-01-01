#pragma once
#define TESTS

#include "../console.hpp"

#include "str_tests.hpp"

int main(int argc, char **argv) 
{ 
  pronto::console console_;
  console_.no_color = false;
  console_.inf("TESTS:");

  auto _ = console_.indent();

  return wildcard_path_match_tests(console_);
}
