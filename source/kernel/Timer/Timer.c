#include "Timer.h"

#define IO_TIMER1 0x040 // 8253 Timer #1

#define IRQ_FREQUENCY 100.0 // 100Hz（1s发出100次中断信号）
#define TIMER_FREQ 1193182
#define COUNTER_VALUE(x) TIMER_FREQ / x

#define TIMER_MODE (IO_TIMER1 + 3) // timer mode port
#define TIMER_SEL0 0x00            // select counter 0
#define TIMER_MODE2 0x04           // mode 2, rate generator
#define TIMER_MODE3 0x06           // mode 3
#define TIMER_16BIT 0x30           // r/w counter 16 bits, LSB first

volatile long ticks;
extern irq_desc_t idt_table[IDT_SIZE];
extern proc_manager_t proc_manager;

long SYSTEM_READ_TIMER(void)
{
    return ticks;
}

void timer_init()
{
    // set 8253 timer-chip
    outb(TIMER_MODE, TIMER_SEL0 | TIMER_MODE3 | TIMER_16BIT);
    outb(IO_TIMER1, COUNTER_VALUE(IRQ_FREQUENCY) & 0xFF);
    outb(IO_TIMER1, (COUNTER_VALUE(IRQ_FREQUENCY) >> 8) & 0xFF);

    // initialize time counter 'ticks' to zero
    ticks = 0;
    irq_install(IRQ_Timer, (irq_handler_t)ENTRY(timer));
    pic_enable(IRQ_Timer);
    /* 打开中断 */
    irq_enable();
}

void pic_send_eoi(int pic_num)
{
    pic_num -= PIC_START;
    if (pic_num >= 8)
    {
        outb(PIC_S_CTRL, 1 << 5);
    }

    outb(PIC_M_CTRL, 1 << 5);
}

/* 定时器中断 */
void irq_handler_timer(trap_frame_t *trapframe)
{
    ticks++;
    /**********************老的进程切换*************************************
    /* 线程调度 */
    // thread_time_ticks();

    /* 进程调度 */
    // /* 保存进程的上下文 */
    // if (proc_manager.cur_proc->ifsaved_context == UNSAVED_CONTEXT)
    // {
    //     proc_manager.cur_proc->ifsaved_context = SAVED_CONTEXT;
    //     proc_manager.cur_proc->context->gs = trapframe->gs;
    //     proc_manager.cur_proc->context->fs = trapframe->fs;
    //     proc_manager.cur_proc->context->es = trapframe->es;
    //     proc_manager.cur_proc->context->ds = trapframe->ds;
    //     proc_manager.cur_proc->context->edi = trapframe->edi;
    //     proc_manager.cur_proc->context->esi = trapframe->esi;
    //     proc_manager.cur_proc->context->ebp = trapframe->ebp;
    //     proc_manager.cur_proc->context->esp = (void *)(trapframe->esp + sizeof(trapframe));
    //     proc_manager.cur_proc->context->ebx = trapframe->ebx;
    //     proc_manager.cur_proc->context->edx = trapframe->edx;
    //     proc_manager.cur_proc->context->ecx = trapframe->ecx;
    //     proc_manager.cur_proc->context->eax = trapframe->eax;
    //     proc_manager.cur_proc->context->eip = (void *)trapframe->eip;
    //     proc_manager.cur_proc->context->cs = trapframe->cs;
    //     proc_manager.cur_proc->context->eflags = trapframe->eflags;
    // }

    // /* 进程切换需要是一个原子操作 */
    // proc_manager.irq_state = irq_enter_protection();
    // proc_time_ticks(); /* 进程调度 */
    // irq_leave_protection(proc_manager.irq_state);

    /* 手动结束中断 */
    // pic_send_eoi(IRQ_Timer);
    // task_time_ticks();
    /**********************老的进程切换*************************************
}