ARCH ?= arm-none-eabi

ifeq (,$(filter _%,$(notdir $(CURDIR))))

# trick to automatically switch to OBJDIR and build there
include target.mk

else

# only reached when inside OBJDIR

# Set VERBOSE if you want to see the commands being executed
ifdef VERBOSE
  L = @:
  Q =
else
  L = @echo
  Q = @
  MAKEFLAGS += --no-print-directory
endif
export L Q

# Tell make where to find source files since we are in OBJDIR
VPATH = $(SRCDIR)

# find all Makefile.obj for later processing
MAKEFILE_MKS := $(patsubst %.obj,%.mk,$(shell cd "$(SRCDIR)" && find * -name "Makefile.obj" | sed -e 's,/,__,g' -e 's,^,_mk/,'))

# full name of tools
CROSS   ?= $(ARCH)-
AS      = $(CROSS)as
CC      = $(CROSS)gcc
CXX     = $(CROSS)g++
LD      = $(CROSS)ld
OBJCOPY = $(CROSS)objcopy


# no floating point at all, use software emulation
ARCHFLAGS := -mcpu=arm1176jzf-s -mfloat-abi=soft

# flags when using floating point (pass as soft but use fpu)
# Raspberry Pi
#ARCHFLAGS := -mcpu=arm1176jzf-s -mfloat-abi=softfp -mfpu=vfp
# Raspberry Pi 2
#ARCHFLAGS := -march=armv7-a -mfloat-abi=softfp -mfpu=neon

ASFLAGS  := -ffreestanding -nostdlib -D ASSEMBLY
CFLAGS   := -ffreestanding -nostdlib -O2 -W -Wall -std=gnu99
CXXFLAGS := -ffreestanding -nostdlib -O2 -W -Wall -std=g++11
LDFLAGS  := -ffreestanding -nostdlib

# no user serviceable parts below
DEPENDFLAGS := -MD -MP
INCLUDE := -I$(SRCDIR) -I$(SRCDIR)/include

ASFLAGS  := $(ARCHFLAGS) $(DEPENDFLAGS) $(INCLUDE) $(ASFLAGS)
CFLAGS   := $(ARCHFLAGS) $(DEPENDFLAGS) $(INCLUDE) $(CFLAGS)
CXXFLAGS := $(ARCHFLAGS) $(DEPENDFLAGS) $(INCLUDE) $(CXXFLAGS)

all: kernel.img

# objcopy from elf to binary format for a bootable image
%.img: %.elf
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) OBJCOPY $@
	$(Q) $(OBJCOPY) $< -O binary $@

# final link pass
%.elf: link-arm-eabi.ld _built-in-y.o
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) LINKING $@
	$(Q) $(CC) $(LDFLAGS) -T$(SRCDIR)/link-arm-eabi.ld -o $@ _built-in-y.o -lgcc

# assembly file
%.o: %.S
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) AS $@ '$(TXT)'
	$(Q) $(CC) $(ASFLAGS) -c -o $@ $<

# C file
%.o: %.c
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) CC $@ '$(TXT)'
	$(Q) $(CC) $(CFLAGS) -c -o $@ $<

# C++ file
%.o: %.cc
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) CXX $@ '$(TXT)'
	$(Q) $(CXX) $(CXXFLAGS) -c -o $@ $<

# merge all object files and build-in-y.o of subdirs into build-in-y.o
%built-in-y.o:
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) GATHER $@
	$(Q) $(LD) -i -o $@ $+

# generate Makefiles for all diretories
%Makefile.mk: scripts/gen-Makefile.mk
	@[ -d _mk ] || mkdir -p "_mk"
	$(L) MK $@
	$(Q) $(SRCDIR)/scripts/gen-Makefile.mk "$(SRCDIR)" "$@"

# don't try to build the script itself
scripts/gen-Makefile.mk: ;

# include all the generated Makefiles and automatic dependencies
-include $(MAKEFILE_MKS)
-include *.d

# stop make from deleting the intermediate files
.PRECIOUS: %.elf %.o

endif
