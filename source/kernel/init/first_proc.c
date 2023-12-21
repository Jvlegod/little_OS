#include "comm/mem_info.h"
#include "comm/os_cfg.h"
#include "irq/irq.h"
#include "comm/cpu_instr.h"
#include "gdt/gdt.h"
#include "mmu/pagetable.h"
#include "memory/memory.h"
#include "task/task.h"
#include "debug/debug.h"
#include "sync/sync.h"
#include "syscall/syscall.h"

void first_proc()
{
    int count = 3;
    pid_t pid = fork();

    if (pid < 0)
    {
        // klog_printf("create child process failed\n");
    }
    else if (pid == 0)
    {
        // klog_printf("child: %d\n", count);
    }
    else
    {
        // klog_printf("child task id = %d\n", pid);
        // klog_printf("parent: %d\n", count);
    }

    for (;;)
    {
        // klog_printf("I am user proc");
    }
}