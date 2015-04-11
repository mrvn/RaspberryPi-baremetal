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

#include "gpio.h"
#include "arch_info.h"
#include "asm.h"
#include "peripherals.h"

enum GPIO_Reg {
    // function selector
    GPIO_FSEL0   = 0x00, // 0x??200000
    GPIO_FSEL1   = 0x04, // 0x??200004
    GPIO_FSEL2   = 0x08, // 0x??200008
    GPIO_FSEL3   = 0x0C, // 0x??20000C
    GPIO_FSEL4   = 0x10, // 0x??200010
    GPIO_FSEL5   = 0x14, // 0x??200014

    // set and clear pin output
    GPIO_SET0    = 0x1C, // 0x??20001C
    GPIO_SET1    = 0x20, // 0x??200020
    GPIO_CLR0    = 0x28, // 0x??200028
    GPIO_CLR1    = 0x2C, // 0x??20002C

    // Controls actuation of pull up/down to ALL GPIO pins.
    GPIO_PUD     = 0x94, // 0x??200094
    // Controls actuation of pull up/down for specific GPIO pin.
    GPIO_PUDCLK0 = 0x98, // 0x??200098
    GPIO_PUDCLK1 = 0x9C, // 0x??20009C
};

PERIPHERAL_BASE(GPIO)

void gpio_configure(uint32_t pin, enum FSel fn, enum PullUpDown action) {
    peripheral_use(GPIO_BASE);

    // set pull up down
    // ----------------
    
    // set action & delay for 150 cycles.
    volatile uint32_t *pud = GPIO_reg(GPIO_PUD);
    *pud = action;
    delay(150);

    // trigger action & delay for 150 cycles.
    volatile uint32_t *clock = &GPIO_reg(GPIO_PUDCLK0)[pin / 32];
    *clock = (1 << (pin % 32));
    delay(150);
    
    // clear action
    *pud = OFF;
	
    // remove clock
    *clock = 0;

    // set function
    // ------------
    volatile uint32_t *fsel = &GPIO_reg(GPIO_FSEL0)[pin / 10];
    uint32_t shift = (pin % 10) * 3;
    uint32_t mask = ~(7U << shift);
    *fsel = (*fsel & mask) | (fn << shift);
}

void gpio_set(uint32_t pin, bool state) {
    peripheral_use(GPIO_BASE);

    // set or clear output of pin
    GPIO_reg(state ? GPIO_SET0 : GPIO_CLR0)[pin / 32] = 1U << (pin % 32);
}

