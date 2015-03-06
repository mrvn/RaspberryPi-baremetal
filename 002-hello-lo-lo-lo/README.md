Output some text on the UART and then echo all input
====================================================

The code assumes the UART is already configured by the firmware, which
is the case so far. The UART is configured to 115200 BAUD, 8 bit, not
even parity and 1 stop bit (8N1).

The demo starts by transmitting a greeting. After that it waits for a
char to be received and echos that right back.

Background
==========

UART0 is a PL011 UART located at offset 0x201000 relative to the
PERIPHERAL_BASE which is similar to a 16C650 UART. PL011 is a standard
UART used in most ARM devices. Among other features the UART has a
receive and transmit FIFO and a status register with bits signaling
when the receive FIFO is empty or the transmit FIFO is full. This
allows checking for when a character was received or when one can be
send. There is also a data register, which returns a received char on
read and adds a char to the transmit FIFO on write.

That is all that is being used for now.

Model differences
=================

The difference in the PERIPHERAL_BASE mention in 001-led still
applies. Other than that there is no difference between the models.
