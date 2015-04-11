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

/*
 * ISO C99 Standard: 7.2 Diagnostics       <assert.h>
 */

#include "assert.h"
#include <stdbool.h>
#include "led.h"
#include "asm.h"
#include "timer.h"
#include "kprintf.h"
#include "peripherals.h"

#define UNUSED(x) (void)(x)

// This prints an "Assertion failed" message and blinks the LEDs madly
extern void __assert_fail (const char *__assertion, const char *__file,
                           unsigned int __line, const char *__function) {
    // enter peripheral for LED
    PERIPHERAL_ENTER(lock, NULL, GPIO_BASE);

    kprintf("Assertion failed: %s: %d: %s: assert(%s)\n",
	    __file, __line, __function, __assertion);
    uint64_t next = 0;
    while (true) {
	while (timer_count(&lock) < next) { }
	led_toggle(&lock, LED_ACT);
	if (next % 200000 == 0) led_toggle(&lock, LED_PWR);
	next += 50000;
    }

    // leave peripheral for LED
    PERIPHERAL_LEAVE(lock);
}
