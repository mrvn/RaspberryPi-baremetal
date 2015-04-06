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

/* assembly code to bootstrap a C environment */

// this code muse come first and the linker script places section
// .text.boot first
.section ".text.boot"
.global _start

_start:
	// the bootloader passes 3 arguments:
	// r0 = 0
	// r1 = 0xC42 (ID for Raspberry Pi)
	// r2 = ATAGS
	// those must be preserved to the kernel_main function
	
	// initialize stack pointer
	ldr	sp, =stack_top

	// clear bss segment
	ldr	r3, =_bss_start - 1 // -1 because of the strltb below
	ldr	r4, =_bss_end - 1
1:
	cmp	r3, r4		// already done?
	strltb	r0, [r3, #1]!	// if not store 0 byte and advance r3
	blt	1b		// if not repeat

	// jump to kernel_main
	ldr	r3, =kernel_main
	blx	r3

	// halt cpu
.global halt
halt:
	wfe
	b halt

// constants for ldr macro
constants:
.ltorg
	
.section ".bss"
// 16k stack aligned to 8 byte
.balign 8
.global stack
stack:
	.space	16384
.global stack_top
stack_top:
