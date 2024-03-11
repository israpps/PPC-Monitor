#ifndef PPC_MON_H
#define PPC_MON_H

#include <stdint.h>
 
typedef struct pm_cmd_t
{
    const char *name;
    const char *help;
    int (*func)();
}pm_cmd_t;

typedef struct pm_settings_t
{
    uint32_t baud;          //UART baud, 9600 - 115200
    uint32_t event_cycles;  //How many n MIPS cycles, until rx_func runs again
    uint8_t readback;       //Whether or not written values are read back immediately.
    uint8_t halted;         //TODO
}pm_settings_t;

extern pm_settings_t pm_settings;

/* Core functions */
void pm_rx();

void pm_start();

/* Parser functions */

//Get arg count from rx_buf
int pm_parser_get_argc();

//Get len of argv n
int pm_parser_get_argv_len(int n);

//Get pointer to argv n in rx_buf
char *pm_parser_get_argv_ptr(int n);

//Convert ascii dec or hex arg n to int
int pm_parser_get_argv_dec(int n);

//Returns non-zero if char is digit
int pm_parser_char_is_digit(char c);

/* Command functions */
int pm_register_cmds(pm_cmd_t *cmds, int len);

#endif
