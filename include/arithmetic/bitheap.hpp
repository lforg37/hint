#ifndef ARITHMETIC_BITHEAP_HPP
#define ARITHMETIC_BITHEAP_HPP

#include <concepts>
#include <tuple>
#include <type_traits>

#include "expression/signal.hpp"
#include "expression/value.hpp"

namespace hint {

namespace detail {
namespace bitheap {
template <ValueType VT, bool IsSub> struct Operation {
  using value_t = VT;
  static constexpr bool isSubtraction = IsSub;
};

template <typename T> constexpr bool isOperation = false;

template <ValueType VT, bool IsSub>
constexpr bool isOperation<Operation<VT, IsSub>> = true;

template <typename T>
concept OperationSpec = isOperation<T>;
} // namespace bitheap
} // namespace detail

// TODO:
//  - Handle constants,
//  - handle delays,
//  - handle smart add of same shifted signal etc.
template <detail::bitheap::OperationSpec... OS> class BitHeap {
private:
  std::tuple<typename OS::value_t...> inputs;

  using opposite_t = BitHeap<detail::bitheap::Operation<typename OS::value_t, !OS::isSubtraction>...>;

  template <bool IsSub, ValueType VT>
  constexpr BitHeap<OS..., detail::bitheap::Operation<VT, IsSub>> signalOp(VT val) const {
      return {std::tuple_cat(inputs, std::make_tuple(val))};
  }

public:
  template <detail::bitheap::OperationSpec... OS2>
  constexpr BitHeap<OS..., OS2...> merge(BitHeap<OS2...> &&bh) const {
    return {std::tuple_cat(inputs, bh.inputs)};
  }

  template <ValueType VT>
  constexpr auto addValue(VT val) const {
    return signalOp<false>(val);
  }

  template <ValueType VT>
  constexpr auto subValue(VT val) const {
    return signalOp<true>(val);
  }

  constexpr opposite_t operator-() const {
    return {inputs};
  };

  template <detail::bitheap::OperationSpec... OS2> friend class BitHeap;
};

}; // namespace hint
#endif
