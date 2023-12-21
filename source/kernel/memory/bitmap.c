#include "bitmap.h"

/**
 * @brief 位图的初始化
 *
 * @param bitmap
 * @param count
 * @param start
 */
void bitmap_init(bitmap_t *bitmap, int count, uint8_t *bits)
{
    bitmap->count = count;
    bitmap->bits = bits;
    int num = bitmap_get_bytes(bitmap->count);
    kmemset(bitmap->bits, 0, num);
}

/**
 * @brief 取得位图需要占用的字节/取得位图数组的大小
 *
 * @param count
 */
int bitmap_get_bytes(int count)
{
    /*向上取整*/
    return (count + 8 - 1) / 8;
}

/**
 * @brief 连续设置从index下标开始的count个位
 *
 * @param bitmap
 * @param index
 * @param count
 * @param bit 1/0
 */
void bitmap_set_bit(bitmap_t *bitmap, int index, int count, int bit)
{
    if (index >= bitmap->count)
    {
        /*超出边界*/
        return;
    }

    int cnt = index + count;
    for (int i = index; i < cnt; i++)
    {
        if (bit)
        {
            /*置1*/
            bitmap->bits[i / 8] |= 0x01 << (i % 8);
        }
        else
        {
            /*置0*/
            bitmap->bits[i / 8] &= ~(0x01 << (i % 8));
        }
    }
}

/**
 * @brief 获取位图指定下标的状态
 *
 * @param bitmap
 * @param index
 * @return int
 */
int bitmap_get_bit(bitmap_t *bitmap, int index)
{
    return (bitmap->bits[index / 8] & (0x01 << (index % 8))) ? 1 : 0;
}

/**
 * @brief 位图index处是否被置1
 *
 * @param bitmap
 * @param index
 * @return true
 * @return false
 */
bool bitmap_is_set(bitmap_t *bitmap, int index)
{
    return bitmap_get_bit(bitmap, index) ? true : false;
}

/**
 * @brief 找到合适大小的空间，连续分配count个页面
 *
 * @param addr_manager
 * @param index 起始下标
 * @param count 数目
 * @return i or -1
 */
int bitmap_alloc_nbits(bitmap_t *bitmap, int index, int count)
{
    int i = index;
    while (i < bitmap->count)
    {
        /* 如果找到合适的位置 */
        if (bitmap_get_bit(bitmap, i) == 0)
        {
            bool flag = false;
            for (int j = i; (j < bitmap->count) && (j - i + 1 <= count); j++)
            {
                if (bitmap_get_bit(bitmap, j) == 1)
                {
                    flag = true;
                    i = j;
                    break;
                }
            }

            /* 成功找到合适的位置 */
            if (flag == false)
            {
                /* 在位图上做标记 */
                bitmap_set_bit(bitmap, i, count, 1);
                return i;
            }
        }
        i++;
    }
    return -1;
}