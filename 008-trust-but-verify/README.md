Dotting all the i's and crossing all the t's
============================================

In the previous demos a lot was assumed and taken as granted. The
activity LED needs to be configured right, the UART needs to be
activated and setup right. The code also plays fast and loose
accessing peripherals without barriers. In upcoming demos the data and
instruction caches and branch prediction will be turned on as well as
the MMU and then things would break. So let's clean up the code and do
things right.

This demo also handles the init_array used for constructors and
demonstrates its use for the various init functions.

Background
==========

GPIO
----

The GPIO pins can be configured as input, output or one of 5
functions. For each pin there is also a pull up and pull down resistor
that can be activated. Changing the pull up/down resistors requires
toggling a clock register and holding the signal for at least 150
cycles, which is trickier than it sounds.

LEDs
----

All Raspberry Pi models have a power and activity LED. The activity
LED was already used in previous demos. But never configured. The
power LED can be controller too, at least on the Raspberry Pi 2, but
is not configured as GPIO output and has a pull-up or pull-down
configured to be permanently on. That can be reconfigured and this
demo does so. The relevant GPIO pin for the power LED on the Raspberry
Pi 2 is pin 35.

UART
----

The UART has a number of settings that control the BAUD rate, the
number of bits per character, the number of stop bits, the parity and
the use of FIFO. The interrupt behavior can also be changed. See the
source for details.

Peripherals
-----------

The BCM2835 system uses an AMBA AXI-compatible interface structure.
Don't know what that means? Don't worry. Here comes the important part
from the specs: "In order to keep the system complexity low and data
throughput high, the BCM2835 AXI system does not always return read
data in-order." The specs then go on limiting this saying that access
to the same peripheral will always arrive and return in-order. They
recommend:

  # A memory write barrier before the first write to a peripheral.
  # A memory read barrier after the last read of a peripheral.

Since interrupts can appear at any time they should be safeguarded. If
an interrupt routine reads from a peripheral the routine should start
with a memory read barrier. If an interrupt routine writes to a
peripheral the routine should end with a memory write barrier. That
way it won't mess up even when the interrupt fires in the middle of
code accessing some other peripheral.

Init_array
----------

Constructors are used to initialize global variables that aren't
initialized by simple constants. Functions can also be declared
constructors using __attribute__((constructor(priority))). The
.init_array section contains an array of function pointers that for
those constructors. Constructors must be called before main and sorted
by priority. The sorting is handled by the linker script so the
function pointers can just be called in the order they appear in the
array.
