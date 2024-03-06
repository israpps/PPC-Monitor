BUILD_SCRIPTS := $(abspath $(CURDIR)/../buildscripts)

# Project files:
PPC_BIN = patch
PPC_OBJS = patch.o debug.o cache.o common.o ppc_mon/src/commands.o ppc_mon/src/ppc_mon.o umm_malloc/umm_malloc.o string.o

include $(BUILD_SCRIPTS)/ppc/Defs.make
include $(BUILD_SCRIPTS)/ppc/Rules.bin.make
include $(BUILD_SCRIPTS)/ppc/Rules.make