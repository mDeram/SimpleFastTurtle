#ifndef F_LIST_H
#define F_LIST_H

struct ListNode {
	void *data;
	struct ListNode *next;
};

struct List {
	int size;
	struct ListNode *head;
	struct ListNode *tail;
};

struct List *list_new();
void list_push(struct List *list, void *new_data, size_t data_size);
void list_foreach(struct List *list, void (*callback)(void *));
void list_clear(struct List *list);
void list_delete(struct List *list);

#endif
