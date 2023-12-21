#include "comm/os_cfg.h"
#include "comm/types.h"
#include "comm/cpu_instr.h"
#ifndef GDT16_H
#define GDT16_H

void gdt16_segment_set(uint16_t selector, uint32_t limit, uint32_t base, uint16_t attr);
void gdt16_init();

#endif // !GDT16_H