#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

#define DEV_STORE 0x1
#define DEV_LOAD  0x2

#define PATCH_PREFIX  0x1
#define PATCH_POSTFIX 0x2
#define PATCH_REPLACE 0x3

#define SWAP_ENDIAN(x) ((((x) >> 24) & 0xFF) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

/* APU funcs from binary */
extern uint32_t (*debug_reg_mips_gp_get)(uint32_t reg);
extern uint32_t (*debug_reg_mips_hi_get)();
extern uint32_t (*debug_reg_mips_lo_get)();
extern uint32_t (*debug_reg_mips_pc_get)();
extern uint32_t (*debug_reg_mips_cc_get)();

extern void (*debug_reg_mips_gp_set)(uint32_t reg, uint32_t value);
extern void (*debug_reg_mips_hi_set)(uint32_t value);
extern void (*debug_reg_mips_lo_set)(uint32_t value);
extern void (*debug_reg_mips_pc_set)(uint32_t value);
extern void (*debug_reg_mips_cc_set)(uint32_t value);

//Access entire GTE register file
extern uint32_t (*debug_reg_gte_get)(uint32_t reg);
extern void (*debug_reg_gte_set)(uint32_t reg, uint32_t value);

extern void (*debug_uart_init)(uint32_t baud);

extern const char *mips_reg_names[];

//Access entire MIPS register file
uint32_t debug_reg_mips_get(uint32_t reg);
void debug_reg_mips_set(uint32_t reg, uint32_t value);

uint32_t debug_reg_dcr_get(uint16_t dcr);
uint32_t debug_reg_ppc_get(uint16_t gp);
uint32_t debug_reg_ppc_sp_get(uint16_t sp);
uint32_t debug_reg_ppc_get_tbu();
uint32_t debug_reg_ppc_get_tbl();
uint32_t debug_reg_ppc_get_msr();

void debug_reg_dcr_set(uint16_t dcr, uint32_t value);
void debug_reg_ppc_set(uint16_t gp, uint32_t value);
void debug_reg_ppc_sp_set(uint16_t sp, uint32_t value);

//patch_type: PATCH_PREFIX, PATCH_POSTFIX, PATCH_REPLACE
void debug_patch_dev_load(uint8_t patch_type, uint32_t addr, uint32_t len, void* func_ptr);
void debug_patch_dev_store(uint8_t patch_type, uint32_t addr, uint32_t len, void* func_ptr);

//place branch after reset to debug_reset_handler()
void debug_hook_reset();
void debug_reset_handler();
void debug_run_on_reset(void* func);

#endif