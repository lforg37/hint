#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "arithmetic/bitheap.hpp"

BOOST_AUTO_TEST_CASE(BitHeapEmptyCst) {
  hint::BitHeap<> a{};
};

BOOST_AUTO_TEST_CASE(BitHeapAddCsts) {
  BOOST_REQUIRE(false);
}
