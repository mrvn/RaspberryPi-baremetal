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

/* Exception handling
 */

#include "exceptions.h"
#include "kprintf.h"

extern uint32_t exception_vector[];

void set_vbar(uint32_t *base) {
    asm volatile ("mcr p15, 0, %[base], c12, c0, 0"
		  :: [base] "r" (base));
}

void exceptions_init(void) {
    set_vbar(exception_vector);
}

typedef struct Regs {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;
    uint32_t sp_usr;
    uint32_t lr_usr;
    uint32_t pad;
    uint32_t lr;
    uint32_t spsr;
} Regs;

const char *EXCEPTION[] = {
    "Reset",
    "Undefine Instruction",
    "Supervisor Call",
    "Prefetch Abort",
    "Data Abort",
    "Hypervisor",
    "IRQ",
    "FIQ",
};

#define BIT(w, i, c) (((w) & (1 << (i))) ? c : '-')

const char *MODE[] = {
    "BAD", // 0xb00000
    "BAD", // 0xb00001
    "BAD", // 0xb00010
    "BAD", // 0xb00011
    "BAD", // 0xb00100
    "BAD", // 0xb00101
    "BAD", // 0xb00110
    "BAD", // 0xb00111
    "BAD", // 0xb01000
    "BAD", // 0xb01001
    "BAD", // 0xb01010
    "BAD", // 0xb01011
    "BAD", // 0xb01100
    "BAD", // 0xb01101
    "BAD", // 0xb01110
    "BAD", // 0xb01111
    "usr", // 0xb10000
    "fiq", // 0xb10001
    "irq", // 0xb10010
    "svc", // 0xb10011
    "BAD", // 0xb10100
    "BAD", // 0xb10101
    "mon", // 0xb10110
    "abt", // 0xb10111
    "BAD", // 0xb11000
    "BAD", // 0xb11001
    "hyp", // 0xb11010
    "und", // 0xb11011
    "BAD", // 0xb11100
    "BAD", // 0xb11101
    "BAD", // 0xb11110
    "sys"  // 0xb11111
};

void handler_default(Regs *regs, uint32_t num) {
    kprintf("%s: %s, Regs @ %p\n", __FUNCTION__, EXCEPTION[num], regs);
    kprintf("r0 : %#8.8lx   r1 : %#8.8lx   r2 : %#8.8lx   r3 : %#8.8lx\n",
	    regs->r0, regs->r1, regs->r2, regs->r3);
    kprintf("r4 : %#8.8lx   r5 : %#8.8lx   r6 : %#8.8lx   r7 : %#8.8lx\n",
	    regs->r4, regs->r5, regs->r6, regs->r7);
    kprintf("r8 : %#8.8lx   r9 : %#8.8lx   r10: %#8.8lx   r11: %#8.8lx\n",
	    regs->r8, regs->r9, regs->r10, regs->r11);
    kprintf("r12: %#8.8lx   SPu: %#8.8lx   LRu: %#8.8lx   LR : %#8.8lx\n",
	    regs->r12, regs->sp_usr, regs->lr_usr, regs->lr);
    kprintf("SPSR: %c%c%c%c%c IT[1:0]=%lx %c GE[3:0]=%lx IT[7:2]=%02lx %c%c%c%c%c %s(%#2.2lx)\n",
	    BIT(regs->spsr, 31, 'N'),
	    BIT(regs->spsr, 30, 'Z'),
	    BIT(regs->spsr, 29, 'C'),
	    BIT(regs->spsr, 28, 'V'),
	    BIT(regs->spsr, 27, 'Q'),
	    (regs->spsr >> 25) & 0x3,
	    BIT(regs->spsr, 24, 'J'),
	    // 23-20 reserved
	    (regs->spsr >> 16) & 0xF,
	    (regs->spsr >> 10) & 0x3F,
	    BIT(regs->spsr,  9, 'E'),
	    BIT(regs->spsr,  8, 'A'),
	    BIT(regs->spsr,  7, 'I'),
	    BIT(regs->spsr,  6, 'F'),
	    BIT(regs->spsr,  5, 'T'),
	    MODE[regs->spsr & 0x1F],
	    regs->spsr & 0x1F);
}
