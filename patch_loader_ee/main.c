#include <stdio.h>
#include <debug.h>
#include <iopcontrol.h>
#include <sifrpc.h>
#include <tamtypes.h>
#include <loadfile.h>
#include <sbv_patches.h>

extern unsigned char ppcpatchman_irx[];
extern unsigned int size_ppcpatchman_irx;
#define DPRINTF(x...) \
	scr_printf(x);    \
	printf(x)
int main()
{
	SifInitRpc(0);
	SifLoadFileInit();
	SifIopReset("", 0);
	while (!SifIopSync()) {
	};
	SifInitIopHeap();
	SifLoadFileInit();
	sbv_patch_enable_lmb();  // The old IOP kernel has no support for LoadModuleBuffer. Apply the patch to enable it.
	sbv_patch_disable_prefix_check();
	init_scr();
	scr_setCursor(0);
	DPRINTF("\n\n------------------------- PowerPc Patch Installer v%d.%d -------------------------\n"
	        "\tBy Qnox32. Maintained by El_isra\n"
	        "\tHash: %s\n"
	        "-------------------------------------\n\n",
	        MAJOR, MINOR, GITHASH);
	int ret;
	int id = SifExecModuleBuffer(ppcpatchman_irx, size_ppcpatchman_irx, 0, NULL, &ret);
	DPRINTF("\t%s: %d %d   ", "PPCPATCHMAN.IRX", id, ret);
	if (id < 0) {
		scr_setfontcolor((id == -200) ? 0x0000ff : 0xffff00);  //-200 means a system module is missing :'(. make error more dramatic if so
		DPRINTF("Failed to load\n");
		sleep(5);
		return -1;
	}

	scr_setfontcolor(0x00ff00);
	DPRINTF("Done\n");
	sleep(2);
	return 0;
}

#ifndef SKIP_SIZE_REDUCTION
void _libcglue_timezone_update() {}
void _libcglue_init() {}
void _libcglue_deinit() {}
DISABLE_PATCHED_FUNCTIONS();
DISABLE_EXTRA_TIMERS_FUNCTIONS();
PS2_DISABLE_AUTOSTART_PTHREAD();
#endif