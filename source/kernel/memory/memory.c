#include "memory.h"
#include "memory/bitmap.h"

extern addr_alloc_t addr_manager;

/**
 * @brief 从addr开始释放n个页
 *
 * @param addr
 * @param n
 * @return int
 */
int kfree_pages(uint32_t addr, int n)
{
    /* 此处需要上锁 */
    uint32_t pg_idx = (addr - addr_manager.kernelstart) / addr_manager.page_size;
    bitmap_set_bit(&addr_manager.bitmap, pg_idx, n, 0);
    /* 此处需要上锁 */
    return 0;
}

/**
 * @brief 在内核空间分配n个页
 *
 * @return uint32_t
 */
uint32_t kalloc_pages(int n)
{
    /* 此处需要加锁 */
    /* 找到待分配内存在bitmap中的下标 */
    int page_index = bitmap_alloc_nbits(&addr_manager.bitmap, 0, n);
    /* 超出内核区域的范围 */
    if (page_index >= 1023)
    {
        return 0;
    }

    if (page_index >= 0)
    {
        /* 返回地址信息 */
        return addr_manager.kernelstart + page_index * MEM_PAGE_SIZE;
    }
    return 0;
}

/**
 * @brief 在用户空间分配n个页
 *
 * @param n
 * @return uint32_t
 */
uint32_t ualloc_pages(int n)
{
    /* 此处需要加锁 */
    /* 找到待分配内存在bitmap中的下标 */
    int page_index = bitmap_alloc_nbits(&addr_manager.bitmap,
                                        (MEM_USERFREE_START - MEM_KERNELFREE_START) / MEM_PAGE_SIZE,
                                        n);
    if (page_index >= 0)
    {
        /* 返回地址信息 */
        return addr_manager.kernelstart + page_index * MEM_PAGE_SIZE;
    }
    return 0;
}

void memory_init(mem_info_t *mem_info)
{
    /* 0x80000Byte以内的最后空间 */
    extern uint8_t *free_addr;
    uint8_t *mem_free = (uint8_t *)&free_addr;

    /* 获取1MB以上可用空间的大小 */
    /* 此处有些不准确 */
    uint32_t mem_free_size = total_mem_size(mem_info) - MEM_KERNELFREE_START;
    mem_free_size = DOWN2(mem_free_size, MEM_PAGE_SIZE);
    /* 初始化内存分配管理器 */
    /* bitmap的大小最大为4 * 1024 * 1024 * 1024 / 4096 / 8 = 128kb */
    /* 1MB~2MB可用来存放在内核相关的东西, 2MB~128MB可用来存放用户空间的东西 */
    alloc_init(mem_free, mem_free_size, MEM_KERNELFREE_START, MEM_USERFREE_START, MEM_PAGE_SIZE);
    /* 存放位图过的空间不算在剩余空间里面 */
    mem_free += bitmap_get_bytes(addr_manager.bitmap.count);
    ASSERT(mem_free < (uint8_t *)MEM_EBDA_START);
}

/**
 * @brief 计算可用内存空间的总大小
 *
 * @param mem_info
 * @return uint32_t
 */
uint32_t total_mem_size(mem_info_t *mem_info)
{
    /*需要上锁*/
    uint32_t mem_size = 0;
    for (int i = 0; i < mem_info->count; i++)
    {
        mem_size += mem_info->info[i].offset;
    }
    return mem_size;
}

void alloc_init(uint8_t *bits, uint32_t size, uint32_t kstart, uint32_t ustart, uint32_t page_size)
{
    /*需要上锁*/
    addr_manager.kernelstart = kstart;
    addr_manager.userstart = ustart;
    addr_manager.size = size;
    addr_manager.page_size = page_size;
    /*初始化位图*/
    bitmap_init(&addr_manager.bitmap, addr_manager.size / page_size, bits);
}