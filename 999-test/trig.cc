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

/* Fixed point cos() and sin()
 */

#include <cmath>
#include "templates.h"
#include "trig.h"

template <double (*fn)(double), typename IndexList> struct Table;
template <double (*fn)(double), int... I>
struct Table<fn, IndexList<I...> > {
    enum { N = sizeof...(I) };
    Rot data[N];
    constexpr inline Table() : data({ Rot(fn(double(I) / Rad::UNIT))... }) {}
    Rot operator[](size_t i) const {
        return data[i];
    }
};

static constexpr size_t TABLE_SIZE = M_PI / 2 * Rad::UNIT + 1;
typedef typename Indexes<0, TABLE_SIZE>::Result TableIndexList;
Table<std::cos, TableIndexList> cos_table;
Table<std::sin, TableIndexList> sin_table;

Rot cos(Rad x) {
    if (x == 0) return Rot(1);
    if (x < 0) return cos(-x);
    if (x > FP_PI_2) return -cos(FP_PI - x);
    return cos_table[x.raw()];
}

Rot sin(Rad x) {
    if (x == 0) return Rot(0);
    if (x < 0) return -sin(-x);
    if (x > FP_PI_2) return sin(FP_PI - x);
    return sin_table[x.raw()];
}
