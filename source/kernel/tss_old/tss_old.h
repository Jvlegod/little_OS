#ifndef TSS_H
#define TSS_H
#include "comm/types.h"
#include "comm/os_cfg.h"
#include "gdt/gdt.h"
#include "debug/debug.h"
#include "tools/tools.h"
#include "tools/klib.h"
#include "mmu/pagetable.h"
#include "tools/klist.h"
#include "task/task.h"
#include "irq/irq.h"

/* tss */
typedef struct _tss_t
{
    uint32_t pre_link;
    uint32_t *esp0;
    uint32_t ss0;
    uint32_t *esp1;
    uint32_t ss1;
    uint32_t *esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t (*eip)(void);
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t *esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint32_t iomap;
} tss_t;

typedef struct _task_t
{
    task_status status; /* 进程状态 */
    tss_t tss;          /* 任务状态描述 */
    int tss_sel;        /* 选择子 */

    int ifkernel;
    struct _task_t *parent;

    klist_node_t state_node;   /* 状态节点 */
    klist_node_t general_node; /* 通用的节点 */

    int pid;
    int ticks;         /* 时间片 */
    int default_ticks; /* 初始时间片 */
} task_t;

typedef struct _task_manage_t
{
    task_t *cur_task; /* 当前正在运行的进程 */

    kdouble_list_t general_list; /* 任务链表 */
    kdouble_list_t ready_list;   /* 就绪链表 */
    kdouble_list_t sleep_list;   /* 睡眠链表 */

    irq_status irq_state; /* 用来进行进程切换时的原子操作 */
} task_manage_t;

void tss_task_init();
task_t *init_task(uint32_t entry);
void task_switch_from_to(task_t *from, task_t *to);
void switch_to_tss(int tss_sel);
void task_time_ticks();
task_t *cur_task();
void task_dispatch();
void task_set_general(task_t *task);
int init_tss(task_t *task, uint32_t entry, int ifkernel);
void task_set_ready(task_t *task);
void task_set_block(task_t *task);
task_t *task_schedule();
task_t *task_FIFO();

#endif // !TSS_H