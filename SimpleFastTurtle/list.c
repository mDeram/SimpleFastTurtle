#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "list.h"

/*
 *Double Liked struct List Librairy (DLLL)
 *todo:
 *  use void* type to create type independent lists
 */
static void remove_unique(struct List *list);

void list_init(struct List* list)
{
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}

void list_node_init(struct ListNode* node)
{
    node->value = 0;
    node->prev = NULL;
    node->next = NULL;
}

struct List *list_new()
{
    struct List *list = malloc(sizeof(struct List));

    if (list == NULL)
        exit(EXIT_FAILURE);

    list_init(list);

    return list;
}

//Add a struct ListNode at the end of the struct List
void list_push(struct List* list, int value)
{
    struct ListNode *node = malloc(sizeof(struct ListNode));

    if (node == NULL)
        exit(EXIT_FAILURE);

    //node init
    node->value = value;
    node->prev = list->tail;
    node->next = NULL;

    //list property update
    if (list->size == 0)
    {
        list->head = node;
        list->tail = node;
        list->size = 1;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
        list->size++;
    }
}

//Add a node at the head
void list_shift(struct List* list, int value)
{
    struct ListNode *node = malloc(sizeof(struct ListNode));

    if (node == NULL)
        exit(EXIT_FAILURE);

    //node init
    node->value = value;
    node->prev = NULL;
    node->next = list->head;

    //list property update
    if (list->size == 0)
    {
        list->head = node;
        list->tail = node;
        list->size = 1;
    }
    else
    {
        list->head->prev = node;
        list->head = node;
        list->size++;
    }
}

//Remove the only node of the list
static void remove_unique(struct List *list)
{
    free(list->head);
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

//Remove the tail node
void list_pop(struct List* list)
{
    if (list->size == 0)
        return;
    if (list->size == 1)
    {
        remove_unique(list);
    }
    else
    {
        struct ListNode *to_delete = list->tail;
        list->tail = to_delete->prev;
        list->tail->next = NULL;
        free(to_delete);
        list->size--;
    }
}

//Remove the first node of the list
void list_unshift(struct List* list)
{
    if (list->size == 0)
        return;
    if (list->size == 1)
    {
        remove_unique(list);
    }
    else
    {
        struct ListNode *to_delete = list->head;
        list->head = to_delete->next;
        list->head->prev = NULL;
        free(to_delete);
        list->size--;
    }
}

int list_size(struct List* list)
{
    return list->size;
}

//Delete all nodes from the list
void list_clear(struct List* list)
{
    while (list->head != NULL)
    {
        struct ListNode *to_delete = list->head;
        list->head = to_delete->next;
        free(to_delete);
    }
    list->tail = NULL;
    list->size = 0;
}

//Delete everything (list includ)
void list_destroy(struct List* list)
{
    while (list->head != NULL)
    {
        struct ListNode *to_delete = list->head;
        list->head = to_delete->next;
        free(to_delete);
    }
    free(list);
}

struct ListNode *list_get_node(struct List* list, int index)
{
    if (index >= list->size)
        exit(EXIT_FAILURE);
    if (index > list->size/2)
    {
        //Right side
        int i = list->size-1;
        struct ListNode *to_return = list->tail;
        while (i != index)
        {
            to_return = to_return->prev;
            i--;
        }
        return to_return;
    }
    else
    {
        //Left side
        int i = 0;
        struct ListNode *to_return = list->head;
        while (i != index)
        {
            to_return = to_return->next;
            i++;
        }
        return to_return;
    }
}

void list_insert(struct List* list, int index, int value)
{
    if (index > list->size || index < 0)
    {
        exit(EXIT_FAILURE);
    }
    else if (index == list->size)
    {
        list_push(list, value);
        return;
    }

    //Get the current node at this index
    struct ListNode *cur_node = list_get_node(list, index);
    if (cur_node == NULL)
        exit(EXIT_FAILURE);

    //Create a new node
    struct ListNode *node = malloc(sizeof(struct ListNode));

    if (node == NULL)
        exit(EXIT_FAILURE);

    //node init
    node->value = value;
    node->prev = cur_node->prev;
    node->next = cur_node;

    //list property update
    node->prev->next = node;
    cur_node->prev = node;
    list->size++;
}

int list_get_galue(struct List* list, int index)
{
    if (index >= list->size)
        exit(EXIT_FAILURE);
    return list_get_node(list, index)->value;
}

void list_print(struct List* list)
{
    struct ListNode *current = list->head;
    while (current != NULL)
    {
        printf("%d -> ", current->value);
        current = current->next;
    }
    printf("NULL\n");
}
