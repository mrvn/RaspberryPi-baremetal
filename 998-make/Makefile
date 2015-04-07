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

BASEFLAGS   :=-ffreestanding -nostdlib -O2 -W -Wall -g -fvisibility=hidden
ASFLAGS     := $(BASEFLAGS) -D ASSEMBLY 
CFLAGS      := $(BASEFLAGS) -std=gnu99
CXXFLAGS    := $(BASEFLAGS) -std=g++11
LDFLAGS     := $(BASEFLAGS)

export COLLECT_GCC := $(CC)
export COLLECT_GCC_OPTIONS := $(BASEFLAGS)

LTO_PLUGIN  := $(shell $(CC) --print-file-name liblto_plugin.so)
LTO_WRAPPER := $(shell $(CC) --print-prog-name lto-wrapper)
LD_NO_LTO  = $(CROSS)ld -plugin $(LTO_PLUGIN) -plugin-opt=$(LTO_WRAPPER)
LD_NO_LTO += -plugin-opt=-fresolution=$@.res

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
%.elf: link-arm-eabi.ld _tmp-y.lto _tmp-y.o
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) LINKING $@
	$(Q) $(CC) $(LDFLAGS) -T$(SRCDIR)/link-arm-eabi.ld -o $@ -Wl,--start-group _tmp-y.lto _tmp-y.o -lgcc -Wl,--end-group

# assembly file
%.o: %.S
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) AS $@ '$(ASFLAGS_TXT)'
	$(Q) $(CC) $(ASFLAGS) -c -o $@ $<

# C file
%.lto: %.c
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) CC $@ '$(CFLAGS_TXT)'
	$(Q) $(CC) $(CFLAGS) -flto -c -o $@ $<

# C file
%.o: %.c
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) CC $@ '$(CFLAGS_TXT)'
	$(Q) $(CC) $(CFLAGS) -c -o $@ $<

# C++ file
%.lto: %.cc
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) CXX $@ '$(CXXFLAGS_TXT)'
	$(Q) $(CXX) $(CXXFLAGS) -flto -c -o $@ $<

# C++ file
%.o: %.cc
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) CXX $@ '$(CXXFLAGS_TXT)'
	$(Q) $(CXX) $(CXXFLAGS) -c -o $@ $<

_empty.c:
	$(L) CREATE $@
	$(Q) touch $@

_empty.o: _empty.c
	$(L) CC $@
	$(Q) $(CC) -c -o $@ $<

_empty.lto: _empty.c
	$(L) CC $@
	$(Q) $(CC) $(LTO) -c -o $@ $<

# merge all object files and _tmp-y.lto of subdirectories into _tmp-y.lto
# empty if DOLTO or LDSCRIPT is specified for that directory
%tmp-y.lto:
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) GATHER $@ '$(LD_TXT)'
	$(Q) $(LOCAL_LD) $(LDSCRIPT) -i -o $@ --start-group $+ --end-group

# merge all object files and _tmp-y.o of subdirectories into _tmp-y.o
# also merges _tmp-y.lto of subdirectories if DOLTO or LDSCRIPT is specified
# for that directory
%tmp-y.o:
	@[ -d $@ ] || mkdir -p $(dir $@)
	$(L) GATHER $@ '$(LD_TXT)'
	$(Q) $(LOCAL_LD) $(LDSCRIPT) -i -o $@ --start-group $+ --end-group

# generate Makefiles for all diretories
%Makefile.mk: scripts/gen-Makefile.mk
	@[ -d _mk ] || mkdir -p "_mk"
	$(L) MK $@
	$(Q) $(SRCDIR)/scripts/gen-Makefile.mk "$(SRCDIR)" "$@"

# don't try to build the script itself
scripts/gen-Makefile.mk: ;

# include all the generated Makefiles and automatic dependencies
-include $(MAKEFILE_MKS)
-include $(shell find -name "*.d")

# stop make from deleting the intermediate files
.PRECIOUS: %.elf %.o

endif
