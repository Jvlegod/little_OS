#include "syscall.h"
extern proc_manager_t proc_manager;
extern task_manage_t task_manager;

pid_t sys_getpid()
{
    proc_pcb_t *cur = cur_proc();
    return cur->pid;
}

void sys_write()
{
    hlt();
}

void sys_read()
{
    hlt();
}

pid_t sys_fork(syscall_frame_t *sys_frame)
{
    // 思路一
    // proc_pcb_t *cur = cur_proc();
    // proc_pcb_t *new_proc = proc_init(cur->_filename, cur->name);

    // proc_manager.irq_state = irq_enter_protection();
    // new_proc->ifkernel = cur->ifkernel;
    // new_proc->parent = cur;

    // // 保存上下文
    // new_proc->ifsaved_context = UNSAVED_CONTEXT;
    // new_proc->context->gs = sys_frame->gs;
    // new_proc->context->fs = sys_frame->fs;
    // new_proc->context->es = sys_frame->es;
    // new_proc->context->ds = sys_frame->ds;
    // new_proc->context->edi = sys_frame->edi;
    // new_proc->context->esi = sys_frame->esi;
    // new_proc->context->ebp = sys_frame->ebp;
    // new_proc->context->esp = (void *)(sys_frame->esp + sizeof(sys_frame));
    // new_proc->context->ebx = sys_frame->ebx;
    // new_proc->context->edx = sys_frame->edx;
    // new_proc->context->ecx = sys_frame->ecx;
    // new_proc->context->eax = 0;
    // new_proc->context->eip = (void *)sys_frame->eip;
    // new_proc->context->cs = sys_frame->cs;
    // new_proc->context->eflags = sys_frame->eflags;
    // irq_leave_protection(proc_manager.irq_state);
    // return new_proc->pid;

    // 思路二
    task_t *cur = cur_task();
    task_t *new_task = init_task(sys_frame->eip);
    proc_manager.irq_state = irq_enter_protection();

    // pagetable_destroy(new_task->tss.cr3);
    new_task->tss.gs = sys_frame->gs;
    new_task->tss.fs = sys_frame->fs;
    new_task->tss.es = sys_frame->es;
    new_task->tss.ds = sys_frame->ds;
    new_task->tss.edi = sys_frame->edi;
    new_task->tss.esi = sys_frame->esi;
    new_task->tss.ebp = sys_frame->ebp;
    new_task->tss.esp = (uint32_t *)sys_frame->esp;
    new_task->tss.ebx = sys_frame->ebx;
    new_task->tss.edx = sys_frame->edx;
    new_task->tss.ecx = sys_frame->ecx;
    new_task->tss.eip = (void *)sys_frame->eip;
    new_task->tss.cs = sys_frame->cs;
    new_task->tss.eflags = sys_frame->eflags;
    // new_task->tss.cr3 = copy_pagetable(new_task->tss.cr3);

    new_task->parent = cur;
    new_task->ifkernel = cur->ifkernel;
    new_task->tss.eax = 0;

    irq_leave_protection(proc_manager.irq_state);
    return new_task->pid;
}

void sys_malloc()
{
    hlt();
}

void sys_putchar()
{
    hlt();
}

void syscall_init()
{
    syscall_table[SYS_GETPID] = (syscall_t)sys_getpid;
    syscall_table[SYS_WRITE] = (syscall_t)sys_write;
    syscall_table[SYS_READ] = (syscall_t)sys_read;
    syscall_table[SYS_FORK] = (syscall_t)sys_fork;
    syscall_table[SYS_MALLOC] = (syscall_t)sys_malloc;
    syscall_table[SYS_PUTCHAR] = (syscall_t)sys_putchar;
}