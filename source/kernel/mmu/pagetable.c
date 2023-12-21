#include "pagetable.h"
#include "memory/memory.h"
#include "tools/log.h"

extern pde_t kernel_table[NPDE];
extern addr_alloc_t addr_manager;

void create_kernel_table()
{
    /*建立映射关系*/
    extern uint8_t kernel_base[], kernel_end[], stext[], etext[], sdata[], edata[];

    /* 给每个段分权限保护 */
    static mmu_manager_t kernel_map[NKERNEL_MAP] = {
        {kernel_base, kernel_end, kernel_base, PTE_W | PTE_U},                                                          // 内核栈区
        {stext, etext, stext, PTE_R | PTE_U},                                                                           // 内核代码区
        {sdata, (void *)(MEM_EBDA_START - 1), sdata, PTE_W | PTE_U},                                                    // 内核数据区
        {(void *)MEM_KERNELFREE_START, (void *)(MEM_KERNELFREE_END * 10), (void *)MEM_KERNELFREE_START, PTE_W | PTE_U}, // 内核空闲区
    };

    /* 初始化页目录表 */
    kmemset(kernel_table, 0, sizeof(kernel_table));

    /*根据映射关系开始建立页表*/
    for (int i = 0; i < sizeof(kernel_map) / sizeof(mmu_manager_t); i++)
    {
        mmu_manager_t *map = kernel_map + i;
        int vstart = DOWN2((uint32_t)map->vstart, MEM_PAGE_SIZE);
        int vend = UP2((uint32_t)map->vend, MEM_PAGE_SIZE);
        int npage = (vend - vstart) / MEM_PAGE_SIZE;

        // klog_printf("start to map from vaddr[%x] to vend[%x], the page's number is [%d]\n",
        //             vstart, vend, npage);

        /* 实现虚拟地址到物理地址的映射 */
        vaddr2paddr(kernel_table, vstart, (uint32_t)map->pstart, npage, map->perm);
    }

    /* 切换页表 */
    /* 此处kernel_table的地址必须保持为4KB对齐 */
    // wcr4(rcr4() | CR4_PSE);
    wcr3((uint32_t)kernel_table);
    wcr0(rcr0() | CR0_PG);
}

/**
 * @brief 寻找对应页表项
 *
 * @param dir
 * @param vaddr
 * @return pte_t*
 */
pte_t *find_pte(pde_t *dir, uint32_t vaddr)
{
    /* 页表 */
    pte_t *page_table;

    /* 索引到页目录项 */
    pde_t *pde = dir + VADDR_DIR(vaddr);

    /* 如果页表存在 */
    if (pde->present)
    {
        /* 找到页表的地址 */
        page_table = (pte_t *)PDE_ADDR(pde);
    }
    /* 页表不存在 */
    else
    {
        /* 分配内存空间存储页表 */
        uint32_t page_table_addr = kalloc_pages(1);
        if (page_table_addr == 0)
        {
            return NULL;
        }

        pde->v = page_table_addr | PDE_P | PDE_W | PDE_U;

        /* 创建一个新的页表 */
        page_table = (pte_t *)page_table_addr;
        /* 初始化页表 */
        kmemset(page_table, 0, MEM_PAGE_SIZE);
    }

    return page_table + VADDR_TABLE(vaddr);
}

/**
 * @brief 实现虚拟地址到物理地址的映射
 *
 * @param dir 页目录表
 * @param vaddr 虚拟地址
 * @param paddr 物理地址
 * @param count 分配的页的数目
 * @param perm 属性
 */
void vaddr2paddr(pde_t *dir, uint32_t vaddr, uint32_t paddr, int count, uint32_t perm)
{
    for (int i = 0; i < count; i++)
    {
        /* 找到对应页表项 */
        pte_t *pte = find_pte(dir, vaddr);

        /* 如果页表未查到 */
        if (pte == NULL)
        {
            klog_printf("vaddr2paddr erro: map failed, pte have been existed");
            return;
        }

        ASSERT(pte->present == 0);
        /* 设置页表项的属性 */
        pte->v = paddr | perm | PTE_P;
        // klog_printf("the pte's paddr is %x, its content is %x\n", (uint32_t)pte, pte->v);

        vaddr += MEM_PAGE_SIZE;
        paddr += MEM_PAGE_SIZE;
    }
}

void pagetable_destroy(pde_t *dir)
{
    uint32_t user_pde_start = VADDR_DIR(MEM_KERNELFREE_END);
    pde_t *pde = (pde_t *)dir + user_pde_start;
    ASSERT(dir != NULL);

    for (int i = user_pde_start; i < PDE_CNT; i++, pde++)
    {
        if (!pde->present)
        {
            continue;
        }

        // 获取页表的地址
        pte_t *pte = (pte_t *)PDE_ADDR(pde);
        for (int j = 0; j < PTE_CNT; j++, pte++)
        {
            if (!pte->present)
            {
                continue;
            }
            kfree_pages(PTE_ADDR(pte), 1);
        }
        kfree_pages(PDE_ADDR(pde), 1);
    }
    kfree_pages((uint32_t)dir, 1);
}

uint32_t copy_pagetable(pde_t *dir)
{
    uint32_t to_page_dir = (uint32_t)create_page_table();

    /* 找到待复制页表的用户空间部分 */
    uint32_t user_pde_start = VADDR_DIR(MEM_KERNELFREE_END);
    pde_t *pde = (pde_t *)dir + user_pde_start;

    /* 遍历页目录表 */
    for (int i = user_pde_start; i < PDE_CNT; i++, pde++)
    {
        if (!pde->present)
        {
            continue;
        }

        /* 取出页表的地址 */
        pte_t *pte = (pte_t *)PDE_ADDR(pde);
        for (int j = 0; j < PTE_CNT; j++, pte++)
        {
            if (!pte->present)
            {
                continue;
            }

            /* 分配物理页 */
            uint32_t page = ualloc_pages(1);

            uint32_t vaddr = (i << 22) | (j << 12);
            vaddr2paddr((pde_t *)to_page_dir, vaddr, page, 1, (pte->v & 0x1FF));

            /* 将内容拷贝过去 */
            kmemcpy((void *)page, (void *)vaddr, MEM_PAGE_SIZE);
        }
    }
    return to_page_dir;
}

uint32_t *create_page_table()
{
    pde_t *dir = (pde_t *)ualloc_pages(1); /* 获取一页的空间存储页表 */
    ASSERT(dir != NULL);

    kmemset(dir, 0, MEM_PAGE_SIZE);

    /* 建立内核空间的页目录项 */
    uint32_t user_pde_start = VADDR_DIR(MEM_KERNELFREE_END);
    for (int i = 0; i <= user_pde_start; i++)
    {
        dir[i].v = kernel_table[i].v;
    }
    /* 用户空间暂不建立 */

    return (uint32_t *)dir;
}

void pagetable_init() /*内核页表*/
{
    create_kernel_table();
}

/**
 * @brief 得到虚拟地址映射的物理地址
 *
 * @param dir
 * @return uint32_t
 */
uint32_t get_v2p(pde_t *dir, uint32_t vaddr)
{
    pte_t *pte = find_pte(dir, vaddr);
    return PTE_ADDR(pte) + (vaddr & 0xFFF);
}