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

/* 3D matrix arithmatic
 */

#ifndef KERNEL_MATRIX_H
#define KERNEL_MATRIX_H

#include <iostream>
#include "vector.h"
#include "trig.h"

template<typename T>
class Matrix {
public:
    using V = Vector<T>;
    constexpr Matrix(V x, V y, V z) : x_(x), y_(y), z_(z) { }

    friend std::ostream & operator <<(std::ostream &out,
                                      const Matrix &v) {
        out << "<" << v.x_ << ", " << v.y_ << ", " << v.z_ << ">";
        return out;
    }

    Matrix operator +(const Matrix &other) const {
        return Matrix(x_ + other.x_, y_ + other.y_, z_ + other.z_);
    }

    Matrix operator +=(const Matrix &other) {
        x_ += other.x_;
        y_ += other.y_;
        z_ += other.z_;
        return *this;
    }

    Matrix operator -(const Matrix &other) const {
        return Matrix(x_ - other.x_, y_ - other.y_, z_ - other.z_);
    }

    Matrix operator -=(const Matrix &other) {
        x_ -= other.x_;
        y_ -= other.y_;
        z_ -= other.z_;
        return *this;
    }

    Matrix operator *(int scalar) const {
        return Matrix(x_ * scalar, y_ * scalar, z_ * scalar);
    }

    template<typename SCALAR>
    Matrix<typename T::template mul_t<SCALAR>::type>
    operator *(const SCALAR &scalar) const {
        using TT = typename T::template mul_t<SCALAR>::type;
        return Matrix<TT>(x_ * scalar, y_ * scalar, z_ * scalar);
    }

    Matrix operator /(int scalar) const {
        return Matrix(x_ / scalar, y_ / scalar, z_ / scalar);
    }

    template<typename SCALAR>
    Matrix<typename T::template div_t<SCALAR>::type>
    operator /(const SCALAR &scalar) const {
        using TT = typename T::template div_t<SCALAR>::type;
        return Matrix<TT>(x_ / scalar, y_ / scalar, z_ / scalar);
    }

    template<typename OTHER>
    Vector<typename T::template mul_t<OTHER>::type>
    operator *(const Vector<OTHER> &other) const {
        using TT = typename T::template mul_t<OTHER>::type;
        TT x = x_ * other;
        TT y = y_ * other;
        TT z = z_ * other;
        return Vector<TT>(x, y, z);
    }

    Matrix operator *(const Matrix &other) const {
        const Matrix m = other.transposed();
        const V x(T(x_ * m.x_), T(x_ * m.y_), T(x_ * m.z_));
        const V y(T(y_ * m.x_), T(y_ * m.y_), T(y_ * m.z_));
        const V z(T(z_ * m.x_), T(z_ * m.y_), T(z_ * m.z_));
        return Matrix(x, y, z);
    }

    Matrix transposed() const {
        const V x(x_.x(), y_.x(), z_.x());
        const V y(x_.y(), y_.y(), z_.y());
        const V z(x_.z(), y_.z(), z_.z());
        return Matrix(x, y, z);
    }
    
    V x() const { return x_; }
    V y() const { return y_; }
    V z() const { return z_; }

    template<typename T2>
    explicit operator Matrix<T2>() const {
        return Matrix<T2>(Vector<T2>(x_), Vector<T2>(y_), Vector<T2>(z_));
    }

    Matrix<T> normalized() const {
        V x = x_.normalized();
        V y = y_.normalized();
        V z = x ^ y;
        return Matrix<T>(x, y, z);
    }

    static Matrix<Rot> rotational(Rad alpha, Rad beta, Rad gamma) {
        Rot ac = cos(alpha);
        Rot as = sin(alpha);
        Rot bc = cos(beta);
        Rot bs = sin(beta);
        Rot cc = cos(gamma);
        Rot cs = sin(gamma);
        Rot z = Rot(0);
        Rot o = Rot(1);
        Matrix<Rot> ma(Vector<Rot>( o,   z,   z),
                       Vector<Rot>( z,  ac, -as),
                       Vector<Rot>( z,  as,  ac));
        Matrix<Rot> mb(Vector<Rot>(  bc,  z,  bs),
                       Vector<Rot>(   z,  o,   z),
                       Vector<Rot>( -bs,  z,  bc));
        Matrix<Rot> mc(Vector<Rot>(  cc, -cs,   z),
                       Vector<Rot>(  cs,  cc,   z),
                       Vector<Rot>(   z,   z,   o));
        return ma * mb * mc;
    }
private:
    V x_, y_, z_;
};

#endif // #ifndef KERNEL_MATRIX_H
