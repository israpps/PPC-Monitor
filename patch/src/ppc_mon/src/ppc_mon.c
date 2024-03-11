#include <stdint.h>

//TODO: include paths in makefile
#include "ppc_mon/include/ppc_mon.h"
#include "ppc_mon/include/commands.h"
#include "string.h"
#include "common.h"
#include "debug.h"

#include "umm_malloc/umm_malloc.h"

#define SERIAL_SCROLLBACK_LIMIT 5
#define RX_BUFFER_SIZE 64

typedef struct pm_cmd_list_t
{
    struct pm_cmd_list_t *next_list;
    pm_cmd_t *commands;
    uint32_t length;
}pm_cmd_list_t;

typedef struct line_t
{
    char str[RX_BUFFER_SIZE];
    uint8_t len;
}line_t;

static line_t prev_lines[SERIAL_SCROLLBACK_LIMIT+1];
static uint8_t line_idx; //Ring buffer index
static int scr_idx;		 //Scrollback index
static int scr_cnt;      //Scrollback count

static char rx_buf[RX_BUFFER_SIZE];
static uint8_t rx_idx;
static char prev_char;

pm_cmd_list_t *registered_cmds;
pm_settings_t pm_settings;

static int ascii_dec_to_int(char *str)
{
    int value = 0;
    int dec = 0;

    while ((*str != '\0') && (*str != ' ')) {        
        dec = *str - '0';

        if (dec >= 0 && dec <= 9)
            value = value * 10 + dec;
        else 
            return 0;

        str++;
    }

    return value;
}

static int ascii_hex_to_int(char *str)
{
    int value = 0;
    int hex = 0;

    while ((*str != '\0') && (*str != ' ')) {

        if (*str >= '0' && *str <= '9')
            hex = (*str - '0');
        else if (*str >= 'A' && *str <= 'F')
            hex = (*str - 'A' + 10);
        else if (*str >= 'a' && *str <= 'f')
            hex = (*str - 'a' + 10);
        else
            hex = -1;

        value = value * 16 + hex;

        str++;
    }
    
    return value;
}

//Get arg count from rx_buf
int pm_parser_get_argc()
{
	int argc = 1;
    char *str = &rx_buf;

	while (*str != '\0') {
		//Ignore extra whitespace
		if (*str == ' ' && *(str+1) != ' ')
			argc++;

		str++;
	}

	return argc;
}

int pm_parser_get_argv_len(int n)
{
	uint32_t arg = 0;
	uint32_t len = 0;

    char *str = &rx_buf;

	while (*str != '\0')
	{
		if (arg == n && *str != ' ')
			len++;

		if (*str == ' ' && *(str+1) != ' ') {
			arg++;
            if (arg > n)
                break;
		}
		str++;
	}

	return len;
}

//Get pointer to arg offset in rx_buf
char *pm_parser_get_argv_ptr(int n)
{	
	uint32_t arg = 0;
	uint32_t pos = 0;

    char *str = &rx_buf;

	while (*str != '\0')
	{
		if (arg == n)
			break;
		
		//Ignore extra whitespace
		if (*str == ' ' && *(str+1) != ' ')
			arg++;

		pos++;
		str++;
	}

	return &rx_buf[pos];
}

//Convert ascii dec or hex arg to int
int pm_parser_get_argv_dec(int n)
{
	uint32_t argc = 0;
	uint32_t value = 0;

    char *str = &rx_buf;

	while (*str != '\0')
	{
		if (argc == n) {
			//handle 0x prefix
			if (*str == '0' && *(str+1) == 'x')
				value = ascii_hex_to_int(str+2);
			else
				value = ascii_dec_to_int(str);
			break;
		}
		if (*str == ' ')
			argc++;

		str++;
	}

	return value;
}

int pm_parser_char_is_digit(char c)
{
    return '0' <= c && c <= '9';
}

