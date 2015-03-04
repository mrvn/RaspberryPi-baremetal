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

/* Display a greeting and ask the user for 5 charcaters except for return
 * before returning to boot.S (falling through to panic). Call panic function
 * early if return was pressed.
 */

#include <stdint.h>
#include "arch_info.h"

#define UNUSED(x) (void)(x)

enum Base {
    UART0_BASE = 0x201000, // 0x??201000
};

enum Register {
    UART0_DR = 0x00, // 0x??201000 data register
    UART0_FR = 0x18, // 0x??201018 flag register
};

enum {
    FR_TXFF = 1 << 5, // Transmit FIFO full
    FR_RXFE = 1 << 4, // Receive FIFO empty
};

volatile uint32_t * peripheral(enum Base base, enum Register reg) {
    return (volatile uint32_t *)(PERIPHERAL_BASE + base +reg);
}

void putc(char c) {
    while(*peripheral(UART0_BASE, UART0_FR) & FR_TXFF) { }
    *peripheral(UART0_BASE, UART0_DR) = c;
}

char getc(void) {
    while(*peripheral(UART0_BASE, UART0_FR) & FR_RXFE) { }
    return *peripheral(UART0_BASE, UART0_DR);
}

void puts(const char *str) {
    while (*str) putc(*str++);
}

void panic(void);

void kernel_main(uint32_t r0, uint32_t id, void *atags) {
    UNUSED(r0);
    UNUSED(id);
    UNUSED(atags);

    puts("UART barebone demo 003-c-code\n");
    puts("Enter 5 characters (but not return, hint, hint): ");
    for (int i = 0; i < 5; ++i) {
	char c = getc();
	if (c == '\n') {
	    puts("\nOh no, you pressed return.\n");
	    panic();
	}
	putc(c);
    }
    puts("\nGoodbye\n");
}
