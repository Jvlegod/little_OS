__asm__(".code16gcc");

#include "comm/cpu_instr.h"
#include "loader16.h"
#include "loader.h"
#include "comm/os_cfg.h"
#include "gdt16/gdt16.h"

extern mem_info_t mem;

/**
 * @brief int 0x15 获取内存容量的一号方法
 *
 * @return true
 * @return false
 */
bool detect_method1()
{
    ARDS_entry_t ARDS;
    uint32_t countID;
    uint32_t signature, blength;

    for (int i = 0; i < MAX_MEM_LENGTH; i++)
    {
        ARDS_entry_t *entry = &ARDS;
        // 每次从其中写入并调出内存的信息
        __asm__ __volatile__("int  $0x15"
                             : "=a"(signature), "=c"(blength), "=b"(countID)
                             : "a"(0xE820), "b"(countID), "c"(20), "d"(0x534D4150), "D"(entry));

        // 对返回出的信息进行处理
        if (signature != 0x534d4150)
        {
            return false;
        }

        if (entry->Type == 1)
        {
            mem.info[mem.count].start = entry->BaseL;
            mem.info[mem.count].offset = entry->LengthL;
            mem.count++;
        }

        if (blength == 20)
        {
            continue;
        }

        if (countID == 0)
        {
            return true;
        }
    }
}

/**
 * @brief int 0x15 获取内存容量的二号方法
 *
 * @return true
 * @return false
 */
bool detect_method2()
{
    return false;
}

/**
 * @brief int 0x15 获取内存容量的三号方法
 *
 * @return true
 * @return false
 */
bool detect_method3()
{
    return false;
}

/**
 * @brief 内存检测
 *
 */
void detect_memory()
{
    if (detect_method1() == true)
        goto end;

    if (detect_method2() == true)
        goto end;

    detect_method3();
end:
    return;
}

/**
 * @brief 进入保护模式
 *
 */
static void enter_portected()
{
    // 关中断
    cli();
    // 打开A20总线
    uint8_t a20 = inb(0x92); // 进入南桥芯片并读出数据
    outb(0x92, 0x2 | a20);   // 打开南桥芯片的A20控制位

    // 加载GDT表
    gdt16_init();

    // 开启cr0中的PE位（打开页表也需要将此位置1）
    uint32_t cr0 = rcr0();
    wcr0(cr0 | (1 << 0));
    // 跳入保护模式的汇编代码部分
    far_jmp(8, (uint32_t)protected_mode_entry);
}

/**
 * @brief 16位loader的C入口
 *
 */
void loader16c_entry(void)
{
    // 内存检测
    detect_memory();
    // 进入保护模式
    enter_portected();
}