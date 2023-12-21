#include "comm/mem_info.h"
#include "comm/os_cfg.h"
#include "irq/irq.h"
#include "comm/cpu_instr.h"
#include "gdt/gdt.h"
#include "mmu/pagetable.h"
#include "memory/memory.h"
#include "debug/debug.h"
#include "sync/sync.h"
#include "syscall/syscall.h"
#include "core/task.h"
// #include "task/task.h"
// #include "tss/tss.h"

void main();
gdt_entry_t gdt_table[GDT_SIZE];                                  /* 全局描述符表 */
pde_t kernel_table[NPDE] __attribute__((aligned(MEM_PAGE_SIZE))); /* 内核页表 */
addr_alloc_t addr_manager;                                        /* 内存管理器 */
irq_desc_t idt_table[IDT_SIZE];                                   /* 中断描述符表 */
task_manager_t task_manager;                                      /* 任务管理器 */

/**********************老的进程切换*************************************
// tss_t tss;                                                        /* 创造一个tss */
// thread_manager_t thread_manager;                                  /* 线程管理器 */
// proc_manager_t proc_manager; /* 进程管理器 */
// task_manage_t task_manager;  /* 任务(进程)管理器 */
/**********************老的进程切换*************************************

// void init_proc() __attribute__((section(".init_proc_section")));

/**
* @brief 内核的初始化
*
* @param mem
* @param gdt_table //  gdt_entry_t *gdt_table的参数为弃案
*/
void kernel_init(mem_info_t *mem)
{
    log_init();       /* 初始化日志输出 */
    gdt_init();       /* 初始化gdt */
    memory_init(mem); /* 初始化内存管理情况 */
    irq_init();       /* 初始化中断描述符表 */
    pagetable_init(); /* 初始化页表 */
    /**********************老的进程切换*************************************
    // task_init(main);  /* 初始化进程 */
    // tss_task_init();
    /**********************老的进程切换*************************************/
    timer_init();        /* 初始化定时器 */
    task_manager_init(); /* 初始化任务管理器 */
    syscall_init();      /* 初始化系统调用 */
}

void main()
{
    /**********************老的进程切换*************************************/
    // old进程切换
    // first_proc_init();
    /**********************老的进程切换*************************************/

    for (;;)
        ;
}