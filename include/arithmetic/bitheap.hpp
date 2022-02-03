#ifndef ARITHMETIC_BITHEAP_HPP
#define ARITHMETIC_BITHEAP_HPP

#include <tuple>
#include <type_traits>

#include "detail/bitheap.hpp"

namespace hint {

// TODO:
//  - Handle constants,
//  - handle delays,
//  - handle smart add of same shifted signal etc.
template <detail::BitHeapOpType... OS> class BitHeap {
private:
  using storage_t = std::tuple<typename OS::value_t...>;
  storage_t inputs;

  using opposite_t = BitHeap<
      detail::BitHeapOp<typename OS::value_t, !OS::isSubtraction>...>;

  template <bool IsSub, BitHeapAddable BHAT>
  constexpr BitHeap<OS..., detail::BitHeapOp<BHAT, IsSub>>
  signalOp(BHAT val) const {
    return {std::tuple_cat(inputs, std::make_tuple(val))};
  }

public:
  template <detail::BitHeapOpType... OS2>
  constexpr BitHeap<OS..., OS2...> merge(BitHeap<OS2...> &&bh) const {
    return {std::tuple_cat(inputs, bh.inputs)};
  }

  BitHeap(storage_t &&in) : inputs{in} {}

  BitHeap(OS... ops) : inputs{ops...} {}

  template <BitHeapAddable BHAT> constexpr auto addValue(BHAT val) const {
    return signalOp<false>(val);
  }

  template <BitHeapAddable BHAT> constexpr auto subValue(BHAT val) const {
    return signalOp<true>(val);
  }

  constexpr opposite_t operator-() const { return {inputs}; };

  template <detail::BitHeapOpType... OS2> friend class BitHeap;
};

}; // namespace hint
#endif
