#ifndef BITINT_TOOLS_TYPE_HELPERS_HPP
#define BITINT_TOOLS_TYPE_HELPERS_HPP

#include <boost/hana.hpp>

namespace hint {
namespace detail {

namespace hana = boost::hana;

template <unsigned int W, bool is_signed> struct BitIntBaseType {};

template <unsigned int W> struct BitIntBaseType<W, true> {
  using type = _BitInt(W);
};

template <unsigned int W> struct BitIntBaseType<W, false> {
  using type = unsigned _BitInt(W);
};

template <bool isSigned, unsigned int Width>
using bitint_base_t = typename BitIntBaseType<Width, isSigned>::type;

template <typename T> constexpr bool is_signed_bitint = false;

template <unsigned int W> constexpr bool is_signed_bitint<_BitInt(W)> = true;

template <typename T> constexpr bool is_unsigned_bitint = false;

template <unsigned int W>
constexpr bool is_unsigned_bitint<unsigned _BitInt(W)> = true;

} // namespace detail

template <typename T>
concept SignedBitIntT = detail::is_signed_bitint<T>;

template <typename T>
concept UnsignedBitIntT = detail::is_unsigned_bitint<T>;

template <typename T>
concept BitIntT = SignedBitIntT<T> || UnsignedBitIntT<T>;

template <typename T> constexpr bool signedType = false;

template <SignedBitIntT T> constexpr bool signedType<T> = true;
} // namespace hint
#endif
