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
 * ISO C99 Standard: 7.2 Diagnostics       <assert.h>
 */

#ifndef  _ASSERT_H
#define _ASSERT_H       1

#include <sys/cdefs.h>

#if defined __cplusplus
# define __ASSERT_VOID_CAST static_cast<void>
#else
# define __ASSERT_VOID_CAST (void)
#endif

__BEGIN_DECLS
// This prints an "Assertion failed" message and blinks the LEDs madly
extern void __assert_fail (const char *__assertion, const char *__file,
                           unsigned int __line, const char *__function)
     __THROW __attribute__ ((__noreturn__));
__END_DECLS

# define assert(expr)                                                   \
  ((expr)                                                               \
   ? __ASSERT_VOID_CAST (0)                                             \
   : __assert_fail (__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__))

#if !defined __cplusplus
/* Static assertion.  Requires support in the compiler.  */
# undef static_assert
# define static_assert _Static_assert
#endif

#endif // ##ifndef _ASSERT_H
