#include "task.h"

extern tss_t tss;
extern thread_manager_t thread_manager;
extern proc_manager_t proc_manager;
extern gdt_entry_t gdt_table[GDT_SIZE];
extern pde_t kernel_table[NPDE];

/**
 * @brief 初始化tss
 *
 */
void tss_init()
{
    tss_desc_set(KERNEL_SELECTOR_TSS);
}

/**
 * @brief 从from线程切换到to线程
 *
 * @param from
 * @param to
 */
void switch_thread_from_to(thread_pcb_t *from, thread_pcb_t *to)
{
    simple_switch(&from->kstack, to->kstack);
}

/**
 * @brief 设置tss描述符
 *
 * @param tss_sel 选择子
 * @param ifkernel 判断是否属于内核
 * @param entry 入口
 * @param stack 栈
 */
void tss_desc_set(int tss_sel)
{
    kmemset(&tss, 0, sizeof(tss_t));
    /* 在GDT表中设置TSS描述符 */
    gdt_segment_set(tss_sel, sizeof(tss_t), (uint32_t)&tss, SEG_S_SYSTEM | SEG_TYPE_TSS | SEG_DPL0 | SEG_P_PRESENT);

    /* 设置tss的结构 */
    int data_sel, code_sel;

    data_sel = KERNEL_SELECTOR_DS;
    code_sel = KERNEL_SELECTOR_CS;

    // tss.eip = (uint32_t(*)(void))entry;
    tss.esp = tss.esp0 = (uint32_t *)KERNEL_STACK_SIZE;
    tss.es = tss.ds = tss.fs = tss.gs = data_sel;
    tss.ss0 = tss.ss = data_sel;
    tss.cs = code_sel;
    tss.eflags = ELFAGS_MBS | EFLAGS_IF;

    /* 加载选择子到tr寄存器 */
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
    ltr(tss_sel);
}

/**
 * @brief 将线程加入到普通队列
 *
 * @param pcb
 */
void thread_set_general(thread_pcb_t *pcb)
{
    klist_append(&thread_manager.general_list, &pcb->general_node);
}

void proc_set_general(proc_pcb_t *pcb)
{
    klist_append(&proc_manager.general_list, &pcb->general_node);
}

/**
 * @brief 将任务加入就绪队列的队头
 *
 * @param pcb
 */
void thread_push_ready(thread_pcb_t *pcb)
{
    klist_remove(&thread_manager.ready_list, &pcb->state_node); /* 如果存在于就绪队列中就重新把其加入队尾 */
    pcb->status = TASK_READY;
    klist_push(&thread_manager.ready_list, &pcb->state_node);
}

/**
 * @brief 将线程加入到就绪队列的队尾
 *
 * @param pcb
 */
void thread_set_ready(thread_pcb_t *pcb)
{
    klist_remove(&thread_manager.ready_list, &pcb->state_node); /* 如果存在于就绪队列中就重新把其加入队尾 */
    pcb->status = TASK_READY;
    klist_append(&thread_manager.ready_list, &pcb->state_node);
}

void proc_set_ready(proc_pcb_t *pcb)
{
    klist_remove(&proc_manager.ready_list, &pcb->state_node); /* 如果存在于就绪队列中就重新把其加入队尾 */
    pcb->status = TASK_READY;
    klist_append(&proc_manager.ready_list, &pcb->state_node);
}

void thread_set_block(thread_pcb_t *pcb)
{
    pcb->status = TASK_BLOCKED;
    thread_dispatch();
}

/**
 * @brief 将线程加入到睡眠队列
 *
 * @param pcb
 */
void thread_set_sleep(thread_pcb_t *pcb)
{
    pcb->status = TASK_SLEEP;
    klist_append(&thread_manager.sleep_list, &pcb->state_node);
}

/**
 * @brief 初始化主线程
 *
 * @param name
 * @param prio
 * @param func
 * @param args
 */
void main_thread_init(char *name, int prio, thread_func func, void *args)
{
    thread_manager.cur_thread = &thread_manager.main_thread;
    /* 初始化线程 */
    kthread_init(&thread_manager.main_thread, name, prio);
    kthread_create(&thread_manager.main_thread, func, NULL);
    /* 设置线程的运行状态 */
    thread_set_general(&thread_manager.main_thread);
    thread_set_ready(&thread_manager.main_thread);
    thread_manager.main_thread.ticks = TASK_TICKS_DEFAULT;
    thread_manager.main_thread.status = TASK_RUNNING;
}

