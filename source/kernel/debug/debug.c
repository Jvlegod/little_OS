#include "tools/klib.h"
#include "irq/irq.h"
#include "debug.h"

void panic(char *filename, int line, const char *func, const char *condition)
{
    irq_disable(); /* 关中断 */

    klog_printf("\n\n\n!!!!! error !!!!!\n");
    klog_printf("filename: %s\n", filename);
    klog_printf("line: %d\n", line);
    klog_printf("function: %s\n", func);
    klog_printf("condition: %s\n", condition);
    hlt();
}