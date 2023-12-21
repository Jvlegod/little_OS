#ifndef CPU_H
#define CPU_H

#define EFLAGS_DEFAULT (1 << 1)
#define EFLAGS_IF (1 << 9)

#pragma pack(1)
/**
 * @brief TSS描述符
 *
 */
typedef struct _tss_t
{
    uint32_t pre_link;
    uint32_t esp0, ss0, esp1, ss1, esp2, ss2;
    uint32_t cr3;
    uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs;
    uint32_t ldt;
    uint32_t iomap;
} tss_t;
#pragma pack()

#endif // !CPU_H