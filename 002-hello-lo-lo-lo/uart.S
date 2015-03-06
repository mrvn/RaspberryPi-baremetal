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

/* Output some text on the UART and then echo all input
 *----------------------------------------------------------------
 * The code assumes the UART is already configured by the firmware,
 * which is the case so far. The UART is configured to 115200 BAUD,
 * 8 bit, not even parity and 1 stop bit (8N1).
 */

#ifndef MODEL
#error MODEL not defined
#endif

#if MODEL == 0
#define PERIPHERAL_BASE 0x20000000 // Raspberry Pi Peripheral Base Address
#endif

#if MODEL == 1
#define PERIPHERAL_BASE 0x20000000 // Raspberry Pi B+ Peripheral Base Address
#endif

#if MODEL == 2
#define PERIPHERAL_BASE 0x3F000000 // Raspberry Pi 2 Peripheral Base Address
#endif

#ifndef PERIPHERAL_BASE
#error MODEL must be one of 0 (RPi), 1 (RPi b+) or 2 (RPi 2)
#endif

#define UART0_BASE        0x201000 // 0x??201000 base adress of UART0
#define DR                    0x00 // 0x??201000 data register
#define FR                    0x18 // 0x??201018 flag register
#define FR_TXFF             1 << 5 // Transmit FIFO full
#define FR_RXFE             1 << 4 // Receive FIFO empty

.section ".text"
.global _start

_start:
	// R0 = PERIPHERAL_BASE + UART0_BASE
	ldr     r0, =PERIPHERAL_BASE + UART0_BASE

	// output greeting
	adr	r1, str	- 1		// address of text - 1
	b	check_0			// check for empty string
put_str:
	ldr	r3, [r0, #FR]		// get flag register
	ands	r3, r3, #FR_TXFF	// transmit buffer full?
	bne	put_str			// try again

	strb	r2, [r0, #DR]		// transmit char
check_0:
	ldrb	r2, [r1, #1]!		// get next char and increment
	cmp	r2, #0			// 0 byte?
	bne	put_str			// keep printing if not

	// echo input
loop:
wait_receive:
	ldr	r3, [r0, #FR]		// get flag register
	ands	r3, r3, #FR_RXFE	// receive buffer empty?
	bne	wait_receive		// try again

	ldrb	r2, [r0, #DR]		// receive char

wait_transmit:
	ldr	r3, [r0, #FR]		// get flag register
	ands	r3, r3, #FR_TXFF	// transmit buffer full?
	bne	wait_transmit		// try again

	strb	r2, [r0, #DR]		// transmit char

	b       loop			// keep going

// constants from ldr macros
constants:
.ltorg
	
str:	.asciz "UART barebone demo 003-uart\n"

