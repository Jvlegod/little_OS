#include "klist.h"

/**
 * @brief 初始化链表
 *
 * @param list
 */
void klist_init(kdouble_list_t *list)
{
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
    list->len = 0;
}

/**
 * @brief 在链表head之后push节点
 *
 * @param list
 * @param node
 */
void klist_push(kdouble_list_t *list, klist_node_t *node)
{
    node->prev = &list->head;
    node->next = list->head.next;

    list->head.next->prev = node;
    list->head.next = node;

    list->len++;
}

/**
 * @brief 在链表的tail之前append节点
 *
 * @param list
 * @param node
 */
void klist_append(kdouble_list_t *list, klist_node_t *node)
{
    node->next = &list->tail;
    node->prev = list->tail.prev;

    list->tail.prev->next = node;
    list->tail.prev = node;

    list->len++;
}

/**
 * @brief 将链表的head后的第一个元素pop出去
 *
 * @param list
 */
klist_node_t *klist_pop(kdouble_list_t *list)
{
    klist_node_t *node = list->head.next;
    list->head.next->next->prev = &list->head;
    list->head.next = list->head.next->next;

    list->len--;

    return node;
}

/**
 * @brief 删除链表中指定的节点
 *
 * @param list
 * @param remove_node
 * @return klist_node_t*
 */
klist_node_t *klist_remove(kdouble_list_t *list, klist_node_t *remove_node)
{
    klist_node_t *temp = list->head.next;
    while (temp)
    {
        /* 找到了 */
        if (temp == remove_node)
        {
            klist_node_t *pre = temp->prev;
            pre->next = temp->next;
            temp->next->prev = pre;
            list->len--;
            return remove_node;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * @brief 获取链表的长度
 *
 * @param list
 * @return int
 */
int klist_len(kdouble_list_t *list)
{
    return list->len;
}

/**
 * @brief 获取链表中第一个节点
 *
 * @param list
 * @return klist_node_t*
 */
klist_node_t *klist_get_first_node(kdouble_list_t *list)
{
    return list->head.next;
}

/**
 * @brief 获取链表中最后一个节点
 *
 * @param list
 * @return klist_node_t*
 */
klist_node_t *klist_get_last_node(kdouble_list_t *list)
{
    return list->tail.prev;
}

/**
 * @brief 寻找链表中的节点
 *
 * @param list
 * @param remove_node
 * @return klist_node_t*
 */
klist_node_t *klist_find(kdouble_list_t *list, klist_node_t *target_node)
{
    klist_node_t *temp = list->head.next;
    while (temp)
    {
        if (temp == target_node)
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

/**
 * @brief 判断链表是否为空
 *
 * @param list
 * @return true
 * @return false
 */
bool klist_empty(kdouble_list_t *list)
{
    return klist_len(list) == 0;
}