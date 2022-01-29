#include <boost/test/tools/old/interface.hpp>
#include <type_traits>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BitIntToolsTests

#include <boost/test/unit_test.hpp>

#include "bitint_tools/bitint_constant.hpp"

BOOST_AUTO_TEST_CASE(TestFit) {
  constexpr unsigned _ExtInt(32) testVal{0x3BACD};
  constexpr auto cstVal = hint::fitted_c<testVal>;
  static_assert(cstVal.width == 18, "Incorrect fitting");
}
