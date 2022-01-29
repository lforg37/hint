#ifndef EXTINT_TOOLS_TYPE_HELPERS_HPP
#define EXTINT_TOOLS_TYPE_HELPERS_HPP
namespace hint {
namespace detail {
template <unsigned int W, bool is_signed> struct ExtIntBaseType {};

template <unsigned int W> struct ExtIntBaseType<W, true> {
  using type = _ExtInt(W);
};

template <unsigned int W> struct ExtIntBaseType<W, false> {
  using type = unsigned _ExtInt(W);
};

template <unsigned int W, bool isSigned>
using extint_base_t = typename ExtIntBaseType<W,isSigned>::type;

} // namespace detail
} // namespace hint
#endif
