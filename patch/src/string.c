#include "string.h"

//Found in binary (TODO: verify these are correct)
char (*strcat)(char *dest, const char *src) = (void*)0xa0dc10;
char (*strchr)(const char *str, int c) = (void*)0xa0dc6c;
char (*strcpy)(char *dest, const char *src) = (void*)0xa0dc9c;
size_t (*strlen)(const char *str) = (void*)0xa0dcbc;
char (*strncpy)(char *dest, const char *src, size_t n) = (void*)0xa0dce8;

void (*memcpy)(void *dest, const void *src, size_t n) = (void*)0xa0c98c;
void (*memset)(void *str, int c, size_t n) = (void*)0xa0c9b4;


int strcmp(const char* str1, const char* str2)
{
    while(*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }

    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int strncmp(const char* str1, const char* str2, int n)
{
	while (n > 0)
	{
		if (*str1 != *str2)
			return *(const unsigned char*)str1 - *(const unsigned char*)str2;
		
		if (*str1 == '\0')
			return 0;
		
		n--;
	}
	
	return 0;
}

//TEMP: taken from gcc
//https://github.com/gcc-mirror/gcc/blob/master/libgcc/memmove.c

/* Public domain.  */
#include <stddef.h>

void *
memmove (void *dest, const void *src, int len)
{
  char *d = dest;
  const char *s = src;
  if (d < s)
    while (len--)
      *d++ = *s++;
  else
    {
      char *lasts = s + (len-1);
      char *lastd = d + (len-1);
      while (len--)
        *lastd-- = *lasts--;
    }
  return dest;
}