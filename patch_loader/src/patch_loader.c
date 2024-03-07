#include <stdio.h>
#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <debug.h>
#include <loadfile.h>
#include <iopcontrol.h>
#include <sio.h>
#include <dirent.h>
#include <malloc.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

extern char patch_bin[];  //Generated by bin2s.
extern u32 size_patch_bin;

//TODO: pre-reverse with bin2s if it can
#define REWEN(x) ((((x) >> 24) & 0xFF) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) | ((x) << 24))

#define IOP_BASE_ADDR 0xBC000000
#define PATCH_INSTR_ADDR 0xBCA14000
#define PATCH_BASE_ADDR 0xBCA14004
#define PATCH_BRANCH_ADDR (IOP_BASE_ADDR + 0xa07434)

int main()
{
	int fd;
	SifInitRpc(0);
	SifLoadFileInit();
	SifIopReset("", 0);
	while (!SifIopSync()) {
	};
	init_scr();
	scr_setCursor(0);
	scr_printf("\n\n------------------------- PowerPc Patch Installer v0.2 -------------------------\n"
	           "\tBy Qnox32. Maintained by El_isra\n"
	           "-------------------------------------\n\n");

	if ((fd = open("rom0:DECKARD", O_RDONLY)) < 0) {
		scr_setfontcolor(0x0000ff);
		scr_printf("\tERROR: This PS2 is not a DECKARD model\n\taborting patch to avoid IOP crash\n");
		sleep(5);
		goto quit;
	} else {
		close(fd);
	}

	scr_printf("\t- patch size: 0x%x\n"
	           "\t- patch base address 0x%x\n"
	           "\t- patch branch address 0x%x\n",
	           size_patch_bin, PATCH_BASE_ADDR, PATCH_BRANCH_ADDR);

	scr_printf("\tApplying patch..");
	DI();              //disable interrupts on EE
	ee_kmode_enter();  //enter kernel mode

	//load patch into memory
	for (int i = 0; i < size_patch_bin; i++) {
		*(vu8 *)(PATCH_BASE_ADDR + i) = patch_bin[i];
	}

	//backup original instruction
	u32 original_instr = *(vu32 *)(PATCH_BRANCH_ADDR);

	//copy original instruction to backup addr
	*(vu32 *)(PATCH_INSTR_ADDR) = original_instr;

	//place branch instruction to patch
	*(vu32 *)(PATCH_BRANCH_ADDR) = 0x0640a148;  //ba 0xa14004

	ee_kmode_exit();
	EI();
	scr_printf(". done!\n\tFlushCache()\n");
	FlushCache(0);
	FlushCache(2);
	scr_printf("\tResetting IOP to start patch\n");
	//reset IOP to trigger branch / patch
	SifIopReset("", 0);
	while (!SifIopSync()) {
	};
	scr_printf("\tpatch complete!\n");

quit:
	sleep(2);

	return 0;
}



void _libcglue_timezone_update() {}
void _libcglue_init() {}
void _libcglue_deinit() {}
//DISABLE_PATCHED_FUNCTIONS();
PS2_DISABLE_AUTOSTART_PTHREAD();