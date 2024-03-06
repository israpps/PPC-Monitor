#ifndef STRING_H
#define STRING_H

#include <stddef.h>

//Found in binary
extern char (*strcat)(char *dest, const char *src);
extern char (*strchr)(const char *str, int c);
extern char (*strcpy)(char *dest, const char *src);
extern size_t (*strlen)(const char *str);
extern char (*strncpy)(char *dest, const char *src, size_t n);

extern void (*memcpy)(void *dest, const void *src, size_t n);
extern void (*memset)(void *str, int c, size_t n);

//Implemented here
extern int strcmp(const char* str1, const char* str2);
extern int strncmp(const char* str1, const char* str2, int n);

extern void *memmove (void *dest, const void *src, int len);


#endif