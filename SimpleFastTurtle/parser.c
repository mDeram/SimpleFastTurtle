#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "list.h"
#include "error.h"
#include "warning.h"

#include "parser.h"



static void parser_save(struct List *s_tree_token, struct List *s_list_token);
static void parser_parse(struct List *s_tree_token, struct List *s_list_token);

static int parser_next(struct ListNode **cur_node, struct TokenNode **cur_token);
static struct Statement *parser_statement(struct ListNode **p_cur_node, struct TokenNode **p_cur_token);
static struct Expression *parser_expression(struct ListNode **p_cur_node, struct TokenNode **p_cur_token);
static struct Expression *parser_expression_null();

void parser_process(struct List *s_tree_token,
					struct List *s_list_token,
					int option_save,
					int option_print_tree)
{
	parser_parse(s_tree_token, s_list_token);

	/*OPTIONS*/
    if (option_save)
        parser_save(s_tree_token, s_list_token);

    
    if (option_print_tree)
        list_fprintf(s_tree_token, stdout, token_tree_fprintf);
}

static void parser_save(struct List *s_tree_token, struct List *s_list_token)
{

}

static void parser_parse(struct List *s_tree_token, struct List *s_list_token)
{
	if (!s_list_token->size)
		return;

	struct ListNode **p_cur_node = &s_list_token->head;
	struct TokenNode **p_cur_token = (struct TokenNode **) &(*p_cur_node)->data;

	while ((*p_cur_node) != NULL)
	{
		list_push(s_tree_token, parser_statement(p_cur_node, p_cur_token));
	}
}

static int parser_next(struct ListNode **p_cur_node, struct TokenNode **p_cur_token)
{
	printf("ID: %d\n", (*p_cur_token)->id);
	*p_cur_node = (*p_cur_node)->next;
	if (*p_cur_node == NULL)
		return 1;

	*p_cur_token = (struct TokenNode *)(*p_cur_node)->data;
	return 0;
}

static struct Statement *parser_statement(struct ListNode **p_cur_node, struct TokenNode **p_cur_token)
{
	printf("statement\n");

