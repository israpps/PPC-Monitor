#include <stdint.h>

#include "debug.h"
#include "cache.h"
#include "common.h"

#define LOAD_LUT_ADDR 0xA10000
#define STORE_LUT_ADDR 0xA12000

/* APU funcs from binary */
uint32_t (*debug_reg_mips_gp_get)(uint32_t reg) = (void*)0xa031b0;
uint32_t (*debug_reg_mips_hi_get)() = (void*)0xa031c0;
uint32_t (*debug_reg_mips_lo_get)() = (void*)0xa031d0;
uint32_t (*debug_reg_mips_pc_get)() = (void*)0xa031e0;
uint32_t (*debug_reg_mips_cc_get)() = (void*)0xa031f0;

void (*debug_reg_mips_gp_set)(uint32_t reg, uint32_t value) = (void*)0xa031a8; 
void (*debug_reg_mips_hi_set)(uint32_t value) = (void*)0xa031b8;
void (*debug_reg_mips_lo_set)(uint32_t value) = (void*)0xa031c8;
void (*debug_reg_mips_pc_set)(uint32_t value) = (void*)0xa031d8;
void (*debug_reg_mips_cc_set)(uint32_t value) = (void*)0xa031e8;

uint32_t (*debug_reg_gte_get)(uint32_t reg) = (void*)0xa031a0;
void (*debug_reg_gte_set)(uint32_t reg, uint32_t value) = (void*)0xa03198;

void (*debug_uart_init)(uint32_t baud) = (void*)0xa030c4;

static uint8_t reset_func_idx;
static void *reset_func[0x10];

const char *mips_reg_names[] = {"r0", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
                                "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};

uint32_t debug_reg_mips_get(uint32_t reg)
{
	debug_reg_dcr_set(0x180, reg);	
	return debug_reg_dcr_get(0x181);
}

void debug_reg_mips_set(uint32_t reg, uint32_t value)
{
	debug_reg_dcr_set(0x180, reg);
	debug_reg_dcr_set(0x181, value);
}

/*
* This core does not support mfdcrx and mtdcrx instructions so self modifying
* code is used instead for accessing less common DCRs (possibly unsafe?)
*/
uint32_t debug_reg_dcr_get(uint16_t dcr) 
{
	uint32_t value;
	static uint32_t instr[3];

	if (dcr > 0x3FF) {
		printf("DCR 0x%x not in range 0x0-0x3FF\n", dcr);
		return 0;
	}

	switch (dcr)
	{
	case 0x10:
		asm volatile(
			"mfdcr %[value], 0x10;"
		:[value] "=r" (value));
		break;

	case 0x11:
		asm volatile(
			"mfdcr %[value], 0x11;"
		:[value] "=r" (value));
		break;

	case 0x16:
		asm volatile(
			"mfdcr %[value], 0x16;"
		:[value] "=r" (value));
		break;
	
	case 0x17:
		asm volatile(
			"mfdcr %[value], 0x17;"
		:[value] "=r" (value));
		break;

	case 0x180:
		asm volatile(
			"mfdcr %[value], 0x180;"
		:[value] "=r" (value));
		break;

	case 0x181:
		asm volatile(
			"mfdcr %[value], 0x181;"
		:[value] "=r" (value));
		break;

	case 0x182:
		asm volatile(
			"mfdcr %[value], 0x182;"
		:[value] "=r" (value));
		break;

	case 0x183:
		asm volatile(
			"mfdcr %[value], 0x183;"
		:[value] "=r" (value));
		break;

	case 0x184:
		asm volatile(
			"mfdcr %[value], 0x184;"
		:[value] "=r" (value));
		break;

	default:
		//DCRN ← DCRF5:9 || DCRF0:4
		uint32_t dcrn = (dcr & 0x1F) << 5 | (dcr & 0x3E0) >> 5;
		
		instr[0] = 0x7c600286 | (dcrn << 11); 	//mfdcr r3, dcrn;
		instr[1] = 0x4e800020; 					//blr

		uint32_t (*func)() = (void*)(&instr);

		inval_DI_cache((uint32_t)&instr, 1);

		value = func();

		break;
	}

	return value;
}

void debug_reg_dcr_set(uint16_t dcr, uint32_t value)
{
	static uint32_t instr[3];

	if (dcr > 0x3FF) {
		printf("DCR 0x%x not in range 0x0-0x3FF\n", dcr);
		return;
	}

	switch (dcr)
	{
	case 0x10:
		asm volatile(
			"mtdcr 0x10, %[value];"
		::[value] "r" (value));
		break;

	case 0x11:
		asm volatile(
			"mtdcr 0x11, %[value];"
		::[value] "r" (value));
		break;

	case 0x16:
		asm volatile(
			"mtdcr 0x16, %[value];"
		::[value] "r" (value));
		break;
	
	case 0x17:
		asm volatile(
			"mtdcr 0x17, %[value];"
		::[value] "r" (value));
		break;

	case 0x180:
		asm volatile(
			"mtdcr 0x180, %[value];"
		::[value] "r" (value));
		break;

	case 0x181:
		asm volatile(
			"mtdcr 0x181, %[value];"
		::[value] "r" (value));
		break;

	case 0x182:
		asm volatile(
			"mtdcr 0x182, %[value];"
		::[value] "r" (value));
		break;

	case 0x183:
		asm volatile(
			"mtdcr 0x183, %[value];"
		::[value] "r" (value));
		break;

	case 0x184:
		asm volatile(
			"mtdcr 0x184, %[value];"
		::[value] "r" (value));
		break;

	default:
		//DCRN ← DCRF5:9 || DCRF0:4
		uint32_t dcrn = (dcr & 0x1F) << 5 | (dcr & 0x3E0) >> 5;
		
		instr[0] = 0x7c600386 | (dcrn << 11); 	//mfdcr r3, dcrn;
		instr[1] = 0x4e800020; 					//blr

		//r3 is always first param
		void (*func)(uint32_t value) = (void*)(&instr);

		inval_DI_cache((uint32_t)&instr, 1);

		func(value);

		break;
	}
}

uint32_t debug_reg_ppc_get(uint16_t gp) {}

uint32_t debug_reg_ppc_sp_get(uint16_t sp)
{
	uint32_t value;
	static uint32_t instr[3];

	//TODO: check if valid sp before continuing

	//SPRN = SPRF5:9 | SPRF0:4
	uint32_t sprn = (sp & 0x1F) << 5 | (sp & 0x3E0) >> 5;
	
	instr[0] = 0x7c6002a6 | (sprn << 11); 	//mfspr r3, sprn;
	instr[1] = 0x4e800020; 					//blr

	uint32_t (*func)() = (void*)(&instr);

	inval_DI_cache((uint32_t)&instr, 1);

	value = func();

	return value;
}

uint32_t debug_reg_ppc_get_tbl()
{
	uint32_t tbl = 0;
	uint32_t tbu = 0;
	uint32_t temp = 0;

	asm volatile(
		"loop:"
		"mfspr %[tbu], 0x10d;"
		"mfspr %[tbl], 0x10c;"
		"mfspr %[temp], 0x10d;"
		"cmpw %[tbu], %[temp];"
		"bne loop;"
	:[tbl] "=r" (tbl), [tbu] "=r" (tbu), [temp] "=r" (temp));

	return tbl;
}

uint32_t debug_reg_ppc_get_tbu()
{
	uint32_t tbu = 0;
	uint32_t temp = 0;

	asm volatile(
		"loop2:"
		"mfspr %[tbu], 0x10d;"
		"mfspr %[temp], 0x10d;"
		"cmpw %[tbu], %[temp];"
		"bne loop2;"
	:[tbu] "=r" (tbu), [temp] "=r" (temp));

	return tbu;
}

uint32_t debug_reg_ppc_get_msr()
{
	uint32_t msr = 0;

	asm volatile(
		"mfmsr %[msr];"
	:[msr] "=r" (msr));

	return msr;
}

void debug_reg_ppc_set(uint16_t gp, uint32_t value) {}

void debug_reg_ppc_sp_set(uint16_t sp, uint32_t value)
{
	static uint32_t instr[3];

	//SPRN = SPRF5:9 | SPRF0:4
	uint32_t sprn = (sp & 0x1F) << 5 | (sp & 0x3E0) >> 5;
	
	instr[0] = 0x7c6003a6 | (sprn << 11); 	//mtspr sprn, r3;
	instr[1] = 0x4e800020; 					//blr

	//r3 is always first param
	void (*func)(uint32_t value) = (void*)(&instr);

	inval_DI_cache((uint32_t)&instr, 1);

	func(value);
}

//each entry in the table spans 0x10 (16 bytes.)
//ex. an entry for 0x1F808300 would cover 0x1F808300 to 0x1F808310.
void debug_lut_add_entry(uint32_t lut_addr, uint32_t addr, uint32_t len, void *func_ptr)
{
    uint32_t size = len >> 4;
    
    if (size == 0) {
        printf("len too short for entry, minimum size is 0x10\n");
    }

    for (int i = 0; i < len; i+=4) {
        *(uint32_t*)(lut_addr+i) = (uint32_t)func_ptr;
    }
}

//TODO:
void debug_prefix_dispatch(uint32_t addr, uint32_t arg2, uint32_t arg3){}

void debug_patch_dev(uint8_t patch_type, uint32_t addr, uint32_t len, void *load_func, void *store_func)
{
    if (patch_type == PATCH_PREFIX) {

    }

    if (patch_type == PATCH_POSTFIX) {

    }

    if (patch_type == PATCH_REPLACE) {
        if (load_func != 0x0)
            debug_lut_add_entry(LOAD_LUT_ADDR, addr, len, load_func);

        if (store_func != 0x0)
            debug_lut_add_entry(STORE_LUT_ADDR, addr, len, store_func);
    }
}

void debug_hook_reset()
{
	uint32_t ba = 0x48000002 | (uint32_t)&debug_reset_handler;
	*(uint32_t*)(0xa04ca4) = ba; //ba debug_reset_hook

	inval_I_cache(0xa04ca4, 1);

	reset_func_idx = 0;
}

void debug_reset_handler()
{
	if (reset_func_idx != 0) {
		for (int i = 0; i < reset_func_idx; i++) {
			void (*func)() = reset_func[i];
			func();
		}
	}
}

void debug_run_on_reset(void* func)
{
	if (reset_func_idx > 0x10) {
		printf("Reset func list full\n");
		return;
	}

	//printf("Adding function @ 0x%x to list, num %i\n", (uint32_t)func, reset_func_idx);
	reset_func[reset_func_idx] = func;

	reset_func_idx++;
}

