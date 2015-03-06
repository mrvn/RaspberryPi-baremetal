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

/* Information about the differe Raspberry Pi models
 */

#ifndef KERNEL_ARCHINFO_H
#define KERNEL_ARCHINFO_H

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#ifndef MODEL
#error MODEL not defined
#endif

#if MODEL == 0
#define PERIPHERAL_BASE 0x20000000 // Raspberry Pi Peripheral Base Address
#define GPBASE            0x200000 // $20200000
#define GPSETx                0x1C // $2020001C GPSET0
#define GPCLRx                0x28 // $20200028 GPCLR0
#define PIN                     16 // LED is connected to pin 16
#endif

#if MODEL == 1
#define PERIPHERAL_BASE 0x20000000 // Raspberry Pi B+ Peripheral Base Address
#define GPBASE            0x200000 // $20200000
#define GPSETx                0x20 // $20200020 GPSET1
#define GPCLRx                0x2C // $2020002C GPCLR1
#define PIN                     15 // LED is connected to pin 47 (32 + 15)
#endif

#if MODEL == 2
#define PERIPHERAL_BASE 0x3F000000 // Raspberry Pi 2 Peripheral Base Address
#define GPBASE            0x200000 // $3F200000
#define GPSETx                0x20 // $3F200020 GPSET1
#define GPCLRx                0x2C // $3F20002C GPCLR1
#define PIN                     15 // LED is connected to pin 47 (32 + 15)
#endif

#ifndef PERIPHERAL_BASE
#error MODEL must be one of 0 (RPi), 1 (RPi b+) or 2 (RPi 2)
#endif

#ifdef __cplusplus
#ifdef 0
{
#endif
}
#endif

#endif // ##ifndef KERNEL_ARCHINFO_H
