#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ExpressionTests

#include <boost/test/unit_test.hpp>

#include "expression/value.hpp"

BOOST_AUTO_TEST_CASE(TestUDL) {
    using namespace hint::litterals;
    auto b = 0X17'5_cst;
    BOOST_REQUIRE(false);
}
