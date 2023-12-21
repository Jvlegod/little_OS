// #include "loader32.h"
#include "loader.h"
#include "comm/elf.h"

void load2mem(uint32_t sector, uint16_t sector_count, uint8_t *buf)
{
    outb(0x1F6, (uint8_t)(0xE0));

    outb(0x1F2, (uint8_t)(sector_count >> 8));
    outb(0x1F3, (uint8_t)(sector >> 24));
    outb(0x1F4, (uint8_t)(0));
    outb(0x1F5, (uint8_t)(0));

    outb(0x1F2, (uint8_t)(sector_count));
    outb(0x1F3, (uint8_t)(sector));
    outb(0x1F4, (uint8_t)(sector >> 8));
    outb(0x1F5, (uint8_t)(sector >> 16));

    outb(0x1F7, (uint8_t)0x24);

    // 读取数据
    uint16_t *data_buf = (uint16_t *)buf;
    while (sector_count-- > 0)
    {
        // 判断是否忙并且数据是否准备就绪
        while ((inb(0x1F7) & 0x88) != 0x8)
            ;

        // 每次读取一个word
        for (int i = 0; i < SECTOR_SIZE / 2; i++)
        {
            *data_buf++ = inw(0x1F0);
        }
    }
}

uint32_t read_elf(uint8_t *file_buffer)
{
    Elf32_Ehdr *elf_hdr = (Elf32_Ehdr *)file_buffer;
    // 判定魔数是否正确
    if ((elf_hdr->e_ident[0] != 0x7F) || (elf_hdr->e_ident[1] != 'E') ||
        (elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F'))
    {
        return 0;
    }

    // 循环解析每一个program header的信息
    for (int i = 0; i < elf_hdr->e_phnum; i++)
    {
        // 拿到program header条目
        Elf32_Phdr *phdr = (Elf32_Phdr *)(file_buffer + elf_hdr->e_phoff) + i;
        // 检查程序是否可加载
        if (phdr->p_type != PT_LOAD)
        {
            continue;
        }
        // 拿到相应的section的地址
        uint8_t *src = file_buffer + phdr->p_offset;
        // 拿到将要存放到的内存地址
        uint8_t *dest = (uint8_t *)phdr->p_paddr;
        // 开始将section的信息放到相应的地址处
        for (int j = 0; j < phdr->p_filesz; j++)
        {
            *dest++ = *src++;
        }

        // 剩余区域填充为0
        dest = (uint8_t *)phdr->p_paddr + phdr->p_filesz;
        for (int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++)
        {
            *dest++ = 0;
        }
    }
    return elf_hdr->e_entry;
}

void enable_page_mode()
{
#define PDE_P (1 << 0)
#define PDE_PS (1 << 7)
#define PDE_W (1 << 1)
#define CR4_PSE (1 << 4)
#define CR0_PG (1 << 31)
    static uint32_t page_dir[1024] __attribute__((aligned(4096))) = {
        [0] = PDE_P | PDE_W,
    };

    // wcr4(rcr4() | CR4_PSE);
    wcr3((uint32_t)page_dir);
    wcr0(rcr0() | CR0_PG);
}

void loader32c_entry(void)
{
    // 加载内核的二进制文件到内存中
    load2mem(100, 500, (uint8_t *)KERNEL_ENTRY);
    // 解析ELF文件，将各个段放置到相应的位置
    uint32_t kernel_entry = read_elf((uint8_t *)KERNEL_ENTRY);
    // 跳跃到内核区
    // enable_page_mode();
    // ((void (*)(mem_info_t *, gdt_entry_t *))kernel_entry)(&mem, gdt_table);
    ((void (*)(mem_info_t *))kernel_entry)(&mem);
    // ((void (*)(mem_info_t *))KERNEL_ENTRY)(&mem);
}