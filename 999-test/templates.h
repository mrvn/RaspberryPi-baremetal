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

/* Helper functions for templates
 */

#ifndef KERNEL_TEMPLATES_H
#define KERNEL_TEMPLATES_H

#include <stdint.h>
#include <stddef.h>

// enable type only is condition is true
// -------------------------------------
template<bool C, typename T> struct enable_if;

// specialize for true
template <typename T> struct enable_if<true, T>  { using type = T; };

// wrapper to extract the type
template<bool C, typename T> using enable_if_t = typename enable_if<C,T>::type;

// pick T1 or T2 depending on condition
// ------------------------------------
template<bool C, typename T1, typename T2> struct conditional;

// specialize for true
template <typename T1, typename T2> struct conditional<true, T1, T2>  {
    using type = T1;
};

// specialize for false
template <typename T1, typename T2> struct conditional<false, T1, T2>  {
    using type = T2;
};

// wrapper to extract the type
template<bool C, typename T1, typename T2>
using conditional_t = typename conditional<C,T1,T2>::type;

// Int<size> type using the next bigger storage class
// --------------------------------------------------
template<size_t bits>
using Int =
  conditional_t<(bits<= 8), int8_t,
  conditional_t<(bits<=16), int16_t,
  conditional_t<(bits<=32), int32_t,
  enable_if_t  <(bits<=64), int64_t> > > >;

// UInt<size> type using the next bigger storage class
// ---------------------------------------------------
template<size_t bits>
using UInt =
  conditional_t<(bits<= 8), uint8_t,
  conditional_t<(bits<=16), uint16_t,
  conditional_t<(bits<=32), uint32_t,
  enable_if_t  <(bits<=64), uint64_t> > > >;

// Building a list of indices for constexpr initializations
// --------------------------------------------------------
template <int...> struct IndexList {};

template <int left_base, typename LeftList,
          int right_base, typename RightList> struct Merge;

template <int left_base, int... left,
          int right_base, int... right>
struct Merge<left_base, IndexList<left...>,
             right_base, IndexList<right...> > {
    typedef IndexList<left..., right...> Result;
};

template <int base, int n> struct Indexes {
    static constexpr int left_base = base;
    static constexpr int left = n / 2;
    static constexpr int right_base = base + n / 2;
    static constexpr int right = n - n / 2;
    typedef typename Merge<left_base,
                           typename Indexes<left_base, left>::Result,
                           right_base,
                           typename Indexes<right_base, right>::Result
                           >::Result Result;
};

template <int base> struct Indexes<base, 0> {
    typedef IndexList<> Result;
};

template <int base> struct Indexes<base, 1> {
    typedef IndexList<base> Result;
};

#endif // #ifndef KERNEL_TEMPLATES_H
