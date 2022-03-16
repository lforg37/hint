#include <boost/test/tools/old/interface.hpp>
#include <iostream>
#include <type_traits>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE BitIntToolsTests

#include <boost/test/unit_test.hpp>

#include "bitint_tools/bitint_constant.hpp"
#include "bitint_tools/type_helpers.hpp"

BOOST_AUTO_TEST_CASE(TestFit) {
  constexpr unsigned _BitInt(32) testVal{0x3BACD};
  constexpr auto cst = hint::fitted_c<testVal>;
  static_assert(cst.width == 18, "Incorrect fitting");
  static_assert(cst.value == 0x3bacd, "Incorrect value");
}


BOOST_AUTO_TEST_CASE(TestUDL) {
  using hint::operator ""_ubi;
  using hint::operator ""_sbi;

  constexpr auto c = 0b1101_ubi;
  using utype = decltype(c);
  using target_type = unsigned _BitInt(4);
  static_assert(std::is_same_v<std::remove_cvref_t<utype>, target_type>);
  static_assert( c == 13);

  constexpr auto d = 0b1101_sbi;
  using stype = decltype(d);
  using target_stype = signed _BitInt(4);
  static_assert(std::is_same_v<std::remove_cvref_t<stype>, target_stype>);
  static_assert(d == -3);
}
