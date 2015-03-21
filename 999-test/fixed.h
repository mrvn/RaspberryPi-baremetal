/* Copyright (C) 2015 Goswin von Brederlow <goswin-v-b@web.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/* Fixed point arithmatic
 */

#ifndef KERNEL_FIXEDPOINT_H
#define KERNEL_FIXEDPOINT_H

#include <assert.h>
#include <math.h>
#include <iostream>
#include "templates.h"

template<size_t bits, size_t fraction>
class FixedPoint {
public:
    static constexpr size_t BITS = bits;
    static constexpr size_t FRACTION = fraction;
    using T = Int<bits + fraction>;
    static constexpr T UNIT = T(1) << fraction;
    
    //using TT = Int<2 * bits + 2 * fraction>;
    using SQUARED = FixedPoint<2 * bits, 2 * fraction>;

    template<typename OTHER>
    struct mul_t;

    template<size_t bits_other, size_t fraction_other>
    struct mul_t<FixedPoint<bits_other, fraction_other> > {
        using type = FixedPoint<bits + bits_other, fraction + fraction_other>;
    };

    template<typename OTHER>
    struct div_t;

    template<size_t bits_other, size_t fraction_other>
    struct div_t<FixedPoint<bits_other, fraction_other> > {
        using type =
            FixedPoint<bits + fraction_other, fraction - fraction_other>;
    };

    constexpr FixedPoint(T x, size_t shift) : raw_(x << shift) {
        static_assert(sizeof(T) * 8 == bits + fraction, "storage does not match bits + fraction");
    }
    constexpr FixedPoint(int x = 0) : raw_(T(x) << fraction) {
        static_assert(sizeof(T) * 8 == bits + fraction, "storage does not match bits + fraction");
    }
    explicit constexpr FixedPoint(double x) : raw_(x * (T(1) << fraction)) {
        static_assert(sizeof(T) * 8 == bits + fraction, "storage does not match bits + fraction");
    }
    explicit operator double() const {
        return double(raw_) / (T(1) << fraction);
    }
    explicit operator T() const {
        return raw_ >> fraction;
    }

    T raw() const {
        return raw_;
    }
    
    template<size_t bits_other, size_t fraction_other, size_t left_shift>
    class LeftShift {
    public:
        static constexpr FixedPoint<bits_other, fraction_other>
        shift(const FixedPoint<bits, fraction> v) {
            using FP2 = FixedPoint<bits_other, fraction_other>;
            using T2 = typename FP2::T;
            return FP2(T2(v.raw_) << left_shift, 0);
        }
    };

    template<size_t bits_other, size_t fraction_other, size_t right_shift>
    class RightShift {
    public:
        static constexpr FixedPoint<bits_other, fraction_other>
        shift(const FixedPoint<bits, fraction> v) {
            using FP2 = FixedPoint<bits_other, fraction_other>;
            using T2 = typename FP2::T;
            return FP2(T2(v.raw_ >> right_shift), 0);
        }
    };

    template<size_t bits_other, size_t fraction_other>
    explicit operator FixedPoint<bits_other, fraction_other>() const {
        return conditional<(fraction < fraction_other),
            LeftShift<bits_other, fraction_other, fraction_other - fraction>,
            RightShift<bits_other, fraction_other, fraction - fraction_other> >::type::shift(*this);
    }

    T floor() const {
        return raw_ >> fraction;
    }

    bool operator <(const FixedPoint other) const {
        return raw_ < other.raw_;
    }
    
    bool operator <=(const FixedPoint other) const {
        return raw_ <= other.raw_;
    }
    
    bool operator ==(const FixedPoint other) const {
        return raw_ == other.raw_;
    }
    
    bool operator >=(const FixedPoint other) const {
        return raw_ >= other.raw_;
    }
    
    bool operator >(const FixedPoint other) const {
        return raw_ > other.raw_;
    }
    
    bool operator !=(const FixedPoint other) const {
        return raw_ != other.raw_;
    }
    
    FixedPoint
    operator +(void) const {
        return *this;
    }

    FixedPoint
    operator -(void) const {
        return FixedPoint(-raw_, 0);
    }

    FixedPoint
    operator +(const FixedPoint other) const {
        return FixedPoint(raw_ + other.raw_, 0);
    }

    FixedPoint
    operator +=(const FixedPoint other) {
        raw_ += other.raw_;
        return *this;
    }

    FixedPoint
    operator -(const FixedPoint other) const {
        return FixedPoint(raw_ - other.raw_, 0);
    }

