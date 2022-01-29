#ifndef EXTINT_TOOLS_MANIPULATIONS_HPP
#define EXTINT_TOOLS_MANIPULATIONS_HPP

#include <bit>
#include <type_traits>

#include "type_helpers.hpp"

namespace hint {

template<typename T>
constexpr auto extIntWidth = 0;

template<std::size_t N>
constexpr auto extIntWidth<unsigned _ExtInt(N)> = N;

template<auto Val>
consteval auto toFit() {
  if constexpr (Val == 0) {
    using ret_val_t = unsigned _ExtInt(1);
    return std::integral_constant<ret_val_t , ret_val_t{0}>{};
  } else {
    constexpr auto n = extIntWidth<decltype(Val)>;
    using ret_val_t = detail::extint_base_t<n, false>;
    return std::integral_constant<ret_val_t, static_cast<ret_val_t>(Val)>{};
  }
}
}
#endif
