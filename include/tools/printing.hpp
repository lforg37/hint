#ifndef PRINTING_HPP
#define PRINTING_HPP

#include <sstream>
#include <string>
#include <type_traits>

using namespace std;
#include "hint.hpp"
namespace hint {
    template <typename val, unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
    struct printer{
        inline void operator()(stringstream & s, wrapper<W, is_signed> const & signal)
        {
            s << (signal.template isSet<val::value>() ? '1' : '0');
            printer<integral_constant<unsigned int, val::value-1>, W, is_signed, wrapper>{}(s, signal);
        }
    };

    template <unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
    struct printer<integral_constant<unsigned int, 0>, W, is_signed, wrapper>
    {
        inline void operator()(stringstream& s, wrapper<W, is_signed> const & signal)
        {
            s << (signal.template isSet<0>() ? '1' : '0');
        }
    };


    template <unsigned int W, bool is_signed, template<unsigned int, bool> class wrapper>
    string to_string(wrapper<W, is_signed> const & signal)
    {
        stringstream s{};
        printer<integral_constant<unsigned int, W-1>, W, is_signed, wrapper>{}(s, signal);
        return s.str();
    }
}

#endif // PRINTING_HPP