/**
 * @brief 线程调度
 *
 * @param function
 * @param args
 */
void kthread(thread_func *function, void *args)
{
    function(args);
}

/**
 * @brief 初始化线程的名字和优先级
 *
 * @param pthread
 * @param name
 * @param prio
 */
void kthread_init(thread_pcb_t *pthread, char *name, int prio)
{
    kmemset(pthread, 0, sizeof(pthread));
    kstrcpy(pthread->name, name);
    pthread->status = TASK_CREATED;
    pthread->priority = prio;
    pthread->kstack = (uint32_t *)((uint32_t)pthread + MEM_PAGE_SIZE);
    pthread->stack_magic = TASK_STACK_MAGIC; // 魔数
}

/**
 * @brief 创建线程
 *
 * @param pthread 线程pcb
 * @param func 线程的函数名
 * @param args 线程的参数
 */
void kthread_create(thread_pcb_t *pthread, thread_func func, void *args)
{
    pthread->kstack -= sizeof(intr_stack);
    pthread->kstack -= sizeof(thread_stack);

    thread_stack *kthread_stack = (thread_stack *)pthread->kstack;
    kthread_stack->eip = kthread;
    kthread_stack->function = func;
    kthread_stack->args = args;
    kthread_stack->ebp = kthread_stack->ebx = kthread_stack->esi = kthread_stack->edi = 0;
}

thread_pcb_t *thread_start(char *name, int prio, thread_func func, void *args)
{
    irq_status status = irq_enter_protection();
    thread_pcb_t *thread = (thread_pcb_t *)kalloc_pages(1);
    kthread_init(thread, name, prio);
    kthread_create(thread, func, NULL);

    thread->ticks = TASK_TICKS_DEFAULT;
    thread_set_general(thread);
    thread_set_ready(thread);
    irq_leave_protection(status);
    return thread;
}

/**
 * @brief 初始化进程和线程
 *
 */
thread_pcb_t *task_init(thread_func *main)
{
    /* 初始化管理链表 */
    // klist_init(&thread_manager.ready_list);
    // klist_init(&thread_manager.sleep_list);
    // klist_init(&thread_manager.general_list);

    // main_thread_init("main", 0, main, NULL); /* 创建主线程 */

    klist_init(&proc_manager.ready_list);
    klist_init(&proc_manager.sleep_list);
    klist_init(&proc_manager.general_list);

    proc_pcb_t *pcb = (proc_pcb_t *)kalloc_pages(1); /* 给PCB分配一页内存 */
    proc_manager.cur_proc = pcb;                     /* 将PCB设置为当前正在运行的进程 */
    pcb->_filename = main;
    kstrcpy(pcb->name, "main_proc");
    pcb->pgdir = (uint32_t *)kernel_table; /* 创建页表相关 */
    pcb->ticks = TASK_TICKS_DEFAULT;
    pcb->pid = alloc_pid(); /* 给进程分配PID */
    pcb->parent = NULL;
    pcb->ifkernel = KERNEL_PROC;
    pcb->ifsaved_context = UNSAVED_CONTEXT;
    proc_set_general(pcb);
    proc_set_ready(pcb);
    proc_start(pcb, KERNEL_PROC);
    tss_init(); /* 初始化tss */
}

thread_pcb_t *cur_thread()
{
    return thread_manager.cur_thread;
}

/**
 * @brief 此处负责线程调度的算法选择
 *
 * @return thread_pcb_t*
 */
thread_pcb_t *thread_schedule()
{
    int sections = SCHEDULE_SECTIONS;
    switch (sections)
    {
    case THREAD_FIFO:
        return thread_FIFO();
        break;
    default:
        break;
    }
    return NULL;
}

proc_pcb_t *proc_schedule()
{
    int sections = SCHEDULE_SECTIONS;
    switch (sections)
    {
    case THREAD_FIFO:
        return proc_FIFO();
        break;
    default:
        break;
    }
    return NULL;
}

/**
 * @brief 线程调度，切换线程
 *
 */
void thread_dispatch()
{
    thread_pcb_t *cur = cur_thread();
    thread_pcb_t *next = thread_schedule();
    if (next != cur && next->status == TASK_READY)
    {
        thread_manager.cur_thread = next;
        next->status = TASK_RUNNING;
        switch_thread_from_to(cur, next);
    }
    else if (klist_len(&thread_manager.ready_list) == 1)
    {
        /* 只有一个线程则无法切换 */
        return;
    }
    return;
}

