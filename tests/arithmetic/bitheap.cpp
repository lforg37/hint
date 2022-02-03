#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "arithmetic/bitheap.hpp"

BOOST_AUTO_TEST_CASE(BitHeapEmptyCst) { hint::BitHeap<> a{}; };

BOOST_AUTO_TEST_CASE(BitHeapAddCsts) {
  using namespace hint::litterals;
  hint::BitHeap<> a{};
  auto b = a.addValue(0x1245_cst);
  constexpr unsigned _ExtInt(13) val = 0x1245;
  static_assert(
      std::is_same_v<
          decltype(b),
          hint::BitHeap<hint::detail::BitHeapOp<
              hint::Constant<false, 12, 0, val>, false>>>,
      "Wrong type");
}
