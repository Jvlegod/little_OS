#ifndef TASK_H
#define TASK_H

#include "comm/types.h"
#include "cpu/cpu.h"
#include "debug/debug.h"
#include "comm/os_cfg.h"
#include "gdt/gdt.h"
#include "tools/klist.h"

#define TASK_NAME 32

typedef enum _task_state_t
{
    TASK_CREATED,
    TASK_RUNNING,
    TASK_SLEEP,
    TASK_READY,
    TASK_WAITTING
} state_t;

typedef struct _task_t
{
    state_t state;
    char name[TASK_NAME];

    klist_node_t run_node;
    klist_node_t all_node;

    tss_t tss;
    int tss_sel;
} task_t;

typedef struct _task_manager_t
{
    task_t *curr_task; // 当前任务

    kdouble_list_t ready_list; // 就绪队列
    kdouble_list_t task_list;  // 任务队列

    task_t first_task;
} task_manager_t;

int task_init(task_t *task, char *name, uint32_t entry, uint32_t esp);
void task_switch_from_to(task_t *from, task_t *to);
void switch_to_tss(int tss_sel);
void task_manager_init();
void task_first_init();
task_t *get_first_task();
void task_set_ready(task_t *task);
void task_set_block();

#endif // !TASK_H