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

/*
 * init priorities
 */

#ifndef KERNEL_INIT_PRIORITIES_H
#define KERNEL_INIT_PRIORITIES_H 1

enum INIT_PRIORITIES {
    INIT_ARCH_INFO  = 1000,
    INIT_LED,
    INIT_UART,
    INIT_EXCEPTIONS,
};

#define CONSTRUCTOR(name, pri)						\
void __attribute__((constructor(pri))) __CONCAT(name,_init)(void) {	\
    static_assert(__CONCAT(INIT_, name) == pri,				\
		  __STRING(name) " init priority mismatch");		\
    do

#define CONSTRUCTOR_END while(0); }

#endif // ##ifndef KERNEL_INIT_PRIORITIES_H
