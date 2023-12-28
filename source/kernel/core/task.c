#include "task.h"

extern task_manager_t task_manager;

static int tss_init(task_t *task, int flag, uint32_t entry, uint32_t esp)
{
    kmemset(&task->tss, 0, sizeof(tss_t));

    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0)
    {
        klog_printf("alloc tss failed");
        return -1;
    }

    gdt_segment_set(tss_sel, sizeof(tss_t), (uint32_t)&task->tss, SEG_S_SYSTEM | SEG_TYPE_TSS | SEG_DPL0 | SEG_P_PRESENT);

    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss = task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_CS;
    task->tss.cs = KERNEL_SELECTOR_CS;
    task->tss.eflags = EFLAGS_DEFAULT | EFLAGS_IF;

    task->tss_sel = tss_sel;
    return 0;
}

int task_init(task_t *task, char *name, uint32_t entry, uint32_t esp)
{
    ASSERT(task != NULL);

    tss_init(task, entry, esp);

    task->state = TASK_CREATED;
    kmemcpy(task->name, name, sizeof(name));
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    klist_node_init(&task->run_node);
    klist_node_init(&task->all_node);

    task_set_ready(task);
    klist_append(&task_manager.task_list, &task->all_node);

    return 0;
}

void task_switch_from_to(task_t *from, task_t *to)
{
    switch_to_tss(to->tss_sel);
}

void switch_to_tss(int tss_sel)
{
    far_jmp(tss_sel, 0);
}

void task_manager_init()
{
    klist_init(&task_manager.ready_list);
    klist_init(&task_manager.task_list);
    task_manager.curr_task = NULL;
}

void task_first_init()
{
    task_init(&task_manager.first_task, "first_task", 0, 0);
    ltr(task_manager.first_task.tss_sel);
    task_manager.curr_task = &task_manager.first_task;
}

task_t *get_first_task()
{
    return task_manager.first_task;
}

void task_set_ready(task_t *task)
{
    klist_append(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_block()
{
    klist_remove(&task_manager.ready_list, &task->run_node);
}

task_t *task_next_run()
{
    klist_node_t *task_node = klist_get_first_node(&task_manager.ready_list);
    return KLIST_STRUCT_ADDR(task_node, task_t, run_node);
}

task_t *task_current()
{
    return task_manager.curr_task;
}

int sys_sched_yield()
{
    if (klist_count(&task_manager.ready_list) > 1)
    {
        task_t *curr_task = task_current();

        task_set_block(curr_task);
        task_set_ready(curr_task);

        task_dispatch();
    }
    return 0;
}

void task_dispatch()
{
    task_t *to = task_next_run();
    if (to != task_manager.curr_task)
    {
        task_t *from = task_current();
        task_manager.curr_task = to;
        to->state = TASK_RUNNING;

        task_switch_from_to(from, to);
    }
}

void task_time_tick()
{
    task_t *curr_task = task_current();

    if (--curr_task->slice_ticks == 0)
    {
        curr_task->slice_ticks = curr_task->time_ticks;

        task_set_block(curr_task);
        task_set_ready(curr_task);

        task_dispatch();
    }
}