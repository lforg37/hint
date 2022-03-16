#ifndef BITINT_TOOLS_TYPE_HELPERS_HPP
#define BITINT_TOOLS_TYPE_HELPERS_HPP

#include <algorithm>
#include <string_view>

namespace hint {
namespace detail {

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

namespace detail {
template<char... Bits>
struct BitIntUDLHelper{
  static constexpr char tab[]{Bits..., '\0'};
  static constexpr std::string_view prefixed_bits{tab};
  static_assert(prefixed_bits.starts_with("0b") || prefixed_bits.starts_with("0B"), "Only binary litterals are accepted");
  static constexpr auto bits = prefixed_bits.substr(2);
  static constexpr auto nb_sep = std::count(bits.begin(), bits.end(), '\'');
  static constexpr auto width = bits.size() - nb_sep;

  template<bool isSigned>
  static constexpr bitint_base_t<isSigned, width> get_value() {
    bitint_base_t<isSigned, width> result{0};
    for (auto c : bits) {
      if (c == '\'') continue;
      result <<= 1;
      if (c == '1') result += 1;
    }
    return result;
  };
};
}

template<char... Bits>
constexpr auto operator ""_ubi() { //Unsigned bit int
  return detail::BitIntUDLHelper<Bits...>::template get_value<false>();
}

template<char... Bits>
constexpr auto operator ""_sbi() { //Signed bit int
  return detail::BitIntUDLHelper<Bits...>::template get_value<true>();
}

} // namespace hint
#endif
