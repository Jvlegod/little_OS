#ifndef TRAPFRAME_H
#define TRAPFRAME_H
#include "types.h"

typedef struct _trap_frame_t
{
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
    /*  44 */ uint32_t num;
    /*  48 */ uint32_t erro_code;
    /*  52 */ uint32_t eip;
    /*  56 */ uint32_t cs;
    /*  60 */ uint32_t eflags;
} trap_frame_t;
#endif // !TRAPFRAME_H