void proc_dispatch()
{
    proc_pcb_t *cur = cur_proc();
    /* 找到进程 */
    proc_pcb_t *next = proc_schedule();
    if (next != cur && next->status == TASK_READY)
    {
        irq_status status;
        proc_manager.cur_proc = next;
        next->status = TASK_RUNNING;

        /* 切换进程 */
        cur->ifsaved_context = UNSAVED_CONTEXT;
        switch_pgdir(next->pgdir); /* 切换页表 */
        update_tss_esp0(next);     /* 切换内核栈 */
        irq_leave_protection(proc_manager.irq_state);
        /* 此处可能还需要修改 */
        proc_switch(next); /* 切换进程 */
    }
    else if (klist_len(&proc_manager.ready_list) == 1)
    {
        /* 只有一个进程则无法切换 */
        irq_leave_protection(proc_manager.irq_state);
        return;
    }
    return;
}

/**
 * @brief 先进先出算法
 *
 * @return thread_pcb_t*
 */
thread_pcb_t *thread_FIFO()
{
    klist_node_t *next_node = klist_get_first_node(&thread_manager.ready_list);
    thread_pcb_t *next = KLIST_STRUCT_ADDR(next_node, thread_pcb_t, state_node);

    return next;
}

proc_pcb_t *proc_FIFO()
{
    klist_node_t *next_node = klist_get_first_node(&proc_manager.ready_list);
    proc_pcb_t *next = KLIST_STRUCT_ADDR(next_node, proc_pcb_t, state_node);

    return next;
}

void thread_time_ticks()
{
    // irq_status status = irq_enter_protection();
    thread_pcb_t *cur = cur_thread();

    if (--cur->ticks <= 0)
    {
        /* 重新插入就绪队列的尾部 */
        thread_set_ready(cur);

        cur->ticks = TASK_TICKS_DEFAULT;
        thread_dispatch();
    }
    // status = irq_enter_protection();
}

thread_manager_t *thread_get_manager()
{
    return &thread_manager;
}

/**
 * @brief 创建进程
 *
 * @param _filename
 */
proc_pcb_t *proc_init(void *_filename, char const *proc_name)
{
    proc_pcb_t *pcb = (proc_pcb_t *)kalloc_pages(1); /* 给PCB分配一页内存 */
    pcb->_filename = _filename;
    kstrcpy(pcb->name, proc_name);
    pcb->pgdir = create_page_table(); /* 创建页表相关 */
    pcb->ticks = TASK_TICKS_DEFAULT;
    pcb->pid = alloc_pid();    /* 给进程分配PID */
    pcb->parent = NULL;        /* 初始化父pcb */
    pcb->ifkernel = USER_PROC; /* 确定为用户进程 */
    pcb->ifsaved_context = UNSAVED_CONTEXT;
    proc_set_general(pcb);
    proc_set_ready(pcb);
    proc_start(pcb, USER_PROC);
    // 初始化进程的线程表

    // kthread_init(pcb->main_thread, proc_name, TASK_DEFAULF_PRIO);
    // kthread_create(pcb->main_thread, (void *)proc_start, pcb);
    return pcb;
}

/**
 * @brief 初始化进程的相关上下文信息
 *
 * @param pcb
 */
