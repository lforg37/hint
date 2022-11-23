#ifndef BITINT_TOOLS_BITINT_CONSTANT_HPP
#define BITINT_TOOLS_BITINT_CONSTANT_HPP

#include "manipulation.hpp"
#include "type_helpers.hpp"
#include <cstddef>


namespace hint {

namespace detail {
  template<auto Val> constexpr auto clz() {
    // TODO: can be replaced by efficient recursive clz
    constexpr auto n = bitIntWidth<decltype(Val)>;
    using mask_t = unsigned _BitInt(n);
    auto Mask = mask_t{1} << (n - 1);
    std::size_t i;
    for (i = 0 ; i < n ; ++i) {
      if (Mask & Val) {break;}
      Mask >>= 1;
    }
    return i;
  }

  template<auto Val> constexpr auto ctz() {
    // TODO: can be replaced by efficient recursive ctz
    constexpr auto n = bitIntWidth<decltype(Val)>;
    using mask_t = unsigned _BitInt(n);
    auto Mask = mask_t{1};
    std::size_t i;
    for (i = 0 ; i < n ; ++i) {
      if (Mask & Val) {break;}
      Mask <<= 1;
    }
    return i;
  }
}

template<BitIntT T, T Val>
struct BitIntConstant {
  static constexpr T value = Val;
  using value_type = T;
  static constexpr unsigned int width = bitIntWidth<T>;
  static constexpr bool isSigned = signedType<T>;
};

template<auto Val>
constexpr BitIntConstant<decltype(Val), Val> bitInt_c{};

template<typename T>
constexpr bool isBitIntConstant = false;

template<BitIntT T, T val>
constexpr bool isBitIntConstant<BitIntConstant<T, val>> = true;

template<typename T>
concept BitIntConstT = isBitIntConstant<T>;

template<auto Val>
consteval auto toFit() {
  if constexpr (Val == 0) {
    using ret_val_t = unsigned _BitInt(1);
    return BitIntConstant<ret_val_t , ret_val_t{0}>{};
  } else {
    constexpr auto n = bitIntWidth<decltype(Val)>;
    constexpr bool signedtype = signedType<decltype(Val)>;
    if constexpr (signedtype && Val < 0) {
      constexpr auto lo = detail::clz<~Val>();
      using ret_val_t = detail::bitint_base_t<true, n-lo+1>;
      return BitIntConstant<ret_val_t, static_cast<ret_val_t>(Val)>{}; 
    } else {
      constexpr auto lz = detail::clz<Val>();
      using ret_val_t = detail::bitint_base_t<false, n-lz>;
      return BitIntConstant<ret_val_t, static_cast<ret_val_t>(Val)>{};
    }
  }
}

template<auto Val>
constexpr auto fitted_c = toFit<Val>();
    
} // namespace hint

#endif
