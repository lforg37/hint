#ifndef BITINT_TOOLS_MANIPULATIONS_HPP
#define BITINT_TOOLS_MANIPULATIONS_HPP

#include <bit>
#include <type_traits>

#include "type_helpers.hpp"

namespace hint {

template<typename T>
constexpr auto bitIntWidth = 0;

template<std::size_t N>
constexpr auto bitIntWidth<unsigned _ExtInt(N)> = N;

template<std::size_t N>
constexpr auto bitIntWidth<_ExtInt(N)> = N;
}
#endif
