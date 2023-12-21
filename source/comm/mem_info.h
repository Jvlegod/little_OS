#ifndef BOOT_INFO_H
#define BOOT_INFO_H
#include "comm/types.h"

#define MAX_MEM_LENGTH 20
#define MEM_PAGE_SIZE 4096 /*页面大小*/

typedef struct _mem_info_t
{
    // 实际使用的时候只需要使用其低32位
    struct _info_t
    {
        uint32_t start;
        uint32_t offset;
    } info[MAX_MEM_LENGTH];

    int count;
} mem_info_t, boot_info_t;

typedef struct _ARDS_entry_t
{

    uint32_t BaseL;

    uint32_t BaseH;

    uint32_t LengthL;

    uint32_t LengthH;

    uint32_t Type; // 值为1时表明为我们可用的RAM空间
} __attribute__((packed)) ARDS_entry_t;

/**
 * @brief 地址管理器,管理虚拟地址到物理地址的映射
 *
 */
typedef struct _mmu_manager_t
{
    void *vstart; /*虚拟地址*/
    void *vend;
    void *pstart;  /*物理地址*/
    uint32_t perm; /*地址的权限*/
} mmu_manager_t;

#endif // !BOOT_INFO_H