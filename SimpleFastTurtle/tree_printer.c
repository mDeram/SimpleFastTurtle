#include "tree_printer.h"

/*
 *  Display the AST
 *  ┌ ┘ └ ┐ ─ | ├
 */

static void ast_ident_add(char to_add[]);
static void ast_ident_grow();
static void ast_ident_recover(size_t index);
static void ast_ident_list_end(const char e_pos);
static void ast_prefix_list_end(FILE *output, const char e_pos);
static void ast_ident_apply(FILE *output);



static struct AstIdent s_ident;



void token_tree_fprintf(FILE *output, struct List *s_tree_token)
{
    s_ident.index = 0;
    s_ident.size = 1280;
    s_ident.ident = malloc(sizeof(char)*s_ident.size);
    s_ident.ident[s_ident.index] = '\0';

    if (s_tree_token->size)
        list_fprintf_pos(s_tree_token, output, token_statement_fprintf);

    free(s_ident.ident);
}

void token_statement_fprintf(FILE *output, void *data, char e_pos)
{
    struct Statement *s_statement = (struct Statement*)data;
    size_t index = s_ident.index;
    size_t statement_name_len = 0;

    ast_ident_apply(output);
    ast_prefix_list_end(output, e_pos);

    /*Token*/
    if (s_statement->token == NULL)
    {
        fprintf(output, "%s", "expr");
        statement_name_len = strlen("expr");
    }
    else
    {
        fprintf(output, "%s", s_statement->token->token);
        statement_name_len = strlen(s_statement->token->token);
    }

    /*Expressions*/
    ast_ident_list_end(e_pos);
    size_t big_index = s_ident.index;

    if (s_statement->statements != NULL && s_statement->statements->size)
        ast_ident_add("|");
    else
        ast_ident_add(" ");

    for (int i = 1; i < statement_name_len; i++)
        ast_ident_add(" ");

    if (s_statement->expressions != NULL && s_statement->expressions->size)
    {
        fprintf(output, "┐\n");
        list_fprintf_pos(s_statement->expressions, output,
                         token_expression_fprintf);
    }
    else
    {
        fprintf(output, "\n");
    }

    ast_ident_recover(big_index);
    
    /*Statements*/
    if (s_statement->statements != NULL && s_statement->statements->size)
    {
        list_fprintf_pos(s_statement->statements, output,
                         token_statement_fprintf);
    }

    ast_ident_recover(index);
}

void token_expression_fprintf(FILE *output, void *data, char e_pos)
{
    struct Expression *s_expression = (struct Expression*)data;
    size_t index = s_ident.index;

    ast_ident_apply(output);
    ast_prefix_list_end(output, e_pos);
    ast_ident_list_end(e_pos);

    token_expression_tree_fprintf(output, s_expression);

    ast_ident_recover(index);
}

void token_expression_tree_fprintf(FILE *output, void *data)
{
    struct Expression *s_expression = (struct Expression*)data;
    size_t index = s_ident.index;

    switch(s_expression->type)
    {
    case EXPRESSION_TYPE_ID:
        fprintf(output, "%s\n", s_expression->identifier->token);
        break;

    case EXPRESSION_TYPE_LI:
        fprintf(output, "%s\n", s_expression->literal->token);
        break;

    case EXPRESSION_TYPE_OP:
        fprintf(output, "%s\n", s_expression->operator->token->token);

        /*Left*/
        ast_ident_apply(output);
        fprintf(output, "├─L ");

        ast_ident_add("| ");
        token_expression_tree_fprintf(output, s_expression->operator->left);
        ast_ident_recover(index);

        /*Right*/
        ast_ident_apply(output);
        fprintf(output, "└─R ");

        ast_ident_add("  ");
        token_expression_tree_fprintf(output, s_expression->operator->right);
        ast_ident_recover(index);
        break;

    case EXPRESSION_TYPE_FN:
        fprintf(output, "%s(\n", s_expression->function->identifier->token);

        ast_ident_apply(output);
        fprintf(output, " └┐\n");

        ast_ident_add("  ");

        list_fprintf_pos(s_expression->function->params, output,
                         token_expression_fprintf);

        ast_ident_apply(output);
        fprintf(output, ")\n");

        ast_ident_recover(index);
        break;

    case EXPRESSION_TYPE_ARRAY:
        fprintf(output, "%s[\n", s_expression->array->identifier->token);

        ast_ident_add("  ");
        ast_ident_apply(output);

        token_expression_tree_fprintf(output, s_expression->array->param);

        ast_ident_apply(output);
        fprintf(output, "]\n");

        ast_ident_recover(index);
        break;

    default:
        fprintf(output, "Invalid\n");
    }
}

static void ast_ident_add(char to_add[])
{
    size_t len = strlen(to_add);

    while (s_ident.index + len >= s_ident.size-1)
        ast_ident_grow();

    strcat(s_ident.ident, to_add);
    s_ident.index += len;
}

static void ast_ident_grow()
{
    s_ident.size *= 2;
    s_ident.ident = realloc(s_ident.ident, sizeof(s_ident.size));
    if (s_ident.ident == NULL) exit(EXIT_FAILURE);
}

static void ast_ident_recover(size_t index)
{
    s_ident.index = index;
    s_ident.ident[index] = '\0';
}

static void ast_ident_list_end(const char e_pos)
{
    if (e_pos == LIST_END || e_pos == LIST_ALL)
        ast_ident_add("  ");
    else
        ast_ident_add("| ");
}

static void ast_prefix_list_end(FILE *output, const char e_pos)
{
    if (e_pos == LIST_END || e_pos == LIST_ALL)
        fprintf(output, "└─");
    else
        fprintf(output, "├─");
}

static void ast_ident_apply(FILE *output)
{
    fprintf(output, "%s", s_ident.ident);
}
