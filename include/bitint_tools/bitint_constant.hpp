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
    using mask_t = unsigned _ExtInt(n);
    auto Mask = mask_t{1} << (n - 1);
    std::size_t i;
    for (i = 0 ; i < n ; ++i) {
      if (Mask & Val) {break;}
      Mask >>= 1;
    }
    return i;
  }
}

template<BitIntT T, T Val>
struct BitIntConstant {
  static constexpr T value = Val;
  using value_type = T;
  static constexpr unsigned int width = bitIntWidth<T>;
};

template<auto T>
constexpr bool bitInt_c = false;

template<BitIntT T, T Val>
constexpr BitIntConstant<T, Val> bitInt_c<Val>{};

template<typename T>
constexpr bool isBitIntConstant = false;

template<BitIntT T, T val>
constexpr bool isBitIntConstant<BitIntConstant<T, val>> = true;

template<typename T>
concept BitIntConstT = isBitIntConstant<T>;

template<auto Val>
consteval auto toFit() {
  if constexpr (Val == 0) {
    using ret_val_t = unsigned _ExtInt(1);
    return BitIntConstant<ret_val_t , ret_val_t{0}>{};
  } else {
    constexpr auto n = bitIntWidth<decltype(Val)> - detail::clz<Val>(); 
    using ret_val_t = detail::bitint_base_t<n, false>;
    return BitIntConstant<ret_val_t, static_cast<ret_val_t>(Val)>{};
  }
}

template<auto T>
constexpr bool fitted_c = false;

template<BitIntT T, T Val>
constexpr auto fitted_c<Val> = toFit<Val>();
    
} // namespace hint

#endif
