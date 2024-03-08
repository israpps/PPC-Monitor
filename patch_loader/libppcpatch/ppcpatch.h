#ifndef PPCPATCH_H
#define PPCPATCH_H
#include <tamtypes.h>
#include <errno.h>


#ifndef PPCPATCH_IOP_BASE_ADDR
#define PPCPATCH_IOP_BASE_ADDR 0xBC000000
#endif

#ifndef PPCPATCH_PATCH_INSTR_ADDR
#define PPCPATCH_PATCH_INSTR_ADDR 0xBCA14000
#endif

#ifndef PPCPATCH_PATCH_BASE_ADDR
#define PPCPATCH_PATCH_BASE_ADDR 0xBCA14004
#endif

#ifndef PPCPATCH_PATCH_BRANCH_ADDR
#define PPCPATCH_PATCH_BRANCH_ADDR (PPCPATCH_IOP_BASE_ADDR + 0xa07434)
#endif

#define PPCPATCH_BRANCH_INSTR 0x0640a148


/** @brief writes a patch into a free RAM space on PPC CPU and places a branch instruction to fire the patch upon next IOP Reset
 * @param patch pointer to the buffer
 * @param size_patch size of the buffer
 * @return 0 on success
 * @note this function does not perform the IOP Reboot. it is intended to give the developer the chance to reboot the IOP later on if it needs to.  
 * if you dont have problems with rebooting the IOP right after installing the patch. use DeckardPatchBufferFull instead
 * @warning ONLY use this on DECKARD PS2s
*/
int DeckardPatchBuffer(u8* patch, u32 size_patch);


/** @brief does the same than DeckardPatchBuffer, but previously rebooting the IOP, and after loading the patch, it flushes cache and reboots the IOP again. to automatically fire the patch. recommended for those who dont need too much control over the process
 * @param patch pointer to the buffer
 * @param size_patch size of the buffer
 * @return 0 on success
 * @note keep in mind that since IOP reboot is performed. whatever IRX modules you loaded will need to be reloaded (eg: memory card drivers)
 * @warning ONLY use this on DECKARD PS2s
*/
int DeckardPatchBufferFull(u8* patch, u32 size_patch);
#endif