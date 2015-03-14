Add exception handling
======================

This demonstrates the use of the exception vectors and the exception
base address to catch errors and display meaningfull diagnostics. This
will later also be used to handle interrupts.

Background
==========

When an exception occurs the cpu jumps to one of the entries of the
exception vector. Depending on what exception triggers this the
execution mode is switched, interrupts and fast interrupts and various
other flags can be changed. Generally each exceptions disables the
lower priority ones and itself. So for example an interrupt does not
disable fast interrupts but disables additional interrupts. The old
cpu state (CPSR) is saves in the SPSR and the return address is stored
in the link register. For more details see the source or the ARM arm.

As said on exception the cpu jumps to one of the entries of the
exception vector. The location of the exception vector is
configurable. At boot it's located at 0x00000000. This can be changed
by setting the high vector bit in the System Control Register. Setting
this bis changes the base address of the exception vector to
0xFFFF0000. ARM cpus that implement the Security Extensions also have
another way to change the base address of the execption vector that is
used when the high vector bit is not set, the VBAR (Vector Base
Address Register). The VBAR holds the base address of the exception
vector. This must be 32 byte aligned. The Raspberry Pis do implement
the security extensions and therefore have a VBAR. As this is the most
flexible way for the exception vector base address this is what the
demo uses.

Exception Vector
----------------

Unlike on other architectures the exception vector does not hold the
address of an exception handler. Instead the cpu jump to the vector
and executes it. Since one can't handle an exception with a single
instruction (including returning from it) there is usualy a jump
instruction stored in the entry that jumps to the actual handler.

Banked Registers
----------------

Depending on the processor mode some registers are banked. Which means
the mode uses a seperate register only available in that mode in place
of the normal one. Most modes have a banked SP (stack pointer) and LR
(link register). The FIQ mode also has banked registers for
R8-R12. The stack pointer, if stack is going to be used in the
execption, must be set before use. The link register on the other hand
is set when an execption occurs. Because it is banked this does not
overwrite the link register of the previous mode, assuming the mode is
switched. The link register must be saved somehwere if there is a
chance the same mode could be reentered. For example if interrupts are
to be enabled again inside an interrupt handler a second interrupt
would overwrite the link register for the IRQ mode. The same goes for
the SPSR register. Luckily the cpu provides some usefull instructions
to save those, even save them on the stack of a different mode. This
allows a complex interrupt handler to easily save them on the
supervisor stack, switche to SVC mode and reenabling interrupts. In
the demo the handler only prints diagnostics so this is not
demonstrated.
