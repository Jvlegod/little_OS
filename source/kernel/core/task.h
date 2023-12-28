#ifndef TASK_H
#define TASK_H

#include "comm/types.h"
#include "cpu/cpu.h"
#include "debug/debug.h"
#include "comm/os_cfg.h"
#include "gdt/gdt.h"
#include "tools/klist.h"
#include "tools/klib.h"

#define TASK_NAME 32
#define TASK_TIME_SLICE_DEFAULT 10

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

    int slice_ticks; // 切片时钟
    int time_ticks;  // 定时器滴答

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
int sys_sched_yield();
void task_dispatch();
task_t *task_next_run();
task_t *task_current();
void task_time_tick();

#endif // !TASK_H