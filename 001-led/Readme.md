Turn the activity LED repeadetly on and off forever
===================================================

This is about the simplest example to give using just 12 opcodes (or
48 bytes). To make this realy simple the code assumes the LED is
already configured by the firmware, which is the case so far. You can
see it blink briefly when you power on when the firmware initializes
it. If you don't see the later examples for how to fully configure it.

Background
==========

The activity LED on the Raspberry Pi is connected to one of the GPIO
pins of the cpu. At boot the pin is configured as output without
pull-up or pull-down resistors. An GPIO output can be either high or
low, which will make the LED light up or stay dark.

To change the output of a pin the GPIO peripheral provides two (sets
of) registers, the GPSETx and GPCLRx registers. Writing a 1 to any bit
in GPSETx register will switch the output corresponding to the bit to
high while writing a 1 to any bit in the GPCLRx register will switch
the output corresponding to the bit to low. All other outputs remain
as is. This means there is no need to load, modify and store a
register to preserve the state of other outputs. One simply sets a bit
in one or the other register (set).

Model differences
=================

The first difference is the PERIPHERAL_BASE. ARM uses memory mapped IO
to communicate with peripherals like the GPIO. That means the
registers of the peripherals simply appear like normal ram. You simply
read or write to the right address, defined here as PERIPHERAL_BASE.
The early Raspberry Pi came with 256MiB or 512MiB of memory and the
peripherals where mapped at 0x20000000, right after the maximum of
512MiB ram. The newer Raspberry Pi 2 now has 1GiB of ram, which would
have placed the peripherals right in the middle. Instead they where
moved up to 0x3F000000. This still overlaps the ram and some of that
is simply lost (16 MiB).

The second difference is how the LED is connected. This changed when
they board layout changed for the Raspberry PI b+ to accomodate 4 USB
connectors. Originaly the activity LED was connected to pin 16 of the
GPIO outputs. Now it is connected to pin 47 (bit 15 in the second
register).

The led.S file therefore has 3 sets of defines for the 3 cases:
Raspberry Pi, Raspberry Pi b+ and Raspberry Pi 2. The Makefile
compiles 3 images, one for each model.
