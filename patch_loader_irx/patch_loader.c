#include "intrman.h"
#include "loadcore.h"
#include "thbase.h"
#include "stdio.h"

#include "irx_imports.h"

extern char patch_bin[];  // Generated by bin2c.
extern u32 size_patch_bin;

#define PATCH_START_ADDR 0xA14000

// El_isra: keep IRX version matching PPC monitor version for simplicity
#define MODNAME "ppcpatchman"
#define IRX_VMAJOR 0
#define IRX_VMINOR 2
IRX_ID(MODNAME, IRX_VMAJOR, IRX_VMINOR);


#define GM_IF ((vu32 *)0x1F801450)
#define GM_IOP_TYPE (0x80000000)
int module_start(int argc, char *argv[])
{
	int rv;
	/*
    Bit 31 of GM_IF is:
    0 Regular IOP
    1 DECKARD IOP
    */
	if ((*GM_IF & GM_IOP_TYPE) == 0) {
		printf(MODNAME ": WARNING. Patch aborted, this PS2 does not have DECKARD\n");
		/* return a different value that modload also assumes to be a failure ((ret & 3) != 0). 
		to keep things the sony style, use -2. a wierd return value seen in some system modules */
		return -2;
	}
	printf(MODNAME ": Copying patch to DECKARD memory 0x%x - 0x%x\n", PATCH_START_ADDR, (PATCH_START_ADDR + size_patch_bin));

	for (int i = 0; i < size_patch_bin; i++) {
		*(vu8 *)(PATCH_START_ADDR + i) = patch_bin[i];

		// Freezes without delay. Unsure why
		if (i % 100 == 0) {
			DelayThread(1000);
		}
	}

	// Replace func ptr for 0x1f8fff60 with address of the patch
	*(vu32 *)(0xabfbd8) = PATCH_START_ADDR;

	printf(MODNAME ": Attempting to trigger patch\n");

	// Trigger jump to patch by reading from 0x1f8fff60
	rv = *(u32 *)(0x1f8fff60);

	if (rv == 0xFEEDFACE) {
		printf(MODNAME ": Patch trigger successful: 0x%x\n", rv);
	} else {
		printf(MODNAME ": Patch trigger failed: 0x%x\n", rv);
		return -3;
	}
	return MODULE_NO_RESIDENT_END;
}

int _start(int argc, char *argv[])
{
	return module_start(argc, argv);
}