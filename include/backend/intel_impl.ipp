#ifndef INTEL_IMPL_IPP
#define INTEL_IMPL_IPP

#include <type_traits>
#include <cstdint>

#ifdef __INTELFPGA_COMPILER__
#include "HLS/ac_int.h"
#else
#include "ref/ac_int.h"
#endif
using namespace std;

#include "tools/static_math.hpp"

template <unsigned int W, bool is_signed>
class IntelWrapper;

template<unsigned int W, bool is_signed>
IntelWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*(
		IntelWrapper<W, is_signed> const & lhs,
		IntelWrapper<W, is_signed> const & rhs
	)
{
	return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) *
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
}

template<unsigned int shiftedSize, bool isShiftedSigned, unsigned int shifterSize>
IntelWrapper<shiftedSize, isShiftedSigned> operator>>(
		IntelWrapper<shiftedSize, isShiftedSigned> const & lhs,
		IntelWrapper<shifterSize, false> const & rhs
		) {
	return static_cast<typename IntelWrapper<shiftedSize, isShiftedSigned>::storage_type const &>(lhs) >>
			static_cast<typename IntelWrapper<shifterSize, false>::storage_type const &>(rhs);
}

template<unsigned int W, bool is_signed>
IntelWrapper<W+1, is_signed> operator+(
		IntelWrapper<W, is_signed> const & lhs,
		IntelWrapper<W, is_signed> const & rhs
	)
{
	return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) +
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
}

template<unsigned int W, bool is_signed>
IntelWrapper<W, false> operator&(
		IntelWrapper<W, is_signed> const & lhs,
		IntelWrapper<W, is_signed> const & rhs
	)
{
	return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) &
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
}

template<unsigned int W, bool is_signed>
IntelWrapper<W, false> operator|(
		IntelWrapper<W, is_signed> const & lhs,
		IntelWrapper<W, is_signed> const & rhs
	)
{
	return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) |
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
}

template<unsigned int W, bool is_signed>
IntelWrapper<W, false> operator^(
		IntelWrapper<W, is_signed> const & lhs,
		IntelWrapper<W, is_signed> const & rhs
	)
{
	return	static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(lhs) ^
			static_cast<typename IntelWrapper<W, is_signed>::storage_type const &>(rhs);
}


template <unsigned int W, bool is_signed>
class IntelWrapper : private ac_int<W, is_signed>
{
public:
	typedef IntelWrapper<W, is_signed> type;
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
    inline IntelWrapper<high - low + 1, false> slice(
        typename enable_if<high >= low and high < W>::type* = 0
    ) const
    {
        return us_storage_helper<high-low+1>{
            storage_type::template slc<high - low + 1>(low)
        };
    }

    template<unsigned int idx>
    inline IntelWrapper<1, false> get(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
       return us_storage_helper<1>{
              storage_type::template slc<1>(idx)
        };
    }

	inline IntelWrapper<1, false> operator>(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator>(rhs)};
		return ret;
	}

	inline IntelWrapper<1, false> operator>=(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator>=(rhs)};
		return ret;
	}

	inline IntelWrapper<1, false> operator<=(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator<=(rhs)};
		return ret;
	}

	inline IntelWrapper<1, false> operator<(type const & rhs) const
	{
		us_wrapper_helper<1> ret{storage_type::operator<(rhs)};
		return ret;
	}

    template<unsigned int idx>
    inline bool isSet(
       typename enable_if<idx < W>::type* = 0
    ) const
    {
        // auto& this_ac = static_cast<storage_type const &>(*this);
        return ((*this).template slc<1>(idx) == 1);
    }

    inline IntelWrapper<W, false> bitwise_and(IntelWrapper<W, is_signed> rhs) const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        // auto& rhs_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<W>{(*this) & rhs};
    }

    inline IntelWrapper<W, false> bitwise_or(IntelWrapper<W, is_signed> rhs) const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        // auto& rhs_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<W>{(*this) | rhs};
    }

    inline IntelWrapper<W, false> bitwise_xor(IntelWrapper<W, is_signed> rhs) const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        // auto& rhs_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<W>{(*this) ^ rhs};
    }

    inline IntelWrapper<W, false> invert() const
    {
        return us_storage_helper<W>{~(*this)};
    }

    template<unsigned int newSize>
    inline IntelWrapper<newSize, false> leftpad(
            typename enable_if<(newSize >= W)>::type* = 0
            ) const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        us_storage_helper<W> unsigned_this = (*this);
        us_storage_helper<newSize> ret = unsigned_this;
        return ret;
    }

    template<unsigned int Wrhs, bool isSignedRhs>
    inline IntelWrapper<W + Wrhs, false>
    concatenate(IntelWrapper<Wrhs, isSignedRhs> const val) const
    {
        ac_int<W + Wrhs, false> res;
		res.set_slc(0, val);
		res.set_slc(Wrhs, *this);

        return res;
    }

	uint64_t to_uint(typename enable_if<(W <= numeric_limits<uint64_t>::digits)>::type * = 0 ) const
	{
		return storage_type::to_uint64();
	}

    inline IntelWrapper<1, false> operator==(IntelWrapper<W, is_signed> const rhs) const {
        return us_storage_helper<1>{(storage_type::operator==(rhs))};
    }

    inline IntelWrapper<W, is_signed>& operator=(IntelWrapper const rhs)
    {
        // auto& this_ap = static_cast<storage_type&>(*this);
        // auto& rhs_ap = static_cast<storage_type const &>(rhs);
        storage_type::operator=(rhs);
        return *this;
    }

    static inline IntelWrapper<W, false> generateSequence(IntelWrapper<1, false> const val)
    {
        // auto& to_fill = static_cast<storage_type const &>(val);
        us_storage_helper<W> zero{0};
        us_storage_helper<W> ret;
        if(val == 1){
            ret = us_storage_helper<W>{~zero};
        } 
        else{
            ret = zero;
        }
        return ret;
    }

    inline IntelWrapper<W+1, is_signed> addWithCarry(
            IntelWrapper<W, is_signed> const op2,
            IntelWrapper<1, false> const cin
        ) const
    {
        // auto& op1_ap = static_cast<storage_type const &>(*this);
        // auto& op2_ap = static_cast<storage_type const &>(op2);
        // auto& cin_ap = static_cast<us_storage_helper<1> const &>(cin);
        auto res = storage_type::operator+(op2) + cin;
        return storage_helper<W+1>{res};
    }

	inline IntelWrapper<W+1, is_signed> subWithCarry(
			IntelWrapper<W, is_signed> const op2,
			IntelWrapper<1, false> const cin
		) const
	{
		// auto& op1_ap = static_cast<storage_type const &>(*this);
		// auto& op2_ap = static_cast<storage_type const &>(op2);
		// auto& cin_ap = static_cast<us_storage_helper<1> const &>(cin);
		auto res = storage_type::operator-(op2) + cin;
		return storage_helper<W+1>{res};
	}

	inline IntelWrapper<W+1, is_signed> addWithBorrow(
			IntelWrapper<W, is_signed> const op2,
			IntelWrapper<1, false> const bin
		) const
	{
		auto res = storage_type::operator+(op2) - bin;
		return storage_helper<W+1>{res};
	}

	inline IntelWrapper<W, false> modularAdd(IntelWrapper<W, is_signed> const op2) const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        // auto& op_2 = static_cast<storage_type const &>(op2);
        return us_storage_helper<W>{(*this) + op2};
    }

	inline IntelWrapper<W, false> modularSub(IntelWrapper<W, is_signed> const op2) const
	{
		// auto& this_ap = static_cast<storage_type const &>(*this);
		// auto& op_2 = static_cast<storage_type const &>(op2);
		return us_storage_helper<W>{(*this) - op2};
	}

    static inline IntelWrapper<W, is_signed> mux(
            IntelWrapper<1, false> const control,
            IntelWrapper<W, is_signed> const opt1,
            IntelWrapper<W, is_signed> const opt0
        )
    {
        // auto& ac_control = static_cast<us_storage_helper<1> const &>(control);
        // return (control) ? opt1 : opt0;
        ac_int<W, is_signed> result;
        if(control==1){
            result = opt1;
        }
        else{
            result = opt0;
        }
        return result;
    }

    inline us_wrapper_helper<W> as_unsigned() const
    {
    	return us_storage_helper<W>{static_cast<storage_type const &>(*this)};
    }

    inline us_wrapper_helper<1> or_reduction() const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<1>{(*this).or_reduce()};
    }

    inline us_wrapper_helper<1> and_reduction() const
    {
        // auto& this_ap = static_cast<storage_type const &>(*this);
        return us_storage_helper<1>{(*this).and_reduce()};
    }

    inline storage_type const & unravel() const
    {
        return (*this);
    }

    inline us_wrapper_helper<W> backwards() const
    {
        // auto& this_ac = static_cast<storage_type const &>(*this);
        us_storage_helper<W> out;
		#pragma unroll W
        for(unsigned int i = 0 ; i < W ; ++i) {
            out.template set_slc(i, (*this).template slc<1>(W - i - 1));
        }
        return out;
    }

	friend
	IntelWrapper<Arithmetic_Prop<W, W>::_prodSize, is_signed> operator*<W, is_signed>(
			type const & lhs,
			type const & rhs
		);
	friend
	IntelWrapper<W+1, is_signed> operator+<W, is_signed>(
			type const & lhs,
			type const & rhs
		);

	friend
	IntelWrapper<W, false> operator|<W, is_signed>(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		);

	friend
	IntelWrapper<W, false> operator&<W, is_signed>(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		);

	friend
	IntelWrapper<W, false> operator^<W, is_signed>(
			IntelWrapper<W, is_signed> const & lhs,
			IntelWrapper<W, is_signed> const & rhs
		);

	template<unsigned int ShiftSize>
	friend IntelWrapper<W, is_signed> operator>>(
			type const & lhs,
			us_wrapper_helper<ShiftSize> const & rhs
			);

	template<unsigned int ShiftedSize, bool isSignedShifted>
	friend IntelWrapper<ShiftedSize, isSignedShifted> operator>>(
			IntelWrapper<ShiftedSize, isSignedShifted> & lhs,
			us_wrapper_helper<W> const & rhs
		);

    template<unsigned int N, bool val>
    friend class IntelWrapper;
};

#endif
