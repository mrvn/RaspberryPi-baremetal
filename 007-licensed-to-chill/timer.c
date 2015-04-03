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
#include "exceptions.h"

static uint32_t status(void) {
    volatile uint32_t *ctrl = peripheral(TIMER_BASE, TIMER_CS);
    return *ctrl & 0x7;
}

static void ctrl_set(uint32_t t) {
    volatile uint32_t *ctrl = peripheral(TIMER_BASE, TIMER_CS);
    *ctrl |= t;
}

static uint64_t count(void) {
    volatile uint32_t *hi = peripheral(TIMER_BASE, TIMER_CHI);
    volatile uint32_t *lo = peripheral(TIMER_BASE, TIMER_CLO);
    return (((uint64_t)*hi) << 32) | *lo;
}
/*
static uint32_t lowcount(void) {
    volatile uint32_t *lo = peripheral(TIMER_BASE, TIMER_CLO);
    return *lo;
}
*/
static uint32_t cmp(uint32_t num) {
    enum Register reg =
	(num < 2) ? ((num < 1) ? TIMER_C0 : TIMER_C1)
	          : ((num < 3) ? TIMER_C2 : TIMER_C3);
    volatile uint32_t *cmp = peripheral(TIMER_BASE, reg);
    return *cmp;
}

static void set_cmp(uint32_t num, uint32_t t) {
    enum Register reg =
	(num < 2) ? ((num < 1) ? TIMER_C0 : TIMER_C1)
	          : ((num < 3) ? TIMER_C2 : TIMER_C3);
    volatile uint32_t *cmp = peripheral(TIMER_BASE, reg);
    *cmp = t;
}

void timer_test(void) {
    kprintf("timer CS    = %#lx\n", status());
    kprintf("timer count = %#18llx\n", count());
    kprintf("timer cmp 0 = %#10lx\n", cmp(0));
    kprintf("timer cmp 1 = %#10lx\n", cmp(1));
    kprintf("timer cmp 2 = %#10lx\n", cmp(2));
    kprintf("timer cmp 3 = %#10lx\n", cmp(3));
    kprintf("\n");
    
    // trigger on the second next second (at least one second from now)
    set_cmp(1, count() / 1000000 * 1000000 + 2000000);
    
    // clear pending bit and enable irq
    ctrl_set(1U << 1);
    enable_irq(IRQ_TIMER1);
    enable_irqs();

    while (1) {
	// chill out
	asm volatile ("wfi");
    }
}

void handle_timer1(void) {
    uint64_t t = count();
    kprintf("timer CS    = %lu\n", status());
    kprintf("timer count = %#18llx\n", t);
    kprintf("timer cmp 0 = %#10lx\n", cmp(0));
    kprintf("timer cmp 1 = %#10lx\n", cmp(1));
    kprintf("timer cmp 2 = %#10lx\n", cmp(2));
    kprintf("timer cmp 3 = %#10lx\n", cmp(3));

    uint32_t frac, seconds, minutes, hours, next;
    frac = t % 1000000;
    t /= 1000000;
    next = t * 1000000 + 1000000;
    seconds = t % 60;
    t /= 60;
    minutes = t % 60;
    t /= 60;
    hours = t;
    kprintf("time = %lu:%02lu:%02lu.%06lu\n", hours, minutes, seconds, frac);
    kprintf("\n");
    
    // clear pending bit
    ctrl_set(1U << 1);
    // trigger one the next second mark
    set_cmp(1, next);
}
