#ifndef STATIC_MATH_HPP
#define STATIC_MATH_HPP

#include <cstdint>

template <int num, int denom>
struct Static_Ceil_Div{
        static constexpr int val = (num % denom) ? (num / denom) + 1 : num / denom;
};

constexpr int ceilLog2(int N, uint8_t remains = 0)
{
        return (N <= 1) ? remains : 1 + ceilLog2(N>>1, remains | (N%2));
}

constexpr int ceil2Power(int N)
{
        return 1 << ceilLog2(N);
}

constexpr int log2(int N)
{
         return ((N<2) ? 1 : 1+log2(N>>1));
}

constexpr int r2pow(int N)
{
         return 1 << log2(N);
}

constexpr int floorLog2(int N)
{
    return (N<= 1) ? 0 : 1 + floorLog2(N>>1);
}



template<int N>
class Static_Val
{
        public:
                static constexpr int _rlog2 = log2(N);
                static constexpr int _r2pow = r2pow(N);
                static constexpr int _log2 = ceilLog2(N);
                static constexpr int _2pow = ceil2Power(N);
                static constexpr int _flog2 = floorLog2(N);
                static constexpr int _clog2 = ceilLog2(N);
				static constexpr int _storage = ceilLog2(N+1);
};

template<int N>
constexpr bool is2Pow()
{
    return (1 << Static_Val<N>::_clog2) == N;
}


template<int N>
constexpr bool isOneBelow2Pow()
{
    return (((1 << Static_Val<N>::_clog2 ) - 1) == N);
}


#endif
