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

/* Uart related functions
 */

#include "uart.h"
#include "arch_info.h"

enum {
    FR_TXFF = 1 << 5, // Transmit FIFO full
    FR_RXFE = 1 << 4, // Receive FIFO empty
};

void putc(char c) {
    // wait for space in the transmit FIFO
    while(*peripheral(UART0_BASE, UART0_FR) & FR_TXFF) { }
    // add char to transmit FIFO
    *peripheral(UART0_BASE, UART0_DR) = c;
}

char getc(void) {
    // wait for data in the receive FIFO
    while(*peripheral(UART0_BASE, UART0_FR) & FR_RXFE) { }
    // extract char from receive FIFO
    return *peripheral(UART0_BASE, UART0_DR);
}

void puts(const char *str) {
    // putc until 0 byte
    while (*str) putc(*str++);
}

void put_uint32(uint32_t x) {
    static const char HEX[] = "0123456789ABCDEF";
    putc('0');
    putc('x');
    for (int i = 28; i >= 0; i -= 4) {
        putc(HEX[(x >> i) % 16]);
    }
}

void show(const char *str, uint32_t x) {
    puts(str);
    puts(": ");
    put_uint32(x);
    putc('\n');
}
