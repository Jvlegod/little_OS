#include "irq.h"
#include "comm/trapframe.h"
#include "tools/klib.h"

extern irq_desc_t idt_table[IDT_SIZE];

void pic_enable(uint8_t pic_num)
{
    if (pic_num < PIC_START)
    {
        klog_printf("pic_enable erro: init failed");
        return;
    }

    pic_num -= PIC_START;

    /* irq[0 - 8] */
    if (pic_num < 8)
    {
        int mask = inb(PIC_M_DATA) & ~(1 << pic_num);
        outb(PIC_M_DATA, mask);
    }
    /* irq[9 - 15] */
    else
    {
        int mask = inb(PIC_S_DATA) & ~(1 << (pic_num - 8));
        outb(PIC_S_DATA, mask);
    }
}

/**
 * @brief 初始化中断向量表
 *
 */
void irq_init()
{
    /* 初始化中断代理 */
    pic_init();

    for (int i = 0; i < IDT_SIZE; i++)
    {
        irq_segment_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)ENTRY(unknown), GATE_TYPE_IDT | GATE_P | GATE_DPL_0);
    }

    /****************中断程序安装区域*********************/
    irq_install(IRQ0_DE, (irq_handler_t)ENTRY(divider));
    irq_install(IRQ1_DB, (irq_handler_t)ENTRY(debug));
    irq_install(IRQ2_NMI, (irq_handler_t)ENTRY(NMI));
    irq_install(IRQ3_BP, (irq_handler_t)ENTRY(breakpoint));
    irq_install(IRQ4_OF, (irq_handler_t)ENTRY(overflow));
    irq_install(IRQ5_BR, (irq_handler_t)ENTRY(bound_range));
    irq_install(IRQ6_UD, (irq_handler_t)ENTRY(invalid_opcode));
    irq_install(IRQ7_NM, (irq_handler_t)ENTRY(device_unfound));
    irq_install(IRQ8_DF, (irq_handler_t)ENTRY(double_fault));
    // irq_install(IRQ9_MF, (irq_handler_t)ENTRY(divider));
    irq_install(IRQ10_TS, (irq_handler_t)ENTRY(invalid_tss));
    irq_install(IRQ11_NP, (irq_handler_t)ENTRY(segment_not_present));
    irq_install(IRQ12_SS, (irq_handler_t)ENTRY(stack_segment_fault));
    irq_install(IRQ13_GP, (irq_handler_t)ENTRY(general_protection));
    irq_install(IRQ14_PF, (irq_handler_t)ENTRY(page_fault));
    irq_install(IRQ16_MF, (irq_handler_t)ENTRY(float_point_error));
    irq_install(IRQ17_AC, (irq_handler_t)ENTRY(aligment_check));
    irq_install(IRQ18_MC, (irq_handler_t)ENTRY(machine_check));
    irq_install(IRQ19_XM, (irq_handler_t)ENTRY(SIMD));
    irq_install(IRQ20_VE, (irq_handler_t)ENTRY(virtual_exception));
    /****************中断程序安装区域*********************/
    /****************系统调用单独给一个特权级*************/
    irq_segment_set(idt_table + IRQ128_INT, KERNEL_SELECTOR_CS, (uint32_t)syscall_handler, GATE_TYPE_IDT | GATE_P | GATE_DPL_3);
    /****************系统调用单独给一个特权级*************/
    lidt((uint32_t)idt_table, sizeof(irq_desc_t) * IDT_SIZE);

    return;
}

/**
 * @brief 设置中断门
 *
 * @param desc 中断门描述符
 * @param selector 程序段选择子
 * @param offset 段内偏移量
 * @param attr 中断门的属性
 * @return true
 * @return false
 */
bool irq_segment_set(irq_desc_t *desc, uint16_t selector, uint32_t offset, uint16_t attr)
{
    if (offset > 0xFFFFF)
    {
        return false;
    }

    desc->limit15_0 = offset & 0xFFFF;
    desc->selector = selector;
    desc->attribute = attr;
    desc->limit31_16 = (offset >> 16) & 0xFFFF;
}

/**
 * @brief 安装中断的程序
 *
 * @param irq_num 中断向量号
 * @param handler 中断函数
 */
bool irq_install(uint16_t irq_num, irq_handler_t handler)
{
    if (irq_num >= IDT_SIZE)
    {
        return false;
    }
    irq_segment_set(idt_table + irq_num, KERNEL_SELECTOR_CS, (uint32_t)handler, GATE_TYPE_IDT | GATE_P | GATE_DPL_0);
    return true;
}

void pic_init()
{

    /* 初始化主片 */
    outb(PIC_M_CTRL, 0x11); // ICW1：边沿触发 + 级联8259A + 需要ICW4
    outb(PIC_M_DATA, 0x20); // ICW2：起始中断向量号是0x20（32）IR[0 - 7]为0x20~0x27

    outb(PIC_M_DATA, 0x04); // ICW3：IR2接从片
    outb(PIC_M_DATA, 0x01); // ICW4：8086模式，非自动EOI

    /* 初始化从片 */
    outb(PIC_S_CTRL, 0x11); // ICW1：边沿触发 + 级联8259A + 需要ICW4
    outb(PIC_S_DATA, 0x28); // ICW2：起始中断向量号为0x28（40）IR[8 - 15]为0x28~0x2F

    outb(PIC_S_DATA, 0x02); // ICW3：设置从片到主片的IR2引脚
    outb(PIC_S_DATA, 0x01); // ICW4：8086模式，正常EOI
}

/* 未知异常 */
void irq_handler_unknown(trap_frame_t *trapframe)
{
    // hlt();
    // for (;;)
    //     ;
    return;
}

/* 除0异常 */
void irq_handler_divider(trap_frame_t *trapframe)
{
    // hlt();
    // for (;;)
    //     ;
    return;
}

void irq_handler_debug(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_NMI(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_breakpoint(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_overflow(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_bound_range(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_invalid_opcode(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_device_unfound(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_double_fault(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_invalid_tss(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_segment_not_present(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_stack_segment_fault(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_general_protection(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_page_fault(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_float_point_error(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_aligment_check(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_machine_check(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}
void irq_handler_SIMD(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}

void irq_handler_virtual_exception(trap_frame_t *trapframe)
{
    hlt();
    for (;;)
        ;
}

/**
 * @brief 打开中断
 *
 * @return irq_status 操作之前的状态
 */
irq_status irq_enable()
{
    irq_status old_status = get_irq_status();
    if (old_status == INTR_OFF)
    {
        sti();
    }
    return old_status;
}

/**
 * @brief 关闭中断
 *
 * @return irq_status 操作之前的状态
 */
irq_status irq_disable()
{
    irq_status old_status = get_irq_status();
    if (old_status == INTR_ON)
    {
        cli();
    }
    return old_status;
}

/**
 * @brief 取得中断的状态
 *
 * @return irq_status
 */
irq_status get_irq_status()
{
    uint32_t elfags = 0;
    GET_EFLAGS(elfags);
    return (EFLAGS_IF & elfags) ? INTR_ON : INTR_OFF;
}

/**
 * @brief 关中断并保留关中断前的状态
 *
 * @return irq_status
 */
irq_status irq_enter_protection()
{
    irq_status status = irq_disable();
    return status;
}

/**
 * @brief 恢复关中断之前的状态
 *
 * @param status
 * @return irq_status
 */
irq_status irq_leave_protection(irq_status status)
{
    irq_set_status(status);
}

irq_status irq_set_status(irq_status status)
{
    return (status == INTR_ON) ? irq_enable() : irq_disable();
}