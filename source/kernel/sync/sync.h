#ifndef SYNC_H
#define SYNC_H
#include "debug/debug.h"
#include "tools/klist.h"
#include "comm/types.h"
// #include "task/task.h"
#include "irq/irq.h"

/* 进程和线程复用的信号量 */
typedef struct _semaphore_t
{
    uint8_t value; /* 信号量的值 */
    kdouble_list_t wait_list;
} semaphore_t;

void sem_init(semaphore_t *sem, bool shared, uint8_t value);
/****************************弃用********************************/
// void thread_sem_wait(semaphore_t *sem);
// void thread_sem_post(semaphore_t *sem);
/****************************弃用********************************/
void sem_destroy(semaphore_t *sem);

#endif // !SYNC_H