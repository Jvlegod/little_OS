#ifndef IRQ_H
#define IRQ_H
#include "comm/types.h"
#include "comm/os_cfg.h"
#include "comm/trapframe.h"
#include "comm/cpu_instr.h"
#include "tools/log.h"
#include "Timer/Timer.h"
#include "tools/log.h"
#include "syscall/syscall.h"

typedef void (*irq_handler_t)(void);

#define ENTRY(irq) exception_handler_##irq
#define GET_EFLAGS(EFLAG_VAR) __asm__ __volatile__("pushfl\n\t"  \
                                                   "popl %0\n\t" \
                                                   : "=g"(EFLAG_VAR))
// 中断向量号的定义
#define IRQ0_DE 0
#define IRQ1_DB 1
#define IRQ2_NMI 2
#define IRQ3_BP 3
#define IRQ4_OF 4
#define IRQ5_BR 5
#define IRQ6_UD 6
#define IRQ7_NM 7
#define IRQ8_DF 8
#define IRQ9_MF 9
#define IRQ10_TS 10
#define IRQ11_NP 11
#define IRQ12_SS 12
#define IRQ13_GP 13
#define IRQ14_PF 14
#define IRQ15_RESERVED 15
#define IRQ16_MF 16
#define IRQ17_AC 17
#define IRQ18_MC 18
#define IRQ19_XM 19
#define IRQ20_VE 20
#define IRQ_Timer 0x20

#define IRQ128_INT 0x80 // 系统调用中断号

#define PIC_M_CTRL 0x20 // 主片的控制端口
#define PIC_M_DATA 0x21 // 主片的数据端口
#define PIC_S_CTRL 0xA0 // 从片的控制端口
#define PIC_S_DATA 0xA1 // 从片的数据端口
#define PIC_START 0x20  // 中断向量号起始地址

void intr_exit(void);

void pic_enable(uint8_t pic_num);
void timer_init();
void irq_init();
void pic_init();
bool irq_segment_set(irq_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr);
bool irq_install(uint16_t irq_num, irq_handler_t handler);

void exception_handler_unknown(void);
void exception_handler_divider(void);
void exception_handler_debug(void);
void exception_handler_NMI(void);
void exception_handler_breakpoint(void);
void exception_handler_overflow(void);
void exception_handler_bound_range(void);
void exception_handler_invalid_opcode(void);
void exception_handler_device_unfound(void);
void exception_handler_double_fault(void);
void exception_handler_invalid_tss(void);
void exception_handler_segment_not_present(void);
void exception_handler_stack_segment_fault(void);
void exception_handler_general_protection(void);
void exception_handler_page_fault(void);
void exception_handler_float_point_error(void);
void exception_handler_aligment_check(void);
void exception_handler_machine_check(void);
void exception_handler_SIMD(void);
void exception_handler_virtual_exception(void);
void exception_handler_timer(void);

void irq_handler_unknown(trap_frame_t *trapframe);
void irq_handler_divider(trap_frame_t *trapframe);
void irq_handler_debug(trap_frame_t *trapframe);
void irq_handler_NMI(trap_frame_t *trapframe);
void irq_handler_breakpoint(trap_frame_t *trapframe);
void irq_handler_overflow(trap_frame_t *trapframe);
void irq_handler_bound_range(trap_frame_t *trapframe);
void irq_handler_invalid_opcode(trap_frame_t *trapframe);
void irq_handler_device_unfound(trap_frame_t *trapframe);
void irq_handler_double_fault(trap_frame_t *trapframe);
void irq_handler_invalid_tss(trap_frame_t *trapframe);
void irq_handler_segment_not_present(trap_frame_t *trapframe);
void irq_handler_stack_segment_fault(trap_frame_t *trapframe);
void irq_handler_general_protection(trap_frame_t *trapframe);
void irq_handler_page_fault(trap_frame_t *trapframe);
void irq_handler_float_point_error(trap_frame_t *trapframe);
void irq_handler_aligment_check(trap_frame_t *trapframe);
void irq_handler_machine_check(trap_frame_t *trapframe);
void irq_handler_SIMD(trap_frame_t *trapframe);
void irq_handler_virtual_exception(trap_frame_t *trapframe);
void irq_handler_timer(trap_frame_t *trapframe);

irq_status irq_enable();
irq_status irq_disable();
irq_status get_irq_status();
irq_status irq_enter_protection();
irq_status irq_leave_protection(irq_status status);
irq_status irq_set_status(irq_status status);

void syscall_handler(void);

#endif // !IRQ_H