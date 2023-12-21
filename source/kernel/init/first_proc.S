    .text
    .global first_task_entry
    .extern first_proc
first_task_entry:
    // mov %ss, %ax
    // mov %ax, %ds
    // mov %ax, %es
    // mov %ax, %fs
    // mov %ax, %gs
    jmp first_proc