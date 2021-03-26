#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE ExtIntBackendTestModule

#include <array>
#include <cstdint>
#include <iostream>
#include <type_traits>

#include <boost/test/unit_test.hpp>

#include "hint.hpp"

using hint::ExtIntWrapper;
using std::array;
using std::cout;
using std::endl;

namespace {

template <unsigned int t>
using int2type = std::integral_constant<unsigned int, t>;
};

BOOST_AUTO_TEST_CASE(TestInstantiation) {
  constexpr ExtIntWrapper<12, false> test{};
  constexpr unsigned _ExtInt(12) zero{0};
  static_assert(test.unravel() == zero, "Error with default general Ctor");
}

BOOST_AUTO_TEST_CASE(TestInstantiationWidth1) {
  constexpr ExtIntWrapper<1, false> test{};
  static_assert(!test.unravel(), "Error with default w=1 Ctor");
}

BOOST_AUTO_TEST_CASE(TestNonEmptyCtor) {
  ExtIntWrapper<12, false> test{0b111111111111};
  BOOST_REQUIRE_EQUAL(static_cast<int>(test.unravel()), 0b111111111111);
};

BOOST_AUTO_TEST_CASE(TestEqualityOp) {
  for (unsigned int i : {0, 12, 0b111111, 17, 1}) {
    for (unsigned int j : {0, 12, 0b111111, 17, 1}) {
      ExtIntWrapper<6, false> wrappedI{static_cast<unsigned _ExtInt(6)>(i)},
          wrappedJ{static_cast<unsigned _ExtInt(6)>(j)};
      auto wrappedEqual = (wrappedI == wrappedJ);
      auto inEqual = (i == j);
      BOOST_REQUIRE_EQUAL(wrappedEqual.unravel(), inEqual);
    }
  }
}

BOOST_AUTO_TEST_CASE(TestGet) {
  constexpr auto in_val = 0b01101;
  constexpr ExtIntWrapper<5, false> in{in_val};
  constexpr auto a = in.template get<0>();
  constexpr auto b = in.template get<1>();
  constexpr auto c = in.template get<2>();
  constexpr auto d = in.template get<3>();
  constexpr auto e = in.template get<4>();

  static_assert(a.unravel(), "Error with bit 0");
  static_assert(!b.unravel(), "Error with bit 1");
  static_assert(c.unravel(), "Error with bit 2");
  static_assert(d.unravel(), "Error with bit 3");
  static_assert(!e.unravel(), "Error with bit 4");
}

BOOST_AUTO_TEST_CASE(TestSlice) {
  using zero = int2type<0>;
  using one = int2type<1>;
  using two = int2type<2>;
  using three = int2type<3>;
  using four = int2type<4>;
  using five = int2type<5>;
  using six = int2type<6>;

  constexpr uint8_t inval = 0b110010;
  constexpr ExtIntWrapper<6, false> in{inval};

  constexpr auto slicer = [in]<typename SW, typename SS>(SW, SS) {
    constexpr auto slice_width = SW::value;
    constexpr auto slice_start = SS::value;
    return in.template slice<slice_start + slice_width - 1, slice_start>();
  };

  constexpr auto constructed = [inval]<typename SW, typename SS>(SW, SS) {
    constexpr auto slice_width = SW::value;
    constexpr auto slice_start = SS::value;
    constexpr uint8_t mask_in{((1 << slice_width) - 1) << slice_start};
    return ExtIntWrapper<slice_width, false>{(inval & mask_in) >> slice_start};
  };

  static_assert((slicer(one{}, zero{}) == constructed(one{}, zero{})).unravel(),
                "Slicing error");
  static_assert((slicer(one{}, one{}) == constructed(one{}, one{})).unravel(),
                "Slicing error");
  static_assert((slicer(one{}, two{}) == constructed(one{}, two{})).unravel(),
                "Slicing error");
  static_assert((slicer(one{}, three{}) == constructed(one{}, three{})).unravel(),
                "Slicing error");
  static_assert((slicer(one{}, four{}) == constructed(one{}, four{})).unravel(),
                "Slicing error");
  static_assert((slicer(one{}, five{}) == constructed(one{}, five{})).unravel(),
                "Slicing error");
  static_assert((slicer(two{}, zero{}) == constructed(two{}, zero{})).unravel(),
                "Slicing error");
  static_assert((slicer(two{}, one{}) == constructed(two{}, one{})).unravel(),
                "Slicing error");
  static_assert((slicer(two{}, two{}) == constructed(two{}, two{})).unravel(),
                "Slicing error");
  static_assert((slicer(two{}, three{}) == constructed(two{}, three{})).unravel(),
                "Slicing error");
  static_assert((slicer(two{}, four{}) == constructed(two{}, four{})).unravel(),
                "Slicing error");
  static_assert((slicer(three{}, zero{}) ==
                    constructed(three{}, zero{})).unravel(),
                "Slicing error");
  static_assert((slicer(three{}, one{}) == constructed(three{}, one{})).unravel(),
                "Slicing error");
  static_assert((slicer(three{}, two{}) == constructed(three{}, two{})).unravel(),
                "Slicing error");
  static_assert((slicer(three{}, three{}) ==
                    constructed(three{}, three{})).unravel(),
                "Slicing error");
  static_assert((slicer(four{}, zero{}) == constructed(four{}, zero{})).unravel(),
                "Slicing error");
  static_assert((slicer(four{}, one{}) == constructed(four{}, one{})).unravel(),
                "Slicing error");
  static_assert((slicer(four{}, two{}) == constructed(four{}, two{})).unravel(),
                "Slicing error");
  static_assert((slicer(five{}, zero{}) == constructed(five{}, zero{})).unravel(),
                "Slicing error");
  static_assert((slicer(five{}, one{}) == constructed(five{}, one{})).unravel(),
                "Slicing error");
  static_assert((slicer(six{}, zero{}) == constructed(six{}, zero{})).unravel(),
                "Slicing error");
}

BOOST_AUTO_TEST_CASE(TestInvert) {
  constexpr ExtIntWrapper<7, false> a{0}, b{0b1111111}, c{0b1010101}, d{0b0101010};
  static_assert((a.invert() == b).unravel(), "Invert error");
  static_assert((b.invert() == a).unravel(), "Invert error");
  static_assert((c.invert() == d).unravel(), "Invert error");
  static_assert((d.invert() == c).unravel(), "Invert error");
}