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

/* assembly entry point for Exception handling
 */

#define MODE_USER    0b10000
#define MODE_FIQ     0b10001
#define MODE_IRQ     0b10010
#define MODE_SVC     0b10011
#define MODE_MONITOR 0b10110
#define MODE_ABORT   0b10111
#define MODE_HYPER   0b11010
#define MODE_UNDEF   0b11011
#define MODE_SYSTEM  0b11111

.section ".text"
.balign 32
.global exception_vector
exception_vector:
	b	stub_reset
	b	stub_undefined_instruction
	b	stub_supervisor_call
	b	stub_prefetch_abort
	b	stub_data_abort
	b	stub_hypervisor_trap
	b	stub_irq
	b	stub_fiq

// save all registers and SPSR on the SYS stack, (LR corrected by offset)
.macro saveall, num, offset
	// corrcet lr
	sub     lr, #\offset
        // save SPSR and LR onto the SYS stack
        srsdb   #0x13!
        // switch to SYS mode, interrupts disabled
        cpsid   i, #0x13
	// dummy push so the stack ends up aligned
	push	{r0}
        // save user space SP/LR
        sub     sp, #8
        stmia   sp, {sp, lr}^
        // save all registers on the stack
        push    {r0-r12}
        // move pointer to registers into first argument
        mov     r0, sp
	// move number for exception into second argument
	mov	r1, #\num
.endm

// restore all registes and return from exception
.macro restoreall
        // restore all regs from stack
        pop     {r0-r12}
        // restore user space SP/LR and skip dummy
        ldmia   sp, {sp, lr}^
        add     sp, #12
        // return to whence we came from
        rfeia   sp!
.endm

.macro	make, name, handler, num, offset
	.global	stub_\name
stub_\name:
	saveall	\num, \offset
	bl	handler_\handler
	restoreall
.endm

make reset,                 default, 0, 0
make undefined_instruction, default, 1, 4
make supervisor_call,       default, 2, 0
make prefetch_abort,        default, 3, 4
make data_abort,            default, 4, 8
make hypervisor_trap,       default, 5, 0
make irq,                   default, 6, 4
make fiq,                   default, 7, 4


panic:
	ldr	r0, =0x3F200020
	ldr	r1, =0x3F20002C
	mov	r2, #1<<15
1:
        str r2,[r0]     // set bit

        mov r3,#0x100000        // wait a while
2:
        subs r3,#1
        bne 2b

        str r2,[r1]     // clear bit
        
        mov r3,#0x100000        // wait a while
2:
        subs r3,#1
        bne 2b

        b 1b                  // again and again

