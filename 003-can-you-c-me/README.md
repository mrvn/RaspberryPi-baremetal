Output some text on the UART and then echo input (in C)
=======================================================

The code assumes the UART is already configured by the firmware, which
is the case so far. The UART is configured to 115200 BAUD, 8 bit, not
even parity and 1 stop bit (8N1).

The demo starts setting up a minimal C runtime environment, calls the
kernel_main() C function and rapidly blinks the LED when it returns.

The kernel_main() C function outputs a greeting to the UART and asks
the user to input 5 characters, which it echoes back. The code aborts
early if return is pressed, calling the panic function from boot.S.

Improvements
============

This demo is the first to include a header file that is included by
other source files. This makes it a prerequisite for the files
including it. Every time the header file changes the source files
including it should be rebuild. This can be added manually as
dependencies in the Makefile but that is tiresome and prone to
errors. Instead a feature of gcc is used to generate dependencies
automatically as a side effect of compiling the source. In the
Makefile you may notice 'DEPENDFLAGS := -MD -MP'. These two options
tell gcc to create a dependency file (*.d) in GNU Make syntax each
time it compiles a source file. And at the bottom of the Makefile
there is '-include *.d' which tells make to include any *.d file it
finds and not give any errors if none are found, which will be the
case if the source has never been compiled before or recently cleaned.

Background
==========

C runtime environment
---------------------

C code requires a little setup before it can be called. Normally this
is done by the C runtime startup code. But since we are working
bare-metal we have to provide this ourself. At a minimum C code needs
a valid stack. It also assumes that uninitialized static and global
variables, those being placed in the .bss section by the compiler,
will be filled with zero. The boot.S file handles that before calling
kernel_main().

Makefile
--------

The Makefile has grown compared to the previous demo. The reason for
this is that now 2 files need to be compiled and linked and the
pattern matching used in the first 2 examples is insufficient for
this. If you don't fully understand the Makefile then don't worry
about it. It gets simpler again when a single kernel.img is build in
future demos instead of 3 separate flavors.

What you should take away from the Makefile is the use of ASFLAGS and
CFLAGS. Instead of repeating those flag for every gcc invocation they
are defined once at the top and then reused. Makes it easier to see
what flags will be used and to change them.

Linker script
-------------

The linker script has also grown. Before we only needed a .text
section for the assembly code and we threw everything in there. With C
code the compiler places code and different types of data in different
sections and the important ones where added.

The .text section has also got a new entry 'KEEP(*(.text.boot))'
corresponding with boot.S using '.section ".text.boot"' for its
code. It is important to know that when the linker links multiple
files it can place them in any order. Normally the order of the files
on the command line is used within each section but that is not
guaranteed. But the code from boot.s MUST come first. By moving the
code to the .text.boot section and specifying explicitly that
.text.boot comes first the proper placement is enforced.

Last 2 new symbols are declared in the linker script: _bss_start and
_bss_end. These are set to the start and end address of the bss
section and are used in boot.S to clear that section. In later demos
you will see how such symbols can also be accessed from C code.

main.c
------

Notice the use of the volatile keyword for the peripheral() function.
This has something to do with compiler optimizations. As to why I
declared a peripheral function as opposed to simply declaring a
pointer in putc()/getc() will become apparent in the next demo.

Normally when a pointer is dereferenced twice and the compiler can
proof (to itself) that nothing in between the two instances could have
changed the pointer then it doesn't have to load the value again from
memory. Instead the previously value can be reused. Also a value
stored in a variable (or through a pointer) doesn't actually have to
be stored at all if the compiler can proof (to itself) that it's never
read back again. The volatile keyword tells the compiler that the
value can change by itself and needs to be loaded fresh every time it
is used and explicitly written to memory every time it is
changed. Without volatile the compiler would be within its rights (and
likely to do so) to optimize the access to the peripheral away and
the UART wouldn't work right.

Model differences
=================

The difference in the PERIPHERAL_BASE mention in 001-led still
applies. Other than that there is no difference between the models.
This time those differences are hidden in "arch_info.h", which is
included by both the boot.S and main.c files.
