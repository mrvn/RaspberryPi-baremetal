Using the timer interrupt to sleep
==================================

In the last demo the exception vector was introduced to catch and
explain misbehaving code. But that also opens up the way to catch and
handle interrupts. This demo uses the timer interrupt to turn the LEDs
on the Raspberry Pi on and off at precise intervals. Yes, you read
right, LEDs, plural. The power LED on the Raspberry Pi 2 can be
software controlled to if the GPIO pins are reconfigured a bit.

Background
==========

LEDs
----

All Raspberry Pi models have a power and activity LED. The activity
LED is setup at boot to be controlled by setting the GPIO output of
the pin it is connected to to high or low. The power LED on the other
hand, at least on the Raspberry Pi 2, is not configured as GPIO output
and may have a pull-up or pull-down configured. But that can be
reconfigured. The relevant GPIO pin for the power LED on the Raspberry
Pi 2 is pin 35.

IRQs
----

The Raspberry Pi has receives interrupts from 3 sources, the GPU, the
VC and the ARM peripherals. These interrupts are split up over 3
registers each with a pending, enable and disable flavour. A lot of
the interrupts are used by the GPU/VC and should not be interfered
with. The basic pending register has all the arm peripherals
connected, some select GPU/VC interrupts and 2 bits indicating the 2
remaining pending registers have a bit set that is not already present
in the basic pending register. We are interested only in the timer
interrupt, which is bit 0 in the basic pending register. All
interrupts are level sensitive so they remain asserted until disabled
or the interrupt source is cleared.

The base address for ARM interrupt registers is at offset 0x0000B000
relative to the peripheral base address.

System Timer
------------

The system timer peripheral has a 64bit free running counter, split
into high and low 32bit, counting upwards from 0 on power on at 1
MHz. It also has 4 32bit compare registers. Every time the lower 32bit
of the timer match on of the 4 compare registers an interrupt
fires. Two of the compare registers are used by the VC chip and should
not be interfered with. The other 2 are free for use from the ARM
side.

The base address for system timer registers is at offset 0x00003000
relative to the peripheral base address.

This is not to be confused with the ARM timer at offset 0x0000B000
(same as IRQs), which is derived form the system clock and can change
dynamically, e.g. if the system goes into reduced power or in low
power mode. Given the variability the timer is no recommended for
accurate timing. The system timer runs always at a fixed frequency.

