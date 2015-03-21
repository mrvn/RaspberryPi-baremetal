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

#include "cube.h"

DataPoint cube_point[24] = {
    DataPoint(DataFP(+0.50), DataFP(-0.50), DataFP(-0.75)), // 00
    DataPoint(DataFP(+0.50), DataFP(+0.50), DataFP(-0.75)), // 01
    DataPoint(DataFP(-0.50), DataFP(+0.50), DataFP(-0.75)), // 02
    DataPoint(DataFP(-0.50), DataFP(-0.50), DataFP(-0.75)), // 03
    
    DataPoint(DataFP(+0.50), DataFP(-0.75), DataFP(-0.50)), // 04
    DataPoint(DataFP(+0.75), DataFP(-0.50), DataFP(-0.50)), // 05
    DataPoint(DataFP(+0.75), DataFP(+0.50), DataFP(-0.50)), // 06
    DataPoint(DataFP(+0.50), DataFP(+0.75), DataFP(-0.50)), // 07
    DataPoint(DataFP(-0.50), DataFP(+0.75), DataFP(-0.50)), // 08
    DataPoint(DataFP(-0.75), DataFP(+0.50), DataFP(-0.50)), // 09
    DataPoint(DataFP(-0.75), DataFP(-0.50), DataFP(-0.50)), // 10
    DataPoint(DataFP(-0.50), DataFP(-0.75), DataFP(-0.50)), // 11
    
    DataPoint(DataFP(+0.50), DataFP(-0.75), DataFP(+0.50)), // 12
    DataPoint(DataFP(+0.75), DataFP(-0.50), DataFP(+0.50)), // 13
    DataPoint(DataFP(+0.75), DataFP(+0.50), DataFP(+0.50)), // 14
    DataPoint(DataFP(+0.50), DataFP(+0.75), DataFP(+0.50)), // 15
    DataPoint(DataFP(-0.50), DataFP(+0.75), DataFP(+0.50)), // 16
    DataPoint(DataFP(-0.75), DataFP(+0.50), DataFP(+0.50)), // 17
    DataPoint(DataFP(-0.75), DataFP(-0.50), DataFP(+0.50)), // 18
    DataPoint(DataFP(-0.50), DataFP(-0.75), DataFP(+0.50)), // 19
    
    DataPoint(DataFP(+0.50), DataFP(-0.50), DataFP(+0.75)), // 20
    DataPoint(DataFP(+0.50), DataFP(+0.50), DataFP(+0.75)), // 21
    DataPoint(DataFP(-0.50), DataFP(+0.50), DataFP(+0.75)), // 22
    DataPoint(DataFP(-0.50), DataFP(-0.50), DataFP(+0.75)), // 23
};

DataPoly cube_poly[26] = {
    // points are anti clockwise when looking at the polygon,
    // resulting normals point inwards giving z > 0 when visible
    {4, { 0,  3,  2,  1}, DataPoint()},	// 00

    {4, { 0,  1,  6,  5}, DataPoint()},	// 01
    {4, { 1,  2,  8,  7}, DataPoint()},	// 02
    {4, { 3, 10,  9,  2}, DataPoint()},	// 03
    {4, { 3,  0,  4, 11}, DataPoint()},	// 04

    {3, { 0,  5,  4}, DataPoint()},	// 05
    {3, { 1,  7,  6}, DataPoint()},	// 06
    {3, { 2,  9,  8}, DataPoint()},	// 07
    {3, { 3, 11, 10}, DataPoint()},	// 08

    {4, { 4, 12, 19, 11}, DataPoint()},	// 09
    {4, { 5,  6, 14, 13}, DataPoint()},	// 10
    {4, { 7,  8, 16, 15}, DataPoint()},	// 11
    {4, { 9, 10, 18, 17}, DataPoint()},	// 12

    {4, { 4,  5, 13, 12}, DataPoint()},	// 13
    {4, { 6,  7, 15, 14}, DataPoint()},	// 14
    {4, { 8,  9, 17, 16}, DataPoint()},	// 15
    {4, {10, 11, 19, 18}, DataPoint()},	// 16

    {4, {13, 14, 21, 20}, DataPoint()},	// 17
    {4, {15, 16, 22, 21}, DataPoint()},	// 18
    {4, {17, 18, 23, 22}, DataPoint()},	// 19
    {4, {12, 20, 23, 19}, DataPoint()},	// 20

    {3, {12, 13, 20}, DataPoint()},	// 21
    {3, {14, 15, 21}, DataPoint()},	// 22
    {3, {16, 17, 22}, DataPoint()},	// 23
    {3, {18, 19, 23}, DataPoint()},	// 24

    {4, {20, 21, 22, 23}, DataPoint()},	// 25
};

Shape cube = {24, 26, cube_point, cube_poly};
