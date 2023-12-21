#ifndef THREAD_H
#define THREAD_H
#include "tss/tss.h"
#include "tools/klib.h"
#include "gdt/gdt.h"
#include "comm/types.h"
#include "memory/memory.h"
#include "tools/klist.h"
#include "comm/cpu_instr.h"
#include "mmu/pagetable.h"
#include "syscall/syscall.h"
#include "irq/irq.h"

#define TASK_TICKS_DEFAULT 5

#define OFFSET_OF(type, member) ((uint32_t) & ((type *)0)->member)                        /* 获取结构体成员在结构体中的偏移量 */
#define PARENT_STRUCT_ADDR(node, type, member) ((uint32_t)node - OFFSET_OF(type, member)) /* 找到结构体首地址 */
#define KLIST_STRUCT_ADDR(node, struct_type, member) ((struct_type *)(node ? PARENT_STRUCT_ADDR(node, struct_type, member) : 0))

#define TASK_DEFAULF_PRIO 31 /* 默认优先级 */
#define TASK_STACK_MAGIC 0x19870916

#define KERNEL_PROC 0 /* 内核进程 */
#define USER_PROC 1   /* 用户进程 */

/* 此处调度算法相关 */
#define THREAD_FIFO 0

#define SCHEDULE_SECTIONS THREAD_FIFO

// /**
//  * @brief 任务状态
//  *
//  */
// typedef enum
// {
//     TASK_RUNNING, // 运行态
//     TASK_CREATED, // 新建
//     TASK_READY,   // 就绪态
//     TASK_BLOCKED, // 阻塞态
//     TASK_WAITING, // 等待态
//     TASK_HANGING, // 挂起
//     TASK_SLEEP,   // 睡眠
//     TASK_DIED     // 死机
// } task_status;

typedef void thread_func(void *);

/**
 * @brief 线程切换的栈
 *
 */
typedef struct _thread_stack
{
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edi;
    uint32_t esi;

    /* 一个函数指针线程的函数名和参数 */
    void (*eip)(thread_func *func, void *arg);

    void(*unused_retaddr); /* 占位 */
    thread_func *function;
    void *args;
} thread_stack;

/**
 * @brief 中断栈/上下文
 *
 */
typedef struct _intr_stack
{
    uint32_t vec_no;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp_dummy;

    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    // uint32_t err_code; // 错误码
    void (*eip)(void);
    uint32_t cs;
    uint32_t eflags;
    void *esp;
    uint32_t ss;
} intr_stack, context_t;

/**
 * @brief 进程或线程的PCB
 *
 */
typedef struct _thread_pcb_t
{
#define TASK_NAME_LEN 20
    uint32_t *kstack;         /* 运行时栈 */
    char name[TASK_NAME_LEN]; /* 线程的名称 */
    int priority;             /* 优先级 */
    task_status status;       /* 记录线程运行的状态 */
    int stack_magic;          /* 用于检测栈溢出的魔数 */
    int ticks;                /* 时钟滴答 */

    klist_node_t state_node;
    klist_node_t general_node;
} thread_pcb_t;

typedef struct _proc_pcb_t
{
    int pid;                  /* 进程的标志位 */
    uint32_t *kstack;         /* 进程的内核栈 */
    char name[TASK_NAME_LEN]; /* 进程的名称 */
    void *_filename;          /* 进程的文件名 */
    int priority;             /* 优先级 */
    task_status status;       /* 记录进程运行的状态 */
    int stack_magic;          /* 用于检测栈溢出的魔数 */
    int ticks;                /* 时钟滴答 */

    struct _proc_pcb_t *parent; /* 指向父pcb */
    uint8_t ifkernel;           /* 判断用户是否属于内核进程 */
    enum
    {
        SAVED_CONTEXT,
        UNSAVED_CONTEXT
    } ifsaved_context; /* 判断是否保存了上下文 */

    klist_node_t state_node;   /* 状态节点 */
    klist_node_t general_node; /* 通用的节点 */

    thread_pcb_t *thread_table; /* 线程表，放在进程的pcb中 */
    context_t *context;         /* 保存进程的上下文 */
    uint32_t *pgdir;            /* 指向进程的页目录表 */

    thread_pcb_t *main_thread; /* 进程的主线程 */
} proc_pcb_t;

/**
 * @brief 线程调度管理器
 *
 */
typedef struct _thread_manager_t
{
    thread_pcb_t *cur_thread; /* 当前正在运行的线程 */

    kdouble_list_t general_list; /* 任务链表 */
    kdouble_list_t ready_list;   /* 就绪链表 */
    kdouble_list_t sleep_list;   /* 睡眠链表 */

    thread_pcb_t main_thread; /* 主线程 */

} thread_manager_t;

/**
 * @brief 进程调度器
 *
 */
typedef struct _proc_manager_t
{
    proc_pcb_t *cur_proc; /* 当前正在运行的进程 */

    kdouble_list_t general_list; /* 任务链表 */
    kdouble_list_t ready_list;   /* 就绪链表 */
    kdouble_list_t sleep_list;   /* 睡眠链表 */

    irq_status irq_state; /* 用来进行进程切换时的原子操作 */

    proc_pcb_t main_proc; /* 主进程 */

} proc_manager_t;

thread_pcb_t *task_init(thread_func *main);

void tss_desc_set(int tss_sel);
void tss_init();
void update_tss_esp0(proc_pcb_t *pthread);
void switch_pgdir(uint32_t *dir);

thread_pcb_t *thread_start(char *name, int prio, thread_func func, void *args);
void main_thread_init(char *name, int prio, thread_func func, void *args);
void simple_switch(uint32_t **from, uint32_t *to);
void switch_thread_from_to(thread_pcb_t *from, thread_pcb_t *to);
void kthread_create(thread_pcb_t *pthread, thread_func func, void *args);
void kthread_init(thread_pcb_t *pthread, char *name, int prio);
void kthread(thread_func *function, void *args);
void thread_set_ready(thread_pcb_t *pcb);
void thread_push_ready(thread_pcb_t *pcb);
void thread_set_sleep(thread_pcb_t *pcb);
void thread_set_general(thread_pcb_t *pcb);
void thread_set_block(thread_pcb_t *pcb);
void thread_dispatch();
void thread_time_ticks();
thread_pcb_t *cur_thread();
thread_pcb_t *thread_FIFO();
thread_manager_t *thread_get_manager();

void intr_exit(void);

proc_pcb_t *proc_FIFO();
void proc_time_ticks();
proc_pcb_t *proc_init(void *_filename, char const *proc_name);
void proc_start(proc_pcb_t *pcb, uint8_t ifkernel);
void proc_switch(proc_pcb_t *pcb);
proc_pcb_t *proc_schedule();
uint8_t proc_ifkernel();
void proc_dispatch();
void first_proc_init();

int get_pid();
pid_t fork();

int alloc_pid();
proc_pcb_t *cur_proc();

#endif // !THREAD_H