int pm_register_cmds(pm_cmd_t *cmds, int len)
{
    pm_cmd_list_t *prev_list = registered_cmds;
    pm_cmd_list_t *new_list = NULL;

    //First run
    if (registered_cmds == NULL) {
        registered_cmds = umm_malloc(sizeof(pm_cmd_list_t));
        
        if (registered_cmds == NULL) {
            printf("%s: failed to malloc\n", __func__);
            return -1;
        }

        new_list = registered_cmds;

        new_list->commands = cmds;
        new_list->length = len;
        new_list->next_list = NULL;

    //All other runs
    } else {
        while (prev_list->commands != NULL) {
            //Search for duplicate entries
            for (int i = 0; i < len; i++) {
                for (int j = 0; j < prev_list->length; j++) {
                    if (strcmp(prev_list->commands[j].name, cmds[i].name) == 0) {
                        printf("Duplicate command found: %s, aborting\n", prev_list->commands[j].name);
                        return -1;
                    }
                }
            }

            if (prev_list->next_list != NULL) {
                prev_list = prev_list->next_list;
            } else {
                break;
            }
        }

        new_list = umm_malloc(sizeof(pm_cmd_list_t));
        if (new_list == NULL) {
            printf("%s: failed to malloc\n", __func__);
            return -1;
        }

        new_list->commands = cmds;
        new_list->length = len;
        new_list->next_list = NULL;

        //Link new list
        prev_list->next_list = new_list;
    }

    /*
    printf("PPC-MON Registered %i commands\n", new_list->length);
    for (int i = 0; i < new_list->length; i++) {
        printf("Command: %s\n", new_list->commands[i].name);
        printf("Function: 0x%x\n", new_list->commands[i].func);
    }*/

    return 0;
}

//TODO: cleanup
static void pm_handle_cmd()
{
    char cmd[32]; //adjust
    int cmd_len = pm_parser_get_argv_len(0);
    char *offset;

    int mode = 0;

    strncpy(cmd, rx_buf, cmd_len);
    cmd[cmd_len] = '\0';

    //"help" and "list" are special commands
    if (strcmp(cmd, "help") == 0) {
        //Set mode
        mode = 1;
        
        //Get actual cmd
        cmd_len = pm_parser_get_argv_len(1);
        offset = pm_parser_get_argv_ptr(1);
        
        strncpy(cmd, offset, cmd_len);
        cmd[cmd_len] = '\0';

    } else if (strcmp(cmd, "list") == 0) {
        mode = 2;
    }

    pm_cmd_list_t *cmd_list = registered_cmds;
    int (*cmd_func)() = NULL;

    //Search through cmd lists
    while (cmd_list->commands != NULL) {
        for (int i = 0; i < cmd_list->length; i++) {

            if (mode == 2) {
                printf("%s\n", cmd_list->commands[i].name);
            }

            if (strcmp(cmd, cmd_list->commands[i].name) == 0) {
                if (mode == 0) {
                    cmd_func = cmd_list->commands[i].func;
                } else {
                    printf("%s\n", cmd_list->commands[i].help);
                }
                break;
            }
        }

        if (cmd_list->next_list == NULL || cmd_func != NULL)
            break;

        cmd_list = cmd_list->next_list;
    }

    if (cmd_func != NULL) {
        int res = cmd_func();
    
        if (res < 0)
            printf("%s: command returned error %i\n", __func__, res);
    
    } else if (cmd_func == NULL && mode <= 0) {
        printf("Invalid command: %s, use list for available commands or help cmd for usage\n", cmd);
    }
}

static void serial_print_newline()
{
    scr_idx = line_idx;
    scr_cnt = -1;
    rx_idx = 0;

    printf("\r\n>");
}

static void serial_print_prev_line(int idx)
{
    //Clear line and print prev cmd
    printf("\r%*c\r>%s", RX_BUFFER_SIZE, ' ', prev_lines[idx].str);
    
    //Copy prev cmd to current buffer and adjust index
    strcpy(rx_buf, prev_lines[idx].str);
    rx_idx = prev_lines[idx].len;
}

static void serial_scr_up()
{
	if (scr_cnt < SERIAL_SCROLLBACK_LIMIT) {
		scr_cnt++;
		scr_idx--;

		if (scr_idx < 0)
			scr_idx = SERIAL_SCROLLBACK_LIMIT;

		serial_print_prev_line(scr_idx);
	}
}

