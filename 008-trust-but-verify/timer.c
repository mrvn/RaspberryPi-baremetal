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
#include "irq.h"
#include "led.h"

enum {
    TIMER_BASE = 0x003000, // 0x??003000
};

enum TIMER_Reg {
    TIMER_CS  = 0x00, // 0x??003000 System Timer Control/Status
    TIMER_CLO = 0x04, // 0x??003004 System Timer Lower 32 bits
    TIMER_CHI = 0x08, // 0x??003008 System Timer Higher 32 bits
    TIMER_C0  = 0x0C, // 0x??00300C System Timer Compare 0
    TIMER_C1  = 0x10, // 0x??003010 System Timer Compare 1
    TIMER_C2  = 0x14, // 0x??003014 System Timer Compare 2
    TIMER_C3  = 0x18, // 0x??003018 System Timer Compare 3
};

PERIPHERAL_BASE(TIMER)

static uint32_t status(void) {
    volatile uint32_t *ctrl = TIMER_reg(TIMER_CS);
    return *ctrl & 0x7;
}

static void ctrl_set(uint32_t t) {
    volatile uint32_t *ctrl = TIMER_reg(TIMER_CS);
    *ctrl |= t;
}

uint64_t timer_count(void) {
    volatile uint32_t *hi = TIMER_reg(TIMER_CHI);
    volatile uint32_t *lo = TIMER_reg(TIMER_CLO);
    return (((uint64_t)*hi) << 32) | *lo;
}
/*
static uint32_t lowcount(void) {
    volatile uint32_t *lo = TIMER_reg(TIMER_CLO);
    return *lo;
}
*/
static uint32_t cmp(uint32_t num) {
    enum TIMER_Reg reg =
	(num < 2) ? ((num < 1) ? TIMER_C0 : TIMER_C1)
	          : ((num < 3) ? TIMER_C2 : TIMER_C3);
    volatile uint32_t *cmp = TIMER_reg(reg);
    return *cmp;
}

static void set_cmp(uint32_t num, uint32_t t) {
    enum TIMER_Reg reg =
	(num < 2) ? ((num < 1) ? TIMER_C0 : TIMER_C1)
	          : ((num < 3) ? TIMER_C2 : TIMER_C3);
    volatile uint32_t *cmp = TIMER_reg(reg);
    *cmp = t;
}

void timer_test(void) {
    uint64_t t = timer_count();
    kprintf("timer CS    = %#lx\n", status());
    kprintf("timer count = %#18llx\n", t);
    kprintf("timer cmp 0 = %#10lx\n", cmp(0));
    kprintf("timer cmp 1 = %#10lx\n", cmp(1));
    kprintf("timer cmp 2 = %#10lx\n", cmp(2));
    kprintf("timer cmp 3 = %#10lx\n", cmp(3));
    kprintf("\n");
    
    // trigger on the second next second (at least one second from now)
    set_cmp(1, t / 1000000 * 1000000 + 2000000);
    
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
    uint64_t t = timer_count();
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

    // toggle leds
    switch(seconds % 4) {
    case 0:
	led_set(LED_ACT, true);
	break;
    case 1:
	led_set(LED_PWR, true);
	break;
    case 2:
	led_set(LED_ACT, false);
	break;
    case 3:
	led_set(LED_PWR, false);
	break;
    }
}
