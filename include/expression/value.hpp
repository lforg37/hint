#ifndef EXPRESSION_VALUE_HPP
#define EXPRESSION_VALUE_HPP
#include <algorithm>
#include <array>
#include <bits/ranges_util.h>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <type_traits>

#include <boost/hana.hpp>

#include "extint_tools/manipulation.hpp"
#include "extint_tools/type_helpers.hpp"

#include "signal.hpp"

namespace hint {

template <typename T> constexpr bool isValue = false;

/// Represent a value not known at compile time
template <SignalType ST> struct Variable {
public:
  using signal_t = ST;
  using storage_t = detail::extint_base_t<ST::Width, ST::IsSigned>;
  const storage_t val;
};

template <SignalType ST> constexpr bool isValue<Variable<ST>> = true;

/// Represents a value known at compile time
template <SignalType ST, detail::extint_base_t<ST::width, ST::isSigned> Val>
struct Constant {
  using storage_t = detail::extint_base_t<ST::width, ST::isSigned>;
  static constexpr storage_t val = Val;
};

template <SignalType ST, detail::extint_base_t<ST::width, ST::isSigned> Val>
constexpr bool isValue<Constant<ST, Val>> = true;

template <typename T>
concept ValueType = isValue<T>;

template <ValueType T> constexpr bool isValueConstant = false;

template <SignalType ST, detail::extint_base_t<ST::Width, ST::IsSigned> Val>
constexpr bool isValueConstant<Constant<ST, Val>> = true;

namespace litterals {
namespace detail {
constexpr uint8_t getCharVal(char ch) {
  auto cInRange = [](char c, char LowerBound, char UpperBound) constexpr {
    return (c >= LowerBound && c <= UpperBound);
  };

  char lowercase = cInRange(ch, 'A', 'Z') ? ch - 'A' + 'a' : ch;

  if (cInRange(lowercase, '0', '9')) {
    return lowercase - '0';
  } else {
    return lowercase - 'a' + 10;
  }
}

template <unsigned int W> consteval auto clz(unsigned _ExtInt(W) val) {
  size_t toTrim = 0;
  for (auto mask = _ExtInt(W){1} << (W - 1); mask != 1; mask >>= 1) {
    if (val & mask) {
      break;
    } else {
      ++toTrim;
    }
  }
  return toTrim;
};

template <size_t Radix, size_t RadixBitWidth> struct Base {
private:
public:
  static constexpr size_t radix = Radix;
  static constexpr size_t radixBitWidth = RadixBitWidth;
  static_assert(Radix <= 36);
  static constexpr uint8_t getCharValue(char ch) {
    return getCharVal(ch);
  }
};

template <unsigned int N> consteval auto getWidth(unsigned _ExtInt(N)) {
  return N;
}

using Binary = Base<2, 1>;
using Octal = Base<8, 3>;
using Decimal = Base<10, 4>;
using Hexadecimal = Base<16, 4>;

template <int LsbWeight> consteval auto toConstant(auto val) {
  using T = decltype(val);
  constexpr auto fitted = toFit<T::value>();
  constexpr auto width = extIntWidth<typename T::value_type>;
  return Constant<Signal<false, width, LsbWeight>, fitted>{};
}

template <typename Base, char... CharSeq>
consteval auto parseInt() {
  constexpr auto nDigits = sizeof...(CharSeq);
  using return_type = unsigned _ExtInt(nDigits * Base::radixBitWidth);
  return_type acc = 0;
  for (auto c : std::array{CharSeq...}) {
    if (c == '\'')
      continue;
    acc *= static_cast<return_type>(Base::radix);
    acc += static_cast<return_type>(Base::getCharValue(c));
  }
  return acc;
};

// Skip two prefix char
template <char, char, char... CharSeq> consteval auto parseHex() {
  return parseInt<Hexadecimal, CharSeq...>();
}

template <char... CharSeq> consteval auto parseDec() {
  return parseInt<Decimal, CharSeq...>();
}

template <char, char, char... CharSeq> consteval auto parseBin() {
  return parseInt<Binary, CharSeq...>();
}

template <char, char... CharSeq> consteval auto parseOct() {
  return parseInt<Octal, CharSeq...>();
}

} // namespace detail

template <char... CharSeq> constexpr auto operator"" _cst() {
  constexpr std::array chars{CharSeq...};
  constexpr auto len = chars.size();
  constexpr bool firstIsZero = chars[0] == '0';
  if constexpr (firstIsZero) {
    if constexpr (len == 1) {
      return Constant<Signal<false, 1, 0>, 0>{};
    } else {
      constexpr char c2 = chars[1];
      static_assert(c2 != '.', "Decimal floats are not yet handled");
      if constexpr (c2 == 'x' || c2 == 'X') {
        constexpr auto ret = detail::parseHex<CharSeq...>();
        return detail::toConstant<0>(toFit<ret>());
      } else if constexpr (c2 == 'b' || c2 == 'B') {
        constexpr auto ret = detail::parseBin<CharSeq...>();
        return detail::toConstant<0>(toFit<ret>());
      } else {
        constexpr auto ret = detail::parseOct<CharSeq...>();
        return detail::toConstant<0>(toFit<ret>());
      }
    }
  } else {
    constexpr auto ret = detail::parseDec<CharSeq...>();
    return detail::toConstant<0>(toFit<ret>());
  }
};

} // namespace litterals

} // namespace hint

#endif
