BUILD_SCRIPTS := $(abspath $(CURDIR)/buildscripts)

all::
	$(MAKE) -C patch
	$(MAKE) -C patch_loader
	$(MAKE) -C ee_loader

clean::
	$(MAKE) -C patch clean clean
	$(MAKE) -C patch_loader clean
	$(MAKE) -C ee_loader clean

rebuild: clean all

toolchain:
	./buildscripts/toolchain/001-binutils.sh
	./buildscripts/toolchain/002-gcc-stage1.sh