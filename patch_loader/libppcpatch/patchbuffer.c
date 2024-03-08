#include <tamtypes.h>
#include <kernel.h>
#include "ppcpatch.h"

//TODO: pre-reverse with bin2s if it can
#define REWEN(x) ((((x) >> 24) & 0xFF) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) | ((x) << 24))
#define GM_IF ((vu32 *)0x1F801450)
#define GM_IOP_TYPE (0x80000000)

int DeckardPatchBuffer(u8* patch, u32 size_patch) {
	if (size_patch == 0 || patch == NULL)
		return -EINVAL;
    /* borrowed from krat0s XPARAM approach
    Check to see if this is a DECKARD machine or not.
    Bit 31 of GM_IF is for the IOP type.
    0 Regular IOP
    1 DECKARD IOP
    */
    if ((*GM_IF & GM_IOP_TYPE) == 0)
        return -ENOTSUP;

	DI();              //disable interrupts on EE
	ee_kmode_enter();  //enter kernel mode

	//load patch into memory
	for (int i = 0; i < size_patch; i++) {
		*(vu8 *)(PPCPATCH_PATCH_BASE_ADDR + i) = patch[i];
	}

	//backup original instruction
	u32 original_instr = *(vu32 *)(PPCPATCH_PATCH_BRANCH_ADDR);

	//copy original instruction to backup addr
	*(vu32 *)(PPCPATCH_PATCH_INSTR_ADDR) = original_instr;

	//place branch instruction to patch
	*(vu32 *)(PPCPATCH_PATCH_BRANCH_ADDR) = PPCPATCH_BRANCH_INSTR;  //ba 0xa14004

	ee_kmode_exit();
	EI();
    return 0;
}