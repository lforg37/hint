#include <type_traits>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ExpressionTests

#include <boost/test/unit_test.hpp>

#include "expression/value.hpp"

BOOST_AUTO_TEST_CASE(TestUDL) {
  using namespace hint::litterals;
  constexpr unsigned _BitInt(18) val{0x3ACBD};
  auto b = 0X3ACBD_cst;
  static_assert(std::is_same_v<decltype(b), hint::Constant<false, 17, 0, val>>,
                "Wrong type");
}
