#include "list.h"

/*
 *  Linked list library for void pointer data
 */

List *list_new()
{
    List *list = malloc(sizeof(List));
    if (list == NULL) exit(EXIT_FAILURE);

    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return list;
}

void list_push(List *list, void *data)
{
    ListNode *node = malloc(sizeof(ListNode));
    if (node == NULL) exit(EXIT_FAILURE);

    node->data = data;
    node->next = NULL;

    if (list->size == 0)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
    list->size++;
}

void list_fprintf(List *list, FILE *output, void (*callback)(FILE *, void *))
{
    ListNode *node = list->head;
    while (node != NULL)
    {
        (*callback)(output, node->data);
        node = node->next;
    }
}

void list_fprintf_pos(List *list, FILE *output,
                      void (*callback)(FILE *, void *, ListPos))
{
    if (list->size == 1)
    {
        (*callback)(output, list->head->data, LIST_POS_ALL);
    }
    else
    {
        ListNode *node = list->head;
        (*callback)(output, node->data, LIST_POS_START);
        for (int i = 1; i < list->size-1; i++)
        {
            node = node->next;
            (*callback)(output, node->data, LIST_POS_INSIDE);
        }
        node = node->next;
        (*callback)(output, node->data, LIST_POS_END);
    }
}

void list_foreach(List *list, void (*callback)(void *))
{
    ListNode *node = list->head;
    while (node != NULL)
    {
        (*callback)(node->data);
        node = node->next;
    }
}

void list_clear(List *list)
{
    ListNode *node = NULL;
    while (list->head != NULL)
    {
        node = list->head;
        list->head = node->next;
        free(node);
    }
    list->tail = NULL;
    list->size = 0;
}

void list_free(List *list)
{
    list_clear(list);
    free(list);
}



void list_clear_foreach(List *list, void (*callback)(void *))
{
    ListNode *node = NULL;
    while (list->head != NULL)
    {
        node = list->head;
        list->head = node->next;
        (*callback)(node->data);
        free(node);
    }
    list->tail = NULL;
    list->size = 0;
}

void list_free_foreach(List *list, void (*callback)(void *))
{
    list_clear_foreach(list, callback);
    free(list);
}
