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

/* Information about the different Raspberry Pi models
 */

#ifndef KERNEL_ARCHINFO_H
#define KERNEL_ARCHINFO_H

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS
enum Model {
    RASPBERRY_PI,
    RASPBERRY_PI_B_PLUS,
    RASPBERRY_PI_2,
};

extern enum Model model;
extern const char *model_name;
extern uint32_t mem_total;
extern uint32_t initrd_start;
extern uint32_t initrd_size;
extern uint32_t peripheral_base;
extern uint32_t led_act_pin;

enum Base {
    IRQ_BASE   = 0x00B000, // 0x??00B000
    UART0_BASE = 0x201000, // 0x??201000
};

enum Register {
    IRQ_BASIC_PENDING = 0x200, // 0x??00B200
    IRQ_PENDING1      = 0x204, // 0x??00B204 
    IRQ_PENDING2      = 0x208, // 0x??00B208
    IRQ_FIQ_CONTROL   = 0x20C, // 0x??00B20C
    IRQ_ENABLE1       = 0x210, // 0x??00B210
    IRQ_ENABLE2       = 0x214, // 0x??00B214
    IRQ_ENABLE_BASIC  = 0x218, // 0x??00B218
    IRQ_DISABLE1      = 0x21C, // 0x??00B21C
    IRQ_DISABLE2      = 0x220, // 0x??00B220
    IRQ_DISABLE_BASIC = 0x224, // 0x??00B224

    UART0_DR = 0x00, // 0x??201000 data register
    UART0_FR = 0x18, // 0x??201018 flag register
};

volatile uint32_t * peripheral(enum Base base, enum Register reg);

typedef struct Atag Atag;

void arch_info_init(const Atag *atag);
__END_DECLS

#endif // ##ifndef KERNEL_ARCHINFO_H
