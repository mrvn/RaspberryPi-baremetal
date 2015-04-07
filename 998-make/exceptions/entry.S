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
.type exception_vector, STT_FUNC
exception_vector:
	b	stub_reset
	b	stub_undef
	b	stub_svc
	b	stub_prefetch_abort
	b	stub_data_abort
	b	stub_hypervisor_trap
	b	stub_irq
	b	stub_fiq

// save all registers and SPSR on the SVC stack, (LR corrected by offset)
.macro saveall, num, offset
	// corrcet lr
	sub     lr, #\offset
        // save SPSR and LR onto the SVC stack
        srsdb   #0x13!
        // switch to SVC mode, interrupts disabled
        cpsid   i, #0x13
        // save user space SP/LR
        sub     sp, #8
        stmia   sp, {sp, lr}^
        // save all registers on the stack (+ LR for alignment)
        push    {r0-r12, lr}
        // move pointer to registers into first argument
        mov     r0, sp
	// move number for exception into second argument
	mov	r1, #\num
	
	// align stack to 8 byte and save original SP
	// 8 byte alignment is required for ldrd but the exception may
	// have happened while the stack was only 4 byte aligned.

	// align stack to the next lower multiple of 8
	and	sp, sp, #~7
	// push saved SP and dummy register so it again is 8 byte alinged
	push	{r0, r1}
.endm

// restore all registes and return from exception
.macro restoreall
	// restore saved unaligned stack
	ldr	sp, [sp]
        // restore all regs from stack (+ LR for alignment)
        pop     {r0-r12, lr}
        // restore user space SP/LR and skip dummy
        ldmia   sp, {sp, lr}^
        add     sp, #8
        // return to whence we came from
        rfeia   sp!
.endm

.macro	make, num, offset, name
	.global	stub_\name
	.type	stub_\name, STT_FUNC
stub_\name:
	saveall	\num, \offset
	bl	handler_\name
	restoreall
.endm

// num, offset, name
make 0, 0, reset
make 1, 4, undef
make 2, 0, svc
make 3, 4, prefetch_abort
make 4, 8, data_abort
make 5, 0, hypervisor_trap
make 6, 4, irq
make 7, 4, fiq


.global	panic
.type	panic, STT_FUNC
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

