OBJS := boot.o arch_info.o uart.o main.o

CROSS ?= arm-none-eabi-
CC    ?= $(CROSS)gcc

# no floating point at all, use software emulation
ARCHFLAGS := -mcpu=arm1176jzf-s -mfloat-abi=soft

# flags when using floating point (pass as soft but use fpu)
ARCH_RPI  := -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp
ARCH_RPI2 := -march=armv7-a -mfloat-abi=softfp -mfpu=neon

ASFLAGS := -ffreestanding -nostdlib -D ASSEMBLY
CFLAGS  := -ffreestanding -nostdlib -O2 -W -Wall -std=gnu99

# no user serviceable parts below
DEPENDFLAGS := -MD -MP
INCLUDE := -I .
FLAGS := $(ARCHFLAGS) $(DEPENDFLAGS) $(INCLUDE) 

all: kernel.img

%.img: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: link-arm-eabi.ld $(OBJS)
	arm-none-eabi-ld -Tlink-arm-eabi.ld -o $@ $(OBJS)

%.o: %.S
	arm-none-eabi-gcc $(FLAGS) $(ASFLAGS) -c -o $@ $<

%.o: %.c
	arm-none-eabi-gcc $(FLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f kernel*.img kernel*.elf *.o *~ *.d

-include *.d

# stop make from deleting the intermediate files
.PRECIOUS: %.elf %.o
