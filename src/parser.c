#include "parser.h"

/*
 *  Create the Abstract Syntax Tree
 *  Take a list of token
 *  Return a tree of token
 */

static const uint8 OPERATOR_PRECEDENCE[127];
static TokenNode *token_null;



typedef struct {
    ListNode *node;
    TokenNode *token;
} ParserNode;

typedef struct {
    uint start;
    uint stop;
} Boundary;



static void save_in_file(List *tree_token, List *tokens);
static void parse(List *tree_token, List *tokens);
static int next_node(ParserNode *current);
static void remove_all_token(ParserNode *current, char to_remove);
static void parse_expression_block(ParserNode *current, List *s_list);
static void parse_statement_block(ParserNode *current, Statement *statement);
static void statement_inline(ParserNode *current, Statement *statement);
static void statement_block(ParserNode *current, Statement *statement);
static void statement_start(ParserNode *current, Statement *statement);
static void statement_start_with_expression(ParserNode *current,
                                            Statement *statement);
static void parse_statement_keyword(ParserNode *current, Statement *statement);
static Statement *parse_statement(ParserNode *current);
static bool expression_is_token_valid(TokenNode *token);
static bool operator_has_single_operande(TokId id);
static bool expression_can_token_follow(TokenNode *last2,
                                        TokenNode *last1,
                                        TokenNode *cur);
static TokId bracket_end_to_start(TokId bracket);
static TokenNode **expression_to_expr_array(ListNode *copy_node,
                                            uint start, uint stop);
static void find_lowest_precedence(TokenNode **expr_block, uint start, uint stop,
                                   uint8 *lowest_precedence, uint *index);
static Expression *parse_nested_expression(TokenNode **expr_block,
                                           uint start, uint stop);
static void parse_function_params(List *params, TokenNode **expr_block,
                                  int start, int stop);
static bool is_function(TokenNode **expr_block, int start, int stop);
static bool is_expr_array(TokenNode **expr_block, int start, int stop);
static void remove_useless_rb(TokenNode **expr_block, uint *start, uint *stop);
static TokenNode *get_token_null();
static Boundary *boundary_create(uint start, uint stop);



void parser_process(List *tree_token, List *tokens,
                    int option_save, int option_print)
{
    //File line start at 1 so using 0 show that it does not come from the file
    token_null = token_create(0, TOK_TYPE_KEY, TOK_KEY_NULL, "null");

    parse(tree_token, tokens);

    /*OPTIONS*/
    if (option_save)
        save_in_file(tree_token, tokens);

    if (option_print)
        token_tree_fprintf(stdout, tree_token);
}

void parser_free(List *tree_token)
{
    list_free_foreach(tree_token, statement_free);
    token_free(token_null);
}

static void save_in_file(List *tree_token, List *tokens)
{
    FILE *output = fopen("output.p", "w+");
    if (output == NULL)
        error_print(ERROR_PARSER_FILE_OUTPUT_FAILURE);

    token_tree_fprintf(output, tree_token);

    fclose(output);
}

static void parse(List *tree_token, List *tokens)
{
    if (!tokens->size)
        return;

    ParserNode current = {NULL, NULL};
    current.node = tokens->head;
    current.token = (TokenNode *)tokens->head->data;
    printf("TOK: %s\n", current.token->token);

    ListNode *last_node = NULL;

    while (current.node != NULL)
    {
        if (last_node == current.node)
        {
            error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END,
                         &current.token->line);
        }

        last_node = current.node;
        Statement *new_statement = parse_statement(&current);

        if (new_statement != NULL)
            list_push(tree_token, new_statement);
    }
}

static int next_node(ParserNode *current)
{
    current->node = current->node->next;
    if (current->node == NULL)
        return 0;

    current->token = (TokenNode *)current->node->data;
    printf("TOK: %s\n", current->token->token);
    return 1;
}

static void remove_all_token(ParserNode *current, char to_remove)
{
    if (current->node == NULL)
        return;

    while ((current->token->id == to_remove) && next_node(current))
        ;
}

