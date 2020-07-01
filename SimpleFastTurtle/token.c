#include "token.h"

/*
 *  Create, Free, Display functions for "struct TokenNode"
 */



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
    if (node == NULL) exit(EXIT_FAILURE);

    node->line = current_line;
    node->type = type;
    node->id = id;

    size_t len = strlen(token)+1;
    if (type == TOK_TYPE_ID && len > 32)
        len = 32;
    node->token = malloc(sizeof(char)*(len));
    if (node->token == NULL) exit(EXIT_FAILURE);
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



struct Statement *statement_new()
{
    struct Statement *s_statement = malloc(sizeof(struct Statement));
    if (s_statement == NULL) exit(EXIT_FAILURE);

    s_statement->token = NULL;
    s_statement->expressions = list_new();
    s_statement->statements = list_new();

    return s_statement;
}

void statement_free(void *data)
{
    struct Statement *s_statement = (struct Statement *)data;
    list_free_foreach(s_statement->expressions, expression_free);
    list_free_foreach(s_statement->statements, statement_free);
    free(s_statement);
}

struct Expression *expression_new()
{
    struct Expression *s_expression = malloc(sizeof(struct Expression));
    if (s_expression == NULL) exit(EXIT_FAILURE);

    return s_expression;
}

void expression_free(void *data)
{
    struct Expression *s_expression = (struct Expression *)data;
    if (s_expression->type == EXPRESSION_TYPE_OP)
        operator_free(s_expression->operator);
    else if (s_expression->type == EXPRESSION_TYPE_FN)
        function_free(s_expression->function);
    free(s_expression);
}

struct Operator *operator_new()
{
    struct Operator *s_operator = malloc(sizeof(struct Operator));
    if (s_operator == NULL) exit(EXIT_FAILURE);

    s_operator->token = NULL;
    s_operator->left = NULL;
    s_operator->right = NULL;

    return s_operator;
}

void operator_free(void *data)
{
    struct Operator *s_operator = (struct Operator *)data;
    expression_free(s_operator->left);
    expression_free(s_operator->right);
    free(s_operator);
}

struct Function *function_new()
{
    struct Function *s_function = malloc(sizeof(struct Function));
    if (s_function == NULL) exit(EXIT_FAILURE);

    s_function->identifier = NULL;
    s_function->params = list_new();

    return s_function;
}

void function_free(void *data)
{
    struct Function *s_function = (struct Function *)data;
    list_free_foreach(s_function->params, expression_free);
    free(s_function);
}

struct Array *array_new()
{
    struct Array *s_array = malloc(sizeof(struct Array));
    if (s_array == NULL) exit(EXIT_FAILURE);

    s_array->identifier = NULL;
    s_array->param = NULL;

    return s_array;
}

void array_free(void *data)
{
    struct Array *s_array = (struct Array *)data;
    free(s_array);
}
