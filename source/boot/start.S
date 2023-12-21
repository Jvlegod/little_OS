	#include "boot.h"
  	.code16
 	.text
	.global _start
	.extern boot_entry
_start:
//  初始化寄存器
 	mov $0, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	mov $_start, %esp

	call clear_screen
	call read_loader

	jmp boot_entry

// 清屏
clear_screen:
	push %ax
	push %cx
	push %dx
	push %bx

	mov $0x06, %ah
	mov $0,    %al
	mov $0,    %ch
	mov $0,    %cl
	mov $24,   %dh
	mov $79,   %dl
	mov $0x07, %bh
	int $0x10

	pop %bx
	pop %dx
	pop %cx
	pop %ax
	ret

//读取loader到内存中
read_loader:
	push %bx
	push %cx
	push %ax
	push %dx

	mov $0x8000, %bx
	mov $0x2,    %cx		
	mov $0x2,    %ah		
	mov $64,     %al		
	mov $0x0080, %dx	
	int $0x13
	jc read_loader
	
	pop %dx
	pop %ax
	pop %cx
	pop %bx
	
	ret	
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa
