#ifndef F_LIST_H
#define F_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "mwc/forge.h"



#define list_foreach_(list, item)\
    for (ListNode *item = list->head; item != NULL; item = item->next)



typedef enum ListPos ListPos;

typedef struct List List;
typedef struct ListNode ListNode;



List *list_new();
void list_push(List *list, void *data);
void list_fprintf(List *list, FILE *output, void (*callback)(FILE *, void *));
void list_fprintf_pos(List *list, FILE *output,
                      void (*callback)(FILE *, void *, ListPos));
void list_foreach(List *list, void (*callback)(void *));
void list_clear(List *list);
void list_free(List *list);
void list_clear_foreach(List *list, void (*callback)(void *));
void list_free_foreach(List *list, void (*callback)(void *));



struct List {
    ulong size;
    ListNode *head;
    ListNode *tail;
};

struct ListNode {
    void *data;
    ListNode *next;
};



enum ListPos {
    LIST_POS_START,
    LIST_POS_INSIDE,
    LIST_POS_END,
    LIST_POS_ALL, /* if list length = 1 */
};



#endif
