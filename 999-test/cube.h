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

/* Data for cube
 */

#ifndef KERNEL_CUBE_H
#define KERNEL_CUBE_H

#include "fixed.h"
#include "vector.h"

// points in 6.10 fixed point
typedef FixedPoint<6, 10> DataFP;
typedef Vector<DataFP> DataPoint;
typedef struct DataPoly {
    int num_points;
    int point[4];
    DataPoint normal;
} DataPoly;

typedef struct Shape {
    int num_points;
    int num_poly;
    DataPoint *point;
    DataPoly *poly;
} Shape;

extern Shape cube;

#endif // #ifndef KERNEL_CUBE_H
