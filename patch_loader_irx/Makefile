BUILD_SCRIPTS := $(abspath $(CURDIR)/../buildscripts)
PATCH_DIR := $(abspath $(CURDIR)/../patch)

IOP_BIN = irx/ppcpatchman.irx

IOP_OBJS += patch.o imports.o patch_loader.o

all:: $(IOP_BIN)

clean::
	rm -rf obj
	rm -rf irx
	rm -f *.s
	rm -f *.map

$(PATCH_DIR)/bin/patch.bin: $(PATCH_DIR)/bin/patch.elf
	powerpc-ps2-eabi-objcopy -O binary $< $@

patch.c: $(PATCH_DIR)/bin/patch.bin
	$(PS2SDK)/bin/bin2c $< $@ patch_bin

include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal