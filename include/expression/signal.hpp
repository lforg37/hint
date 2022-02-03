#ifndef EXPRESSION_SIGNAL_HPP
#define EXPRESSION_SIGNAL_HPP

#include <concepts>
#include <type_traits>

namespace hint {

template<typename T>
concept SignalType = requires {
  {T::width} -> std::same_as<unsigned int>;
};

template<SignalType ST, unsigned int TargetWidth>
constexpr bool widthsMatch = ST::width == TargetWidth;

template<typename T>
concept DelayedSignalType = requires {
  SignalType<T>;
  {T::delay}->std::same_as<unsigned int>;
};

template<unsigned int Delay, SignalType ST>
struct DelayedSignal : public ST {
  static constexpr unsigned int delay = Delay;
};

} // namespace hint

#endif
