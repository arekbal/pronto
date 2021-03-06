
#define PLATFORM_WINDOWS 1

#include "pch.hh"
#include <iostream>
#include "core.hh"
#include "command_selector.hh"

//#include "tests/tests.hh"

#ifndef TESTS

int main(int argc, char **argv) {
  
  auto selector = pronto::command_selector(argc, argv);

  return selector.execute();
}

#endif // !TESTS

