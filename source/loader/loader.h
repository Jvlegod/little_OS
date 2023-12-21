#ifndef LOADER_H
#define LOADER_H
#include "comm/mem_info.h"
#include "comm/cpu_instr.h"
#include "gdt/gdt.h"
#include "comm/os_cfg.h"

#define KERNEL_ENTRY (1024 * 1024)

void protected_mode_entry();
void detect_memory();
bool detect_method1();
bool detect_method2();
bool detect_method3();
void loader32c_entry(void);

mem_info_t mem;
gdt_entry_t gdt_table[GDT_SIZE];

#endif // !LOADER_H