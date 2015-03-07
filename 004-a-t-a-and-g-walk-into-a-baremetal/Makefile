OBJS := boot.o arch_info.o uart.o main.o

CROSS ?= arm-none-eabi-
CC    ?= $(CROSS)gcc

ARCH_RPI  := -mcpu=arm1176jzf-s -mhard-float -mfpu=vfp
ARCH_RPI2 := -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16

ASFLAGS := -ffreestanding -nostdlib -D ASSEMBLY
CFLAGS  := -ffreestanding -nostdlib -O2 -W -Wall -std=gnu99

# no user seviceable parts below

all: kernel.img

%.img: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: link-arm-eabi.ld $(OBJS)
	arm-none-eabi-ld -Tlink-arm-eabi.ld -o $@ $(OBJS)

%.o: %.S
	arm-none-eabi-gcc $(ARCH_RPI) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	arm-none-eabi-gcc $(ARCH_RPI) $(CFLAGS) -D MODEL=0 -c -o $@ $<

clean:
	rm -f kernel*.img kernel*.elf *.o *~

# stop make from deleting the intermediate files
.PRECIOUS: %.elf %.o
