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
#include <stddef.h>
#include "asm.h"

__BEGIN_DECLS
enum Base {
    NONE       = 0,
    GPIO_BASE  = 0x200000, // 0x??200000
    UART0_BASE = 0x201000, // 0x??201000
    IRQ_BASE   = 0x00B000, // 0x??00B000
    TIMER_BASE = 0x003000, // 0x??003000
};

typedef struct PeripheralLock {
    const struct PeripheralLock *prev;
    enum Base base;
} PeripheralLock;

static inline void peripheral_lock_dmb(const PeripheralLock *lock) {
    if ((lock->prev == NULL) || (lock->prev->base != lock->base)) {
	dmb();
    }
}

static inline void peripheral_lock_enter(PeripheralLock *lock,
					 const PeripheralLock *prev,
					 enum Base base) {
    lock->prev = prev;
    lock->base = base;
    peripheral_lock_dmb(lock);
}

static inline void peripheral_lock_leave(const PeripheralLock *lock) {
    peripheral_lock_dmb(lock);
}

// switching between peripherals needs a memory barrier
// use PERIPHERAL_ENTER / PERIPHERAL_LEAVE around peripheral access
#define PERIPHERAL_ENTER(NAME, PREV, BASE)				\
do {									\
    PeripheralLock NAME;						\
    peripheral_lock_enter(&NAME, PREV, BASE)

#define PERIPHERAL_LEAVE(NAME)						\
    peripheral_lock_leave(&NAME);					\
} while (0)

__END_DECLS

#endif // ##ifndef KERNEL_PERIPHERALS_H
