#include <boost/test/tools/old/interface.hpp>
#include <iostream>
#include <type_traits>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BitIntToolsTests

#include <boost/test/unit_test.hpp>

#include "bitint_tools/bitint_constant.hpp"

BOOST_AUTO_TEST_CASE(TestFit) {
  constexpr unsigned _BitInt(32) testVal{0x3BACD};
  constexpr auto cst = hint::fitted_c<testVal>;
  static_assert(cst.width == 18, "Incorrect fitting");
  static_assert(cst.value == 0x3bacd, "Incorrect value");
}
