#include "ppcpatch.h"
#include <iopcontrol.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <kernel.h>

int DeckardPatchBufferFull(u8* patch, u32 size_patch) {
	if (size_patch == 0 || patch == NULL)
		return 1;

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