#ifndef EXTINT_IMPL_IPP
#define EXTINT_IMPL_IPP

#include "config.hpp"

#include <bit>
#include <cstdint>
#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

#include "bitint_tools/type_helpers.hpp"

#include "primitives/backwards.hpp"

#include "tools/functools.hpp"
#include "tools/int_sequence.hpp"
#include "tools/static_math.hpp"

using namespace std;

namespace hint {

template <unsigned int W, bool is_signed> class BitIntWrapper;

template <unsigned int W1, unsigned int W2, bool is_signed>
constexpr BitIntWrapper<
    Arithmetic_Prop<W1, W2, is_signed, is_signed>::_prodSize, is_signed>
operator*(BitIntWrapper<W1, is_signed> const lhs,
          BitIntWrapper<W2, is_signed> const rhs) {
  return {lhs._val * rhs._val};
}

template <unsigned int shiftedSize, bool isShiftedSigned,
          unsigned int shifterSize>
constexpr BitIntWrapper<shiftedSize, isShiftedSigned>
operator>>(BitIntWrapper<shiftedSize, isShiftedSigned> const lhs,
           BitIntWrapper<shifterSize, false> const rhs) {
  return {lhs._val >> rhs._val};
}

template <unsigned int shiftedSize, bool isShiftedSigned,
          unsigned int shifterSize>
constexpr BitIntWrapper<shiftedSize, isShiftedSigned>
operator<<(BitIntWrapper<shiftedSize, isShiftedSigned> const lhs,
           BitIntWrapper<shifterSize, false> const rhs) {
  return {lhs._val << rhs._val};
}

template <unsigned int W, bool is_signed>
constexpr BitIntWrapper<W + 1, is_signed>
operator+(BitIntWrapper<W, is_signed> const lhs,
          BitIntWrapper<W, is_signed> const rhs) {
  return {lhs._val + rhs._val};
}

template <unsigned int W, bool is_signed>
constexpr BitIntWrapper<W + 1, is_signed>
operator-(BitIntWrapper<W, is_signed> const lhs,
          BitIntWrapper<W, is_signed> const rhs) {
  return {lhs._val - rhs._val};
}

#define HINT_EXTINTIMP_BINARY_OP_IMP(SYMBOL)                                   \
  template <unsigned int W, bool is_signed>                                    \
  constexpr BitIntWrapper<W, false> operator SYMBOL(                           \
      BitIntWrapper<W, is_signed> const lhs,                                   \
      BitIntWrapper<W, is_signed> const rhs) {                                 \
    return {lhs._val SYMBOL rhs._val};                                         \
  }

HINT_EXTINTIMP_BINARY_OP_IMP(|)
HINT_EXTINTIMP_BINARY_OP_IMP(&)
HINT_EXTINTIMP_BINARY_OP_IMP(^)

#undef HINT_EXTINTIMP_BINARY_OP_IMP

template <unsigned int W, bool is_signed> class BitIntWrapper {
public:
  typedef BitIntWrapper<W, is_signed> type;
  typedef typename detail::BitIntBaseType<W, is_signed>::type storage_type;
  template <unsigned int N>
  using storage_helper = typename detail::BitIntBaseType<N, is_signed>::type;
  template <unsigned int N>
  using us_storage_helper = typename detail::BitIntBaseType<N, false>::type;
  template <unsigned int N>
  using signed_storage_helper = typename detail::BitIntBaseType<N, true>::type;
  template <unsigned int N> using wrapper_helper = BitIntWrapper<N, is_signed>;
  template <unsigned int N> using us_wrapper_helper = BitIntWrapper<N, false>;
  template <unsigned int N>
  using signed_wrapper_helper = BitIntWrapper<N, true>;
  static constexpr unsigned int width = W;

private:
  storage_type _val;

  template <unsigned int pos>
  static constexpr us_storage_helper<W> one_at_pos() {
    return us_storage_helper<W>(1) << pos;
  }

  template <unsigned int n>
  static constexpr us_storage_helper<W> first_n_set() {
    auto successor = one_at_pos<n>();
    return successor - 1;
  }

  template <unsigned int width>
  static constexpr us_storage_helper<width>
  rec_backward(us_storage_helper<width> in) {
    if constexpr (width == 1) {
      return in;
    } else {
      constexpr auto c2pow = std::bit_ceil(width);
      constexpr auto low_width = c2pow >> 1;
      constexpr auto high_width = width - low_width;
      constexpr auto low_mask = ~us_storage_helper<low_width>{0};
      auto high_mask = us_storage_helper<width>{~us_storage_helper<high_width>{0}}
                                 << low_width;
      auto high = static_cast<us_storage_helper<high_width>>((in & high_mask) >> low_width);
      auto low = static_cast<us_storage_helper<low_width>>(in & low_mask);
      auto rlow = rec_backward<low_width>(low);
      auto rhigh = rec_backward<high_width>(high);
      auto new_high = us_storage_helper<width>{rlow} << high_width;
      us_storage_helper<width> res{new_high | rhigh};
      return res;
    }
  }

public:
  constexpr BitIntWrapper() : _val{0} {}

  constexpr BitIntWrapper(storage_type const val) : _val{val} {}

  /**
   * @brief get the bit at index idx
   *
   * @tparam idx
   * @return BitIntWrapper<1, false>
   */
  template <unsigned int idx> constexpr BitIntWrapper<1, false> get() const {
    static_assert(idx < W, "Checking bit outside of range");
    if constexpr (W == 1) {
      return {_val};
    }
    constexpr auto mask = one_at_pos<idx>();
    return static_cast<us_storage_helper<1>>((mask & _val) >> idx);
  }

  /**
   * @brief Get a slice from the bit vector representing the stored value
   *
   * @tparam high index of the highest bit to keep in the slice
   * @tparam low index of the lowest bit to keep in the slice
   * @return BitIntWrapper<high - low + 1, false>
   */
  template <unsigned int high, unsigned int low>
  constexpr BitIntWrapper<high - low + 1, false> slice() const {
    static_assert(high >= low and high < W,
                  "Trying to slice outside of bounds");
    if constexpr (high == low) {
      return get<low>();
    } else {
      return static_cast<us_storage_helper<high + 1>>(_val) >> low;
    }
  }

  template <unsigned int idx> constexpr bool isSet() const {
    return (get<idx>()._val != us_storage_helper<1>{0});
  }

  constexpr BitIntWrapper<W, false> invert() const {
    return us_storage_helper<W>{~_val};
  }

#define FORWARD_BITWISE_OP(OP, func_name)                                      \
  constexpr BitIntWrapper<W, false> func_name(type const rhs) const {          \
    return {_val OP rhs._val};                                                 \
  }

  FORWARD_BITWISE_OP(&, bitwise_and)
  FORWARD_BITWISE_OP(|, bitwise_or)
  FORWARD_BITWISE_OP(^, bitwise_xor)

#undef FORWARD_BITWISE_OP

#define FORWARD_CMP_OP(OP)                                                     \
  constexpr us_wrapper_helper<1> operator OP(type const rhs) const {           \
    return {_val OP rhs._val};                                                 \
  }

  FORWARD_CMP_OP(<)
  FORWARD_CMP_OP(>)
  FORWARD_CMP_OP(<=)
  FORWARD_CMP_OP(>=)
  FORWARD_CMP_OP(==)
  FORWARD_CMP_OP(!=)

#undef FORWARD_CMP_OP

  constexpr us_wrapper_helper<W> as_unsigned() const {
    return {static_cast<us_storage_helper<W>>(_val)};
  }

  constexpr signed_wrapper_helper<W> as_signed() const {
    return {static_cast<signed_storage_helper<W>>(_val)};
  }

  /**
   * @brief Left pad representation with zeros
   *
   * @tparam newSize the padded output size
   */
  template <unsigned int newSize>
  constexpr wrapper_helper<newSize> leftpad() const {
    static_assert((newSize >= W),
                  "Trying to left pad a value to a size which is smaller than "
                  "actual size. See slice instead.");
    if constexpr (newSize == W) {
      return {_val};
    } else {
      auto us = reinterpret_cast<us_storage_helper<W>>(_val);
      auto extended = static_cast<storage_helper<newSize>>(us);
      return extended;
    }
  }

  /**
   * @brief Right pad representation with zeros
   *
   * @tparam newSize the padded output size
   */
  template <unsigned int newSize>
  constexpr wrapper_helper<newSize> rightpad() const {
    static_assert((newSize >= W),
                  "Trying to right pad a value to a size which is smaller than "
                  "actual size. See slice instead.");
    constexpr auto sizeDiff = newSize - W;
    if constexpr (sizeDiff > 0) {
      auto ret = static_cast<storage_helper<newSize>>(_val);
      return {ret << sizeDiff};
    } else {
      return {_val};
    }
  }

  /**
   * @brief Perform sign extension of the input (equivalent to leftpad for
   * unsigned values)
   *
   * @tparam newSize sign extended output size
   * @return VivadoWrapper<newSize, is_signed>
   */
  template <unsigned int newSize>
  constexpr wrapper_helper<newSize> sign_extend() const {
    static_assert((newSize >= W),
                  "Trying to sign extend a value to a size which is smaller "
                  "than actual size. See slice instead.");
    return {static_cast<storage_helper<newSize>>(_val)};
  }

  /**
   * @brief Return the concatenation of the two integers
   *
   * @param val The value to append at the right of current value
   * @return us_wrapper_helper<W + Wrhs>
   */
  template <unsigned int Wrhs, bool isSignedRhs>
  constexpr us_wrapper_helper<W + Wrhs>
  concatenate(BitIntWrapper<Wrhs, isSignedRhs> const val) const {
    constexpr auto retSize = W + Wrhs;
    auto leftmost = rightpad<retSize>();
    auto rightmost = val.template leftpad<retSize>();
    return {leftmost | rightmost};
  }

  static constexpr us_wrapper_helper<W>
  generateSequence(us_wrapper_helper<1> const val) {
    if constexpr (W == 1) {
      return val;
    } else {
      constexpr us_wrapper_helper<W> zeros{0};
      constexpr us_wrapper_helper<W> ones = zeros.invert();
      us_wrapper_helper<W> ret = (val._val) ? ones : zeros;
      return ret;
    }
  }

  constexpr wrapper_helper<W + 1>
  addWithCarry(wrapper_helper<W> const op2,
               us_wrapper_helper<1> const cin) const {
    // Should be implemented with a adder between op2._val and _val having
    // cin._val plugged in the carry chain entry.
    storage_helper<W + 1> ext{_val}, extop2{op2._val}, extcarry{cin._val};
    if constexpr (false && W == 1 and is_signed) {
      // TODO: remove once signed BitInt(1) is valid
      return {extcarry - extop2 - ext};
    } else {
      return {ext + extop2 + extcarry};
    }
  }

  constexpr wrapper_helper<W + 1>
  subWithCarry(wrapper_helper<W> const op2,
               us_wrapper_helper<1> const cin) const {
    storage_helper<W + 1> ext{_val}, extop2{op2._val}, extcarry{cin._val};
    // No evident clever instantiation of such an op : should be deprecated once
    // a full expression framework is in place
    if constexpr (false && W == 1 and is_signed) {
      // TODO: remove once signed BitInt(1) is valid
      return {extcarry + extop2 - ext};
    } else {
      return {ext - extop2 + extcarry};
    }
  }

  constexpr wrapper_helper<W + 1>
  addWithBorrow(wrapper_helper<W> const op2,
                us_wrapper_helper<1> const bin) const {
    storage_helper<W + 1> ext{_val}, extop2{op2._val}, extborrow{bin._val};
    // No evident clever instantiation of such an op : should be deprecated once
    // a full expression framework is in place
    if constexpr (false && W == 1 and is_signed) {
      // TODO: remove once signed BitInt(1) is valid
      return {extborrow - extop2 - ext};
    } else {
      return {ext + extop2 - extborrow};
    }
  }

  constexpr us_wrapper_helper<W> modularAdd(type const op2) const {
    if constexpr (false && W == 1) {
      return {op2._val != _val};
    } else {
      return static_cast<us_wrapper_helper<W>>(_val + op2._val);
    }
  }

  constexpr us_wrapper_helper<W> modularSub(type const op2) const {
    if constexpr (false && W == 1) {
      return {op2._val != _val};
    } else {
      return {_val - op2._val};
    }
  }

  static constexpr type mux(us_wrapper_helper<1> const control,
                            type const opt1, type const opt0) {
    auto true_val_mask = type::generateSequence(control);
    auto false_val_mask = type::generateSequence(control.invert());
    auto res = (opt1 & true_val_mask) | (opt0 & false_val_mask); 
    return res;
  }

  constexpr us_wrapper_helper<1> or_reduction() const { return {_val != 0}; }

  constexpr us_wrapper_helper<1> nor_reduction() const { return {_val == 0}; }

  constexpr us_wrapper_helper<1> and_reduction() const {
    return {invert()._val == 0};
  }

  constexpr us_wrapper_helper<W> backwards() const {
    return {rec_backward<W>(_val)};
  }

  template <unsigned int W2>
  constexpr wrapper_helper<
      Arithmetic_Prop<W, W2, is_signed, is_signed>::_prodSize>
  operator*(BitIntWrapper<W2, is_signed> const rhs) const {
    return {_val * rhs.unravel()};
  }

  constexpr storage_type const unravel() const { return _val; }

  friend constexpr BitIntWrapper<W + 1, is_signed> operator+
      <W, is_signed>(type const lhs, type const rhs);

  friend constexpr BitIntWrapper<W + 1, is_signed> operator-
      <W, is_signed>(type const lhs, type const rhs);

#define HINT_EXTINT_BINARY_FRIENDOP(SYM)                                       \
  friend constexpr us_wrapper_helper<W> operator SYM<W, is_signed>(            \
      type const lhs, type const rhs);

  HINT_EXTINT_BINARY_FRIENDOP(&)
  HINT_EXTINT_BINARY_FRIENDOP(|)
  HINT_EXTINT_BINARY_FRIENDOP(^)
#undef HINT_EXTINT_BINARY_FRIENDOP

  template <unsigned int ShiftedSize, bool shiftedSigned,
            unsigned int shifterSize>
  friend constexpr BitIntWrapper<ShiftedSize, shiftedSigned>
  operator>>(BitIntWrapper<ShiftedSize, shiftedSigned> const lhs,
             BitIntWrapper<shifterSize, false> const rhs);

  template <unsigned int ShiftedSize, bool shiftedSigned,
            unsigned int shifterSize>
  friend constexpr BitIntWrapper<ShiftedSize, shiftedSigned>
  operator<<(BitIntWrapper<ShiftedSize, shiftedSigned> const lhs,
             BitIntWrapper<shifterSize, false> const rhs);

  template <unsigned int N, bool val> friend class BitIntWrapper;
};

template <> struct Config_Values<BitIntWrapper> {
  constexpr static unsigned int shift_group_by = 2;
};

} // namespace hint
#endif