static void parse_statement_block(ParserNode *current, Statement *statement)
{
    if (current->node == NULL)
        return;

    ListNode *last_node = current->node;

    if (current->token->id != TOK_SEP_CBS)
        statement_inline(current, statement);
    else
        statement_block(current, statement);

    if (last_node == current->node)
        error_printd(ERROR_PARSER_INVALID_STATEMENT, &current->token->line);
}

static void statement_inline(ParserNode *current, Statement *statement)
{
    if (current->token->id == TOK_SEP_SEMI)
    {
        next_node(current);
    }
    else
    {
        Statement *new_statement = parse_statement(current);
        if (new_statement != NULL)
            list_push(statement->statements, new_statement);
    }
}

static void statement_block(ParserNode *current, Statement *statement)
{
    TokenNode *last_token = current->token;

    if (!next_node(current)) /* { */
    {
        error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK,
                     &statement->token->line);
    }



    while (current->token->id != TOK_SEP_CBE)
    {
        Statement *new_statement = parse_statement(current);
        if (current->node == NULL)
        {
            ulong line;
            if (statement->token != NULL)
                line = statement->token->line;
            else
                line = last_token->line;

            error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END, &line);
        }

        if (new_statement != NULL)
            list_push(statement->statements, new_statement);
    }

    next_node(current); /* } */
}

static void statement_start(ParserNode *current, Statement *statement)
{
    statement->token = current->token;
    if (!next_node(current))
    {
        error_printd(ERROR_PARSER_INVALID_STATEMENT_START,
                     &statement->token->line);
    }
}

static void statement_start_with_expression(ParserNode *current,
                                            Statement *statement)
{
    statement_start(current, statement);
    parse_expression_block(current, statement->expressions);
}

static void parse_statement_keyword(ParserNode *current, Statement *statement)
{
    switch (current->token->id)
    {
    case TOK_KEY_FOR:
    case TOK_KEY_WHILE:
    case TOK_KEY_IF:
    case TOK_KEY_ELIF:
        statement_start_with_expression(current, statement);
        if (!statement->expressions->size)
        {
            error_printd(ERROR_PARSER_INVALID_NUMBER_PARAMETERS,
                         &statement->token->line);
        }

        parse_statement_block(current, statement);
        break;

    case TOK_KEY_FN:
        statement_start(current, statement);
        if (current->token->type != TOK_TYPE_ID)
        {
            error_printd(ERROR_PARSER_INVALID_STATEMENT_START,
                         &statement->token->line);
        }
        statement_start_with_expression(current, statement);

        parse_statement_block(current, statement);
        break;

    case TOK_KEY_ELSE:
        statement_start(current, statement);
        parse_statement_block(current, statement);
        break;

    case TOK_KEY_VAR:
        statement_start_with_expression(current, statement);

        if (!statement->expressions->size)
        {
            error_printd(ERROR_PARSER_INVALID_VAR_ASSIGNMENT,
                         &statement->token->line);
        }
        break;

    case TOK_KEY_BREAK:
        statement_start(current, statement);
        break;

    case TOK_KEY_RETURN:
        statement_start_with_expression(current, statement);
        break;

    default:
        error_printd(ERROR_PARSER_NOT_HANDLED_KEYWORD, current->token);
    }
}

static Statement *parse_statement(ParserNode *current)
{
    remove_all_token(current, TOK_SEP_SEMI);
    if (current->node == NULL || current->token->id == TOK_SEP_CBE)
        return NULL;

    Statement *statement = statement_new();

    if      (current->token->type == TOK_TYPE_KEY)
        parse_statement_keyword(current, statement);
    else if (current->token->id == TOK_SEP_CBS)
        parse_statement_block(current, statement);
    else if (expression_is_token_valid(current->token))
        parse_expression_block(current, statement->expressions);
    else
        error_printd(ERROR_PARSER_INVALID_STATEMENT, &current->token->line);

    return statement;
}

static bool expression_is_token_valid(TokenNode *token)
{
    return (token->type == TOK_TYPE_ID
         || token->type == TOK_TYPE_LI
         || token->type == TOK_TYPE_OP
         || token->id   == TOK_SEP_COMMA
         || token->id   == TOK_SEP_RBS
         || token->id   == TOK_SEP_RBE
         || token->id   == TOK_SEP_SBS
         || token->id   == TOK_SEP_SBE
         || token->id   == TOK_SEP_DOT
         || token->id   == TOK_KEY_NULL);
}

