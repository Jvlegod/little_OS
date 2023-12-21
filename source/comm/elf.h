#ifndef ELF_H
#define ELF_H
#include "types.h"

#define EI_NIDENT 16
typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_SWord;
typedef uint32_t Elf32_Word;

// e_machine
#define EM_NONE 0
#define EM_M32 1
#define EM_SPARC 2
#define EM_386 3
#define EM_68K 4
#define EM_88K 5
#define EM_860 7
#define EM_MIPS 8

// e_type
#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

// p_type
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff

// p_flags
#define PF_X 1
#define PF_W 2
#define PF_R 3
#define PF_MASKOS 0x0ff00000
#define PF_MASKPROC 0xf0000000

// p_align
#define PA_UNALIGN 0
#define PA_ALIGNED(x) x /*x应该是2的n次幂*/

typedef struct
{
    unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */

    Elf32_Half e_type; /* Object file type */

    Elf32_Half e_machine; /* Architecture */

    Elf32_Word e_version; /* Object file version */

    Elf32_Addr e_entry; /* Entry point virtual address */

    Elf32_Off e_phoff; /* Program header table file offset */

    Elf32_Off e_shoff; /* Section header table file offset */

    Elf32_Word e_flags; /* Processor-specific flags */

    Elf32_Half e_ehsize; /* ELF header size in bytes */

    Elf32_Half e_phentsize; /* Program header table entry size */

    Elf32_Half e_phnum; /* Program header table entry count */

    Elf32_Half e_shentsize; /* Section header table entry size */

    Elf32_Half e_shnum; /* Section header table entry count */

    Elf32_Half e_shstrndx; /* Section header string table index */

} Elf32_Ehdr;

typedef struct
{
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

#endif // !ELF_H