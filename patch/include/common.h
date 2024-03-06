#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

/* Temporary file for common functions and structs in binary */
#define XPARAM_BASE_ADDR 0xbe09e0

typedef struct event_t
{
	struct event_t *next_entry;
	struct event_t *prev_entry;
	int trigger_time;
	void (*callback)(uint32_t param);
	int param;
}event_t;

typedef struct dma_entry_t
{
	uint32_t MADR;
	uint32_t BCR;
	uint32_t CHCR;
	uint32_t TADR;
	void* start_func;
	void* callback_func;
	void* MADR_func;
	void* BCR_func;
	void* TADR_func;
	event_t* event_ptr;
	uint32_t unk;
}dma_entry_t;

extern int (*printf)(const char *fmt, ...);
extern event_t* (*add_event)(int trigger_time, void *callback, int param);
extern void (*lut_add_entry)(uint32_t addr, uint32_t size, void *ptr_load_handler, void *ptr_store_handler);

#endif