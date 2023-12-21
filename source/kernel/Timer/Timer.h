#ifndef TIMER_H
#define TIMER_H
#include "comm/cpu_instr.h"
#include "irq/irq.h"
#include "tools/klib.h"
#include "task/task.h"
#include "tss/tss.h"

extern volatile long ticks;

void timer_init();
void pic_send_eoi(int pic_num);
void irq_handler_timer(trap_frame_t *trapframe);

long SYSTEM_READ_TIMER(void);

#endif // !TIMER_H