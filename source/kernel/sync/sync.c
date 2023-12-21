#include "sync.h"

/**
 * @brief 初始化信号量
 *
 * @param sem 信号量结构
 * @param shared 是否被其它进程共享 0表示不被共享，1表示被共享
 * @param value 计数值
 */
void sem_init(semaphore_t *sem, bool shared, uint8_t value)
{
    sem->value = value;
    klist_init(&sem->wait_list);
}

/**
 * @brief 信号量的down操作
 *
 * @param sem
 */
void thread_sem_wait(semaphore_t *sem)
{
    irq_status status = irq_enter_protection();
    thread_pcb_t *cur = cur_thread();
    while (sem->value == 0)
    {
        klist_node_t *wait_node = klist_find(&sem->wait_list, &cur->state_node); /* 先查找在链表中是否存在该节点 */
        ASSERT(wait_node != NULL);                                               /* 该节点已经存在，不应该存在这种情况 */

        /* 先清扫一遍所有队列 */
        klist_remove(&thread_get_manager()->ready_list, &cur->state_node);
        klist_remove(&thread_get_manager()->sleep_list, &cur->state_node);
        klist_append(&sem->wait_list, &cur->state_node); /* 再加入到sem的等待队列中去，阻塞线程 */
        thread_set_block(cur);                           /* 阻塞线程 */
    }
    sem->value--;
    irq_leave_protection(status);
}

/**
 * @brief 信号量的up操作
 *
 * @param sem
 */
void thread_sem_post(semaphore_t *sem)
{
    irq_status status = irq_enter_protection();
    if (!klist_empty(&sem->wait_list))
    {
        klist_node_t *node = sem->wait_list.head.next;
        thread_pcb_t *pcb = KLIST_STRUCT_ADDR(node, thread_pcb_t, state_node);
        klist_remove(&sem->wait_list, node); /* 从等待队列中移除 */
        thread_push_ready(pcb);              /* 加入就绪队列 */
    }
    sem->value++;
    irq_leave_protection(status);
}

void sem_destroy(semaphore_t *sem)
{
    return;
}