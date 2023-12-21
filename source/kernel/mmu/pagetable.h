#ifndef PAGETABLE_H
#define PAGETABLE_H
#define NPDE 1024
#include "comm/types.h"
#include "comm/mem_info.h"
#include "tools/tools.h"
#include "tools/log.h"
#include "comm/cpu_instr.h"
#include "memory/memory.h"
#include "comm/os_cfg.h"
#include "debug/debug.h"

#define NKERNEL_MAP 4                                    /* 内核去映射区段的数目 */
#define VADDR_DIR(vaddr) (vaddr >> 22)                   /* 页目录项的索引 */
#define VADDR_TABLE(vaddr) ((vaddr >> 12) & 0x00003FF)   /* 页表项的索引 */
#define PDE_ADDR(pde) ((uint32_t)pde->phy_pt_addr << 12) /* 页表的地址 */
#define PTE_ADDR(pte) ((uint32_t)pte->phy_page_addr << 12)

// 页目录项的属性设置
#define PDE_CNT 1024
#define PDE_P (1 << 0)
#define PDE_W (1 << 1)
#define PDE_R (0 << 1)
#define PDE_U (1 << 2)
#define PDE_S (0 << 2)
#define PDE_PWT (1 << 3)
#define PDE_PCD (1 << 4)
#define PDE_A (1 << 5)
#define PDE_D (1 << 6)
// 第7位4KB模式默认为0，4MB模式为1
#define PDE_PS (1 << 7)
#define PDE_G (1 << 8)

// 页表项的属性设置
#define PTE_CNT 1024
#define PTE_P (1 << 0)
#define PTE_W (1 << 1)
#define PTE_R (0 << 1)
#define PTE_U (1 << 2)
#define PTE_S (0 << 2)
#define PTE_PWT (1 << 3)
#define PTE_PCD (1 << 4)
#define PTE_A (1 << 5)
#define PTE_D (1 << 6)
#define PTE_PAT (1 << 7)
#define PTE_G (1 << 8)

#pragma pack(1)
/**
 * @brief 页目录项
 *
 */
typedef union _pde_t
{
    uint32_t v;
    struct
    {
        uint32_t present : 1;       // 0 (P) Present; must be 1 to map a 4-KByte page
        uint32_t write_disable : 1; // 1 (R/W) Read/write, if 0, writes may not be allowe
        uint32_t user_mode_acc : 1; // 2 (U/S) if 0, user-mode accesses are not allowed t
        uint32_t write_through : 1; // 3 (PWT) Page-level write-through
        uint32_t cache_disable : 1; // 4 (PCD) Page-level cache disable
        uint32_t accessed : 1;      // 5 (A) Accessed
        uint32_t : 1;               // 6 Ignored;
        uint32_t ps : 1;            // 7 (PS)
        uint32_t : 4;               // 11:8 Ignored
        uint32_t phy_pt_addr : 20;  // 高20位页表的物理地址
    };
} pde_t;

/**
 * @brief 页表项
 */
typedef union _pte_t
{
    uint32_t v;
    struct
    {
        uint32_t present : 1;        // 0 (P) Present; must be 1 to map a 4-KByte page
        uint32_t write_disable : 1;  // 1 (R/W) Read/write, if 0, writes may not be allowe
        uint32_t user_mode_acc : 1;  // 2 (U/S) if 0, user-mode accesses are not allowed t
        uint32_t write_through : 1;  // 3 (PWT) Page-level write-through
        uint32_t cache_disable : 1;  // 4 (PCD) Page-level cache disable
        uint32_t accessed : 1;       // 5 (A) Accessed;
        uint32_t dirty : 1;          // 6 (D) Dirty
        uint32_t pat : 1;            // 7 PAT
        uint32_t global : 1;         // 8 (G) Global
        uint32_t : 3;                // Ignored
        uint32_t phy_page_addr : 20; // 高20位物理地址
    };
} pte_t;

#pragma pack()

void pagetable_init();      /*页表初始化*/
void create_kernel_table(); /*创建内核页表*/
uint32_t *create_page_table();
void pagetable_destroy(pde_t *dir);
uint32_t copy_pagetable(pde_t *dir);
void vaddr2paddr(pde_t *dir, uint32_t vaddr, uint32_t paddr, int count, uint32_t perm);
pte_t *find_pte(pde_t *dir, uint32_t vaddr);
uint32_t get_v2p(pde_t *dir, uint32_t vaddr);

#endif // !PAGETABLE_H