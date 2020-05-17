#ifndef F_LIST_H
#define F_LIST_H

struct ListNode
{
    int value;
    struct ListNode *prev;
    struct ListNode *next;
};

struct List
{
    int size;
    struct ListNode *head;
    struct ListNode *tail;
};

void list_init(struct List* list);
void list_node_init(struct ListNode* node);
struct List *list_new();
void list_push(struct List* list, int value);
void list_shift(struct List* list, int value);
void list_pop(struct List* list);
void list_unshift(struct List* list);
int list_size(struct List* list);
void list_clear(struct List* list);
void list_destroy(struct List* list);
struct ListNode *list_get_node(struct List* list, int index);
void list_insert(struct List* list, int index, int value);
int list_get_value(struct List* list, int index);
void list_print(struct List* list);

#endif
