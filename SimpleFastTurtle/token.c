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
	char ident[1000] = " ";
	fprintf(output, " ");

	fprintf(output, "%s", s_tree->token->token);

	fprintf(output, "\n");

	void i_apply_ident(FILE *output, void *data, char e_pos)
	{
		token_statement_fprintf(output, data, ident, e_pos);
	}

	if (s_tree->statements != NULL && s_tree->statements->size)
		list_fprintf_pos(s_tree->statements, output, i_apply_ident);

	fprintf(output, "\n");
}

void token_statement_fprintf(FILE *output, void *data, char ident[], char e_pos)
{
	struct Statement *s_statement = (struct Statement*)data;

	/* Before : Next gen display */
	fprintf(output, "%s", ident);
	if (e_pos == LIST_END || e_pos == LIST_ALL)
		fprintf(output, "└─");
	else
		fprintf(output, "├─");

	/* Token */
	fprintf(output, "%s", s_statement->token->token);

	/* After : Content of the statement with updated ident */
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
	ident[i+2] = '\0';

	/* Big ident */
	if (s_statement->statements != NULL && s_statement->statements->size)
		ident[i+2] = '|';
	else
		ident[i+2] = ' ';
	ident[i+3] = ' ';
	ident[i+4] = '\0';

	if (s_statement->token->id == TOK_KEY_VAR && s_statement->expression)
	{
		fprintf(output, "┐");
		fprintf(output, "\n");
		token_expression_fprintf(output, s_statement->expression, ident, e_pos);
	}
	else if (s_statement->expressions != NULL && s_statement->expressions->size)
	{
		fprintf(output, "┐");
		fprintf(output, "\n");
		void i_apply_ident(FILE *output, void *data, char e_pos)
		{
			token_expression_fprintf(output, data, ident, e_pos);
		}
		list_fprintf_pos(s_statement->expressions, output, i_apply_ident);
	}
	else
	{
		fprintf(output, "\n");
	}
	/* Reset big ident */
	ident[i+2] = '\0';

	if (s_statement->statements != NULL && s_statement->statements->size)
	{
		void i_apply_ident(FILE *output, void *data, char e_pos)
		{
			token_statement_fprintf(output, data, ident, e_pos);
		}
		list_fprintf_pos(s_statement->statements, output, i_apply_ident);
	}

	/* Reset ident */
	ident[i] = '\0';
}

void token_expression_fprintf(FILE *output, void *data, char ident[], char e_pos)
{
	struct Expression *s_expression = (struct Expression*)data;

	/* Before : Next gen display */
	fprintf(output, "%s", ident);
	if (e_pos == LIST_END || e_pos == LIST_ALL)
		fprintf(output, "└─");
	else
		fprintf(output, "├─");

	/* Operator */
	//fprintf(output, "%s", s_expression->token->token);
	fprintf(output, "Expr\n");
	/* After : Content of the statement with updated ident */
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

	/* Print expressions recursively */


	/* Reset ident */
	ident[i] = '\0';
}

/*
void token_s_statement_free(void *data)
{

}
*/
