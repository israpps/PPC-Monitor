PATCH_DIR := $(abspath $(CURDIR)/../patch)
EE_BIN = unc_ee_loader.elf
EE_BIN_PKD = ee_loader.elf
EE_OBJS_DIR = obj/
EE_SRC_DIR = src/
EE_ASM_DIR = asm/

NEWLIB_NANO = 1

EE_LIBS = -ldebug -lpatches

EE_OBJS = main.o
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

# Add embedded IRX files
EE_IRX_FILES =\
	ppcpatchman.irx \

EE_IRX_OBJS = $(addsuffix _irx.o, $(basename $(EE_IRX_FILES)))
EE_OBJS += $(EE_IRX_OBJS)

# Where to find the IRX files
vpath %.irx $(PS2SDK)/iop/irx/
vpath %.irx $(ROOT_DIR)/../patch_loader_irx/irx/

# Rule to generate them
%_irx.o: %.irx
	bin2c $< $*_irx.c $*_irx
	mips64r5900el-ps2-elf-gcc -c $*_irx.c -o $*_irx.o

all: $(EE_BIN_PKD)

$(EE_BIN_PKD): $(EE_BIN)
	ps2-packer $< $@

clean:
	rm -f -r $(EE_OBJS) $(EE_BIN) *_irx.c

EE_CFLAGS += -DMAJOR=$(PPC_MAJOR) -DMINOR=$(PPC_MINOR) -DREVISION=$(PPC_REVISION) -DGITHASH=\"$(COMMIT_HASH)\"

include $(PATCH_DIR)/version.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal