Parse boot arguments and detect Raspberry Pi model
==================================================

At boot the Raspberry Pi firmware follows the old Linux boot protocol for
ARM systems called ATAG. Optionally the bootloader can use the newer
device tree but that is not the default so lets ignore that for
now. The demo will auto detect what Raspberry Pi model it is running
on, how much memory is available for the ARM cpu and where any initrd
is located and print that information.

Improvements
============

In this demo there are multiple include files and there is a bit more
meat to them. Specifically they declare external variables and
functions. Should those include files ever be included in a c++ source
the C++ name mangling would break things. To prevent this the
declarations are enclosed between __BEGIN_DECLS and __END_DECLS, which
are defined in <sys/cdefs.h>. Those macros enclose the declaration in
an extern "C" block if C++ is being used and do nothing otherwise.
cdefs.h also contains a few other macros which will be used later on.

Background
==========

Unfortunately all Raspberry Pi models use the same machine type
(0xC42) so that doesn't help figuring out what model is being
booted. But the command line passed to the kernel is sufficiently
different to make that destinction. And that's what is used in the
Demo. Specifically the definition of the activity LED (disk_led) is
used as that is different on each model:

* Raspberry Pi   : nothing or bcm2708.disk_led_gpio=16
* Raspberry Pi B+: bcm2708.disk_led_gpio=47
* Raspberry Pi 2 : bcm2709.disk_led_gpio=47

The code defaults to a Raspberry Pi unless one of the other two
strings is detected. That way it doesn't matter what the original
says.

Environment at boot
-------------------

The ATAG boot protocol defines a sane state for the system to be in
before calling the kernel. Namely this is:

* The CPU must be in SVC (supervisor) mode with both IRQ and FIQ interrupts disabled.
* The MMU must be off, i.e. code running from physical RAM with no translated addressing.
* Data cache must be off
* Instruction cache may be either on or off
* CPU register 0 must be 0
* CPU register 1 must be the ARM Linux machine type
* CPU register 2 must be the physical address of the parameter list

The bootloader is expected to call the kernel image by jumping
directly to the first instruction of the kernel image.

ATAGs
-----

The Raspberry Pi firmware uses only 4 tags and only those are
supported in the demo. See arch_info.cc for the tags in question.

More complete information about ATAGs can be found here:
http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html



Model differences
=================

The model differences are abstracted away into arch_info.cc. All other
code uses the interface defined in arch_info.h and is otherwise
independent of the Raspberry Pi model. From now on all demos will use
this.

