#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "token.h"

/*
struct TokenTree *token_tree_new()
{
	struct TokenTree *tree = malloc(sizeof(struct TokenTree));

	if (tree == NULL)
		exit(EXIT_FAILURE);

	tree->size = 0;

	return tree;
}

void token_tree_clear(struct TokenTree *list)
{

}

void token_tree_delete(struct TokenTree *list)
{

}
*/


struct TokenList *token_list_new()
{
	struct TokenList *list = malloc(sizeof(struct TokenList));

	if (list == NULL)
		exit(EXIT_FAILURE);

	list->size = 0;
	list->head = NULL;
	list->tail = NULL;

	return list;
}

void token_list_push(struct TokenList *list, const unsigned long int current_line, const char type, const char id, const char *token)
{
	struct TokenListNode *list_node = malloc(sizeof(struct TokenListNode));
	if (list_node == NULL)
		exit(EXIT_FAILURE);

	struct TokenNode *token_node = malloc(sizeof(struct TokenNode));
	if (token_node == NULL)
		exit(EXIT_FAILURE);

	token_node->line = current_line;
	token_node->type = type;
	token_node->id = id;

	size_t len = strlen(token)+1;
	if (type == TOK_TYPE_ID && len > 32)
		len = 32;
	token_node->token = malloc(sizeof(char)*(len));
	if (token_node->token == NULL)
		exit(EXIT_FAILURE);
	strncpy(token_node->token, token, len-1);
	token_node->token[len-1] = '\0';

	list_node->data = token_node;
	list_node->next = NULL;

	if (list->size == 0)
	{
		list->head = list_node;
		list->tail = list_node;
	}
	else
	{
		list->tail->next = list_node;
		list->tail = list_node;
	}
	list->size++;
}

void token_list_foreach(struct TokenList *list, void (*callback)(struct TokenListNode *))
{
	struct TokenListNode *node = list->head;
	while (node != NULL)
	{
		(*callback)(node);
		node = node->next;
	}
}

void token_list_fprintf(FILE *output, struct TokenList *list)
{
	struct TokenListNode *node = list->head;
	while (node != NULL)
	{
		fprintf(output, "[Line: %lu\t\tType: %d\t\tId: %d\t\tSymbol: %s\t\t]\n", node->data->line, node->data->type, node->data->id, node->data->token);
		node = node->next;
	}
}

void token_list_clear(struct TokenList *list)
{
	struct TokenListNode *node = NULL;
	while (list->head != NULL)
	{
		node = list->head;
		list->head = node->next;
		free(node->data->token);
		free(node->data);
		free(node);
	}
	list->tail = NULL;
	list->size = 0;
}

void token_list_delete(struct TokenList *list)
{
	token_list_clear(list);
	free(list);
}
