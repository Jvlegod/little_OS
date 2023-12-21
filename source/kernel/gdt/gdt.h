#include "comm/os_cfg.h"
#include "comm/types.h"
#include "comm/cpu_instr.h"
#ifndef GDT_H
#define GDT_H

int gdt_alloc_seg();
void gdt_segment_set(uint16_t selector, uint32_t limit, uint32_t base, uint16_t attr);
void gdt_init();

#endif // !GDT_H