void proc_start(proc_pcb_t *pcb, uint8_t ifkernel)
{
    void *function = pcb->_filename;
    /* 存储上下文的内容 */
    if (ifkernel == KERNEL_PROC)
    {
        pcb->kstack = (uint32_t *)KERNEL_STACK_SIZE;
    }
    else
    {
        pcb->kstack = (uint32_t *)((uint32_t)pcb + MEM_PAGE_SIZE - 1); /* 维护proc栈底的位置 */
    }
    pcb->context = (context_t *)((uint32_t)pcb + sizeof(proc_pcb_t)); /* 将上下文放在靠近pcb的位置 */

    pcb->priority = TASK_DEFAULF_PRIO;
    pcb->stack_magic = TASK_STACK_MAGIC;
    pcb->status = TASK_READY;
    pcb->thread_table = NULL;
    pcb->ticks = TASK_TICKS_DEFAULT;
    pcb->context->eip = function;
    pcb->context->eflags = (EFLAGS_IF | ELFAGS_MBS);

    if (ifkernel == KERNEL_PROC)
    {
        pcb->context->esp = (void *)(kalloc_pages(1) + MEM_PAGE_SIZE);
        /* 上下文初始化 */
        pcb->context->eax = pcb->context->ebx = pcb->context->edx = pcb->context->ecx = 0;
        pcb->context->edi = pcb->context->esi = pcb->context->ebp = pcb->context->esp_dummy = 0;
        pcb->context->gs = pcb->context->fs = pcb->context->es = pcb->context->ds = KERNEL_SELECTOR_DS;
        pcb->context->ss = KERNEL_SELECTOR_DS;
        pcb->context->cs = KERNEL_SELECTOR_CS;
    }
    else
    {
        pcb->context->esp = (void *)(ualloc_pages(1) + MEM_PAGE_SIZE);
        /* 上下文初始化 */
        pcb->context->eax = pcb->context->ebx = pcb->context->edx = pcb->context->ecx = 0;
        pcb->context->edi = pcb->context->esi = pcb->context->ebp = pcb->context->esp_dummy = 0;
        pcb->context->gs = 0; /* 显存用不到 */
        pcb->context->fs = pcb->context->es = pcb->context->ds = USER_SELECTOR_DS;
        pcb->context->ss = USER_SELECTOR_DS;
        pcb->context->cs = USER_SELECTOR_CS;
    }

    /* 进入新的进程 */
    // proc_switch(pcb);
}

void proc_switch(proc_pcb_t *pcb)
{
    __asm__ __volatile__(
        "push %[ss]\n\t"
        "push %[esp]\n\t"
        "push %[eflags]\n\t"
        "push %[cs]\n\t"
        "push %[eip]\n\t"
        "push %[ebp]\n\t"
        "push %[gs]\n\t"
        "push %[fs]\n\t"
        "push %[es]\n\t"
        "push %[ds]\n\t"
        "pop %%ds\n\t"
        "pop %%es\n\t"
        "pop %%fs\n\t"
        "pop %%gs\n\t"
        "pop %%ebp\n\t"
        "iret"
        :
        : "a"(pcb->context->eax), "b"(pcb->context->ebx), "d"(pcb->context->edx), "c"(pcb->context->ecx),
          "D"(pcb->context->edi), "S"(pcb->context->esi), [ebp] "g"(pcb->context->ebp),
          [gs] "g"(pcb->context->gs), [fs] "g"(pcb->context->fs), [es] "g"(pcb->context->es), [ds] "g"(pcb->context->ds),
          [ss] "g"(pcb->context->ss), [esp] "g"(pcb->context->esp), [eflags] "g"(pcb->context->eflags), [cs] "g"(pcb->context->cs), [eip] "g"(pcb->context->eip)
        : "memory");
}

void update_tss_esp0(proc_pcb_t *proc)
{
    /* 切换内核栈 */
    tss.esp0 = (uint32_t *)(proc->kstack);
}

void switch_pgdir(uint32_t *dir)
{
    wcr3((uint32_t)dir);
}

/**
 * @brief 实践轮片进程调度
 *
 */
void proc_time_ticks()
{
    /* 获取进程的信息 */
    proc_pcb_t *cur = cur_proc();

    if (--cur->ticks <= 0)
    {
        /* 重新插入就绪队列的尾部 */
        proc_set_ready(cur);

        cur->ticks = TASK_TICKS_DEFAULT;

        proc_dispatch();
    }
}

/**
 * @brief 获取进程的PID
 *
 * @return int
 */
int alloc_pid()
{
    static int global_pid = 0;
    /* 此处需要上锁 */
    ++global_pid;
    return global_pid;
}

/**
 * @brief 获取现在的进程
 *
 * @return proc_pcb_t*
 */
proc_pcb_t *cur_proc()
{
    return proc_manager.cur_proc;
}

/**
 * @brief 判断当前运行的进程是否属于内核进程
 *
 * @return uint8_t
 */
uint8_t proc_ifkernel()
{
    return proc_manager.cur_proc->ifkernel == KERNEL_PROC;
}

/**
 * @brief fork函数
 *
 * @return int 目标进程的pid
 */
pid_t fork()
{
    return _syscall0(SYS_FORK);
}

/**
 * @brief 取得当前进程的PID
 *
 */
pid_t get_pid()
{
    return _syscall0(SYS_GETPID);
}

/**
 * @brief 第一个用户进程的切换和初始化
 *
 */
void first_proc_init()
{
    /* 声明函数入口 */
    void first_task_entry();

    // proc_init(first_task_entry, "first_task_entry");
    init_task((uint32_t)first_task_entry);
}