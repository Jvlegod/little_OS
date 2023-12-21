#ifndef BITMAP_H
#define BITMAP_H
#include "tools/klib.h"
#include "comm/types.h"

typedef struct _bitmap_t
{
    int count; /*位图需要表示的数据的数量*/
    uint8_t *bits;
} bitmap_t;

void bitmap_init(bitmap_t *bitmap, int count, uint8_t *bits);
int bitmap_get_bytes(int count);
void bitmap_set_bit(bitmap_t *bitmap, int index, int count, int bit);
int bitmap_get_bit(bitmap_t *bitmap, int index);
bool bitmap_is_set(bitmap_t *bitmap, int index);
int bitmap_alloc_nbits(bitmap_t *bitmap, int index, int count);
#endif // !BITMAP_H