OBJS := boot.o main.o

CROSS ?= arm-none-eabi-
CC    ?= $(CROSS)gcc

ARCH_RPI  := -mcpu=arm1176jzf-s -mhard-float -mfpu=vfp
ARCH_RPI2 := -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16

ASFLAGS := -ffreestanding -nostdlib -D ASSEMBLY
CFLAGS  := -ffreestanding -nostdlib -O2 -W -Wall -std=gnu99

# no user seviceable parts below

OBJS_RPI  := $(patsubst %.o,%-rpi.o,$(OBJS))
OBJS_RPIB := $(patsubst %.o,%-rpi-b-plus.o,$(OBJS))
OBJS_RPI2 := $(patsubst %.o,%-rpi2.o,$(OBJS))

all: kernel-rpi.img kernel-rpi-b-plus.img kernel-rpi2.img

%.img: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%-rpi.elf: link-arm-eabi.ld $(OBJS_RPI)
	arm-none-eabi-ld -Tlink-arm-eabi.ld -o $@ $(OBJS_RPI)

%-rpi-b-plus.elf: link-arm-eabi.ld $(OBJS_RPIB)
	arm-none-eabi-ld -Tlink-arm-eabi.ld -o $@ $(OBJS_RPIB)

%-rpi2.elf: link-arm-eabi.ld $(OBJS_RPI2)
	arm-none-eabi-ld -Tlink-arm-eabi.ld -o $@ $(OBJS_RPI2)

%-rpi.o: %.S
	arm-none-eabi-gcc $(ARCH_RPI) $(ASFLAGS) -D MODEL=0 -c -o $@ $<

%-rpi.o: %.c
	arm-none-eabi-gcc $(ARCH_RPI) $(CFLAGS) -D MODEL=0 -c -o $@ $<

%-rpi-b-plus.o: %.S
	arm-none-eabi-gcc $(ARCH_RPI) $(ASFLAGS) -D MODEL=1 -c -o $@ $<

%-rpi-b-plus.o: %.c
	arm-none-eabi-gcc $(ARCH_RPI) $(CFLAGS) -D MODEL=1 -c -o $@ $<

%-rpi2.o: %.S
	arm-none-eabi-gcc $(ARCH_RPI2) $(ASFLAGS) -D MODEL=2 -c -o $@ $<

%-rpi2.o: %.c
	arm-none-eabi-gcc $(ARCH_RPI2) $(CFLAGS) -D MODEL=2 -c -o $@ $<

clean:
	rm -f kernel*.img kernel*.elf *.o *~

# stop make from deleting the intermediate files
.PRECIOUS: %-rpi.elf %-rpi-b-plus.elf %-rpi2.elf %-rpi.o %-rpi-b-plus.o %-rpi2.o
