#ifndef KLIB_H
#define KLIB_H
#include "comm/types.h"
#include <stdarg.h>
#include "log.h"

typedef enum
{
    __INT_TYPE,
    __UINT8_TYPE,
    __UINT16_TYPE,
    __UINT32_TYPE,
    __FLOAT_TYPE,
    __DOUBLE_TYPE,
    __CHAR_TYPE,
} DateType;

char *kitoa_s(char *buf, int value, int size);
void kmemcpy(void *dest, void *src, int size);
void kmemset(void *dest, uint8_t data, int size);
void kstrcpy(char *dest, char const *src);
void klog_printf(const char *fmt, ...);
void klog_vsprintf(char *buf, const char *fmt, va_list args);
void kreverse_intstr(int *buf);
void kreverse_charstr(char *buf);
void kswap(void *a, void *b, DateType type);
int karr_strlen(int *buf);
int kstrlen(char const *buf);
int kintlen(int num);

#endif // !KLIB_H