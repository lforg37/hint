#ifndef EXPRESSION_VALUE_HPP
#define EXPRESSION_VALUE_HPP
#include <algorithm>
#include <charconv>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <boost/hana.hpp>

#include "bitint_tools/bitint_constant.hpp"
#include "bitint_tools/type_helpers.hpp"

#include "signal.hpp"

namespace hint {

template <typename T> constexpr bool isValue = false;

/// Represent a value not known at compile time
template <SignalType ST> struct Variable {
public:
  using signal_t = ST;
  using storage_t = detail::bitint_base_t<ST::Width, ST::IsSigned>;
  const storage_t val;
};

template <SignalType ST> constexpr bool isValue<Variable<ST>> = true;

/// Represents a value known at compile time
template <SignalType ST, detail::bitint_base_t<ST::width, ST::isSigned> Val>
struct Constant {
  using storage_t = detail::bitint_base_t<ST::width, ST::isSigned>;
  static constexpr storage_t val = Val;
};

template <SignalType ST, detail::bitint_base_t<ST::width, ST::isSigned> Val>
constexpr bool isValue<Constant<ST, Val>> = true;

template <typename T>
concept ValueType = isValue<T>;

template <ValueType T> constexpr bool isValueConstant = false;

template <SignalType ST, detail::bitint_base_t<ST::Width, ST::IsSigned> Val>
constexpr bool isValueConstant<Constant<ST, Val>> = true;

namespace litterals {
namespace hana = boost::hana;
namespace detail {
template <char C> struct CharHolder {
  constexpr bool operator==(const char val) const { return val == C; }
  constexpr bool operator!=(const char val) const { return val != C; }
  
  static constexpr char c = C;

  constexpr char get() const { return C; }
};

template <char C> static consteval uint8_t getCharVal() {
  auto cInRange = [](char c, char LowerBound, char UpperBound) consteval {
    return (c >= LowerBound && c <= UpperBound);
  };

  constexpr char lowercase = cInRange(C, 'A', 'Z') ? C - 'A' + 'a' : C;
  static_assert(cInRange(lowercase, '0', '9') || cInRange(lowercase, 'a', 'z'),
                "Incorrect symbol for digit");

  if constexpr (cInRange(lowercase, '0', '9')) {
    return lowercase - '0';
  } else {
    return lowercase - 'a' + 10;
  }
}

template <size_t Radix, size_t RadixBitWidth> struct Base {
private:
public:
  static constexpr size_t radix = Radix;
  static constexpr size_t radixBitWidth = RadixBitWidth;
  static_assert(Radix <= 36);
  template <char C> static consteval uint8_t getCharValue(CharHolder<C>) {
    constexpr auto val = getCharVal<C>();
    static_assert(val < Radix, "Incorrect digit for current radix");
    return val;
  }
};

template <unsigned int N> consteval auto getWidth(unsigned _ExtInt(N)) {
  return N;
}

using Binary = Base<2, 1>;
using Octal = Base<8, 3>;
using Decimal = Base<10, 4>;
using Hexadecimal = Base<16, 4>;

template <typename Base> constexpr auto parseInt(auto in0) {
  constexpr auto in = decltype(in0){};
  constexpr auto outWidth = hana::length(in) * Base::radixBitWidth;
  using untrimmed_ret_t = unsigned _ExtInt(outWidth);
  constexpr untrimmed_ret_t zero{0};
  auto parse = [](untrimmed_ret_t state, auto ch) constexpr -> untrimmed_ret_t {
    return state * static_cast<untrimmed_ret_t>(Base::radix) +
           static_cast<untrimmed_ret_t>(Base::getCharValue(ch));
  };
  constexpr auto untrimmed = hana::fold_left(in, zero, parse);
  return fitted_c<untrimmed>;
};

// Hex value can be either integers or floats
consteval auto parseHex(auto in0) {
  constexpr auto in = decltype(in0){};
  constexpr auto noPrefix = hana::drop_front_exactly(in,  hana::size_c<2>);
  constexpr auto floatMarker = [](auto &&ch) {
    char c = ch.get();
    return c == '.' || c == 'p' || c == 'P';
  };

  static_assert(hana::count_if(noPrefix, floatMarker) == 0,
                "Hex float are not handled yet");
  constexpr auto resVal = parseInt<Hexadecimal>(noPrefix);
  return Constant<Signal<false, resVal.width, 0>, resVal.value>{};
}

// Hex value can be either integers or floats
consteval auto parseDec(auto in0) {
  constexpr auto in = decltype(in0){};
  auto floatMarker = [](auto &&ch) {
    char c = ch.get();
    return c == '.' || c == 'e' || c == 'E';
  };

  static_assert(hana::count_if(in, floatMarker) == 0,
                "Decimal float are not handled yet");
  constexpr auto resVal = parseInt<Decimal>(in);
  return Constant<Signal<false, resVal.width, 0>, resVal.value>{};
}

template<typename Base, size_t Drop>
consteval auto parseSimpleIntegerRadix(auto in) {
  constexpr auto inCp = decltype(in){};
  constexpr auto noPrefix = hana::drop_front_exactly(inCp, hana::size_c<Drop>);
  constexpr auto resVal = parseInt<Base>(noPrefix);
  return Constant<Signal<false, resVal.width, 0>, resVal.value>{};
}
} // namespace detail

template <char... CharSeq> constexpr auto operator"" _cst() {
  constexpr auto unfilteredCharTuple = hana::make_tuple(detail::CharHolder<CharSeq>{}...);
  constexpr auto charTuple = hana::filter(unfilteredCharTuple, [](auto c){
    constexpr bool isNotNoise = decltype(c)::c != '\'';
    return hana::integral_c<int, isNotNoise>;
  });
  constexpr auto len = sizeof...(CharSeq);
  constexpr bool firstIsZero = hana::front(charTuple) == '0';
  if constexpr (firstIsZero) {
    if constexpr (len == 1) {
      return Constant<Signal<false, 1, 0>, 0> {};
    } else {
      constexpr char c2 = hana::at(charTuple, hana::size_c<1>).get();
      static_assert(c2 != '.', "Decimal floats are not yet handled");
      if constexpr (c2 == 'x' || c2 == 'X') {
        return detail::parseHex(charTuple);
      } else if constexpr (c2 == 'b' || c2 == 'B') {
        return detail::parseSimpleIntegerRadix<detail::Binary, 2>(charTuple);
      } else {
        return detail::parseSimpleIntegerRadix<detail::Octal, 1>(charTuple);
      }
    }
  } else {
    detail::parseDec(charTuple);
  }
};

} // namespace litterals

} // namespace hint

#endif
