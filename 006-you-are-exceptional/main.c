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

#define UNUSED(x) (void)(x)

// main C function, called from boot.S
void kernel_main(uint32_t r0, uint32_t id, const Atag *atag) {
    UNUSED(r0); // always 0
    UNUSED(id); // 0xC42 for Raspberry Pi

    arch_info_init(atag);
    exceptions_init();

    kprintf("\ncalling SVC\n");
    // calling svc from within svc clobbers the LR register, tell gcc about it
    asm volatile ("svc #0" ::: "lr");

    kprintf("\nundefine instruction 0xee011f51\n");
    asm volatile (".word 0xee011f51");
    
    kprintf("\nunaligned access *(0x1)\n");
    *(volatile uint32_t *)0x1;
    
    kprintf("\nGoodbye\n");
}
