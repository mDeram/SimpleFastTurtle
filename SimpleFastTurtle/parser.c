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
					break;
				//error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &s_statement->token->line);

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
					list_push(s_statement->expressions,
								parser_expression(p_cur_node, p_cur_token));
				}

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

	if ((*p_cur_token)->id == TOK_SEP_CBS || (*p_cur_token)->id == TOK_SEP_SEMI)
		return NULL;

	/*parse the expression recursively*/
	struct Expression *s_expression = malloc(sizeof(struct Expression));
	//s_expression->token = *p_cur_token;

	/* 
	 * Find a litteral or identifier
	 * Then check the operator/separator at his left, and at his right
	 * If there is only one OP then apply the op to him (which mean that the op nest him either at
	 * his left of his right)
	 * If there are 2 OP then check for the precedence between them, then nest the operation that
	 * have a highest precedence with the LI/ID to the left/right of the OP with the least precedence
	 *
	 * Separators are being check more "manually" in the code because they come together
	 * for the brackets and the behavior is different than operator
	 */
	int round_bracket_stack = 0;
	while ((*p_cur_token)->id != TOK_SEP_CBS
		&& (*p_cur_token)->id != TOK_SEP_SEMI
		&& (*p_cur_token)->id != TOK_SEP_COMMA)
	{
		if ((*p_cur_token)->id == TOK_SEP_RBS)
			round_bracket_stack++;
		else if ((*p_cur_token)->id == TOK_SEP_RBE)
		{
			round_bracket_stack--;
			if (round_bracket_stack < 0)
				;//error
		}
		else if (!round_bracket_stack)
		{
			//parse expression
		}
		
		if (parser_next(p_cur_node, p_cur_token))
			;//error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_START, &s_statement->token->line);
	}

	printf("end expression\n");
	return s_expression;
}

static struct Expression *parser_expression_null()
{

}



/*
 * From :
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
 */
short OPERATOR_PRECEDENCE[127] = {
/* Single operators */
	[TOK_OP_ASIGN]		= 3,
	[TOK_OP_NOT]		= 17,
	[TOK_OP_INF]		= 12,
	[TOK_OP_SUP]		= 12,
	[TOK_OP_OR]			= 8,
	[TOK_OP_AND]		= 10,
	[TOK_OP_XOR]		= 9,
	[TOK_OP_ADD]		= 17,
	[TOK_OP_SUB]		= 17,
	[TOK_OP_BY]			= 15,
	[TOK_OP_DIV]		= 15,
	[TOK_OP_MOD]		= 15,

/* Double operators */
	[TOK_OP_INCR]		= 17,
	[TOK_OP_DECR]		= 17,
	[TOK_OP_EXPO]		= 16,
	[TOK_OP_SQRT]		= 16,
	[TOK_OP_EQUAL]		= 11,
	[TOK_OP_NOT_EQUAL]	= 11,
	[TOK_OP_INF_EQUAL]	= 12,
	[TOK_OP_SUP_EQUAL]	= 12,
	[TOK_OP_LOGIC_AND]	= 6,
	[TOK_OP_LOGIC_OR]	= 5,
	[TOK_OP_ADD_ASIGN]	= 3,
	[TOK_OP_SUB_ASIGN]	= 3,
	[TOK_OP_BY_ASIGN]	= 3,
	[TOK_OP_DIV_ASIGN]	= 3,
	[TOK_OP_MOD_ASIGN]	= 3,

/* Separator */
	[TOK_SEP_RBS]		= 21,
	[TOK_SEP_RBE]		= 21,
	[TOK_SEP_SBS]		= 20,
	[TOK_SEP_SBE]		= 20,
	[TOK_SEP_DOT]		= 20,
};

short OPERATOR_ASSOCIATIVITY[127] = {
/* Single operators */
	[TOK_OP_ASIGN]		= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_NOT]		= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_INF]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_SUP]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_OR]			= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_AND]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_XOR]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_ADD]		= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_SUB]		= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_BY]			= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_DIV]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_MOD]		= ASSOCIATIVITY_LEFT_TO_RIGHT,

/* Double operators */
	[TOK_OP_INCR]		= ASSOCIATIVITY_NA,
	[TOK_OP_DECR]		= ASSOCIATIVITY_NA,
	[TOK_OP_EXPO]		= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_SQRT]		= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_EQUAL]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_NOT_EQUAL]	= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_INF_EQUAL]	= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_SUP_EQUAL]	= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_LOGIC_AND]	= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_LOGIC_OR]	= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_OP_ADD_ASIGN]	= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_SUB_ASIGN]	= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_BY_ASIGN]	= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_DIV_ASIGN]	= ASSOCIATIVITY_RIGHT_TO_LEFT,
	[TOK_OP_MOD_ASIGN]	= ASSOCIATIVITY_RIGHT_TO_LEFT,

/* Separator */
	[TOK_SEP_RBS]		= ASSOCIATIVITY_NA,
	[TOK_SEP_RBE]		= ASSOCIATIVITY_NA,
	[TOK_SEP_SBS]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_SEP_SBE]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
	[TOK_SEP_DOT]		= ASSOCIATIVITY_LEFT_TO_RIGHT,
};
