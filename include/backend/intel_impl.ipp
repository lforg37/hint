#ifndef INTEL_IMPL_IPP
#define INTEL_IMPL_IPP

#include <type_traits>

#ifdef __INTELFPGA_COMPILER__
#include "HLS/ac_int.h"
#else
#include "ref/ac_int.h"
#endif
using namespace std;

template <unsigned int W, bool is_signed>
class IntelWrapper : private ac_int<W, is_signed>
{
public:
    typedef IntelWrapper<W, true> type;
    typedef ac_int<W, is_signed> storage_type;
    template<unsigned int N>
    using storage_helper = ac_int<N, is_signed>;
    template<unsigned int N>
    using us_storage_helper = ac_int<N, false>;
    template<unsigned int N>
    using wrapper_helper = IntelWrapper<N, is_signed>;
    template<unsigned int N>
    using us_wrapper_helper = IntelWrapper<N, false>;

    IntelWrapper():storage_type{0}{}


    IntelWrapper(storage_type const & val):storage_type{val}{
    }

    template<unsigned int high, unsigned int low>
    IntelWrapper<high - low + 1, false> slice(
        typename enable_if<high >= low and high < W>::type* = 0
    ) const
    {
        return us_storage_helper<high-low+1>{
            storage_type::template slc<high - low + 1>(low)
        };
    }

    template<unsigned int idx>
    IntelWrapper<1, false> get(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
       return us_storage_helper<1>{
              storage_type::template slc<1>(idx)
        };
    }

    template<unsigned int idx>
    bool isSet(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        auto& this_ac = static_cast<storage_type const &>(*this);
        return (this_ac.template slc<1>(idx) == 1);
    }

    IntelWrapper<W, false> bitwise_and(IntelWrapper<W, is_signed> rhs) const
    {
        auto& this_ap = static_cast<storage_type const &>(*this);
        auto& rhs_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<W>{this_ap & rhs};
    }

    IntelWrapper<W, false> bitwise_or(IntelWrapper<W, is_signed> rhs) const
    {
        auto& this_ap = static_cast<storage_type const &>(*this);
        auto& rhs_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<W>{this_ap | rhs};
    }

    template<unsigned int newSize>
    IntelWrapper<newSize, false> leftpad(
            typename enable_if<(newSize >= W)>::type* = 0
            ) const
    {
        auto& this_ap = static_cast<storage_type const &>(*this);
        us_storage_helper<W> unsigned_this = this_ap;
        us_storage_helper<newSize> ret = unsigned_this;
        return ret;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    IntelWrapper<W + Wrhs, false>
    concatenate(IntelWrapper<Wrhs, isSignedRhs> const & val) const
    {
        auto& this_ac = static_cast<storage_type const &>(*this);
        auto & low_ac = static_cast<ac_int<Wrhs, isSignedRhs> const &>(val);
        us_storage_helper<Wrhs> unsigned_low_ac = low_ac;
        us_storage_helper<W> unsigned_upper_ac = this_ac;

        ac_int<W + Wrhs, false> res;
        res.template set_slc(0, unsigned_low_ac);
        res.template set_slc(Wrhs, unsigned_upper_ac);

        return res;
    }

    IntelWrapper<1, false> operator==(IntelWrapper<W, is_signed> const & rhs) const {
        return us_storage_helper<1>{(static_cast<storage_type const &>(*this) == static_cast<storage_type const &>(rhs))};
    }

    IntelWrapper<W, is_signed>& operator=(IntelWrapper const & rhs)
    {
        auto& this_ap = static_cast<storage_type&>(*this);
        auto& rhs_ap = static_cast<storage_type const &>(rhs);
        this_ap = rhs_ap;
        return *this;
    }

    static IntelWrapper<W, false> generateSequence(IntelWrapper<1, false> const & val)
    {
        auto& to_fill = static_cast<storage_type const &>(val);
        us_storage_helper<W> zero{0};
        us_storage_helper<W> ret = (to_fill==1) ? us_storage_helper<W>{~zero} : zero;
        return ret;
    }

    IntelWrapper<W+1, is_signed> addWithCarry(
            IntelWrapper<W, is_signed> const & op2,
            IntelWrapper<1, false> const & cin
        ) const
    {
        auto& op1_ap = static_cast<storage_type const &>(*this);
        auto& op2_ap = static_cast<storage_type const &>(op2);
        auto& cin_ap = static_cast<us_storage_helper<1> const &>(cin);
        auto res = op1_ap + op2_ap + cin_ap;
        return storage_helper<W+1>{res};
    }

    IntelWrapper<W, false> modularAdd(IntelWrapper<W, is_signed> const & op2) const
    {
        auto& this_ap = static_cast<storage_type const &>(*this);
        auto& op_2 = static_cast<storage_type const &>(op2);
        return us_storage_helper<W>{this_ap + op_2};
    }

    static IntelWrapper<W, is_signed> mux(
            IntelWrapper<1, false> const & control,
            IntelWrapper<W, is_signed> const & opt1,
            IntelWrapper<W, is_signed> const & opt0
        )
    {
        auto& ac_control = static_cast<us_storage_helper<1> const &>(control);
        return (control) ? opt1 : opt0;
    }

    us_wrapper_helper<W> as_unsigned() const
    {
        us_storage_helper<W> val = static_cast<storage_type const &>(*this);
        return val;
    }

    us_wrapper_helper<1> or_reduction() const
    {
        auto& this_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<1>{this_ap.or_reduce()};
    }

    us_wrapper_helper<1> and_reduction() const
    {
        auto& this_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<1>{this_ap.and_reduce()};
    }

    storage_type const & unravel() const
    {
        return static_cast<storage_type const &>(*this);
    }

    us_wrapper_helper<W> reverse() const
    {
        auto& this_ac = static_cast<storage_type const &>(*this);
        us_storage_helper<W> out;
        for(unsigned int i = 0 ; i < W ; ++i) {
            out.template set_slc(i, this_ac.template slc<1>(W - i - 1));
        }
        return out;
    }


    template<unsigned int N, bool val>
    friend class IntelWrapper;
};


#endif
