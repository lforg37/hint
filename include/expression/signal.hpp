#ifndef EXPRESSION_SIGNAL_HPP
#define EXPRESSION_SIGNAL_HPP

#include <concepts>
#include <type_traits>

namespace hint {

namespace detail {
struct SignalMarker {};
}

template <bool IsSigned, unsigned int Width, int LsbWeight,
          unsigned int Delay = 0>
struct Signal : public detail::SignalMarker {
  static constexpr int msbWeight = LsbWeight + Width - 1;
  static constexpr int lsbWeight = LsbWeight;
  static constexpr unsigned int width = Width;
  static constexpr bool emptySignal = Width == 0;
  static constexpr bool isSigned = IsSigned;
  static constexpr unsigned int delay = Delay;
};

template<typename T>
concept SignalType = std::is_base_of_v<detail::SignalMarker, T>;

/*
// TODO: if c++20 use constraints to restrain types to signals
template <typename... SignalTypes> struct SignalCollectionsProperties;

template <bool IsSigned, unsigned int Width, int LsbWeight, unsigned int Delay>
struct SignalCollectionsProperties<Signal<IsSigned, Width, LsbWeight, Delay>> {
  using signal_t = Signal<IsSigned, Width, LsbWeight, Delay>;
  static constexpr int lsbWeight = LsbWeight;
  static constexpr int msbWeight = signal_t::msbWeight;
  static constexpr unsigned int width = Width;
  static constexpr bool one_signed = IsSigned;
  static constexpr unsigned int minDelay = Delay;
  static constexpr unsigned int maxDelay = Delay;
};

template <bool IsSigned, unsigned int Width, int LsbWeight, unsigned int Delay,
          typename... SignalTypes>
struct SignalCollectionsProperties<Signal<IsSigned, Width, LsbWeight, Delay>,
                                   SignalTypes...> {
  using subsignal_t = SignalCollectionsProperties<SignalTypes...>;
  static constexpr int lsbWeight = gcem::min(LsbWeight, subsignal_t::lsbWeight);
  static constexpr int msbWeight_unsigned =
      gcem::max(Width + LsbWeight - 1, subsignal_t::msbWeight);
  static constexpr int msbWeight =
      msbWeight + static_cast<int>(IsSigned && !subsignal_t::one_signed);
  static constexpr unsigned int width = msbWeight - LsbWeight + 1;
  static constexpr bool one_signed = IsSigned || subsignal_t::one_signed;
  static constexpr unsigned int minDelay = gcem::min(Delay, subsignal_t::minDelay);
  static constexpr unsigned int maxDelay = gcem::max(Delay, subsignal_t::maxDelay);
};
*/

} // namespace hint

#endif
