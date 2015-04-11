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

#ifndef KERNEL_UART_H
#define KERNEL_UART_H

#include <stdint.h>
#include <sys/cdefs.h>
#include "peripherals.h"

__BEGIN_DECLS
void putc(char c);
char getc(void);
void puts(const char *str);

void uart_putc(const PeripheralLock *prev, char c);
char uart_getc(const PeripheralLock *prev);
void uart_puts(const PeripheralLock *prev, const char *str);
__END_DECLS

#endif // ##ifndef KERNEL_UART_H
