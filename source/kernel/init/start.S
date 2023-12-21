#include "comm/selector_define.h"
    .text
    .extern kernel_init, main
    .global _start
_start:
    push %ebp
    mov %esp, %ebp
    // mov 0xC(%ebp), %eax
    // push %eax
    mov 0x8(%ebp), %eax
    push %eax

    call kernel_init

    jmp $KERNEL_SELECTOR_CS, $reload_gdt
reload_gdt:
    mov $KERNEL_SELECTOR_DS, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    mov $(stack + KERNEL_STACK_SIZE), %esp

    jmp main

// 内核栈
    .bss
.comm stack, KERNEL_STACK_SIZE

// 中断处理程序
    .text
.macro exception_handler name irq_num erro_code
        .global exception_handler_\name
        .extern irq_handler_\name
    exception_handler_\name:
        .if \erro_code == 0
            push $0
        .endif
        push $\irq_num
        // 0->0 的变化 push了 elfags, cs, eip, erro_code
        
        // push eax, ecx, edx, ebx, esp, ebp, esi, edi
        pusha

        push %ds
        push %es
        push %fs
        push %gs
        
        push %esp
        call irq_handler_\name
        pop %esp

        pop %gs
        pop %fs
        pop %es
        pop %ds

        popa
        add $(2*4), %esp
        
        iret
.endm

// 软件中断
exception_handler unknown, -1, 0
exception_handler divider, 0, 0
exception_handler debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unfound, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler segment_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler float_point_error, 16, 0
exception_handler aligment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler SIMD, 19, 0
exception_handler virtual_exception, 20, 0

// 硬件中断
exception_handler timer, 0x20, 0
/*
    .global exception_handler_timer
    .extern irq_handler_timer
exception_handler_timer:
    push $0
    push 0x20
    // 0->0 的变化 push了 elfags, cs, eip, erro_code
    
    // push eax, ecx, edx, ebx, esp, ebp, esi, edi
    pusha

    push %ds
    push %es
    push %fs
    push %gs
    
    push %esp
    call irq_handler_timer
    pop %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds

    popa
    add $(2*4), %esp
    
    iret
*/

// 系统调用处理函数
    .text
.extern syscall_table
.global syscall_handler
syscall_handler:
    push $0 // 压入erro code
    push $0x80 // 压入中断向量码
    pusha
    push %ds
    push %es
    push %fs
    push %gs
    push %edx // 系统调用的第3个参数
    push %ecx // 系统调用的第2个参数
    push %ebx // 系统调用的第1个参数
    
    push %esp

    mov $syscall_table, %ebx
    mov (%ebx, %eax, 4), %eax
    call *%eax
    
    pop %esp
    
    add $(0xC), %esp

    pop %gs
    pop %fs
    pop %es
    pop %ds
    mov %eax, 0x1C(%esp)
    popa
    add $0x8, %esp // 跳过err code和中断向量码
    iret


// 转换
    .text
    .global simple_switch
simple_switch:
// simple_switch(stack1的地址的地址, stack2的地址)
    mov 4(%esp), %eax
    mov 8(%esp), %edx

    push %esi
    push %edi
    push %ebx
    push %ebp
    mov %esp, (%eax)

    mov %edx, %esp
    pop %ebp
    pop %ebx
    pop %edi
    pop %esi

    ret

// 废案
//    .text
//.global intr_exit
//intr_exit:	     
//    add $4, %esp			   // 跳过中断号
//    popal
//    pop %gs
//    pop %fs
//    pop %es
//    pop %ds
//    add $4, %esp			   // 跳过error_code
//    iret