#pragma once

#include <concepts>

#include "expression/value.hpp"

namespace hint {

template <typename T>
concept BitHeapAddable = ConstantType<T> ||
    (VariableType<T> &&DelayedSignalType<typename T::repr_t>);

namespace detail {

template <BitHeapAddable BHAT, bool IsSub> struct BitHeapOp {
  using value_t = BHAT;
  static constexpr bool isSubtraction = IsSub;
};

template <typename T> constexpr bool isBitHeapOpType = false;

template <BitHeapAddable BHAT, bool IsSub>
constexpr bool isBitHeapOpType<BitHeapOp<BHAT, IsSub>> = true;

template <typename T>
concept BitHeapOpType = isBitHeapOpType<T>;

} // namespace detail

template<typename T>
concept BitHeapCompressorType = requires {
  { T::latency }->std::same_as<unsigned int>;
  { T::nInputs }->std::same_as<unsigned int>;
  { T::nOutputs }->std::same_as<unsigned int>;
  { T::inputs }->std::same_as<std::array<unsigned int, T::nInputs>>;
  { T::outputs }->std::same_as<std::array<unsigned int, T::nOutpus>>;
};

} // namespace hint