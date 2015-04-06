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

/* Assembler primitives
 */

#ifndef KERNEL_ASM_H
#define KERNEL_ASM_H

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS
/* Instruction Synchronization Barrier / Flush Prefetch Buffer
 * The Flush Prefetch Buffer operation flushes the pipeline in the processor,
 * so that all instructions following the pipeline flush are fetched from
 * memory, including the cache, after the instruction has been
 * completed.
 */
static inline void isb(void) {
    // ARMv7
    // asm volatile ("isb");
    asm volatile ("mcr p15, 0, r12, c7, c5, 4");
}

/* Data Synchronization Barrier
 * This memorx barrier completes when all explicit memory transactions
 * occurring in program order before this instruction are completed. No
 * explicit memory transactions occurring in program order after this
 * instruction are started until this instruction completes. In fact, no
 * instructions occurring in program order after the Data Synchronization
 * Barrier complete, or change the interrupt masks, until this instruction
 * completes.
 */
static inline void dsb(void) {
    // ARMv7
    // asm volatile ("dsb");
    asm volatile ("mcr p15, 0, r12, c7, c10, 4");
}

/* Data Memory Barrier
 * This memory barrier ensures that all explicit memory transactions occurring
 * in program order before this instruction are completed. No explicit memory
 * transactions occuring in program order after this instrcution are started
 * until this instruction completes. Other instructions can complete out of
 * order with the Data Memory Barrier instruction.
 */
static inline void dmb(void) {
    // ARMv7
    // asm volatile ("dmb");
    asm volatile ("mcr p15, 0, r12, c7, c10, 5");
}

static inline void delay(uint32_t cycles) {
    while(cycles-- > 0) isb();
}
__END_DECLS

#endif // ##ifndef KERNEL_ASM_H
