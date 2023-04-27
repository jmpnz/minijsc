#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"

TEST_CASE("check tests actually work") {
  auto var = 5;
  CHECK(var == 5);
}