static bool operator_has_single_operande(TokId id)
{
    return (id == TOK_OP_INCR
         || id == TOK_OP_DECR
         || id == TOK_OP_NOT
         || id == TOK_OP_OR
         || id == TOK_OP_AND
         || id == TOK_OP_XOR);
}

static bool expression_can_token_follow(TokenNode *last2,
                                        TokenNode *last1,
                                        TokenNode *cur)
{
    if (last1 == NULL)
        return TRUE;

    /* are last1 and cur both identifier or literal or keyword*/
    if ((last1->type == TOK_TYPE_ID || last1->type == TOK_TYPE_LI || last1->type == TOK_TYPE_KEY)
     &&   (cur->type == TOK_TYPE_ID ||   cur->type == TOK_TYPE_LI ||   cur->type == TOK_TYPE_KEY))
        return FALSE;

    if (last2 == NULL)
        return TRUE;

    if (operator_has_single_operande(last1->id))
        return !(
                    (cur->type == TOK_TYPE_ID
                  || cur->type == TOK_TYPE_LI
                  || cur->type == TOK_TYPE_KEY)
                &&
                    (last2->type == TOK_TYPE_ID
                  || last2->type == TOK_TYPE_LI
                  || last2->type == TOK_TYPE_KEY)
                );

    return TRUE;
}

static TokId bracket_end_to_start(TokId bracket)
{
    switch (bracket)
    {
    case TOK_SEP_RBE:
        return TOK_SEP_RBS;
    case TOK_SEP_SBE:
        return TOK_SEP_SBS;
    default:
        return TOK_NULL;
    }
}

static List *expression_to_boundary_list(ParserNode *current)
{
    uint size = 0;
    List *boundaries = list_new();
    bool is_inside_rb = (current->token->id == TOK_SEP_RBS);
    
    size_t stack_index = 0;
    size_t stack_size  = 16; /*If 16 is too small, stack_size *= 2*/
    uchar *stack       = malloc(sizeof(uchar)*stack_size);
    if (stack == NULL) exit(EXIT_FAILURE);

    TokenNode *last2 = NULL;
    TokenNode *last1 = NULL;

    while (expression_is_token_valid(current->token)
        && expression_can_token_follow(last2, last1, current->token))
    {
        size++;
        if      (current->token->id == TOK_SEP_RBS
              || current->token->id == TOK_SEP_SBS)
        {
            stack[stack_index] = current->token->id;
            stack_index++;
        }
        else if (current->token->id == TOK_SEP_RBE
              || current->token->id == TOK_SEP_SBE)
        {
            stack_index--;
            if (stack_index < 0)
                return NULL;
            if (stack[stack_index] != bracket_end_to_start(current->token->id))
                return NULL;
        }
        else if (current->token->id == TOK_SEP_COMMA)
        {
            if (!stack_index)
            {
                /* Some expressions were inside round bracket but not anymore : 
                 * raising error
                 */
                if (is_inside_rb && boundaries->size)
                    return NULL;

                is_inside_rb = FALSE;

                Boundary *new_boundary = NULL;
                if (!boundaries->size)
                {
                    new_boundary = boundary_create(0, size - 1);
                }
                else
                {
                    uint stop = ((Boundary*)boundaries->tail->data)->stop;
                    new_boundary = boundary_create(stop + 2, size - 1);
                }
                list_push(boundaries, new_boundary);
            }
            else if (stack_index == 1 && is_inside_rb)
            {
                Boundary *new_boundary = NULL;
                if (!boundaries->size)
                {
                    new_boundary = boundary_create(1, size - 1 - is_inside_rb);
                }
                else
                {
                    uint stop = ((Boundary*)boundaries->tail->data)->stop;
                    new_boundary = boundary_create(stop + 2, size - 1 - is_inside_rb);
                }
                list_push(boundaries, new_boundary);
            }
            //else the comma is included in the expression
        }

        last2 = last1;
        last1 = current->token;
        if (!next_node(current))
            break;

        if (stack_index == stack_size-1)
        {
            stack_size *= 2;
            stack = realloc(stack, sizeof(uint)*stack_size);
            if (stack == NULL) exit(EXIT_FAILURE);
        }
    }

    if (stack_index)
        return NULL;

    Boundary *new_boundary = NULL;
    if (!boundaries->size)
    {
        new_boundary = boundary_create(0, size - 1);
    }
    else
    {
        uint stop = ((Boundary *)boundaries->tail->data)->stop;
        new_boundary = boundary_create(stop + 2, size - 1 - is_inside_rb);
    }
    list_push(boundaries, new_boundary);

    free(stack);
    return boundaries;
}

