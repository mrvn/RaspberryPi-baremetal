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

/* Helper for peripherals
 */

#ifndef KERNEL_PERIPHERALS_H
#define KERNEL_PERIPHERALS_H

#include <sys/cdefs.h>
#include "asm.h"

__BEGIN_DECLS
enum Base {
    NONE       = 0,
    GPIO_BASE  = 0x200000, // 0x??200000
    UART0_BASE = 0x201000, // 0x??201000
    IRQ_BASE   = 0x00B000, // 0x??00B000
    TIMER_BASE = 0x003000, // 0x??003000
};

extern enum Base last_peripheral;

// switching between peripherals needs a memory barrier
static inline void peripheral_use(enum Base base) {
    if (last_peripheral != base) dmb();
    last_peripheral = base;
}

__END_DECLS

#endif // ##ifndef KERNEL_PERIPHERALS_H
