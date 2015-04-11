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

/* GPIO related functions
 */

#ifndef KERNEL_GPIO_H
#define KERNEL_GPIO_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <stdbool.h>
#include "peripherals.h"

__BEGIN_DECLS
enum FSel {
    INPUT, OUTPUT, FN5, FN4, FN0, FN1, FN2, FN3,
};
    
enum PullUpDown {
    OFF, UP, DOWN,
};

void gpio_configure(PeripheralLock *prev, uint32_t pin, enum FSel fn,
		    enum PullUpDown action);
void gpio_set(PeripheralLock *prev, uint32_t pin, bool state);
__END_DECLS

#endif // ##ifndef KERNEL_GPIO_H