    FixedPoint
    operator -=(const FixedPoint other) {
        raw_ -= other.raw_;
        return *this;
    }

    FixedPoint
    operator *(const T scalar) const {
        return FixedPoint(raw_ * scalar, 0);
    }

    FixedPoint
    operator *=(const T scalar) {
        raw_ *= scalar;
        return *this;
    }

    FixedPoint
    operator /(const T scalar) const {
        return FixedPoint(raw_ / scalar, 0);
    }

    FixedPoint
    operator /=(const T scalar) {
        raw_ /= scalar;
        return *this;
    }

    template<size_t bits_other, size_t fraction_other>
    FixedPoint<bits + bits_other, fraction + fraction_other>
    operator *(const FixedPoint<bits_other, fraction_other> other) const {
        constexpr size_t bits2 = bits + bits_other;
        constexpr size_t fraction2 = fraction + fraction_other;
        Int<bits2 + fraction2> t = raw_;
        t *= other.raw_;
        return FixedPoint<bits2, fraction2>(t, 0);
    }

    template<size_t bits_other, size_t fraction_other>
    FixedPoint<bits + fraction_other, fraction - fraction_other>
    operator /(const FixedPoint<bits_other, fraction_other> other) const {
        static_assert(fraction >= fraction_other,
                      "resulting fraction can't be negative");
        constexpr size_t bits2 = bits + fraction_other;
        constexpr size_t fraction2 = fraction - fraction_other;
        Int<bits2 + fraction2> t = raw_;
        t /= other.raw_;
        return FixedPoint<bits2, fraction2>(t, 0);
    }

    FixedPoint<bits / 2 + 1, fraction + (bits - bits / 2 - 1)>
    sqrt() const {
        const size_t bits2 = bits / 2 + 1;
        const size_t fraction2 = fraction + bits - bits2;
        assert(raw_ >= 0);

        //std::cerr << "sqrt(" << *this << ") [" << raw_ << "/" << (T(1) << fraction) << "]\n";
        // do the integer part first
        T res = 0;
        T res2 = 0;
        ssize_t x = ((bits - 1) / 2); // -1 because fraction might be 0
        T rem = raw_ >> fraction; // integer part
        if (rem >= (T(1) << (2 * x))) {
            // we can't start with 1 << (bits / 2) because x2 might overflow
            // this catches the case that the result is >= 2 * x
            res = bit(x);
            res2 += bit2(x);
            rem -= bit2(x);
        }
        while ((rem > 0) && (x >= 0)) {
            // t = 2 * res * 2^x + x2
            T t = (res << (x + 1)) + bit2(x);
            /*
            std::cerr << "res = " << res
                      << ", res2 = " << res2
                      << ", x = 2^" << x
                      << ", rem = " << rem
                      << ", res * x * 2 + x2 = " << t
                      << std::endl;
            */
            if (rem >= t) {
                res += bit(x);
                rem -= t;
                res2 += t;
            }
            x -= 1;
        }

        // std::cerr << "integer sqrt(" << *this << ") = " << res << std::endl;
        
        // continue with the fractional part (using fraction2 as shift)
        res <<= fraction2;
        rem = (raw_ - (res2 << fraction)) << (fraction2 - fraction);
        x = fraction2 - 1;
        T x2 = bit(fraction2) / 4;

        //std::cerr << "base = " << (T(1) << fraction2) << std::endl;
        while ((rem > 0) && (x >= 0)) {
            // t = ((2 * res * 2^x) >> fraction2) + x2
            T t = ((res + x2) >> (fraction2 - x - 1));
            /*
            std::cerr << "res = " << res
                      << ", x = 2^" << x
                      << ", x2 = " << x2
                      << ", rem = " << rem
                      << ", res * x * 2 + x2 = " << t
                      << std::endl;
            */
            if (rem >= t) {
                rem -= t;
                res += bit(x);
            }
            x -= 1;
            x2 /= 2; // should be /= 4, this corrects for the shift in
                     // calculating t
        }
        return FixedPoint<bits2, fraction2>(res, 0);
    }
    
    friend std::ostream & operator <<(std::ostream &out,
                                      const FixedPoint<bits, fraction> &x) {
        out << double(x);
        return out;
    }

    template<size_t bits_other, size_t fraction_other>
    friend class FixedPoint;
private:
    constexpr T bit(size_t n) {
        return T(1) << n;
    }

    constexpr T bit2(size_t n) {
        return bit(n * 2);
    }

    T raw_;
};

#endif // #ifndef KERNEL_FIXEDPOINT_H
