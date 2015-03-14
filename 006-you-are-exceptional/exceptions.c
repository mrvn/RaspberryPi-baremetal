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
    uint32_t lr_svc; // lr from SVC mode
    uint32_t sp_usr;
    uint32_t lr_usr;
    uint32_t lr; // lr from the mode of the exception
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

void dump_regs(Regs *regs) {
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

void handler_reset(Regs *regs, uint32_t num) {
    // this will never be called
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
}

void handler_undef(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
    kprintf("undefined intstruction @ %#8.8lx = %#8.8lx\n",
	    regs->lr, *(uint32_t *)regs->lr);

    // skip instruction
    regs->lr += 4;
}

void handler_svc(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
}

uint32_t ifsr_read(void) {
    uint32_t t;
    asm volatile ("mrc p15, 0, %[t], c5, c0, 1" : [t] "=r" (t));
    return t;
}

uint32_t ifar_read(void) {
    uint32_t t;
    asm volatile ("mrc p15, 0, %[t], c6, c0, 2" : [t] "=r" (t));
    return t;
}

uint32_t dfsr_read(void) {
    uint32_t t;
    asm volatile ("mrc p15, 0, %[t], c5, c0, 0" : [t] "=r" (t));
    return t;
}

uint32_t dfar_read(void) {
    uint32_t t;
    asm volatile ("mrc p15, 0, %[t], c6, c0, 0" : [t] "=r" (t));
    return t;
}

void decode_fault_status(uint32_t fsr) {
    uint32_t status = ((fsr & (1 << 10)) >> 6) | (fsr & 0x7);
    switch (status) {
    case 0b00001:
	kprintf("alignment fault\n");
	break;
    case 0b00100:
	kprintf("fault on instruction cache maintenance\n");
	break;
    case 0b01100:
	kprintf("synchronnous external abort, first level translation fault\n");
	break;
    case 0b01110:
	kprintf("synchronnous external abort, second level translation fault\n");
	break;
    case 0b11100:
	kprintf("synchronnous parity error, first level translation fault\n");
	break;
    case 0b11110:
	kprintf("synchronnous parity error, second level translation fault\n");
	break;

    case 0b00101:
	kprintf("first level translation fault\n");
	break;
    case 0b00111:
	kprintf("second level translation fault\n");
	break;
    case 0b00011:
	kprintf("first level access flag fault\n");
	break;
    case 0b00110:
	kprintf("second level access flag fault\n");
	break;
    case 0b01001:
	kprintf("first level domain fault, domain %ld\n", (fsr & 0xF0) >> 4);
	break;
    case 0b01011:
	kprintf("second level domain fault, domain %ld\n", (fsr & 0xF0) >> 4);
	break;
    case 0b01101:
	kprintf("first level permission fault\n");
	break;
    case 0b01111:
	kprintf("second level permission fault\n");
	break;
    case 0b00010:
	kprintf("debug event\n");
	break;
    case 0b01000:
	kprintf("synchronous external abort\n");
	break;
    case 0b10000:
	kprintf("TLB conflict event\n");
	break;
    case 0b11001:
	kprintf("synchronous parity error on memory access\n");
	break;
    case 0b10110:
	kprintf("asynchronous external abort\n");
	break;
    case 0b11000:
	kprintf("asynchronous parity error on memory access\n");
	break;
    default:
	kprintf("unknown / reserved fault\n");
    }
}

void handler_prefetch_abort(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
    uint32_t ifsr = ifsr_read();
    uint32_t ifar = ifar_read();
    kprintf("prefetch abort, ");
    decode_fault_status(ifsr);
    kprintf("IFAR %#8.8lx (fault address)\n", ifar);
    kprintf("IFSR %#8.8lx (fault status register)\n", ifsr);
}

void handler_data_abort(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
    uint32_t dfsr = dfsr_read();
    uint32_t dfar = dfar_read();
    kprintf("data abort on %s, ", (dfsr & (1 << 11)) ? "write" : "read");
    decode_fault_status(dfsr);
    kprintf("DFAR %#8.8lx (fault address)\n", dfar);
    kprintf("DFSR %#8.8lx (fault status register)\n", dfsr);

    // skip instruction
    regs->lr += 4;
}

void handler_hypervisor_trap(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
}

void handler_irq(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
}

void handler_fiq(Regs *regs, uint32_t num) {
    kprintf("%s: Regs @ %p\n", EXCEPTION[num], regs);
    dump_regs(regs);
}

// data abort: DFSR + DFAR
// prefetch: IFSR + IFAR
