#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "list.h"

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


/* ┌ ┘ └ ┐ ─ | ├ */

void token_tree_fprintf(FILE *output, void *data)
{
	struct Statement *s_tree = (struct Statement*)data;
	char ident[100] = " ";
	fprintf(output, " ");

	fprintf(output, "%s", s_tree->token->token);

	fprintf(output, "\n");

	void i_apply_ident(FILE *output, void *data, char e_pos)
	{
		token_branch_fprintf(output, data, ident, e_pos);
	}

	if (s_tree->statements != NULL && s_tree->statements->size)
		list_fprintf_pos(s_tree->statements, output, i_apply_ident);

	fprintf(output, "\n");
}

void token_branch_fprintf(FILE *output, void *data, char ident[], char e_pos)
{
	struct Statement *s_tree = (struct Statement*)data;
	fprintf(output, "%s", ident);
	if (e_pos == LIST_END || e_pos == LIST_ALL)
		fprintf(output, "└─");
	else
		fprintf(output, "├─");

	fprintf(output, "%s", s_tree->token->token);

	fprintf(output, "\n");

	if (s_tree->statements != NULL && s_tree->statements->size)
	{
		int i = 0;
		while(ident[i] != '\0')
		{
			i++;
		}
		if (e_pos == LIST_END || e_pos == LIST_ALL)
			ident[i] = ' ';
		else
			ident[i] = '|';
		
		ident[i+1] = ' ';
		void i_apply_ident(FILE *output, void *data, char e_pos)
		{
			token_branch_fprintf(output, data, ident, e_pos);
		}
		list_fprintf_pos(s_tree->statements, output, i_apply_ident);
		ident[i] = '\0';
	}
}
/*
void token_branch_free(void *data)
{

}
*/
