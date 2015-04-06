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

/* LED related functions
 */

#include "led.h"
#include "arch_info.h"
#include "gpio.h"
#include "init_priorities.h"
#include "assert.h"

static bool led_state[2];

CONSTRUCTOR(LED, 1001) {
    // disable pull up/down and select output for activity led
    gpio_configure(led_act_pin, OUTPUT, OFF);
    
    // disable pull up/down and select output for power led
    if (led_pwr_pin != NO_LED) {
	gpio_configure(led_pwr_pin, OUTPUT, OFF);
    }

    led_set(LED_ACT, false);
    led_set(LED_PWR, false);
} CONSTRUCTOR_END

void led_set(enum LED led, bool state) {
    if (led == LED_ACT) {
	led_state[LED_ACT] = state;
	gpio_set(led_act_pin, state);
    } else {
	led_state[LED_PWR] = state;
	if (led_pwr_pin != NO_LED) {
	    gpio_set(led_pwr_pin, state);
	}
    }
}

void led_toggle(enum LED led) {
    if (led == LED_ACT) {
	led_set(LED_ACT, !led_state[LED_ACT]);
    } else {
	led_set(LED_PWR, !led_state[LED_PWR]);
    }
}
