#include "ppcpatch.h"
#include <iopcontrol.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <kernel.h>

#define GM_IF ((vu32 *)0x1F801450)
#define GM_IOP_TYPE (0x80000000)

int DeckardPatchBufferFull(u8* patch, u32 size_patch) {
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

	SifInitRpc(0);
	SifLoadFileInit();
	SifIopReset("", 0);
	while (!SifIopSync()) {};

    DeckardPatchBuffer(patch, size_patch);
	FlushCache(0);
	FlushCache(2);
	SifIopReset("", 0);
	while (!SifIopSync()) {};
    return 0;
}