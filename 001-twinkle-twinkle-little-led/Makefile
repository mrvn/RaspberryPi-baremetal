CROSS ?= arm-none-eabi-
CC    ?= $(CROSS)gcc

all: led-rpi.img led-rpi-b-plus.img led-rpi2.img

%.img: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: %.o link-arm-eabi.ld
	arm-none-eabi-ld -Tlink-arm-eabi.ld -o $@ $<

%-rpi.o: %.S
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -mhard-float -mfpu=vfp \
	                  -ffreestanding -nostdlib -D MODEL=0 \
	                  -c -o $@ $<

%-rpi-b-plus.o: %.S
	arm-none-eabi-gcc -mcpu=arm1176jzf-s -mhard-float -mfpu=vfp \
	                  -ffreestanding -nostdlib -D MODEL=1 \
	                  -c -o $@ $<

%-rpi2.o: %.S
	arm-none-eabi-gcc -march=armv7-a -mfloat-abi=hard -mfpu=vfpv3-d16 \
	                  -ffreestanding -nostdlib -D MODEL=2 \
	                  -c -o $@ $<

clean:
	rm -f led*.img led*.elf led*.o *~

# stop make from deleting the intermediate files
.PRECIOUS: %.elf %-rpi.o %-rpi-b-plus.o %-rpi2.o
