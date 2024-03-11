#include "common.h"

/* Functions from binary */

int (*printf)(const char *fmt, ...) = (void *)0xa04a18;
event_t *(*add_event)(int trigger_time, void *callback, int param) = (void *)0xa04ff4;
void (*lut_add_entry)(uint32_t addr, uint32_t size, void *ptr_load_handler, void *ptr_store_handler) = (void *)0xa069a4;
