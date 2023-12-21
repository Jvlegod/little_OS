#ifndef __SYSCALL_H__
#define __SYSCALL_H__
#include "comm/cpu_instr.h"
#include "debug/debug.h"
#include "tools/klib.h"
#include "task/task.h"
#define NSYSCALL 20

#define SYS_GETPID 0
#define SYS_WRITE 1
#define SYS_READ 2
#define SYS_FORK 3
#define SYS_MALLOC 4
#define SYS_PUTCHAR 5

typedef void (*syscall_t)(void);

typedef struct _syscall_frame_t
{
    /* 三个参数 */
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    /*   0 */ uint32_t gs;
    /*   4 */ uint32_t fs;
    /*   8 */ uint32_t es;
    /*  12 */ uint32_t ds;
    /*  14 */ uint32_t edi;
    /*  18 */ uint32_t esi;
    /*  20 */ uint32_t ebp;
    /*  24 */ uint32_t esp;
    /*  28 */ uint32_t ebx;
    /*  32 */ uint32_t edx;
    /*  36 */ uint32_t ecx;
    /*  40 */ uint32_t eax;
    /*  44 */ uint32_t num;       /* 占位 */
    /*  48 */ uint32_t erro_code; /* 占位 */
    /*  52 */ uint32_t eip;
    /*  56 */ uint32_t cs;
    /*  60 */ uint32_t eflags;
} syscall_frame_t;

#define _syscall0(NUMBER) ({            \
    int retval;                         \
    __asm__ __volatile__("int $0x80"    \
                         : "=a"(retval) \
                         : "a"(NUMBER)  \
                         : "memory");   \
    retval;                             \
})

#define _syscall1(NUMBER, ARG1) ({                \
    int retval;                                   \
    __asm__ __volatile__("int $0x80"              \
                         : "=a"(retval)           \
                         : "a"(NUMBER), "b"(ARG1) \
                         : "memory");             \
    retval;                                       \
})

#define _syscall2(NUMBER, ARG1, ARG2) ({                     \
    int retval;                                              \
    __asm__ __volatile__("int $0x80"                         \
                         : "=a"(retval)                      \
                         : "a"(NUMBER), "b"(ARG1), "c"(ARG2) \
                         : "memory");                        \
    retval;                                                  \
})

#define _syscall3(NUMBER, ARG1, ARG2, ARG3) ({                          \
    int retval;                                                         \
    __asm__ __volatile__("int $0x80"                                    \
                         : "=a"(retval)                                 \
                         : "a"(NUMBER), "b"(ARG1), "c"(ARG2), "d"(ARG3) \
                         : "memory");                                   \
    retval;                                                             \
})

pid_t sys_getpid(void);
void sys_write(void);
void sys_read(void);
pid_t sys_fork(syscall_frame_t *sys_frame);
void sys_malloc(void);
void sys_putchar(void);

void syscall_init();

syscall_t syscall_table[NSYSCALL];

#endif // !__SYSCALL_H__