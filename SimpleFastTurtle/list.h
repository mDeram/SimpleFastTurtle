#ifndef F_LIST_H
#define F_LIST_H

struct List *list_new();
void list_push(struct List *list, void *data);
void list_fprintf(struct List *list, FILE *output, void (*callback)(FILE *, void *));
void list_foreach(struct List *list, void (*callback)(void *));
void list_clear(struct List *list);
void list_free(struct List *list);
void list_clear_foreach(struct List *list, void (*callback)(void *));
void list_free_foreach(struct List *list, void (*callback)(void *));



struct List {
	unsigned long int size;
	struct ListNode *head;
	struct ListNode *tail;
};

struct ListNode {
	void *data;
	struct ListNode *next;
};

#endif
