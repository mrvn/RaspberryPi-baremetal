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

/* Exceptions related functions
 */

#ifndef KERNEL_EXCEPTIONS_H
#define KERNEL_EXCEPTIONS_H

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS
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

void exceptions_init(void);
void dump_regs(const Regs *regs);
__END_DECLS

#endif // ##ifndef KERNEL_EXCEPTIONS_H
