#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "token.h"

void token_fprintf(FILE *output, void *data)
{
	struct TokenNode *node = (struct TokenNode *)data;
	fprintf(output,
			"[Line: %lu\t\tType: %d\t\tId: %d\t\tSymbol: %s\t\t]\n",
			node->line,
			node->type,
			node->id,
			node->token);
}

struct TokenNode *token_new(const unsigned long int current_line,
                    		const char type,
                    		const char id,
                    		const char *token)
{
	struct TokenNode *node = malloc(sizeof(struct TokenNode));
	if (node == NULL)
		exit(EXIT_FAILURE);

	node->line = current_line;
	node->type = type;
	node->id = id;

	size_t len = strlen(token)+1;
	if (type == TOK_TYPE_ID && len > 32)
		len = 32;
	node->token = malloc(sizeof(char)*(len));
	if (node->token == NULL)
		exit(EXIT_FAILURE);
	strncpy(node->token, token, len-1);
	node->token[len-1] = '\0';

	return node;
}

void token_free(void *data)
{
	struct TokenNode *node = (struct TokenNode *)data;
	free(node->token);
	free(node);
}

#if 0
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
#endif