static TokenNode **expression_to_expr_array(ListNode *node, uint start, uint stop)
{
    TokenNode **expr_block = malloc(sizeof(TokenNode *)*(stop + 1));
    if (expr_block == NULL) exit(EXIT_FAILURE);

    for (int i = 0; i <= stop; i++)
    {
        if (i >= start)
        {
            expr_block[i] = (TokenNode *)node->data;
            printf("%s", ((TokenNode*)node->data)->token);
        }
        node = node->next;
    }
    printf("\n");
    return expr_block;
}

static void parse_expression_block(ParserNode *current, List *s_list)
{
    uint tmp_line = current->token->line;
    remove_all_token(current, TOK_SEP_COMMA);
    if (current->node == NULL)
        error_printd(ERROR_PARSER_INVALID_EXPRESSION, &tmp_line);

    ListNode *copy_node = current->node;

    List *boundaries = expression_to_boundary_list(current);
    if (boundaries == NULL || !boundaries->size)
        error_printd(ERROR_PARSER_INVALID_EXPRESSION, &tmp_line);

    uint start = ((Boundary *)boundaries->head->data)->start;
    uint stop  = ((Boundary *)boundaries->tail->data)->stop;
    TokenNode **expr_block = expression_to_expr_array(copy_node, start, stop);

    list_foreach_(boundaries, node)
    {
        Boundary *boundary = (Boundary*)node->data;
        Expression *expression = parse_nested_expression(expr_block,
                                                         boundary->start,
                                                         boundary->stop);
        //if (expression == NULL)
        list_push(s_list, expression);
    }

    //free boundaries
    free(expr_block);
}

static void find_lowest_precedence(TokenNode **expr_block, uint start, uint stop,
                                   uint8 *lowest_precedence, uint *index)
{
    uint bracket_stack = 0;
    for (uint i = start; i <= stop; i++)
    {
        if (expr_block[i]->id == TOK_SEP_RBS || expr_block[i]->id == TOK_SEP_SBS)
        {
            bracket_stack++;
        }
        else if (expr_block[i]->id == TOK_SEP_RBE || expr_block[i]->id == TOK_SEP_SBE)
        {
            bracket_stack--;
        }
        else if (!bracket_stack)
        {
            if (expr_block[i]->type == TOK_TYPE_OP)
            {
                int8 cur_precedence = OPERATOR_PRECEDENCE[expr_block[i]->id];
                if (cur_precedence < *lowest_precedence)
                {
                    *lowest_precedence = cur_precedence;
                    *index = i;
                }
            }
            else if (expr_block[i]->id == TOK_SEP_COMMA)
            {
                error_printd(ERROR_PARSER_INVALID_EXPRESSION, &(expr_block[i]->line));
            }
        }
    }
}

/*
 *  Remove all useless round brackets that are arround the nested expression
 *  Find the operator with the lowest precedence and that is not inside brackets
 *  If there is an operator create the Left and Right branch from that operator
 *  If only one element, it's an end node (ID or LI)
 *  Else it's a null node
 */