static void serial_scr_down()
{
    if (scr_cnt > 0) {
		scr_cnt--;
        scr_idx++;
        if (scr_idx > SERIAL_SCROLLBACK_LIMIT)
            scr_idx = 0;

        serial_print_prev_line(scr_idx);
    }
}

void pm_rx()
{
    char c;

    do {
        //Schedule RX function to run every n MIPS cycles (2000 by default)
        if (debug_ppc_config.halted == 0)
            add_event(pm_settings.event_cycles, &pm_rx, 0);

        //Poll UART status register
        while (*(uint8_t*)(0x01000205) & 1 != 0) {

            //Read char from UART FIFO
            c = *(char*)(0x01000200);

            switch (c)
            {
            //CR (Enter)
            case '\r':
                printf("\r\n");

                //term str
                rx_buf[rx_idx] = '\0';
        
                //Process command
                pm_handle_cmd();

                //Copy to prev line buffer if not already present
                if (strcmp(prev_lines[scr_idx].str, rx_buf) != 0) {
                    strcpy(prev_lines[line_idx].str, rx_buf);
                    prev_lines[line_idx].len = rx_idx;

                    line_idx++;

                    if (line_idx > SERIAL_SCROLLBACK_LIMIT)
                        line_idx = 0;
                }
                serial_print_newline();
            break;
            
            //DEL (backspace)
            case 0x7F:
                if (rx_idx != 0) {
                    rx_buf[rx_idx-1] = '\0';
                    rx_idx--;
                    printf("\b \b"); 
                }
            break;

            //EXT (CTRL+C)
            case 0x3:
                //Disable DAC2R and DAC2W event bits in DBCR0
                debug_reg_ppc_sp_set(0x134, (debug_reg_ppc_sp_get(0x134) & ~(3 << 16)));
                //Clear DAC2 address
                debug_reg_ppc_sp_set(0x13D, 0x0);
                
                serial_print_newline();
            break;

            //Partial escape sequences, do nothing
            case '[':
            break;

            case 0x1B:
            break;
            
            default:
                //Escape sequences
                if (prev_char == '[') {
                    //Up arrow
                    if (c == 'A') {
                        serial_scr_up();
                    //Down arrow
                    } else if (c == 'B') {
                        serial_scr_down();
                    //Right arrow
                    } else if (c == 'C') {

                    //Left arrow
                    } else if (c == 'D') {

                    }
                //Add char to buffer
                } else {
                    if (rx_idx < RX_BUFFER_SIZE) {
                        printf("%c", c); //echo char back
                        rx_buf[rx_idx] = c;
                        rx_idx++;
                    } else {
                        printf("\nBuffer full, command discarded\n");
                        serial_print_newline();
                    }
                }
            break;
            }

            prev_char = c;
        }

    } while (debug_ppc_config.halted);
}

void pm_start()
{   
    //Init values
    rx_idx = 0;
    line_idx = 0;
    scr_idx = 0;
    scr_cnt = -1;

    memset(rx_buf, '\0', RX_BUFFER_SIZE);

    for (int i = 0; i <= SERIAL_SCROLLBACK_LIMIT; i++) {
        prev_lines[i].str[0] = '\0';
    }

    registered_cmds = NULL;

    //PPC-MON settings
    pm_settings.baud = 57600;
    pm_settings.event_cycles = 2000;
    pm_settings.readback = 1;
    pm_settings.halted = 0;

    //Apply baud / reinit UART
    debug_uart_init(pm_settings.baud);

    //Register PPC-MON core commands, mem, reg, mips, etc
    pm_register_cmds(&pm_core_cmds, 12);

	//Preserve PPC-MON RX event through mode reset (PS2 <-> PS1)
	debug_run_on_reset(&pm_rx); 

    //Init debug_ppc_config values
    debug_ppc_config.hooked = 0;
    debug_ppc_config.halted = 0;
    debug_ppc_config.addr = 0;
    debug_ppc_config.rw = 0;
    debug_ppc_config.wb = 0;

	printf("\nWelcome to PPC-MON v%d.%d.1\n", MAJOR, MINOR);
	printf(">");

    //Start RX func
	pm_rx();
}