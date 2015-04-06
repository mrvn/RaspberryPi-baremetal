OBJS := boot.o arch_info.o assert.o gpio.o led.o uart.o kprintf.o
OBJS += entry.o exceptions.o irq.o timer.o main.o

CROSS   ?= arm-none-eabi-
CC      := $(CROSS)gcc
OBJCOPY := $(CROSS)objcopy

# no floating point at all, use software emulation
ARCHFLAGS := -mcpu=arm1176jzf-s -mfloat-abi=soft

# flags when using floating point (pass as soft but use fpu)
ARCH_RPI  := -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp
ARCH_RPI2 := -march=armv7-a -mfloat-abi=softfp -mfpu=neon

ASFLAGS := -ffreestanding -nostdlib -D ASSEMBLY
CFLAGS  := -ffreestanding -nostdlib -O2 -W -Wall -std=gnu11
LDFLAGS := -ffreestanding -nostdlib

# no user serviceable parts below
DEPENDFLAGS := -MD -MP
INCLUDE := -I .
FLAGS := $(ARCHFLAGS) $(DEPENDFLAGS) $(INCLUDE)

all: kernel.img

%.img: %.elf
	$(OBJCOPY) $< -O binary $@

%.elf: link-arm-eabi.ld $(OBJS)
	$(CC) $(LDFLAGS) -Tlink-arm-eabi.ld -o $@ $(OBJS) -lgcc

%.o: %.S
	$(CC) $(FLAGS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(FLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f kernel*.img kernel*.elf *.o *~ *.d

-include *.d

# stop make from deleting the intermediate files
.PRECIOUS: %.elf %.o