static Expression *parse_nested_expression(TokenNode **expr_block,
                                           uint start, uint stop)
{
    Expression *expression = expression_new();

    remove_useless_rb(expr_block, &start, &stop);

    uint8 lowest_precedence = 100;
    uint lowest_precedence_index = 0;

    find_lowest_precedence(expr_block, start, stop,
                           &lowest_precedence, &lowest_precedence_index);

    if (lowest_precedence < 100)
    {
        expression->type = EXPR_TYPE_OP;
        expression->operator = operator_new();
        expression->operator->token = expr_block[lowest_precedence_index];

        if (lowest_precedence_index == start)
        {
            expression->operator->left = expression_new();
            expression->operator->left->type = EXPR_TYPE_LI;
            expression->operator->left->identifier = get_token_null();
        }
        else
        {
            expression->operator->left
                    = parse_nested_expression(expr_block,
                                              start,
                                              lowest_precedence_index - 1);
        }

        if (lowest_precedence_index == stop)
        {
            expression->operator->right = expression_new();
            expression->operator->right->type = EXPR_TYPE_LI;
            expression->operator->right->identifier = get_token_null();
        }
        else
        {
            expression->operator->right
                    = parse_nested_expression(expr_block,
                                              lowest_precedence_index + 1,
                                              stop);
        }
    }
    else if (start == stop) /* only one token between the boundaries */
    {
        if (expr_block[start]->type == TOK_TYPE_LI
         || expr_block[start]->id   == TOK_KEY_NULL)
        {
            expression->type = EXPR_TYPE_LI;
            expression->literal = expr_block[start];
        }
        else if (expr_block[start]->type == TOK_TYPE_ID)
        {
            expression->type = EXPR_TYPE_ID;
            expression->identifier = expr_block[start];
        }
        else
        {
            error_printd(ERROR_PARSER_INVALID_EXPRESSION,
                         &expr_block[start]->line);
        }
    }
    else if (is_function(expr_block, start, stop))
    {
        expression->type = EXPR_TYPE_FN;
        expression->function = function_new();
        expression->function->identifier = expr_block[start];
        parse_function_params(expression->function->params,
                              expr_block, start+2, stop-1);
    }
    else if (is_expr_array(expr_block, start, stop))
    {
        expression->type = EXPR_TYPE_ARRAY;
        expression->expr_array = expr_array_new();
        expression->expr_array->identifier = expr_block[start];
        expression->expr_array->param = parse_nested_expression(expr_block,
                                                                start+2,
                                                                stop-1);
    }
    else
    {
        expression->type = EXPR_TYPE_LI;
        expression->identifier = get_token_null();
    }

    return expression;
}

static void parse_function_params(List *params, TokenNode **expr_block,
                                  int start, int stop)
{
    int i = start;
    int bracket_stack = 0;
    while (start <= stop)
    {
        if (expr_block[i]->id == TOK_SEP_RBS)
            bracket_stack++;
        else if (expr_block[i]->id == TOK_SEP_RBE)
            bracket_stack--;

        if (i == stop || (!bracket_stack && expr_block[i]->id == TOK_SEP_COMMA))
        {
            if (expr_block[i]->id == TOK_SEP_COMMA)
            {
                list_push(params, parse_nested_expression(expr_block, start, i-1));
            }
            else
            {
                list_push(params, parse_nested_expression(expr_block, start, i));
            }
            start = i+1;
        }
        else if (!expression_is_token_valid(expr_block[i]))
        {
            error_printd(ERROR_PARSER_INVALID_EXPRESSION,
                         &(expr_block[i]->line));
        }

        i++;
    }
}

static bool is_function(TokenNode **expr_block, int start, int stop)
{
    if ((stop - start + 1) < 3)
        return FALSE;

    return expr_block[start]->type == TOK_TYPE_ID
        && expr_block[start+1]->id == TOK_SEP_RBS
        && expr_block[stop]->id    == TOK_SEP_RBE;
}

static bool is_expr_array(TokenNode **expr_block, int start, int stop)
{
    if ((stop - start + 1) < 3)
        return FALSE;

    return expr_block[start]->type == TOK_TYPE_ID
        && expr_block[start+1]->id == TOK_SEP_SBS
        && expr_block[stop]->id    == TOK_SEP_SBE;
}

