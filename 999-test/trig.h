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

#ifndef KERNEL_TRIG_H
#define KERNEL_TRIG_H

#include <stddef.h>
#include "fixed.h"

static constexpr size_t rot_bits = 2;
static constexpr size_t rot_fraction = 14;
static constexpr size_t rad_bits = 4;
static constexpr size_t rad_fraction = 12;

using Rot = FixedPoint<rot_bits, rot_fraction>;
using Rad = FixedPoint<rad_bits, rad_fraction>;

static constexpr Rad FP_PI(M_PI);
static constexpr Rad FP_PI_2(M_PI / 2);

Rot cos(Rad x);
Rot sin(Rad x);

#endif // #ifndef KERNEL_TRIG_H
