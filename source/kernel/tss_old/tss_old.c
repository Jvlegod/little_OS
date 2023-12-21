#include "tss.h"

extern gdt_entry_t gdt_table[GDT_SIZE];
extern task_manage_t task_manager;
pde_t kernel_table[NPDE] __attribute__((aligned(MEM_PAGE_SIZE)));

void tss_task_init()
{
    // 队列初始化
    klist_init(&task_manager.ready_list);
    klist_init(&task_manager.sleep_list);
    klist_init(&task_manager.general_list);

    irq_status status = irq_enter_protection();
    task_t *task = (task_t *)kalloc_pages(1);
    ASSERT(task != NULL);
    task->ticks = task->default_ticks = TASK_TICKS_DEFAULT;
    task->pid = alloc_pid();
    task_manager.cur_task = task;
    task->ifkernel = KERNEL_PROC;
    task->parent = NULL;

    task_set_general(task);
    task_set_ready(task);
    task->status = TASK_RUNNING;

    init_tss(task, 0, task->ifkernel);
    ltr(task->tss_sel);
    irq_leave_protection(status);
}

int init_tss(task_t *task, uint32_t entry, int ifkernel)
{
    /* 获取空闲项 */
    int tss_sel = gdt_alloc_seg();
    ASSERT(tss_sel > 0);

    /* 设置选择子 */
    gdt_segment_set(tss_sel, sizeof(tss_t), (uint32_t)&task->tss,
                    SEG_S_SYSTEM | SEG_TYPE_TSS | SEG_DPL0 | SEG_P_PRESENT);

    kmemset(&task->tss, 0, sizeof(tss_t));

    uint32_t kernel_stack = kalloc_pages(1);
    task->tss.eip = (void *)entry;
    task->tss.esp0 = (uint32_t *)kernel_stack;
    task->tss.ss0 = KERNEL_SELECTOR_DS;
    if (ifkernel == KERNEL_PROC)
    {
        task->tss.esp = (uint32_t *)kalloc_pages(1);
        task->tss.ss = task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_DS;
        task->tss.cs = KERNEL_SELECTOR_CS;
        task->tss.cr3 = (uint32_t)kernel_table;
    }
    else
    {
        task->tss.esp = (uint32_t *)ualloc_pages(1);
        task->tss.ss = task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = USER_SELECTOR_DS;
        task->tss.cs = USER_SELECTOR_CS;
        task->tss.cr3 = (uint32_t)create_page_table();
    }

    task->tss.eflags = ELFAGS_MBS | EFLAGS_IF;
    task->tss_sel = tss_sel;

    return 0;
}

task_t *init_task(uint32_t entry)
{
    irq_status status = irq_enter_protection();
    task_t *task = (task_t *)kalloc_pages(1);
    task->ticks = task->default_ticks = TASK_TICKS_DEFAULT;
    task->pid = alloc_pid();
    task->ifkernel = USER_PROC;
    task->parent = NULL;

    task_set_general(task);
    task_set_ready(task);

    ASSERT(task != NULL);
    init_tss(task, entry, task->ifkernel);
    irq_leave_protection(status);
    return task;
}

void task_switch_from_to(task_t *from, task_t *to)
{
    switch_to_tss(to->tss_sel);
}

void switch_to_tss(int tss_sel)
{
    far_jmp(tss_sel, 0);
}

task_t *cur_task()
{
    return task_manager.cur_task;
}

void task_time_ticks()
{
    irq_status state = irq_enter_protection();
    /* 获取进程的信息 */
    task_t *cur = cur_task();

    if (--cur->ticks <= 0)
    {
        /* 重新插入就绪队列的尾部 */
        task_set_ready(cur);

        cur->ticks = cur->default_ticks;

        task_dispatch();
    }
    irq_leave_protection(state);
}

void task_dispatch()
{
    task_t *cur = cur_task();
    task_t *next = task_schedule();
    if (next != cur && next->status == TASK_READY)
    {
        task_manager.cur_task = next;
        next->status = TASK_RUNNING;
        task_switch_from_to(cur, next);
    }
    else if (klist_len(&task_manager.ready_list) == 1)
    {
        /* 只有一个线程则无法切换 */
        return;
    }
    return;
}
void task_set_general(task_t *task)
{
    klist_append(&task_manager.general_list, &task->general_node);
}

void task_set_ready(task_t *task)
{
    klist_remove(&task_manager.ready_list, &task->state_node); /* 如果存在于就绪队列中就重新把其加入队尾 */
    task->status = TASK_READY;
    klist_append(&task_manager.ready_list, &task->state_node);
}

void task_set_block(task_t *task)
{
    task->status = TASK_BLOCKED;
    task_dispatch();
}

task_t *task_schedule()
{
    int sections = SCHEDULE_SECTIONS;
    switch (sections)
    {
    case THREAD_FIFO:
        return task_FIFO();
        break;
    default:
        break;
    }
    return NULL;
}

task_t *task_FIFO()
{
    klist_node_t *next_node = klist_get_first_node(&task_manager.ready_list);
    task_t *next = KLIST_STRUCT_ADDR(next_node, task_t, state_node);

    return next;
}