#ifndef CPU_INSTR_H
#define CPU_INSTR_H
#include "types.h"

/**
 * @brief 停机
 *
 */
static inline void hlt()
{
    __asm__ __volatile__("hlt");
}

/**
 * @brief 关闭中断
 *
 */
static inline void cli()
{
    __asm__ __volatile__("cli");
}

/**
 * @brief 打开中断
 *
 */
static inline void sti()
{
    __asm__ __volatile__("sti");
}

/**
 * @brief 读取一个word的数据
 *
 * @param port
 * @return uint16_t
 */
static inline uint16_t inw(uint16_t port)
{
    uint16_t rv;
    __asm__ __volatile__("in %[port], %[rv]"
                         : [rv] "=a"(rv)
                         : [port] "d"(port));
    return rv;
}

/**
 * @brief 写入一个word的数据
 *
 * @param port
 * @param data
 */
static inline void outw(uint16_t port, uint16_t data)
{
    __asm__ __volatile__("outw %[data], %[port]"
                         :
                         : [data] "a"(data), [port] "d"(port));
}

/**
 * @brief 将数据从端口读出
 *
 * @param port
 * @return uint8_t
 */
static inline uint8_t inb(uint16_t port)
{
    uint8_t rv;
    __asm__ __volatile__("inb %[port], %[rv]"
                         : [rv] "=a"(rv)
                         : [port] "d"(port));
    return rv;
}

/**
 * @brief 将数从端口读出
 *
 * @param port
 * @param data
 */
static inline void outb(uint16_t port, uint8_t data)
{
    __asm__ __volatile__("outb %[data], %[port]"
                         :
                         : [data] "a"(data), [port] "d"(port));
}

/**
 * @brief 将数据从cr0寄存器中读出
 *
 * @return uint32_t
 */
static inline uint32_t rcr0()
{
    uint32_t rv;
    __asm__ __volatile__("mov %%cr0, %[rv]"
                         : [rv] "=r"(rv));
    return rv;
}

/**
 * @brief 将数据写入cr0寄存器中
 *
 * @param data
 */
static inline void wcr0(uint32_t data)
{
    __asm__ __volatile__("mov %[data], %%cr0" ::[data] "r"(data));
}

/**
 * @brief 将数据写入cr3寄存器中
 *
 * @param data
 */
static inline void wcr3(uint32_t data)
{
    __asm__ __volatile__("mov %[data], %%cr3" ::[data] "r"(data));
}

/**
 * @brief 将数据从cr3寄存器中读出
 *
 * @return uint32_t
 */
static inline uint32_t rcr3()
{
    uint32_t rv;
    __asm__ __volatile__("mov %%cr3, %[rv]"
                         : [rv] "=r"(rv));
    return rv;
}

/**
 * @brief 将数据写入cr4寄存器中
 *
 * @param data
 */
static inline void wcr4(uint32_t data)
{
    __asm__ __volatile__("mov %[data], %%cr4" ::[data] "r"(data));
}

/**
 * @brief 将数据从cr4寄存器中读出
 *
 * @return uint32_t
 */
static inline uint32_t rcr4()
{
    uint32_t rv;
    __asm__ __volatile__("mov %%cr4, %[rv]"
                         : [rv] "=r"(rv));
    return rv;
}

/**
 * @brief 加载gdt表
 *
 * @param start
 * @param offset
 */
static inline void lgdt(uint32_t start, uint16_t offset)
{
    struct
    {
        // 决定了gdt表在内存中的大小
        uint16_t offset;
        uint16_t start15_0;
        uint16_t start32_16;
    } gdt;

    gdt.start32_16 = start >> 16;
    gdt.start15_0 = start & 0xFFFF;
    gdt.offset = offset - 1;

    __asm__ __volatile__("lgdt %[gdt]" ::[gdt] "m"(gdt));
}

/**
 * @brief 加载idt
 *
 * @param start
 * @param size
 */
static inline void lidt(uint32_t start, uint32_t size)
{
    struct
    {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    } idt;

    idt.start31_16 = start >> 16;
    idt.start15_0 = start & 0xFFFF;
    idt.limit = size - 1;

    __asm__ __volatile__("lidt %0" ::"m"(idt));
}

static void inline ltr(uint32_t tss_sel)
{

    __asm__ __volatile__("ltr %%ax" ::"a"(tss_sel));
}

static inline void far_jmp(uint32_t selc, uint32_t offset)
{
    uint32_t addr[] = {offset, selc};
    __asm__ __volatile__("ljmpl *(%[addr])" ::[addr] "r"(addr));
}

static inline void write_eflags(uint32_t eflags)
{
    __asm__ __volatile__("push %%eax\n\t"
                         "popf" ::"a"(eflags));
}

#endif // !CPU_INSTR_H