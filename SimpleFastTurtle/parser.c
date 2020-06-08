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
static int parser_is_precedent(const unsigned char op1, const unsigned char op2);
static struct Expression *parser_expression_null();
static void parser_remove_useless_round_brackets(struct TokenNode **expression_arr,
												int *start,
												int *stop);
static struct Expression *parser_nested_expression(struct TokenNode **expression_arr,
													int start,
													int stop);

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
    	token_tree_fprintf(stdout, s_tree_token);
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

	/*remove all ',' before parsing the statement*/
	unsigned long int temp_line = (*p_cur_token)->line;
	while ((*p_cur_token)->id == TOK_SEP_COMMA)
	{
		if (parser_next(p_cur_node, p_cur_token)) /* , */
			error_printd(ERROR_PARSER_INVALID_EXPRESSION, &temp_line);
	}

	/* Count Expression size and check if there is as much opening and closing round brackets */
	int bracket_stack = 0;
	unsigned int expression_size = 0;
	struct ListNode *copy_node = *p_cur_node;

	while ((*p_cur_token)->id != TOK_SEP_CBS
		&& (*p_cur_token)->id != TOK_SEP_SEMI
		&& (*p_cur_token)->id != TOK_SEP_COMMA)
	{
		expression_size++;

		if ((*p_cur_token)->id == TOK_SEP_RBS)
			bracket_stack++;
		else if ((*p_cur_token)->id == TOK_SEP_RBE)
			bracket_stack--;

		if (bracket_stack < 0 || parser_next(p_cur_node, p_cur_token))
			break;
	}

	if (!bracket_stack)
		; //error invalid expression

	/* Copy the expression to an array */
	struct TokenNode **expression_arr = malloc(sizeof(struct TokenNode*)*expression_size);

	for (int i = 0; i < expression_size; i++)
	{
		expression_arr[i] = (struct TokenNode*)copy_node->data;
		copy_node = copy_node->next;
	}

	struct Expression *s_expression
			= parser_nested_expression(expression_arr, 0, expression_size-1);

	free(expression_arr);
	printf("end expression\n");
	return s_expression;
}

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
static struct Expression *parser_nested_expression(struct TokenNode **expression_arr,
													int start,
													int stop)
{
	printf("nested\n");
	struct Expression *s_expression = malloc(sizeof(struct Expression));

	parser_remove_useless_round_brackets(expression_arr, &start, &stop);

	/* Expression parsing */
	short lowest_precedence = 100;
	int lowest_precedence_index = 0;
	int bracket_stack = 0;
	for (int i = start; i <= stop; i++)
	{	
		if (expression_arr[i]->id == TOK_SEP_RBS)
		{
			bracket_stack++;
		}
		else if (expression_arr[i]->id == TOK_SEP_RBE)
		{
			bracket_stack--;
		}
		else if (!bracket_stack)
		{
			if (expression_arr[i]->type == TOK_TYPE_OP)
			{
				short cur_precedence = OPERATOR_PRECEDENCE[expression_arr[i]->id];
				printf("prec %d\n", cur_precedence);
				if (cur_precedence < lowest_precedence)
				{
					lowest_precedence = cur_precedence;
					lowest_precedence_index = i;
				}
			}
		}
	}

	if (lowest_precedence < 100)
	{
		s_expression->operator = malloc(sizeof(struct Operator));
		s_expression->type = EXPRESSION_TYPE_OP;
		s_expression->operator->token = expression_arr[lowest_precedence_index];
		s_expression->operator->left
				= parser_nested_expression(expression_arr, start, lowest_precedence_index-1);
		s_expression->operator->right
				= parser_nested_expression(expression_arr, lowest_precedence_index+1, stop);
	}
	else if (start == stop) /* only one token in the array */
	{
		if (expression_arr[start]->type == TOK_TYPE_LI)
		{
			s_expression->type = EXPRESSION_TYPE_LI;
			s_expression->literal = expression_arr[start];
		}
		else if (expression_arr[start]->type == TOK_TYPE_ID)
		{
			s_expression->type = EXPRESSION_TYPE_ID;
			s_expression->identifier = expression_arr[start];
		}
	}

	return s_expression;
}

static void parser_remove_useless_round_brackets(struct TokenNode *expression[],
												int *start,
												int *stop)
{
	int lowest_inside = 0;
	int bracket_stack = 0;
	int min = *start;
	int max = *stop;
	while(expression[min]->id == TOK_SEP_RBS && expression[max]->id == TOK_SEP_RBE)
	{
		lowest_inside++;
		bracket_stack++;
		min++;
		max--;
	}

	while(min <= max)
	{
		if (!lowest_inside)
			return;

		if (expression[min]->id == TOK_SEP_RBS)
			bracket_stack++;
		else if (expression[min]->id == TOK_SEP_RBE)
			bracket_stack--;

		if (bracket_stack < lowest_inside)
			lowest_inside--;

		min++;
	}

	*start += lowest_inside;
	*stop -= lowest_inside;

	return;
}

static int parser_is_precedent(const unsigned char op1, const unsigned char op2)
{
	return OPERATOR_PRECEDENCE[op1] >= OPERATOR_PRECEDENCE[op2];
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
	[TOK_OP_ADD]		= 14,
	[TOK_OP_SUB]		= 14,
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
