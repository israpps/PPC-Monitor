BUILD_SCRIPTS := $(abspath $(CURDIR)/../buildscripts)
PATCH_DIR := $(abspath $(CURDIR)/../patch)

IOP_BIN = irx/patch_loader.irx

IOP_OBJS += patch.o imports.o patch_loader.o

BIN2S = $(PS2SDK)/bin/bin2s

all::
	powerpc-ps2-eabi-objcopy -O binary $(PATCH_DIR)/bin/patch.elf $(PATCH_DIR)/bin/patch.bin
	$(BIN2S) $(PATCH_DIR)/bin/patch.bin patch.s patch_bin
	$(MAKE) $(IOP_BIN)

clean::
	rm -rf obj
	rm -rf irx
	rm -f *.s
	rm -f *.map

include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal