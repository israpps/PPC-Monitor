
#include "ppc_mon/include/ppc_mon.h"
#include "ppc_mon/include/commands.h"
#include "common.h"
#include "string.h"

#include <stdint.h>

#include "debug.h"

/* Memory commands:
*  mem r [width] <address> [length]
*  mem w [width] <address> <value> [length]
*
*  arg1: <r|w>
*  arg2: [width]
*  arg3: <addr> 
*  arg4: <value> OR [len]
*  arg5: [len]
*/
static int command_mem()
{
    char *rw = pm_parser_get_argv_ptr(1);
    char *w = pm_parser_get_argv_ptr(2);

    uint8_t width = 4;
    uint32_t addr = 0;
    uint32_t value = 0;
    uint32_t len = 0;
    
    uint8_t argn = 2;
    uint32_t counter = 0;

    uint32_t readback = 0;

    //If width is specified 
    if (!pm_parser_char_is_digit(*w)) {

        if (*w == 'h')
            width = 2;
        else if (*w == 'b')
            width = 1; 
    
        argn = 3;
    }

    if (*rw == 'r') {
        addr = pm_parser_get_argv_dec(argn);
        len = pm_parser_get_argv_dec(argn + 1);

        do {
            switch (width)
            {
            case 4:
                value = *(volatile uint32_t*)(addr + (counter * width));
                break;
            case 2:
                value = *(volatile uint16_t*)(addr + (counter * width));
                break;
            case 1:
                value = *(volatile uint8_t*)(addr + (counter * width));
                break;
            default:
                break;
            }

            printf("0x%x: 0x%.*x\n", addr + (counter * width), (width * 2), value);
            counter++;
        } while (counter < len);

    } else if (*rw == 'w') {
        addr = pm_parser_get_argv_dec(argn);
        value = pm_parser_get_argv_dec(argn + 1);
        len = pm_parser_get_argv_dec(argn + 2);

        do {
            switch (width)
            {
            case 4:
                *(volatile uint32_t*)(addr + (counter * width)) = value;                
                if (pm_settings.readback)
                    readback = *(volatile uint32_t*)(addr + (counter * width)); 
                
                break;
            case 2:
                *(volatile uint16_t*)(addr + (counter * width)) = value;
                if (pm_settings.readback)
                    readback = *(volatile uint16_t*)(addr + (counter * width));

                break;
            case 1:
                *(volatile uint8_t*)(addr + (counter * width)) = value; 
                if (pm_settings.readback)
                    readback = *(volatile uint8_t*)(addr + (counter * width));

                break;
            default:
                break;
            }

            if (pm_settings.readback)
                printf("0x%x: 0x%.*x (RB)\n", addr + (counter * width), (width * 2), readback);

            counter++;
        } while (counter < len);

    } else {
        printf("Invalid action: %c\n", *rw);
        return -1;
    }

}

/* Reg commands:
*  reg r <reg> <num> [length]
*  reg w <reg> <num> <value> [length]
*
*  arg1: <r|w>
*  arg2: <reg>
*  arg3: <num> 
*  arg4: <value> OR [len]
*  arg5: [len]
*/
static int command_reg()
{
    char *rw = pm_parser_get_argv_ptr(1);
    char *reg = pm_parser_get_argv_ptr(2);

    uint32_t reg_num = 0;
    uint32_t value = 0;
    uint32_t len = 0;

    uint32_t counter = 0;
    uint32_t readback = 0;

    if (*rw == 'r') {
        reg_num = pm_parser_get_argv_dec(3);
        len = pm_parser_get_argv_dec(4);

        do {
            switch (*reg) 
            {
            case 'd':
                value = debug_reg_dcr_get(reg_num + counter);
                break;
            case 'r':
                value = debug_reg_ppc_get(reg_num + counter);
                break;

            case 's':
                value = debug_reg_ppc_sp_get(reg_num + counter);
                break;

            case 'm':
                value = debug_reg_mips_get(reg_num + counter);
                break;
            
            case 'g':
                value = debug_reg_gte_get(reg_num + counter);
                break;
            
            //Unknown device DCR 0x16-0x17
            case 'u':
                debug_reg_dcr_set(0x16, reg_num + counter);
                value = debug_reg_dcr_get(0x17);
                break;
            
            //SDRAM controller DCR 0x10-0x11
            case 'c':
                debug_reg_dcr_set(0x10, reg_num + counter);
                value = debug_reg_dcr_get(0x11);
                break;

            default:
                printf("Invalid register: %c\n", *reg);
                return -1;
                break;
            }

            printf("(%c) 0x%x: 0x%.8x\n", *reg, reg_num + counter, value);
            counter++;
        } while (counter < len);

    } else if (*rw == 'w') {
        reg_num = pm_parser_get_argv_dec(3);
        value = pm_parser_get_argv_dec(4);
        len = pm_parser_get_argv_dec(5);

        do {
            switch (*reg) 
            {
            case 'd':
                debug_reg_dcr_set(reg_num + counter, value);
                if (pm_settings.readback)
                    readback = debug_reg_dcr_get(reg_num + counter);
                break;
            case 'r':
                debug_reg_ppc_set(reg_num + counter, value);
                if (pm_settings.readback)
                    readback = debug_reg_ppc_get(reg_num + counter);
                break;

            case 's':
                debug_reg_ppc_sp_set(reg_num + counter, value);
                if (pm_settings.readback)
                    readback = debug_reg_ppc_sp_get(reg_num + counter);
                break;

            case 'm':
                
                debug_reg_mips_set(reg_num, value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_get(reg_num);
                break;
            
            case 'g':
                debug_reg_gte_set(reg_num, value);
                if (pm_settings.readback)
                    readback = debug_reg_gte_get(reg_num);
                break;
            
            //Unknown device DCR 0x16-0x17
            case 'u':
                debug_reg_dcr_set(0x16, reg_num + counter);
                debug_reg_dcr_set(0x17, value);
                
                if (pm_settings.readback) {
                    debug_reg_dcr_set(0x16, reg_num + counter);
                    readback = debug_reg_dcr_get(0x17);
                }
                break;
            
            //SDRAM controller DCR 0x10-0x11
            case 'c':
                debug_reg_dcr_set(0x10, reg_num + counter);
                debug_reg_dcr_set(0x11, reg_num + counter);

                if (pm_settings.readback) {
                    debug_reg_dcr_set(0x10, reg_num + counter);
                    readback = debug_reg_dcr_get(0x11);
                }
                break;

            default:
                printf("Invalid register: %c\n", *reg);
                return -1;
                break;
            }

            if (pm_settings.readback)
                printf("(%c) 0x%x: 0x%.8x (RB)\n", *reg, reg_num + counter, readback);

            counter++;
        } while (counter < len);

    } else {
       printf("Invalid action: %c\n", *rw);
       return -1;
    }


}

/* Dump commands:
*  dump <address> <length> {format}
*
*  arg1: <address>
*  arg2: <length>
*  arg3: [format] (not implemented yet)
*/
static int command_dump()
{
    //TODO: add addr range check here
    uint32_t addr = pm_parser_get_argv_dec(1);
    uint32_t len = pm_parser_get_argv_dec(2);
    uint32_t format = pm_parser_get_argv_dec(3);

    uint32_t value = 0;

    for (int i = 0; i < len; i+=16) {
        printf("%.8x  ", (addr + i));

        //Print bytes
        for (int j = 0; j < 16; j++) {
            if ((i + j) < len) {
                printf("%.2x ", *(uint8_t*)(addr + (i+j)));
            } else {
                printf("   ");
            }
        }
        printf("  ");
        
        //Print ASCII
        for (int j = 0; j < 16; j++) {
            if ((i + j) < len) {
                value = *(uint8_t*)(addr + (i+j));
                if (value > 31 && value < 127)
                    printf("%c", value);
                else
                    printf(".");
            }
        }
        printf("\n");
    }

    return 0;
}

/* Find commands:
*  find <value> <address> <length>
*
*  arg1: <value>
*  arg2: <address>
*  arg3: <length>
*/
static int command_find()
{
    //TODO: addr range check

    uint32_t value = pm_parser_get_argv_dec(1);
    uint32_t addr = pm_parser_get_argv_dec(2);
    uint32_t len = pm_parser_get_argv_dec(3);
    
    uint32_t results = 0;

    value = SWAP_ENDIAN(value);

    for (int i = 0; i < len; i++) {
        results = *(uint32_t*)(addr + (i*4));
        if (results == value) {
            printf("Match @ 0x%x\n", (addr + (i*4)));
        }
    }
}

/* MIPS commands:
*  mips <r> <reg>
*  mips <w> <reg> <value>
*  
*  arg1: <r|w>
*  arg2: <reg>
*  arg3: <value>
*/
static int command_mips()
{
    char *rw = pm_parser_get_argv_ptr(1);
    char *reg = pm_parser_get_argv_ptr(2);

    uint32_t value = pm_parser_get_argv_dec(3);
    uint32_t readback = 0;

    int reg_num = -1;

    //Attempt to resolve GP register name or number
    //GP register 0 - 32
    if (pm_parser_char_is_digit(*reg)) {
        reg_num = pm_parser_get_argv_dec(2);

    //GP register by name: a0, v1, k2 etc.
    } else if (pm_parser_get_argv_len(2) == 2) {
        
        //Match reg name to number
        for (int i = 0; i < 32; i++) {
            if (strcmp(reg, mips_reg_names[i]) == 0) {
                reg_num = i;
                break;
            }
        }
    }

    //Dump
    if (*rw == 'd') {
        printf("APU-MIPS Instruction:     0x%.8x Operand Addr:  0x%.8x\n", debug_reg_dcr_get(0x182), debug_reg_dcr_get(0x184));
        printf("APU-MIPS Program Counter: 0x%.8x Cycle Count:   0x%.8x\n", debug_reg_mips_pc_get(), debug_reg_mips_cc_get());
        printf("APU-MIPS HI:              0x%.8x LO:            0x%.8x\n", debug_reg_mips_hi_get(), debug_reg_mips_lo_get());
        printf("APU-MIPS Status Register: 0x%.8x In Delay Slot: 0x%x\n", debug_reg_mips_get(0x4), debug_reg_ppc_sp_get(0x105));
        printf("\n");
        printf("Unk1: 0x%.8x Unk2: 0x%.8x Unk3: 0x%.8x\n", *(uint32_t*)0xbe03c8,  *(uint32_t*)0xbe03cc,  *(uint32_t*)0xbe03d0);
        printf("Unk4: 0x%.8x Unk5: 0x%.8x Unk6: 0x%.8x\n", *(uint32_t*)0xbe03d4,  *(uint32_t*)0xbe03d8,  *(uint32_t*)0xbe03dc);
        printf("Unk7: 0x%.8x Unk8: 0x%.8x\n", *(uint32_t*)0xbe03e4,  *(uint32_t*)0xbe03ec);
        printf("\n");
        printf("APU-MIPS Registers:\n");
        for (int i = 0; i < 32; i++) {
            printf("%s: 0x%.8x ", mips_reg_names[i], debug_reg_mips_gp_get(i));

            if ((i+1) % 4 == 0)
                printf("\n");
        }
        printf("\n");

    //Read
    } else if (*rw == 'r') {
        //GP
        if (reg_num != -1) {
            value = debug_reg_mips_get(reg_num);
            printf("%.2i(%s): 0x%.8x\n", reg_num, mips_reg_names[reg_num], value);
        //Non-GP
        } else {
            switch (*reg)
            {
            case 'i':
                value = debug_reg_dcr_get(0x182);
                break;
            case 'o':
                value = debug_reg_dcr_get(0x184);
                break;
            case 's':
                value = debug_reg_mips_get(0x4);
                break;
            case 'h':
                value = debug_reg_mips_hi_get();
                break;
            case 'l':
                value = debug_reg_mips_lo_get();
                break;
            case 'p':
                value = debug_reg_mips_pc_get();
                break;
            case 'c':
                value = debug_reg_mips_cc_get();
                break;

            default:
                printf("Invalid reg: %c\n", *reg);
                return -1;
            }

            printf("(%c): 0x%.8x\n", *reg, value);
        }

    //Write
    } else if (*rw == 'w') {
        //GP
        if (reg_num != -1) {
            debug_reg_mips_gp_set(reg_num, value);
            if (pm_settings.readback) {
                readback = debug_reg_mips_gp_get(reg_num);
                printf("%.2i(%c): 0x%.8x\n", reg_num, mips_reg_names[reg_num], value);
            }
        //Non-GP
        } else {
            switch (*reg)
            {
            //All
            case 'i':
                debug_reg_dcr_set(0x182, value);
                if (pm_settings.readback)
                    readback = debug_reg_dcr_get(0x182);
                break;
            case 's':
                debug_reg_mips_set(0x4, value);
                break;
            case 'h':
                debug_reg_mips_hi_set(value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_hi_get();
                break;
            case 'l':
                debug_reg_mips_lo_set(value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_lo_get();
                break;
            case 'p':
                debug_reg_mips_pc_set(value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_pc_get();
                break;
            case 'c':
                debug_reg_mips_cc_set(value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_cc_get();
                break;

            default:
                printf("Invalid reg: %c\n", *reg);
                return -1;
            }

            if (pm_settings.readback)
                printf("(%c): 0x%.8x (RB)\n", *reg, readback);
        }
    } else {
        printf("Invalid action: %c\n", *rw);
        return -1;
    }

    return 0;
}

/* COP0 commands:
*  cop0 <r> <h/s> <num>
*  cop0 <w> <h/s> <num> <value>
*  
*  arg1: <r|w>
*  arg2: <a|h|s>
*  arg3: <num>
*  arg4: <value>
*/
static int command_cop0()
{
    char *rw = pm_parser_get_argv_ptr(1);
    char *hs = pm_parser_get_argv_ptr(2);

    uint32_t reg_num = pm_parser_get_argv_dec(3);
    uint32_t value = pm_parser_get_argv_dec(4);

    uint32_t readback = 0;

    //Dump all register
    if (*rw == 'd') {

        printf("Software:\n");
        for (int i = 0; i < 32; i++) {
            printf("cop0r%.2i: 0x%.8x ", i,  *(uint32_t*)(0xbe0344 + (i * 4)));

            if ((i+1) % 4 == 0)
                printf("\n");
        }
        printf("\n");

        printf("Hardware:\n");
        printf("cop0r%.2i: 0x%.8x ", 3, debug_reg_mips_get(0x1C));
        printf("cop0r%.2i: 0x%.8x ", 5, debug_reg_mips_get(0x1E));
        printf("cop0r%.2i: 0x%.8x ", 7, debug_reg_mips_get(0x1B));
        printf("cop0r%.2i: 0x%.8x\n", 9, debug_reg_mips_get(0x1F));
        printf("cop0r%.2i: 0x%.8x ", 11, debug_reg_mips_get(0x1D));
        printf("\n");

    //Read
    } else if (*rw == 'r') {
        
        //Hardware
        if (*hs == 'h') {
            switch (reg_num)
            {
            //BPC 0x1C
            case 3:
                value = debug_reg_mips_get(0x1C);
                break;

            //BDA 0x1E
            case 5:
                value = debug_reg_mips_get(0x1E);
                break;

            //DCIC 0x1B
            case 7:
                value = debug_reg_mips_get(0x1B);
                break;

            //BDAM 0x1F
            case 9:
                value = debug_reg_mips_get(0x1F);
                break;

            //BPCM 0x1D
            case 11:
                value = debug_reg_mips_get(0x1D);
                break;

            default:
                printf("cop0r%i does not exist in hardware\n", reg_num);
                return -1;
            }

            printf("(HW) cop0r%i: 0x%x\n", reg_num, value);
        //Software
        //0xbe0344

        //TODO: limit this 
        } else if (*hs == 's') {
            if (reg_num < 32) {
                value = *(uint32_t*)(0xbe0344 + (reg_num * 4));
                printf("(SW) (0x%x) cop0r%i: 0x%.8x\n", (0xbe0344 + (reg_num * 4)), reg_num, value);
            } else {
                printf("No such register: cop0r%i\n", reg_num);
            }
        }
    
    //Write
    } else if (*rw == 'w') {
        
        //Hardware
        if (*hs == 'h') {
            switch (reg_num)
            {
            //BPC 0x1C
            case 3:
                debug_reg_mips_set(0x1C, value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_get(0x1C);
                break;

            //BDA 0x1E
            case 5:
                debug_reg_mips_set(0x1E, value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_get(0x1E);
                break;

            //DCIC 0x1B
            case 7:
                debug_reg_mips_set(0x1B, value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_get(0x1B);
                break;

            //BDAM 0x1F
            case 9:
                debug_reg_mips_set(0x1F, value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_get(0x1F);
                break;

            //BPCM 0x1D
            case 11:
                debug_reg_mips_set(0x1D, value);
                if (pm_settings.readback)
                    readback = debug_reg_mips_get(0x1D);
                break;

            default:
                printf("cop0r%i does not exist in hardware\n", reg_num);
                return -1;
            }
            if (pm_settings.readback)
                printf("(HW) cop0r%i: 0x%x (RB)\n", reg_num, readback);
        
        //Software
        } else if (*hs == 's') {
            if (reg_num < 32) {
                *(uint32_t*)(0xbe0344 + (reg_num * 4)) = value;
                if (pm_settings.readback) {
                    readback = *(uint32_t*)(0xbe0344 + (reg_num * 4));
                    printf("(SW) (0x%x) cop0r%i: 0x%.8x (RB)\n", (0xbe0344 + (reg_num * 4)), reg_num, readback);
                }
            } else {
                printf("No such register: cop0r%i\n", reg_num);
            }
        }
    //Invalid action
    } else {
        printf("Invalid action: %c\n", *rw);
        return -1;
    }

    return 0;
}

/* GTE commands:
*  gte <r> <reg>
*  gte <w> <reg> <value>
*  
*  arg1: <r|w>
*  arg2: <reg>
*  arg3: <value>
*/
static int command_gte()
{
    char *rw = pm_parser_get_argv_ptr(1);
    char *reg = pm_parser_get_argv_ptr(2);

    uint32_t value = pm_parser_get_argv_ptr(3);
    uint32_t reg_num = 0;
    uint32_t readback = 0;

    //Dump
    if (*rw == 'd') {
        printf("APU-GTE Instruction: 0x%.8x\n", debug_reg_gte_get(0x80));
        printf("APU-GTE Status:      0x%.8x\n", debug_reg_gte_get(0x81));
        printf("APU-GTE Cycles:      0x%.8x\n", debug_reg_gte_get(0x82));
        printf("(SW) (0x%x) Instruction: 0x%.8x\n", 0xbe03e0, *(uint32_t*)(0xbe03e0));
        
        printf("\nGTE Data Registers:\n");

        for (int i = 0; i < 32; i++) {
            printf("cop2r%.2i: 0%.8x ", i, debug_reg_gte_get(i));
            
            if ((i + 1) % 4 == 0)
                printf("\n");
        }
        printf("\nGTE Control Registers:\n");
        value = 0;
        for (int i = 32; i < 64; i++) {
            printf("cop2r%.2i: 0%.8x ", i, debug_reg_gte_get(i));
            
            if ((i + 1) % 4 == 0)
                printf("\n");
        }
        //Print any non-zero values found in range (64-128 & 131-255)
        printf("\nNon-zero registers found between cop2r64 - cop2r128 & cop2r131 - cop2r255:\n");
        for (int i = 0x40; i < 0x80; i++) {
            if (debug_reg_gte_get(i) != 0)
                printf("cop2r%i: 0x%x\n", i, debug_reg_gte_get(i));
        }
        for (int i = 0x83; i < 0xFF; i++) {
            if (debug_reg_gte_get(i) != 0)
                printf("cop2r%i: 0x%x\n", i, debug_reg_gte_get(i));
        }
    //Read
    } else if (*rw == 'r') {
        switch (*reg)
        {        
        //Instruction
        case 'i':
            value = debug_reg_gte_get(0x80);
            reg_num = 0x80;
            break;

        //Status
        case 's':
            value = debug_reg_gte_get(0x81);
            reg_num = 0x81;
            break;

        //Cycles
        case 'c':
            value = debug_reg_gte_get(0x82);
            reg_num = 0x82;
            break;

        default:
            if (pm_parser_char_is_digit(*reg)) {
                reg_num = pm_parser_get_argv_dec(2);
                if (reg_num < 0xFF) {
                    value = debug_reg_gte_get(reg_num); 
                } else {
                    printf("Invalid GTE reg %i\n", reg_num);
                    return -1;
                }
            } else {
                printf("Invalid GTE reg %c\n", *reg);
                return -1;
            }
            break;
        }

        printf("cop2r%.2i: 0x%.8x\n", reg_num, value);

    //Write
    } else if (*rw == 'w') {

        switch (*reg)
        {        
        //Instruction
        case 'i':
            debug_reg_gte_set(0x80, value);
            if (pm_settings.readback) {
                readback = debug_reg_gte_get(0x80);
                reg_num = 0x80;
            }
            break;

        //Status
        case 's':
            debug_reg_gte_set(0x81, value);
            if (pm_settings.readback) {
                readback = debug_reg_gte_get(0x81);
                reg_num = 0x81;
            }
            break;

        //Cycles
        case 'c':
            debug_reg_gte_set(0x82, value);
            if (pm_settings.readback) {
                readback = debug_reg_gte_get(0x82);
                reg_num = 0x82;
            }
            break;

        default:
            if (pm_parser_char_is_digit(*reg)) {
                reg_num = pm_parser_get_argv_dec(2);
                if (reg_num < 0xFF) {
                    debug_reg_gte_set(reg_num, value);
                    if (pm_settings.readback)
                        readback = debug_reg_gte_get(reg_num); 
                } else {
                    printf("Invalid GTE reg %i\n", reg_num);
                    return -1;
                }
            } else {
                printf("Invalid GTE reg %c\n", *reg);
                return -1;
            }
            break;
        }
        if (pm_settings.readback)
            printf("cop2r%.2i: 0x%.8x\n", reg_num, readback);
    
    } else {
        printf("Invalid action: %c\n", *rw);
        return -1;
    }

    return 0;
}

/* XPARAM commands:
*  xparam <d|r|w> {num} {value}
*  
*  arg1: <d|r|w>
*  arg2: {num}
*  arg3: {value}
*/
//TODO: resolve XPARAM_CPU_DELAY str to num or something like that?
static int command_xparam()
{
    //TEMP:
    void *xparam_str_ptr = 0xbe0a30;

    char *rw = pm_parser_get_argv_ptr(1);
    uint32_t num = pm_parser_get_argv_dec(2);
    uint32_t value = pm_parser_get_argv_dec(3);

    //List all XPARAMS
    if (*rw == 'd') {
        for (int i = 0; i < 18; i++) {
            printf("%.2i: 0x%.8x (%s)\n", i, *(uint32_t*)(0xbe09e0 + (i * 4)), *(char**)(xparam_str_ptr + (i * 4)));
        }

    //Read
    } else if (*rw == 'r') {
        if (num < 18) {
            value = *(uint32_t*)(XPARAM_BASE_ADDR + (num * 4));
            printf("%.2i: 0x%.8x (%s)\n", num, value, *(char**)(xparam_str_ptr + (num * 4)));
        } else {
            printf("XPARAM %i does not exist\n", num);
        }
    //Write
    } else if (*rw =='w') {
        if (num < 18) {
            *(uint32_t*)(XPARAM_BASE_ADDR + (num * 4)) = value;
        
            value = *(uint32_t*)(XPARAM_BASE_ADDR + (num * 4));
            printf("%.2i: 0x%.8x (%s)\n", num, value, *(char**)(xparam_str_ptr + (num * 4)));
        } else {
            printf("XPARAM %i does not exist\n", num);
        }
    } else {
        printf("Invalid action: %c\n", *rw);
        return -1;
    }
}

/* Emulator commands:
*  emu <d> <func>
*
*  arg1: <d>
*  arg2: <func>
*/
static int command_emu()
{
    char *d = pm_parser_get_argv_ptr(1);
    char *func = pm_parser_get_argv_ptr(2);

    uint32_t tbu = 0;
    uint32_t tbl = 0;

    //Dump
    if (*d == 'd') {

        switch (*func)
        {
        //Timing related (0xbe0294 - 0xbe02c4)
        case 't':
            tbu = debug_reg_ppc_get_tbu();
            tbl = debug_reg_ppc_get_tbl();

            printf("(0x%x) MIPS Clock Value:     0x%x (%iHz)\n", 0xbe0294, *(uint32_t*)0xbe0294, *(uint32_t*)0xbe0294);
            printf("(0x%x) MIPS Clock factor:    0x%x\n", 0xbe02b0, *(uint32_t*)0xbe02b0);
            printf("(0x%x) Anticipated Cycles at end:  0x%x\n", 0xbe0298, *(uint32_t*)0xbe0298);
            printf("(0x%x) Actual Cycles at end:       0x%x\n", 0xbe029c, *(uint32_t*)0xbe029c);
            printf("(0x%x) TBU at last delay event:    0x%x\n", 0xbe00a8, *(uint32_t*)0xbe00a8);
            printf("(0x%x) TBL at last delay event:    0x%x\n", 0xbe00ac, *(uint32_t*)0xbe00ac);
            printf("(0x%x) First open event:   0x%x\n", 0xbe02a0, *(uint32_t*)0xbe02a0);
            printf("(0x%x) End of event list:  0x%x\n", 0xbe02a4, *(uint32_t*)0xbe02a4);
            printf("(0x%x) Next event to fire: 0x%x\n", 0xbe02a8, *(uint32_t*)0xbe02a8);
            printf("(0x%x) Last event to fire: 0x%x\n", 0xbe02ac, *(uint32_t*)0xbe02ac);
            printf("(0x%x) CPU delay event:    0x%x\n", 0xbe02c0, *(uint32_t*)0xbe02c0);
            printf("(0x%x) Cycles until event: 0x%x\n", 0xbe02b4, *(uint32_t*)0xbe02b4);
            printf("(0x%x) Unk:                0x%x\n", 0xbe00b0, *(uint32_t*)0xbe00b0);

            //440MHz, period ~= 2.27ns
            printf("TBU Curr: 0x%x TBL Curr: 0x%x\n", tbu, tbl);
            printf("TBU Diff: 0x%x TBL Diff: 0x%x\n", (tbu - *(uint32_t*)0xbe00a8), (tbl - *(uint32_t*)0xbe00ac));

            break;

        //Events (0xbe00b4 - 0xbe0294)
        case 'e':
            printf("Events:\n");
            for (int i = 0; i < 0x1E0; i+=0x14) {
                printf("---------Event %i---------\n", (i / 0x14));
                printf("(0x%x) next entry:    0x%x\n", (0xbe00b4+i), *(uint32_t*)(0xbe00b4+i));
                printf("(0x%x) prev entry:    0x%x\n", (0xbe00b8+i), *(uint32_t*)(0xbe00b8+i));
                printf("(0x%x) trigger time:  0x%x\n", (0xbe00bc+i), *(uint32_t*)(0xbe00bc+i));
                printf("(0x%x) callback func: 0x%x\n", (0xbe00c0+i), *(uint32_t*)(0xbe00c0+i));
                printf("(0x%x) param:         0x%x\n", (0xbe00c4+i), *(uint32_t*)(0xbe00c4+i));
            }
            break;

        //DMA struct (0xbe0428 - 0xbe0690)
        case 'd':
            printf("(0x%x) DMA DPCR1: 0x%x\n",      0xbe0428, *(uint32_t*)0xbe0428);
            printf("(0x%x) DMA DPCR2: 0x%x\n",      0xbe042c, *(uint32_t*)0xbe042c);
            printf("(0x%x) DMA CTRL:  0x%x\n",      0xbe0430, *(uint32_t*)0xbe0430);
            printf("(0x%x) DMA INTR CTRL:  0x%x\n", 0xbe0434, *(uint32_t*)0xbe0434);
            printf("(0x%x) DMA INTR FLAGS: 0x%x\n", 0xbe0438, *(uint32_t*)0xbe0438);
            printf("(0x%x) DMA INTR MASK:  0x%x\n", 0xbe043c, *(uint32_t*)0xbe043c);
            printf("(0x%x) DMA IRM: 0x%x\n",        0xbe0440, *(uint32_t*)0xbe0440);
            printf("(0x%x) DMA DICR2(?): 0x%x\n",   0xbe0444, *(uint32_t*)0xbe0444);
            printf("(0x%x) DMA INTR enable(?): 0x%x\n", 0xbe0448, *(uint32_t*)0xbe0448);
            printf("(0x%x) DMA pending intr bits(?): 0x%x\n", 0xbe044c, *(uint32_t*)0xbe044c);
            printf("(0x%x) DMA next event(?): 0x%x\n", 0xbe0450, *(uint32_t*)0xbe0450);

            //(0xbe0454 - 0xbe0690)
            for (int i = 0; i < 0x23C; i+=0x2C) {
                printf("---------DMA Channel %i---------\n", (i / 0x2C));
                printf("(0x%x) MADR Reg: 0x%x\n", (0xbe0454 + i), *(uint32_t*)(0xbe0454 + i));
                printf("(0x%x) BCR Reg:  0x%x\n", (0xbe0458 + i), *(uint32_t*)(0xbe0458 + i));
                printf("(0x%x) CHCR Reg: 0x%x\n", (0xbe045c + i), *(uint32_t*)(0xbe045c + i));
                printf("(0x%x) TADR Reg: 0x%x\n", (0xbe0460 + i), *(uint32_t*)(0xbe0460 + i));
                printf("(0x%x) Start func:    0x%x\n", (0xbe0464 + i), *(uint32_t*)(0xbe0464 + i));
                printf("(0x%x) Callback func: 0x%x\n", (0xbe0468 + i), *(uint32_t*)(0xbe0468 + i));
                printf("(0x%x) MADR func: 0x%x\n", (0xbe046c + i), *(uint32_t*)(0xbe046c + i));
                printf("(0x%x) BCR func:  0x%x\n", (0xbe0470 + i), *(uint32_t*)(0xbe0470 + i));
                printf("(0x%x) TADR func: 0x%x\n", (0xbe0474 + i), *(uint32_t*)(0xbe0474 + i));
                printf("(0x%x) Event Ptr: 0x%x\n", (0xbe0478 + i), *(uint32_t*)(0xbe0478 + i));
                printf("(0x%x) Unk:       0x%x\n", (0xbe047c + i), *(uint32_t*)(0xbe047c + i));
            }
            break;

        //Interrupt related (0xbe0408 - 0xbe0a30) + HW INTC registers
        case 'i':
            printf("(0x%x) Intr SI_MASK:       0x%x\n", 0xbe0408, *(uint32_t*)(0xbe0408));
            printf("(0x%x) Intr I_STAT_SHADOW: 0x%x\n", 0xbe040c, *(uint32_t*)(0xbe040c));
            printf("(0x%x) Intr I_MASK_SHADOW: 0x%x\n", 0xbe0410, *(uint32_t*)(0xbe0410));
            printf("(0x%x) Intr I_CTRL_SHADOW: 0x%x\n", 0xbe0414, *(uint32_t*)(0xbe0414));
            printf("(0x%x) Intr Unk1 counter:  0x%x\n", 0xbe0418, *(uint32_t*)(0xbe0418));
            printf("(0x%x) Intr Unk2 counter:  0x%x\n", 0xbe041c, *(uint32_t*)(0xbe041c));
            printf("(0x%x) Intr VBLANK rel?:   0x%x\n", 0xbe0420, *(uint32_t*)(0xbe0420));
            printf("(0x%x) Intr Unk3 dec:      0x%x\n", 0xbe0424, *(uint32_t*)(0xbe0424));
            printf("(0x%x) Intr Unk4 dec:      0x%x\n", 0xbe0428, *(uint32_t*)(0xbe0428));
            printf("(0x%x) Intr Unk5 flag:     0x%x\n", 0xbe0a28, *(uint32_t*)(0xbe0a28));
            printf("(0x%x) Intr Unk6 flag:     0x%x\n", 0xbe0a2c, *(uint32_t*)(0xbe0a2c));
            break;

        //cop0 struct (0xbe02c4 - 0xbe03f8)
        case 'c':
            //Theres a struct (0xbe02c4 - 0xbe03c8) spanning all GP and cop0 registers
            //SW GP's appear unused, but dump them anyway
            for (int i = 0; i < 0x80; i+=4) {
                printf("(0x%x) MIPS r%i: 0x%x\n", (0xbe02c4+i), (i/4), *(uint32_t*)(0xbe02c4+i));
            }

            //Some of the COP0 registers are used however
            for (int i = 0; i < 0x80; i+=4) {
                printf("(0x%x) cop0r%i: 0x%x\n", (0xbe0344+i), (i/4), *(uint32_t*)(0xbe0344+i));
            }

            //Misc MIPS variables
            printf("(0x%x) MIPS Exception Vector: 0x%x\n", 0xbe03c4, *(uint32_t*)0xbe03c4);
            printf("(0x%x) MIPS Unk1: 0x%x\n", 0xbe03c8, *(uint32_t*)0xbe03c8);
            printf("(0x%x) MIPS Unk2: 0x%x\n", 0xbe03cc, *(uint32_t*)0xbe03cc);
            printf("(0x%x) MIPS Unk3: 0x%x\n", 0xbe03d0, *(uint32_t*)0xbe03d0);
            printf("(0x%x) MIPS Unk4: 0x%x\n", 0xbe03d4, *(uint32_t*)0xbe03d4);
            printf("(0x%x) MIPS Unk5: 0x%x\n", 0xbe03d8, *(uint32_t*)0xbe03d8);
            printf("(0x%x) MIPS Unk6: 0x%x\n", 0xbe03dc, *(uint32_t*)0xbe03dc);
            printf("(0x%x) MIPS GTE instr?: 0x%x\n", 0xbe03e0, *(uint32_t*)0xbe03e0);
            printf("(0x%x) MIPS Unk7: 0x%x\n", 0xbe03e4, *(uint32_t*)0xbe03e4);
            printf("(0x%x) MIPS Cache Control: 0x%x\n", 0xbe03e8, *(uint32_t*)0xbe03e8);
            printf("(0x%x) MIPS Unk8: 0x%x\n", 0xbe03ec, *(uint32_t*)0xbe03ec);
            printf("(0x%x) MIPS Scratchpad word 0: 0x%x\n", 0xbe03f0, *(uint32_t*)0xbe03f0);
            printf("(0x%x) MIPS Scratchpad word 1: 0x%x\n", 0xbe03f4, *(uint32_t*)0xbe03f4);
            break;

        //Rom ver related (0xbe082c - 0xbe0934)
        case 'r':
            printf("(0x%x) Rom Ver Enabled?: 0x%x\n", 0xbe082c, *(uint32_t*)0xbe082c);
            printf("(0x%x) Rom ptr: 0x%x\n", 0xbe0830, *(uint32_t*)0xbe0830);
            printf("0x100 copied from 0x1FC7FF00 on romver disable?:\n");
            for (int i = 0; i < 0x100; i+=4) {
                printf("(0x%x) 0x%x\n", 0xbe0834 + i, *(uint32_t*)(0xbe0834 + i));
            }
        break;

        //iLink related (0xbe0948 - 0xbe09e0)
        case 'l':
            for (int i = 0; i < 0x98; i+=4) {
                printf("(0x%x) iLink unk%i: 0x%x\n", (0xbe0948+i), (i/4), *(uint32_t*)(0xbe0948+i));
            }
        break;

        //SPU-SPU2 related (0xbe06e0 - 0xbe0788)
        case 's':
            printf("(0x%x) SPU active?:       0x%x\n", 0xbe06e0, *(uint32_t*)0xbe06e0);
            printf("(0x%x) SPU active flag?:  0x%x\n", 0xbe06e4, *(uint32_t*)0xbe06e4);
            printf("(0x%x) SPU start cycles?: 0x%x\n", 0xbe06e8, *(uint32_t*)0xbe06e8);
            printf("(0x%x) SPU Unk1: 0x%x\n", 0xbe06ec, *(uint32_t*)0xbe06ec);
            printf("(0x%x) SPU Unk2 TBL?: 0x%x\n", 0xbe06f0, *(uint32_t*)0xbe06f0);
            printf("(0x%x) SPU Unk3 TBL?: 0x%x\n", 0xbe06f4, *(uint32_t*)0xbe06f4);

            for (int i = 0; i < 0x18; i+=4) {
                printf("(0x%x) SPU ?: 0x%x\n", 0xbe06f8+i, *(uint32_t*)(0xbe06f8+i));
            }
        break;

        //PS1GPU / PGIF related (0xbe06a0 - 0xbe06e0)
        case 'p':
            for (int i = 0; i < 0x40; i+=4) {
                printf("(0x%x) PS1GPU/PGIF ?: 0x%x\n", 0xbe06a0+i, *(uint32_t*)(0xbe06a0+i));
            }
        break;
        
        //SIO0 related (0xbe07a8 - 0xbe07c4)
        case '0':
            printf("(0x%x) SIO0 TBL1: 0x%x\n", 0xbe07a8, *(uint32_t*)0xbe07a8);
            printf("(0x%x) SIO0 TBL2: 0x%x\n", 0xbe07ac, *(uint32_t*)0xbe07ac);
            printf("(0x%x) SIO0 transfer state: 0x%x\n", 0xbe07b0, *(uint32_t*)0xbe07b0);
            for (int i = 0; i < 0x10; i+=4) {
                printf("(0x%x) SIO0 ?: 0x%x\n", 0xbe07b4+i, *(uint32_t*)(0xbe07b4+i));
            }
        break;
        
        //PS1 timer related (0xbe0690 - 0xbe069c)
        case '1':
            printf("(0x%x) Timer0 delay enabled: 0x%x\n", 0xbe0690, *(uint32_t*)0xbe0690);
            printf("(0x%x) Timer0 last TBL: 0x%x\n",      0xbe0694, *(uint32_t*)0xbe0694);
            printf("(0x%x) Timer0 last val read: 0x%x\n", 0xbe0698, *(uint32_t*)0xbe0698);
        break;
        
        //SIO2 related (0xbe07c4 - 0xbe0824)
        case '2':
            printf("(0x%x) SIO2 pCtrl0_0: 0x%x\n", 0xbe07c4, *(uint32_t*)0xbe07c4);
            printf("(0x%x) SIO2 pCtrl0_1: 0x%x\n", 0xbe07c8, *(uint32_t*)0xbe07c8);
            printf("(0x%x) SIO2 pCtrl0_2: 0x%x\n", 0xbe07cc, *(uint32_t*)0xbe07cc);
            printf("(0x%x) SIO2 pCtrl0_3: 0x%x\n", 0xbe07d0, *(uint32_t*)0xbe07d0);
            printf("(0x%x) SIO2 pCtrl1_0: 0x%x\n", 0xbe07d4, *(uint32_t*)0xbe07d4);
            printf("(0x%x) SIO2 pCtrl1_1: 0x%x\n", 0xbe07d8, *(uint32_t*)0xbe07d8);
            printf("(0x%x) SIO2 pCtrl1_2: 0x%x\n", 0xbe07dc, *(uint32_t*)0xbe07dc);
            printf("(0x%x) SIO2 pCtrl1_3: 0x%x\n", 0xbe07e0, *(uint32_t*)0xbe07e0);
            for (int i = 0; i < 0x40; i+=4) {
                printf("(0x%x) SIO2 Queued transfer %i: 0x%x\n", 0xbe07e4+i, (i/4), *(uint32_t*)(0xbe07e4+i));
            }
        break;

        //MEDC related (0xbe0934 - 0xbe0948)
        case '3':
            for (int i = 0; i < 0x14; i+=4) {
                printf("(0x%x) MDEC ?: 0x%x\n", 0xbe0934+i, *(uint32_t*)(0xbe0934+i));
            }
        break;

        //DEV9 related (0xbe0788 - 0xbe07a4)
        case '9':
            for (int i = 0; i < 0x1C; i+=4) {
                printf("(0x%x) DEV9 ?: 0x%x\n", 0xbe0788+i, *(uint32_t*)(0xbe0788+i));
            }
        break;

        default:
            printf("Invalid func %c\n", *func);
            break;
        }
    } else {
        printf("Invalid action: %c\n", *d);
        return -1;
    }

    return 0;
}


/* Settings command:
*  settings <d|r|w> <num> {value}
*
*  arg1: <d|r|w>
*  arg2: <num>
*  arg3: {value}
*/
static int command_settings()
{
    char *drw = pm_parser_get_argv_ptr(1);

    uint8_t num = pm_parser_get_argv_dec(2);
    uint32_t value = pm_parser_get_argv_dec(3);

    if (*drw == 'd') {
        printf("1) Baud: %i\n", pm_settings.baud);
        printf("2) Event cycles: %i\n", pm_settings.event_cycles);
        printf("3) Readback: %i\n", pm_settings.readback);
    } else if (*drw == 'r') {
        
        switch (num)
        {
        case 1:
            printf("1) Baud: %i\n", pm_settings.baud);
            break;
        
        case 2:
            printf("2) Event cycles: %i\n", pm_settings.event_cycles);
            break;

        case 3:
            printf("3) Readback: %i\n", pm_settings.readback);
            break;

        default:
            break;
        }
        
    } else if (*drw == 'w') {
        switch (num)
        {
        case 1:
            if (value >= 9600 && value <= 115200) {
                pm_settings.baud = value;
                //set uart
                debug_uart_init(pm_settings.baud);
                printf("1) Baud set to %i\n", pm_settings.baud);
            } else {
                printf("Baud: %i is outside range 9600 - 115200\n");
            }
        break;

        case 2:
            if (value < 1)
                printf("Cycles can not be 0\n");
            else
                pm_settings.event_cycles = value;

            printf("2) Cycles set to: %i\n", pm_settings.event_cycles);
        break;   

        case 3:
            if (value > 1)
                pm_settings.readback = 1;
            else
                pm_settings.readback = 0;
            printf("3) Readback set to: %i\n", pm_settings.readback);
        break;

        default:
            break;
        }
    } else {
        printf("Invalid action %c\n", *drw);
        return -1;
    }
}

/* TODO: PPC core commands:
*  ppc <d|r|w>
*  
*  arg1: <d|r|w>
*/
static int command_ppc()
{
    char *drw = pm_parser_get_argv_ptr(1);

    //Dump
    if (*drw == 'd') {
        //info
        printf("PVR:    0x%x\n", debug_reg_ppc_sp_get(0x11F));
        printf("PIR:    0x%x\n", debug_reg_ppc_sp_get(0x11E));
        printf("MSR:    0x%x\n", debug_reg_ppc_get_msr());
        printf("CCR0:   0x%x\n", debug_reg_ppc_sp_get(0x3B3));
        printf("CCR1:   0x%x\n", debug_reg_ppc_sp_get(0x378));
        printf("MMUCR:  0x%x\n", debug_reg_ppc_sp_get(0x3B2));
        printf("RSTCFG: 0x%x\n", debug_reg_ppc_sp_get(0x39B));

        printf("DBCR0:  0x%x\n", debug_reg_ppc_sp_get(0x134));
        printf("DBCR1:  0x%x\n", debug_reg_ppc_sp_get(0x135));
        printf("DBCR2:  0x%x\n", debug_reg_ppc_sp_get(0x136));
        printf("DBDR:   0X%x\n", debug_reg_ppc_sp_get(0x3F3));
        
        /*
        //Instruction Cache Normal Victim 0 - 3
        for (int i = 0x370; i < 0x374; i++) {
            printf("INV%i: 0x%x\n", (i - 0x370), debug_reg_ppc_sp_get(i));
        }

        //Instruction Cache Transient Victim 0 - 3
        for (int i = 0x374; i < 0x378; i++) {
            printf("ITV%i: 0x%x\n", (i - 0x374), debug_reg_ppc_sp_get(i));
        }

        //Data Cache Normal Victim 0 - 3
        for (int i = 0x390; i < 0x394; i++) {
            printf("DNV%i: 0x%x\n", (i - 0x390), debug_reg_ppc_sp_get(i));
        }

        //Data Cache Transient Victim 0 - 3
        for (int i = 0x394; i < 0x398; i++) {
            printf("DTV%i: 0x%x\n", (i - 0x394), debug_reg_ppc_sp_get(i));
        }*/


    } else if (*drw == 'r') {

    } else if (*drw == 'w') {

    } else {
        printf("Invalid action: %c\n", *drw);
        return -1;
    }
}

pm_cmd_t pm_core_cmds[] = {
    {
        .name = "mem",
        .help = "mem r [width] <address> [length]\n"
                "mem w [width] <address> <value> [length]\n\n"
                "[width]:\n"
                "  Optional, default width is word (4 bytes)\n\n"
                "[length]:\n"
                "  Optional, read/write to/from (addr:addr + length)",
        
        .func = &command_mem
    },
    {
        .name = "reg",
        .help = "reg r <reg> <num> [length]\n"
                "reg w <reg> <num> <value> [length]\n\n"
                "<reg>:\n"
                "  d = DCR\n"
                "  r = PPC GP (not implemented)\n"
                "  s = PPC SP\n"
                "  m = APU register file 1 (MIPS + internal)\n"
                "  g = APU register file 2 (GTE + internal)\n"
                "  u = Unk indirect DCR (0x16-0x17)\n\n"
                "[length]:\n"
                "  Optional, read/write to/from span (reg:reg + length)",
        .func = &command_reg
    },
    {
        .name = "dump",
        .help = "dump <address> <length> [format]\n\n"
                "[format]:\n"
                "  Not implemented yet",
        .func = &command_dump
    },
    {
        .name = "find",
        .help = "find <value> <address> <length>",
        .func = &command_find
    },
    {
        .name = "mips",
        .help = "mips <d>\n"
                "mips <r> <reg>\n"
                "mips <w> <reg> <value>\n\n"
                "<reg>:\n"
                "  0-32 or a0, v0, etc\n"
                "  i = instruction\n"
                "  o = operand\n"
                "  s = status register (0x4)\n"
                "  h = HI\n"
                "  l = LO\n"
                "  p = PC\n"
                "  c = cycle count",
        .func = &command_mips
    },
    {
        .name = "cop0",
        .help = "cop0 <r> <h/s> <num>\n"
                "cop0 <w> <h/s> <num> <value>\n\n"
                "<h/s>:\n"
                "  h = hardware (only 3,5,7,9, and 11 exist in hardware)\n"
                "  s = software (all registers exist in software)",
        .func = &command_cop0
    },
    {
        .name = "gte",
        .help = "gte <r> <reg>\n"
                "gte <w> <reg> <value>\n\n"
                "<reg>:\n"
                "  0-255 = GTE registers\n"
                "  i = instruction\n"
                "  s = status\n"
                "  c = cycles",
        .func = &command_gte
    },
    {
        .name = "xparam",
        .help = "xparam <d|r|w> {num} {value}",
        .func = &command_xparam
    },
    {
        .name = "emu",
        .help = "emu <d> <func>\n\n"
                "<func>:\n"
                "  t = Timing       (be0294 - be02c4)\n"
                "  e = Events       (be00b4 - be0294)\n"
                "  d = DMA          (be0428 - be0690)\n"
                "  i = Interrupts   (be0408 - be0a30) + HW INTC registers\n"
                "  c = cop0         (be02c4 - be03f8)\n"
                "  r = RomVer       (be082c - be0934)\n"
                "  l = iLink        (be0948 - be09e0)\n"
                "  s = SPU-SPU2     (be06e4 - be0788)\n"
                "  p = PS1GPU/PGIF  (be06a0 - be06e0)\n"
                "  0 = SIO0         (be07a8 - be07c4)\n"
                "  1 = PS1 timer0   (be0690 - be069c)\n"
                "  2 = SIO2         (be07c4 - be0824)\n",
        .func = &command_emu
    },
    {
        .name = "settings",
        .help = "settings <d|r|w> <num> {value}",
        .func = &command_settings
    },
    {
        .name = "ppc",
        .help = "ppc <d|r|w>",
        .func = &command_ppc
    }
};