static void remove_useless_rb(TokenNode **expr_block, uint *start, uint *stop)
{
    if (*stop < *start)
        return;

    int lowest_inside = 0;
    int bracket_stack = 0;
    int min = *start;
    int max = *stop;

    while (expr_block[min]->id == TOK_SEP_RBS
        && expr_block[max]->id == TOK_SEP_RBE)
    {
        lowest_inside++;
        bracket_stack++;
        min++;
        max--;
    }

    while (min <= max)
    {
        if (!lowest_inside)
            return;

        if (expr_block[min]->id == TOK_SEP_RBS)
            bracket_stack++;
        else if (expr_block[min]->id == TOK_SEP_RBE)
            bracket_stack--;

        if (bracket_stack < lowest_inside)
            lowest_inside--;

        min++;
    }

    *start += lowest_inside;
    *stop -= lowest_inside;

    return;
}

static TokenNode *get_token_null()
{
    return token_null;
}

static Boundary *boundary_create(uint start, uint stop)
{
    Boundary *new = malloc(sizeof(Boundary));

    new->start = start;
    new->stop = stop;

    return new;
}



/*
 * From :
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
 */
static const uint8 OPERATOR_PRECEDENCE[127] = {
/* Single operators */
    [TOK_OP_ASIGN]      = 3,
    [TOK_OP_NOT]        = 17,
    [TOK_OP_INF]        = 12,
    [TOK_OP_SUP]        = 12,
    [TOK_OP_OR]         = 8,
    [TOK_OP_AND]        = 10,
    [TOK_OP_XOR]        = 9,
    [TOK_OP_ADD]        = 14,
    [TOK_OP_SUB]        = 14,
    [TOK_OP_BY]         = 15,
    [TOK_OP_DIV]        = 15,
    [TOK_OP_MOD]        = 15,

/* Double operators */
    [TOK_OP_INCR]       = 17,
    [TOK_OP_DECR]       = 17,
    [TOK_OP_EXPO]       = 16,
    [TOK_OP_SQRT]       = 16,
    [TOK_OP_EQUAL]      = 11,
    [TOK_OP_NOT_EQUAL]  = 11,
    [TOK_OP_INF_EQUAL]  = 12,
    [TOK_OP_SUP_EQUAL]  = 12,
    [TOK_OP_LOGIC_AND]  = 6,
    [TOK_OP_LOGIC_OR]   = 5,
    [TOK_OP_ADD_ASIGN]  = 3,
    [TOK_OP_SUB_ASIGN]  = 3,
    [TOK_OP_BY_ASIGN]   = 3,
    [TOK_OP_DIV_ASIGN]  = 3,
    [TOK_OP_MOD_ASIGN]  = 3,

/* Separator */
    [TOK_SEP_RBS]       = 21,
    [TOK_SEP_RBE]       = 21,
    [TOK_SEP_SBS]       = 20,
    [TOK_SEP_SBE]       = 20,
    [TOK_SEP_DOT]       = 20,
};

/* MOVEME: never used in that file, belongs to the compiler */
const short OPERATOR_ASSO[127] = {
/* Single operators */
    [TOK_OP_ASIGN]      = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_NOT]        = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_INF]        = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_SUP]        = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_OR]         = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_AND]        = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_XOR]        = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_ADD]        = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_SUB]        = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_BY]         = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_DIV]        = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_MOD]        = ASSO_LEFT_TO_RIGHT,

/* Double operators */
    [TOK_OP_INCR]       = ASSO_NA,
    [TOK_OP_DECR]       = ASSO_NA,
    [TOK_OP_EXPO]       = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_SQRT]       = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_EQUAL]      = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_NOT_EQUAL]  = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_INF_EQUAL]  = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_SUP_EQUAL]  = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_LOGIC_AND]  = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_LOGIC_OR]   = ASSO_LEFT_TO_RIGHT,
    [TOK_OP_ADD_ASIGN]  = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_SUB_ASIGN]  = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_BY_ASIGN]   = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_DIV_ASIGN]  = ASSO_RIGHT_TO_LEFT,
    [TOK_OP_MOD_ASIGN]  = ASSO_RIGHT_TO_LEFT,

/* Separator */
    [TOK_SEP_RBS]       = ASSO_NA,
    [TOK_SEP_RBE]       = ASSO_NA,
    [TOK_SEP_SBS]       = ASSO_LEFT_TO_RIGHT,
    [TOK_SEP_SBE]       = ASSO_LEFT_TO_RIGHT,
    [TOK_SEP_DOT]       = ASSO_LEFT_TO_RIGHT,
};
