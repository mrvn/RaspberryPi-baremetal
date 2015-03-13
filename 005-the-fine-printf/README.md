Introducing the family of printf functions
==========================================

So far only limited output was done using putc, puts and put_uint32.
But being able to output formatted text through one of the family of
printf functions is invaluable.

The central function used in this demo is vcprintf(), which uses a
callback and a state to store or output characters one at a time. All
other functions are calls to vcprintf() in various forms. The
(v)snprintf functions use a callback that stores chars in the output
buffer while kprintf uses the UARTs putc() as callback.

The advantage of this design is that it works for both printing to a
buffer and printing to the UART directly with the same code. Other
implementations place a buffer on the stack, snprintf() to that and
finally write the buffer to the UART. This requires a larger stack for
the kernel to accommodate the temporary buffer and limits the amount
of text that can be printed through kprintf() at a time. Using the
callback keeps the stack usage to a minimum and has no limit on the
output size.

Background
==========

The family of printf functions introduces the concept of variable
number of arguments. Depending on the contents of the format string 0
or more arguments follow the format string. Accessing those arguments
at runtime is highly dependent on the calling conventions of the
architecture. To support this in a portable way requires direct
support from the compiler, which is defined in stdarg.h. The functions
defined in stdarg.h handle all the magic of getting arguments out of
registers or from the stack as determined by the calling convention so
you don't have to care about the details. See "man 3 stdarg" for a
complete description of the interface.

The demo also introduces a gcc extension that can be invaluable. The
printf.h file declares the kprintf, snprintf and cprintf functions ad
__PRINTFLIKE, which is a macro defined at the top. What that does is
tell gcc that the functions are like printf in that they take a format
string and variable number of arguments. The compiler then checks the
format string against the arguments supplied to any of the functions
at compile time and issues warnings if there are too few or to many
arguments or if any of the arguments have the wrong type for the
format specifier used in the format string. If you ever used printf in
normal C code then I'm sure you will have seen those errors
before. The __PRINTFLIKE macro just tells gcc to produce the same
warnings for these functions too.
