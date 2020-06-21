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

/* ┌ ┘ └ ┐ ─ | ├ */
void token_tree_fprintf(FILE *output, struct List *s_tree_token)
{
    char ident[1000] = " ";
    fprintf(output, "\n");

    void i_apply_ident(FILE *output, void *data, char e_pos)
    {
        token_statement_fprintf(output, data, ident, e_pos);
    }

    if (s_tree_token->size)
        list_fprintf_pos(s_tree_token, output, i_apply_ident);

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
    if (s_statement->token == NULL)
        fprintf(output, "%s", "expr");
    else
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

    if (s_statement->expressions != NULL && s_statement->expressions->size)
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

    token_expression_tree_fprintf(output, s_expression, ident);

    ident[i] = '\0';
}

void token_expression_tree_fprintf(FILE *output, void *data, char ident[])
{
    struct Expression *s_expression = (struct Expression*)data;
    if (s_expression->type == EXPRESSION_TYPE_ID)
    {
        fprintf(output, "%s\n", s_expression->identifier->token);
    }
    else if (s_expression->type == EXPRESSION_TYPE_LI)
    {
        fprintf(output, "%s\n", s_expression->literal->token);
    }
    else if (s_expression->type == EXPRESSION_TYPE_OP)
    {
        fprintf(output, "%s\n", s_expression->operator->token->token);

        int i = 0;
        while(ident[i] != '\0')
        {
            i++;
        }

        /* Print expressions recursively */

        fprintf(output, "%s├─L ", ident);
        /* We have to show that L is linked to R but R is then linked to nothing next */
        ident[i] = '|';
        ident[i+1] = ' ';
        ident[i+2] = '\0';
        token_expression_tree_fprintf(output, s_expression->operator->left, ident);
        ident[i] = '\0';
        fprintf(output, "%s└─R ", ident);
        ident[i] = ' ';
        token_expression_tree_fprintf(output, s_expression->operator->right, ident);

        /* Reset ident */
        ident[i] = '\0';
    }
    else if (s_expression->type == EXPRESSION_TYPE_FN)
    {
        fprintf(output, "%s\n", s_expression->function->identifier->token);

        int i = 0;
        while(ident[i] != '\0')
        {
            i++;
        }

        /* Print params as expressions */

        fprintf(output, "%s└", ident);
        fprintf(output, "┐");
        fprintf(output, "\n");
        ident[i] = ' ';
        ident[i+1] = '\0';
        void i_apply_ident(FILE *output, void *data, char e_pos)
        {
            token_expression_fprintf(output, data, ident, e_pos);
        }
        list_fprintf_pos(s_expression->function->params, output, i_apply_ident);
        /* Reset ident */
        ident[i] = '\0';
    }
    else if (s_expression->type == EXPRESSION_TYPE_ARRAY)
    {
        fprintf(output, "%s[\n", s_expression->array->identifier->token);

        int i = 0;
        while(ident[i] != '\0')
        {
            i++;
        }

        /* Print params as expressions */

        ident[i] = ' ';
        ident[i+1] = ' ';
        ident[i+2] = '\0';
        fprintf(output, "%s", ident);
        token_expression_tree_fprintf(output, s_expression->array->param, ident);
        ident[i] = '\0';
        fprintf(output, "%s]\n", ident);
    }
    else
    {
        fprintf(output, "Invalid\n");
    }
}
