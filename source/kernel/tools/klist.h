#ifndef LIST_H
#define LIST_H
#include "comm/types.h"

/* 链表节点 */
typedef struct _klist_node_t
{
    struct _klist_node_t *prev; // 指向前驱节点
    struct _klist_node_t *next; // 指向后驱节点
} klist_node_t;

typedef struct _kdouble_list_t
{
    klist_node_t head; // 链表头
    klist_node_t tail; // 链表尾
    int len;           // 链表的长度
} kdouble_list_t;

void klist_init(kdouble_list_t *list);
void klist_push(kdouble_list_t *list, klist_node_t *node);
void klist_append(kdouble_list_t *list, klist_node_t *node);
void klist_insert(kdouble_list_t *list, klist_node_t *node);
klist_node_t *klist_pop(kdouble_list_t *list);
int klist_len(kdouble_list_t *list);
void klist_visited(kdouble_list_t *list);
klist_node_t *klist_remove(kdouble_list_t *list, klist_node_t *remove_node);
klist_node_t *klist_get_first_node(kdouble_list_t *list);
klist_node_t *klist_get_last_node(kdouble_list_t *list);
klist_node_t *klist_find(kdouble_list_t *list, klist_node_t *target_node);
bool klist_empty(kdouble_list_t *list);

#endif // !LIST_H