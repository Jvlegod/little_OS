SECTIONS {
    /*
        0~1MB       内核主要位置
        1MB~2MB     内核内存分配区域
        2MB~128MB   用户内存分配区域
    */
    PROVIDE(kernel_base = 0x0);
    . = 0x10000;

    PROVIDE(kernel_end = .);
    . = ALIGN(4096);

    PROVIDE(stext = .);
    .text : {
        *(.text)
    }

    .rodata : {
        *(.rodata)
    }
    PROVIDE(etext = .);

    . = ALIGN(4096);
    PROVIDE(sdata = .);
    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }

    PROVIDE(edata = .);
    PROVIDE(free_addr = .);

    /* 1MB的位置(内核空间开始的位置) */
    . = 0x100000; /* 1MB的位置 */

    . = 0x170000;
    /* 将init_proc函数放在2MB以上的位置 */
    .init_proc_section ALIGN(4096) :
    {
        *(.init_proc_section)
    }

    /* 2MB的位置(用户空间开始的位置) */
    . = 0x200000;
    
}