#ifndef SELECTOR_DEFINE_H
#define SELECTOR_DEFINE_H

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
#endif // !SELECTOR_DEFINE_H