#ifndef MEMORY_H
#define MEMORY_H
#include "comm/mem_info.h"
#include "comm/types.h"
#include "tools/klib.h"
#include "tools/tools.h"
#include "memory/bitmap.h"
#include "DEBUG/debug.h"

#define MEM_KERNELFREE_START ((1024) * (1024))
#define MEM_KERNELFREE_END (2 * 1024 * 1024)
#define MEM_USERFREE_START (2 * 1024 * 1024)
#define MEM_USERFREE_END (128 * 1024 * 1024 - 1)
#define MEM_EBDA_START 0x80000

/**
 * @brief 管理地址分配的结构
 */
typedef struct _addr_alloc_t
{
    bitmap_t bitmap;

    uint32_t page_size;   // 页大小
    uint32_t kernelstart; // 内核空间可分配内存的起始地址
    uint32_t userstart;   // 用户空间可分配内存的起始地址
    uint32_t size;        // 可用地址空间总共的大小
} addr_alloc_t;

void memory_init(mem_info_t *mem_info);
uint32_t total_mem_size(mem_info_t *mem_info);
void alloc_init(uint8_t *bits, uint32_t size, uint32_t kstart, uint32_t ustart, uint32_t page_size);
uint32_t kalloc_pages(int n);
uint32_t ualloc_pages(int n);
int kfree_pages(uint32_t addr, int n);

#endif // !MEMORY_H