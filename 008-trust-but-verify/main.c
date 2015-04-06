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

/* Detect Raspberry Pi model, install exception handlers and try a few of
 * them.
 */

#include <stdint.h>
#include "arch_info.h"
#include "kprintf.h"
#include "exceptions.h"
#include "timer.h"
#include "led.h"
#include "uart.h"
#include "assert.h"

#define UNUSED(x) (void)(x)

// constructors
typedef void (*constructor_t)(void);
extern constructor_t _init_array_start[];
extern constructor_t _init_array_end[];

void kernel_constructors(void) {
    for(constructor_t *fn = _init_array_start; fn != _init_array_end; ++fn) {
        (*fn)();
    }
}


// main C function, called from boot.S
void kernel_main(void) {
    /*
    arch_info_init();
    exceptions_init();
    led_init();
    uart_init();
    */

    timer_test();
    
    kprintf("\nGoodbye\n");
}
