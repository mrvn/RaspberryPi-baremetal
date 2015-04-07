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

/* Interrupt handling
 */

#include "irq.h"
#include "arch_info.h"
#include "kprintf.h"
#include "exceptions.h"

enum {
    IRQ_BASE   = 0x00B000, // 0x??00B000
};

enum IRQ_Reg {
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
};

PERIPHERAL_BASE(IRQ)

extern void handle_timer1(void);

void handler_irq(Regs *regs, uint32_t num) {
    (void)num;
    volatile uint32_t *pending1 = IRQ_reg(IRQ_PENDING1);
    if ((*pending1 & (1U << 1)) != 0) {
	handle_timer1();
    } else {
	kprintf("%s: Regs @ %p\n", "IRQ", regs);
	dump_regs(regs);
    }
}

void handler_fiq(Regs *regs, uint32_t num) {
    (void)num;
    kprintf("%s: Regs @ %p\n", "FIQ", regs);
    dump_regs(regs);
}

uint32_t cpsr_read(void) {
    uint32_t t;
    asm volatile ("mrs %[t], CPSR" : [t] "=r" (t));
    return t;
}

void cpsr_write_c(uint32_t t) {
    asm volatile ("msr CPSR_c, %[t]" : : [t] "r" (t));
}

enum CPSR {
    CPSR_FIQ_DISABLE = 1U << 6,
    CPSR_IRQ_DISABLE = 1U << 7,
};

void enable_irqs(void) {
    uint32_t cpsr = cpsr_read();
    cpsr &= ~CPSR_IRQ_DISABLE;
    cpsr_write_c(cpsr);
}

void disable_irqs(void) {
    uint32_t cpsr = cpsr_read();
    cpsr |= CPSR_IRQ_DISABLE;
    cpsr_write_c(cpsr);
}

void enable_irq(enum IRQ irq) {
    enum IRQ_Reg reg =
	(irq < 32) ? IRQ_ENABLE1
	           : ((irq < 64) ? IRQ_ENABLE2 : IRQ_ENABLE_BASIC);
    volatile uint32_t * enable = IRQ_reg(reg);
    *enable = 1U << (irq % 32);
}

void disable_irq(enum IRQ irq) {
    enum IRQ_Reg reg =
	(irq < 32) ? IRQ_DISABLE1
	           : ((irq < 64) ? IRQ_DISABLE2 : IRQ_DISABLE_BASIC);
    volatile uint32_t * disable = IRQ_reg(reg);
    *disable = 1U << (irq % 32);
}
