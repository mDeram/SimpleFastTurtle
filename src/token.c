#include "token.h"

/*
 *  Create, Free, Display functions for "TokenNode"
 */



void token_fprintf(FILE *output, void *data)
{
    TokenNode *node = (TokenNode *)data;
    fprintf(output, "[Line: %lu\t\tType: %d\t\tId: %d\t\tSymbol: %s\t\t]\n",
                      node->line,  node->type, node->id, node->token);
}

TokenNode *token_create(const ulong current_line,
                        const uint8 type,
                        const uint8 id,
                        const char *token)
{
    TokenNode *node = malloc(sizeof(TokenNode));
    if (node == NULL) exit(EXIT_FAILURE);

    node->line = current_line;
    node->type = type;
    node->id = id;

    /* copy the string and cut it if needed */
    size_t len = strlen(token)+1;
    if (type == TOK_TYPE_ID && len > 32)
        len = 32;
    node->token = malloc(sizeof(char)*len);
    if (node->token == NULL) exit(EXIT_FAILURE);
    strncpy(node->token, token, len-1);
    node->token[len-1] = '\0';

    return node;
}

void token_free(void *data)
{
    TokenNode *node = (TokenNode *)data;
    free(node->token);
    free(node);
}



Statement *statement_new()
{
    Statement *s_statement = malloc(sizeof(Statement));
    if (s_statement == NULL) exit(EXIT_FAILURE);

    s_statement->token = NULL;
    s_statement->expressions = list_new();
    s_statement->statements = list_new();

    return s_statement;
}

void statement_free(void *data)
{
    Statement *s_statement = (Statement *)data;
    list_free_foreach(s_statement->expressions, expression_free);
    list_free_foreach(s_statement->statements, statement_free);
    free(s_statement);
}

Expression *expression_new()
{
    Expression *s_expression = malloc(sizeof(Expression));
    if (s_expression == NULL) exit(EXIT_FAILURE);

    return s_expression;
}

void expression_free(void *data)
{
    Expression *s_expression = (Expression *)data;
    if (s_expression->type == EXPR_TYPE_OP)
        operator_free(s_expression->operator);
    else if (s_expression->type == EXPR_TYPE_FN)
        function_free(s_expression->function);
    free(s_expression);
}

Operator *operator_new()
{
    Operator *s_operator = malloc(sizeof(Operator));
    if (s_operator == NULL) exit(EXIT_FAILURE);

    s_operator->token = NULL;
    s_operator->left = NULL;
    s_operator->right = NULL;

    return s_operator;
}

void operator_free(void *data)
{
    Operator *s_operator = (Operator *)data;
    expression_free(s_operator->left);
    expression_free(s_operator->right);
    free(s_operator);
}

Function *function_new()
{
    Function *s_function = malloc(sizeof(Function));
    if (s_function == NULL) exit(EXIT_FAILURE);

    s_function->identifier = NULL;
    s_function->params = list_new();

    return s_function;
}

void function_free(void *data)
{
    Function *s_function = (Function *)data;
    list_free_foreach(s_function->params, expression_free);
    free(s_function);
}

ExprArray *expr_array_new()
{
    ExprArray *s_expr_array = malloc(sizeof(ExprArray));
    if (s_expr_array == NULL) exit(EXIT_FAILURE);

    s_expr_array->identifier = NULL;
    s_expr_array->param = NULL;

    return s_expr_array;
}

void expr_array_free(void *data)
{
    ExprArray *s_expr_array = (ExprArray *)data;
    free(s_expr_array);
}
