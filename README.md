Bare-metal examples for the Raspberry Pi / Pi b+ / Pi 2
======================================================

This repository contains a series of bare-metal examples for the
Raspberry Pi (http://raspberrypi.org/). The examples are numbered from
simple to complex and many of the later examples reuse the earlier
ones, especially the UART.

Each example can be build running make inside it's directory using a
cross compiler prefixed with "arm-none-eabi-", e.g. arm-none-eabi-gcc,
which must be in the PATH. You can export CROSS or CC and CXX to use a
different compiler. You can also run make in the top directory to
build them all.

Each example builds some object files, links them into a *.elf file
and creates a bootable *.img file from that. The *.img file you can
copy to your SD card to try it. The first few example are build in
multiple flavors for the different Raspberry Pi models and you have
to choose the one for your Raspberry Pi. If you pick the wrong one
nothing bad will happen but they also just wont work. After the
auto-detect example everything will detect which Raspberry Pi model is
being used automatically (that I know of at the time of writing).

Enjoy,
	Goswin von Brederlow

PS: Checkout raspbootin if you don't want to take out the SD card
for every new image you want to try. It allows you to upload the image
over serial.

List of examples
================

| Directory  | Description |
| ---------- | ----------- |
| 001-twinkle-twinkle-little-led | Turn the activity LED repeadetly on and off forever |
| 002-hello-lo-lo-lo             | Output some text on the UART and then echo all input |
| 003-can-you-c-me               | Output some text on the UART and then echo input (in C) |
