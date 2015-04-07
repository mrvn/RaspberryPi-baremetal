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

/* Interrupt related functions
 */

#ifndef KERNEL_IRQ_H
#define KERNEL_IRQ_H

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS
typedef struct Regs Regs;
EXPORT void handler_irq(Regs *regs, uint32_t num);
EXPORT void handler_fiq(Regs *regs, uint32_t num);
EXPORT void enable_irqs(void);
EXPORT void disable_irqs(void);

enum IRQ {
    //IRQ_TIMER0               =  0, // GPU used
    IRQ_TIMER1               =  1,
    //IRQ_TIMER2               =  2, // GPU used
    IRQ_TIMER3               =  3,
    IRQ_AUX                  = 29,
    IRQ_I2C_SPI_SLV          = 43,
    IRQ_PWA0                 = 45,
    IRQ_PWA1                 = 46,
    IRQ_SMI                  = 48,
    IRQ_GPIO0                = 49,
    IRQ_GPIO1                = 50,
    IRQ_GPIO2                = 51,
    IRQ_GPIO3                = 52,
    IRQ_I2C                  = 53,
    IRQ_SPI                  = 54,
    IRQ_PCM                  = 55,
    IRQ_UART                 = 57,
    IRQ_FB                   = 62,
    IRQ_ARM_TIMER            = 64,
    IRQ_ARM_MAILBOX          = 65,
    IRQ_ARM_DOORBELL0        = 66,
    IRQ_ARM_DOORBELL1        = 67,
    IRQ_GPU0_HALTED          = 68,
    IRQ_GPU1_HALTED          = 69,
    IRQ_ILLEGAL_ACCESS_TYPE1 = 70,
    IRQ_ILLEGAL_ACCESS_TYPE0 = 71,
};

EXPORT void enable_irq(enum IRQ irq);
EXPORT void disable_irq(enum IRQ irq);
__END_DECLS

#endif // ##ifndef KERNEL_IRQ_H
