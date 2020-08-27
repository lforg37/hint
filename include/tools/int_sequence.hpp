#ifndef INT_SEQUENCE_HPP
#define INT_SEQUENCE_HPP

template <unsigned int VAL>
struct STToType
{
        static constexpr unsigned int val = VAL;
        using type = STToType;
};

template<class T> using call = typename T::type;

template <typename... elems>
struct Sequence {
        using type = Sequence;
};

template <unsigned int shiftSize, typename Val>
struct Shifter;

template <unsigned int shiftSize, unsigned int Val>
struct Shifter<shiftSize, STToType<Val>>
{
        using type = STToType<Val + shiftSize>;
};

template<unsigned int shiftSize, typename... elems>
struct Shifter<shiftSize, Sequence<elems...> >
{
        using type = Sequence<call<Shifter<shiftSize, elems>>...>;
};

template<unsigned int shiftSize>
struct Shifter<shiftSize, Sequence<>>
{
        using type = Sequence<>;
};

template<typename T1, typename T2> struct Merger;
template<typename... elS1, typename... elS2>
struct Merger<Sequence<elS1...>, Sequence<elS2...>> {
        using type = Sequence<elS1..., elS2...>;
};

template <typename>
struct Unfold;

template<unsigned int shift, typename T1, typename T2>
struct shiftConcat;

template<unsigned int shift, typename... elS1, typename... elS2>
struct shiftConcat<shift, Sequence<elS1...>, Sequence<elS2...>>
{
        using shiftS2 = call<Shifter<sizeof...(elS1), Sequence<elS2...>>>;
        using type = call<Merger<Sequence<elS1...>, shiftS2>>;
};


template <typename>
struct GenSeqElem;

template <>
struct GenSeqElem<STToType<0>> : Sequence<>{};
template <>
struct GenSeqElem<STToType<1>> : Sequence<STToType<0>>{};

template <unsigned int val> using GenSeqCall = call<GenSeqElem<STToType<val>>>;

template <unsigned int NElem>
struct GenSeqElem<STToType<NElem>> : shiftConcat<NElem/2, GenSeqCall<NElem/2>, GenSeqCall<NElem-NElem/2>>{};

template <typename>
struct UnWrapper;

template <unsigned int...>
struct UISequence{};

template <unsigned int... values>
struct UnWrapper<Sequence<STToType<values>...>> {
        using type = UISequence<values...>;
};

template <typename T>
struct ReverseSeq;

template <unsigned int... values>
struct ReverseSeq<UISequence<values...>>
{
    using type = UISequence<(sizeof... (values)-1-values)...>;
};

template <unsigned int NElem>
using make_sequence = call<UnWrapper<GenSeqCall<NElem>>>;

template <unsigned int NElem>
using make_reverse_sequence = call<ReverseSeq<make_sequence<NElem>>>;

#endif // INT_SEQUENCE_HPP