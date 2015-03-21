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

/* 3D vector arithmatic
 */

#ifndef KERNEL_VECTOR_H
#define KERNEL_VECTOR_H

#include <iostream>

template<typename T>
class Vector {
public:
    constexpr Vector(T x = T(0.0), T y = T(0.0), T z = T(0.0))
        : x_(x), y_(y), z_(z) { }

    friend std::ostream & operator <<(std::ostream &out,
                                      const Vector &v) {
        out << "<" << v.x_ << ", " << v.y_ << ", " << v.z_ << ">";
        return out;
    }

    Vector operator +(const Vector &other) const {
        return Vector(x_ + other.x_, y_ + other.y_, z_ + other.z_);
    }

    Vector operator +=(const Vector &other) {
        x_ += other.x_;
        y_ += other.y_;
        z_ += other.z_;
        return *this;
    }

    Vector operator -(const Vector &other) const {
        return Vector(x_ - other.x_, y_ - other.y_, z_ - other.z_);
    }

    Vector operator -=(const Vector &other) {
        x_ -= other.x_;
        y_ -= other.y_;
        z_ -= other.z_;
        return *this;
    }

    Vector operator *(int scalar) const {
        return Vector(x_ * scalar, y_ * scalar, z_ * scalar);
    }

    template<typename SCALAR>
    Vector<typename T::template mul_t<SCALAR>::type>
    operator *(const SCALAR &scalar) const {
        using TT = typename T::template mul_t<SCALAR>::type;
        return Vector<TT>(x_ * scalar, y_ * scalar, z_ * scalar);
    }

    Vector operator /(int scalar) const {
        return Vector(x_ / scalar, y_ / scalar, z_ / scalar);
    }

    template<typename SCALAR>
    Vector<typename T::template div_t<SCALAR>::type>
    operator /(const SCALAR &scalar) const {
        using TT = typename T::template div_t<SCALAR>::type;
        return Vector<TT>(x_ / scalar, y_ / scalar, z_ / scalar);
    }

    template<typename OTHER>
    typename T::template mul_t<OTHER>::type
    operator *(const Vector<OTHER> &other) const {
        return x_ * other.x_ + y_ * other.y_ + z_ * other.z_;
    }

    template<typename OTHER>
    Vector<typename T::template mul_t<OTHER>::type>
    operator ^(const Vector<OTHER> &other) const {
        using TT = typename T::template mul_t<OTHER>::type;
        return Vector<TT>(y_ * other.z_ - z_ * other.y_,
                          z_ * other.x_ - x_ * other.z_,
                          x_ * other.y_ - y_ * other.x_);
    }
    T x() const { return x_; }
    T y() const { return y_; }
    T z() const { return z_; }

    template<typename T2>
    explicit operator Vector<T2>() const {
        return Vector<T2>(T2(x_), T2(y_), T2(z_));
    }

    Vector<T> normalized() const {
        // T  =  6.10
        // d2 = 12.20
        // d  =  7.25 -> 16.16
        // v  =  6.26
        // v2 =  6.26 / 16.16 = 22.10
        // ret   6.10

        // T  =  8.24
        // d2 = 16.48
        // d  =  9.55 -> 32.32
        // v  =  8.56
        // v2 =  8.56 / 32.32 = 40.24
        // ret   8.24
        static constexpr size_t half = T::BITS + T::FRACTION;
        auto d2 = x_ * x_ + y_ * y_ + z_ * z_;
        FixedPoint<half, half> d(d2.sqrt());
        Vector<FixedPoint<T::BITS, T::FRACTION + half> > v(*this);
        Vector<FixedPoint<T::BITS + half, T::FRACTION> > v2 = v / d;
        return Vector<T>(v2);
    }

    typename T::SQUARED length() const {
        return x_ * x_ + y_ * y_ + z_ * z_;
    }

    template<typename T_other>
    friend class Vector;
private:
    T x_, y_, z_;
};

#endif // #ifndef KERNEL_VECTOR_H
