__asm__(".code16gcc");
#include "gdt16.h"

extern gdt_entry_t gdt_table[GDT_SIZE];

/**
 * @brief 设置gdt表的desc
 *
 * @param selector 选择子
 * @param limit 段界限  20位
 * @param base 段基址  32位
 * @param attr 段属性 16位
 */
void gdt16_segment_set(uint16_t selector, uint32_t limit, uint32_t base, uint16_t attr)
{
    gdt_entry_t *desc = gdt_table + (selector >> 3);

    if (limit > 0xFFFFF)
    {
        attr |= 0x8000;
        limit /= 0x1000;
    }

    desc->limit_low = limit & 0xFFFF;
    desc->base_low = base & 0xFFFF;
    desc->base_middle = (base >> 16) & 0xFF;
    desc->attributes = attr | (((limit >> 16) & 0xF) << 8);
    desc->base_high = (base >> 24) & 0xFF;
}

/**
 * @brief 初始化GDT
 *
 */
void gdt16_init()
{
    for (int i = 0; i < GDT_SIZE; i++)
    {
        gdt16_segment_set(i << 3, 0, 0, 0);
    }

    gdt16_segment_set(KERNEL_SELECTOR_DS, 0xFFFFFFFF, 0x00000000, SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);
    gdt16_segment_set(KERNEL_SELECTOR_CS, 0xFFFFFFFF, 0x00000000, SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);

    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}