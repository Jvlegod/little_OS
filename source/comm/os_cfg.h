#ifndef OS_CFG_H
#define OS_CFG_H
#include "comm/types.h"
// 选择子
#define RPL0 0
#define RPL1 1
#define RPL2 2
#define RPL3 3

#define TI_GDT 0
#define TI_LDT 1

#define KERNEL_SELECTOR_CS ((1 << 3) + (TI_GDT << 2) + RPL0)
#define KERNEL_SELECTOR_DS ((2 << 3) + (TI_GDT << 2) + RPL0)
#define KERNEL_SELECTOR_TSS ((3 << 3) + (TI_GDT << 2) + RPL0)
#define USER_SELECTOR_CS ((4 << 3) + (TI_GDT << 2) + RPL3)
#define USER_SELECTOR_DS ((5 << 3) + (TI_GDT << 2) + RPL3)
// 内核栈
#define KERNEL_STACK_SIZE (8 * 1024)

// CR0寄存器相关宏
#define CR0_PG (1 << 31)

// CR4寄存器相关宏
#define CR4_PSE (1 << 4)

// dev的相关宏
#define SECTOR_SIZE 512

// gate的相关宏
#define IDT_SIZE 256
#define GATE_TYPE_IDT (0xE << 8)
#define GATE_P (1 << 15)
#define GATE_DPL_0 (0 << 13)
#define GATE_DPL_3 (3 << 13)

// // gdt和ldt的相关宏
#define GDT_SIZE 256

#define SEG_G (1 << 15)        // 设置段界限的单位，1-4KB，0-字节
#define SEG_D (1 << 14)        // 控制是否是32位、16位的代码或数据段
#define SEG_P_PRESENT (1 << 7) // 段是否存在

#define SEG_DPL0 (0 << 5) // 特权级0，最高特权级
#define SEG_DPL3 (3 << 5) // 特权级3，最低权限

#define SEG_S_SYSTEM (0 << 4) // 是否是系统段，如调用门或者中断
#define SEG_S_NORMAL (1 << 4) // 普通的代码段或数据段
#define SEG_TYPE_TSS (9 << 0) // TSS描述符的Type

#define SEG_TYPE_CODE (1 << 3) // 指定其为代码段
#define SEG_TYPE_DATA (0 << 3) // 数据段

#define SEG_TYPE_C (1 << 2) // 一致代码段(代码段用)
#define SEG_TYPE_E (1 << 2) // 扩展方向,向下扩展(数据段)

#define SEG_TYPE_RW (1 << 1) // 是否可写可读，不设置为只读

// eflags的配置
#define ELFAGS_MBS (1 << 1)
#define EFLAGS_IF (1 << 9)

// // 当S=1时 代码段
// #define SEG_X (0x8)   // 1000b
// #define SEG_XR (0xA)  // 1010b
// #define SEG_XC (0xC)  // 1100b
// #define SEG_SCR (0xE) // 1110b
// // 当S=1时 数据段
// #define SEG_RO (0x0) // 0000b
// #define SEG_W (0x2)  // 0010b
// #define SEG_E (0x4)  // 0100b
// #define SEG_EW (0x6) // 0110b

// gdt/ldt/tss描述符
typedef struct _entry_t
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint16_t attributes; // 包含了limit_high
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t, tss_entry_t;

// 中断门描述符
typedef struct _desc_t
{
    uint16_t limit15_0;
    uint16_t selector;
    uint16_t attribute;
    uint16_t limit31_16;
} __attribute__((packed)) irq_desc_t, task_desc_t;

#endif //! OS_CFG_H