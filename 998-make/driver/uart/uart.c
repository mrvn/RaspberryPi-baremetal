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
    UART0_BASE = 0x201000, // 0x??201000
};

enum UART0_Reg {
    UART0_DR = 0x00, // 0x??201000 data register
    UART0_FR = 0x18, // 0x??201018 flag register
};

PERIPHERAL_BASE(UART0)

enum {
    FR_TXFF = 1 << 5, // Transmit FIFO full
    FR_RXFE = 1 << 4, // Receive FIFO empty
};

void putc(char c) {
    // wait for space in the transmit FIFO
    while(*UART0_reg(UART0_FR) & FR_TXFF) { }
    // add char to transmit FIFO
    *UART0_reg(UART0_DR) = c;
}

char getc(void) {
    // wait for data in the receive FIFO
    while(*UART0_reg(UART0_FR) & FR_RXFE) { }
    // extract char from receive FIFO
    return *UART0_reg(UART0_DR);
}

void puts(const char *str) {
    // putc until 0 byte
    while (*str) putc(*str++);
}
