#ifndef TYPES_H
#define TYPES_H

#define NULL (void *)0

typedef int _pid_t;
typedef enum
{
    true = 1,
    false = 0
} bool;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;

typedef _pid_t pid_t;

typedef enum _irq_status
{
    INTR_ON,
    INTR_OFF
} irq_status;

/**
 * @brief 任务状态
 *
 */
typedef enum
{
    TASK_RUNNING, // 运行态
    TASK_CREATED, // 新建
    TASK_READY,   // 就绪态
    TASK_BLOCKED, // 阻塞态
    TASK_WAITING, // 等待态
    TASK_HANGING, // 挂起
    TASK_SLEEP,   // 睡眠
    TASK_DIED     // 死机
} task_status;

#endif // !TYPES_H