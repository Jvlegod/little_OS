    .code16
    .text
    .extern loader16c_entry
    .global _start
_start:
    jmp  loader16c_entry

    .code32
    .text
    .global protected_mode_entry
    .extern loader32c_entry
protected_mode_entry:
    mov $16, %ax
	mov %ax, %ds
	mov %ax, %ss
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

    jmp $8, $loader32c_entry