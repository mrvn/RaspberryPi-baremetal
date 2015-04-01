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

/* System Timer
 */

#include "timer.h"
#include "arch_info.h"
#include "kprintf.h"

static uint32_t status(void) {
    volatile uint32_t *ctrl = peripheral(TIMER_BASE, TIMER_CS);
    return *ctrl & 0x7;
}

static uint64_t count(void) {
    volatile uint32_t *hi = peripheral(TIMER_BASE, TIMER_CHI);
    volatile uint32_t *lo = peripheral(TIMER_BASE, TIMER_CLO);
    return (((uint64_t)*hi) << 32) | *lo;
}

static uint32_t cmp_read(uint32_t num) {
    enum Register reg =
	(num < 2) ? ((num < 1) ? TIMER_C0 : TIMER_C1)
	          : ((num < 3) ? TIMER_C2 : TIMER_C3);
    volatile uint32_t *cmp = peripheral(TIMER_BASE, reg);
    return *cmp;
}

void timer_test(void) {
    kprintf("timer CS = %#lx\n", status());
    kprintf("timer count = %#llx\n", count());
    kprintf("timer cmp 0 = %#lx\n", cmp_read(0));
    kprintf("timer cmp 1 = %#lx\n", cmp_read(1));
    kprintf("timer cmp 2 = %#lx\n", cmp_read(2));
    kprintf("timer cmp 3 = %#lx\n", cmp_read(3));
    kprintf("\n");
}