	/*remove all ; before parsing the statement*/
	unsigned long int temp_line = (*p_cur_token)->line;
	while ((*p_cur_token)->id == TOK_SEP_SEMI)
	{
		if (parser_next(p_cur_node, p_cur_token)) /* ; */
			error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &temp_line);
	}

	if ((*p_cur_token)->id == TOK_SEP_CBE)
		return NULL;

	/*parse the statement recursively*/
	struct Statement *s_statement = malloc(sizeof(struct Statement));
	s_statement->token = *p_cur_token;
	s_statement->name = NULL;

	if ((*p_cur_token)->type == TOK_TYPE_KEY)
	{
		switch((*p_cur_token)->id)
		{
			case TOK_KEY_FOR: /*for transformed to a while ?*/
			case TOK_KEY_WHILE:
			case TOK_KEY_IF:
			case TOK_KEY_ELIF:
				s_statement->expressions = list_new();
				s_statement->statements = list_new();

				if (parser_next(p_cur_node, p_cur_token)) /* for/while/if/elif */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				while ((*p_cur_token)->id != TOK_SEP_CBS)
				{
					list_push(s_statement->expressions,
								parser_expression(p_cur_node, p_cur_token)); /*todo check if expression not null*/
				}

				if (s_statement->expressions->size == 0)
					error_printd(ERROR_PARSER_INVALID_NUMBER_PARAMETERS, &s_statement->token->line);

				if (parser_next(p_cur_node, p_cur_token)) /* { */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				while ((*p_cur_token)->id != TOK_SEP_CBE)
				{
					struct Statement *s_new_statement
							= parser_statement(p_cur_node, p_cur_token);
					if (s_new_statement)
						list_push(s_statement->statements, s_new_statement);
				}

				if (s_statement->statements->size == 0)
					warning_printd(WARNING_PARSER_EMPTY_STATEMENT, &s_statement->token->line);

				if (parser_next(p_cur_node, p_cur_token)) /* } */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);

				break;

			case TOK_KEY_FN:
				s_statement->expressions = list_new();
				s_statement->statements = list_new();

				if (parser_next(p_cur_node, p_cur_token)) /* fn */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);

				s_statement->name = *p_cur_token;

				/*todo error not identifier */
				if (parser_next(p_cur_node, p_cur_token)) /* identifier (name) */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				while ((*p_cur_token)->id != TOK_SEP_CBS)
				{
					printf("While id 0 %d\n", (*p_cur_token)->id);
					list_push(s_statement->expressions,
								parser_expression(p_cur_node, p_cur_token));
					printf("While id 1 %d\n", (*p_cur_token)->id);
				}

				if (parser_next(p_cur_node, p_cur_token)) /* { */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				while ((*p_cur_token)->id != TOK_SEP_CBE)
				{
					printf("yayx\n");
					struct Statement *s_new_statement
							= parser_statement(p_cur_node, p_cur_token);
					if (s_new_statement)
						list_push(s_statement->statements, s_new_statement);

				}

				if (s_statement->statements->size == 0)
					warning_printd(WARNING_PARSER_EMPTY_STATEMENT, &s_statement->token->line);

				if (parser_next(p_cur_node, p_cur_token)) /* } */
					break;
				//error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);

				break;
			#if 0
			case TOK_KEY_ELSE:
				s_statement->statements = list_new();

				if (parser_next(p_cur_node, p_cur_token)) /* else */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				if (parser_next(p_cur_node, p_cur_token)) /* { */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				while (cur_token->id != TOK_SEP_CBE)
				{
					struct Statement *s_new_statement
							= parser_statement(p_cur_node, p_cur_token, cur_node, cur_token);

					if (s_new_statement)
						list_push(s_statement->statements, s_new_statement);
				}

				if (s_statement->statements->size == 0)
					warning_printd(WARNING_PARSER_EMPTY_STATEMENT, &s_statement->token->line);

				if (parser_next(p_cur_node, p_cur_token)) /* } */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);

				break;

			case TOK_KEY_VAR:
				if (parser_next(p_cur_node, p_cur_token)) /* var */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);

				s_statement->name = cur_token;

				if (parser_next(p_cur_node, p_cur_token)) /* identifier (name) */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);

				if (cur_token->id == TOK_OP_EQUAL)
				{
					if (parser_next(p_cur_node, p_cur_token)) /* = */
						error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START,
									&s_statement->token->line);

					s_statement->expression
							= parser_expression(p_cur_node, p_cur_token, cur_node, cur_token);
				}
				else
				{
					s_statement->expression = parser_expression_null();
				}

				break;

			case TOK_KEY_BREAK:
				if (parser_next(p_cur_node, p_cur_token)) /* break */
					error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);
				break;

			case TOK_KEY_RETURN:
				break;
			#endif
		}
	}
	return s_statement;
}

static struct Expression *parser_expression(struct ListNode **p_cur_node, struct TokenNode **p_cur_token)
{
	printf("expression\n");

	/*remove all , before parsing the statement*/
	unsigned long int temp_line = (*p_cur_token)->line;
	while ((*p_cur_token)->id == TOK_SEP_COMMA)
	{
		if (parser_next(p_cur_node, p_cur_token)) /* , */
			error_printd(ERROR_PARSER_INVALID_EXPRESSION, &temp_line);
	}

	if ((*p_cur_token)->id == TOK_SEP_CBS)
		return NULL;

	/*parse the expression recursively*/
	struct Expression *s_expression = malloc(sizeof(struct Expression));
	//s_expression->token = *p_cur_token;

	parser_next(p_cur_node, p_cur_token);

	return s_expression;
}

static struct Expression *parser_expression_null()
{

}
