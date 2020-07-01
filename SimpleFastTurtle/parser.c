#include "parser.h"

/*
 *  Create the Abstract Syntax Tree
 *  Take a list of token
 *  Return a tree of token
 */

static void save_in_file(struct List *s_tree_token, struct List *s_list_token);
static void parse(struct List *s_tree_token, struct List *s_list_token);
static int next_node(struct ParserNode *s_cur);
static void remove_all_token(struct ParserNode *s_cur, char to_remove);
static void parse_expression_block(struct ParserNode *s_cur, struct List *s_list);
static void parse_statement_block(struct ParserNode *s_cur,
                                  struct Statement *s_statement);
static void statement_inline(struct ParserNode *s_cur,
                             struct Statement *s_statement);
static void statement_block(struct ParserNode *s_cur,
                            struct Statement *s_statement);
static void statement_start(struct ParserNode *s_cur,
                                  struct Statement *s_statement);
static struct Statement *parse_statement(struct ParserNode *s_cur);
static int expression_is_token_valid(struct TokenNode *token);
static int expression_can_token_follow(struct TokenNode *last2,
                                       struct TokenNode *last,
                                       struct TokenNode *cur);
static short bracket_end_to_start(short bracket);
static struct TokenNode **expression_to_array(struct ListNode *copy_node,
                                              unsigned long start,
                                              unsigned long stop);
static void find_lowest_precedence(struct TokenNode **expression_arr,
                                   unsigned long start, unsigned long stop,
                                   short *lowest_precedence, unsigned long *index);
static struct Expression *parse_nested_expression(
                                            struct TokenNode **expression_arr,
                                            unsigned long start,
                                            unsigned long stop);
static void parse_function_params(struct List *params,
                                  struct TokenNode **expression_arr,
                                  int start, int stop);
static int is_function(struct TokenNode **expression_arr, int start, int stop);
static int is_array(struct TokenNode **expression_arr, int start, int stop);
static void remove_useless_rb(struct TokenNode *expression[],
                              unsigned long *start, unsigned long *stop);
static struct TokenNode *get_token_null();
static struct Boundary *boundary_create(unsigned long start, unsigned long stop);



static const short OPERATOR_PRECEDENCE[127];
static struct TokenNode *token_null;



void parser_process(struct List *s_tree_token,
                    struct List *s_list_token,
                    int option_save,
                    int option_print_tree)
{
    token_null = token_new(0, TOK_TYPE_KEY, TOK_KEY_NULL, "null");

    parse(s_tree_token, s_list_token);

    /*OPTIONS*/
    if (option_save)
        save_in_file(s_tree_token, s_list_token);

    if (option_print_tree)
        token_tree_fprintf(stdout, s_tree_token);
}

void parser_free(struct List *s_tree_token)
{
    list_free_foreach(s_tree_token, statement_free);
    token_free(token_null);
}

static void save_in_file(struct List *s_tree_token, struct List *s_list_token)
{
    FILE *output = fopen("parser.p", "w+");
    if (output == NULL)
        error_print(ERROR_PARSER_FILE_OUTPUT_FAILURE);

    token_tree_fprintf(output, s_tree_token);

    fclose(output);
}

static void parse(struct List *s_tree_token, struct List *s_list_token)
{
    if (!s_list_token->size)
        return;

    struct ParserNode s_cur = {NULL, NULL};
    s_cur.node = s_list_token->head;
    s_cur.token = (struct TokenNode *)s_list_token->head->data;

    struct ListNode *s_last_node = NULL;

    while (s_cur.node != NULL)
    {
        if (s_last_node == s_cur.node)
        {
            error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END,
                         &s_cur.token->line);
        }

        s_last_node = s_cur.node;
        struct Statement *s_new_statement = parse_statement(&s_cur);

        if (s_new_statement != NULL)
            list_push(s_tree_token, s_new_statement);
    }
}

static int next_node(struct ParserNode *s_cur)
{
    printf("ID: %d\n", s_cur->token->id);
    s_cur->node = s_cur->node->next;
    if (s_cur->node == NULL)
        return 0;

    s_cur->token = (struct TokenNode *)s_cur->node->data;
    return 1;
}

static void remove_all_token(struct ParserNode *s_cur, char to_remove)
{
    if (s_cur->node == NULL)
        return;

    while ((s_cur->token->id == to_remove) && next_node(s_cur))
        ;
}

static void parse_statement_block(struct ParserNode *s_cur,
                            struct Statement *s_statement)
{
    if (s_cur->node == NULL)
        return;

    struct ListNode *s_last_node = s_cur->node;

    if (s_cur->token->id != TOK_SEP_CBS)
    {
        statement_inline(s_cur, s_statement);
    }
    else
    {
        statement_block(s_cur, s_statement);
    }

    if (s_last_node == s_cur->node)
        error_printd(ERROR_PARSER_INVALID_STATEMENT, &s_cur->token->line);
}

static void statement_inline(struct ParserNode *s_cur,
                             struct Statement *s_statement)
{
    if (s_cur->token->id == TOK_SEP_SEMI)
    {
        next_node(s_cur);
    }
    else
    {
        struct Statement *s_new_statement = parse_statement(s_cur);
        if (s_new_statement != NULL)
            list_push(s_statement->statements, s_new_statement);
    }
}

static void statement_block(struct ParserNode *s_cur,
                            struct Statement *s_statement)
{
    if (!next_node(s_cur)) /* { */
    {
        error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK,
                     &s_statement->token->line);
    }

    while (s_cur->token->id != TOK_SEP_CBE)
    {
        struct Statement *s_new_statement = parse_statement(s_cur);
        if (s_cur->node == NULL)
        {
            error_printd(ERROR_PARSER_INVALID_STATEMENT_BLOCK_END,
                         &s_statement->token->line);
        }

        if (s_new_statement != NULL)
            list_push(s_statement->statements, s_new_statement);
    }

    next_node(s_cur); /* } */
}

static void statement_start(struct ParserNode *s_cur,
                            struct Statement *s_statement)
{
    s_statement->token = s_cur->token;
    if (!next_node(s_cur))
    {
        error_printd(ERROR_PARSER_INVALID_STATEMENT_START,
                     &s_statement->token->line);
    }
}

static struct Statement *parse_statement(struct ParserNode *s_cur)
{
    remove_all_token(s_cur, TOK_SEP_SEMI);

    if (s_cur->node == NULL || s_cur->token->id == TOK_SEP_CBE)
        return NULL;

    struct Statement *s_statement = statement_new();

    if (s_cur->token->type == TOK_TYPE_KEY)
    {
        switch(s_cur->token->id)
        {
        case TOK_KEY_FOR:
        case TOK_KEY_WHILE:
        case TOK_KEY_IF:
        case TOK_KEY_ELIF:
            statement_start(s_cur, s_statement);
            parse_expression_block(s_cur, s_statement->expressions);
            if (s_statement->expressions->size == 0)
            {
                error_printd(ERROR_PARSER_INVALID_NUMBER_PARAMETERS,
                             &s_statement->token->line);
            }

            parse_statement_block(s_cur, s_statement);
            break;

        case TOK_KEY_FN:
            statement_start(s_cur, s_statement);
            if (s_cur->token->type != TOK_TYPE_ID)
            {
                error_printd(ERROR_PARSER_INVALID_STATEMENT_START,
                             &s_statement->token->line);
            }
            statement_start(s_cur, s_statement);
            parse_expression_block(s_cur, s_statement->expressions);

            parse_statement_block(s_cur, s_statement);
            break;

        case TOK_KEY_ELSE:
            statement_start(s_cur, s_statement);
            parse_statement_block(s_cur, s_statement);
            break;

        case TOK_KEY_VAR:
            statement_start(s_cur, s_statement);
            parse_expression_block(s_cur, s_statement->expressions);

            if (s_statement->expressions->size == 0)
            {
                error_printd(ERROR_PARSER_INVALID_VAR_ASSIGNMENT,
                             &s_statement->token->line);
            }
            break;

        case TOK_KEY_BREAK:
            statement_start(s_cur, s_statement);
            break;

        case TOK_KEY_RETURN:
            statement_start(s_cur, s_statement);
            parse_expression_block(s_cur, s_statement->expressions);
            break;

        default:
            error_printd(ERROR_PARSER_NOT_HANDLED_KEYWORD, s_cur->token);
        }
    }
    else if (expression_is_token_valid(s_cur->token))
    {
        s_statement->token = NULL;
        parse_expression_block(s_cur, s_statement->expressions);
    }
    else
    {
        error_printd(ERROR_PARSER_INVALID_STATEMENT, &s_statement->token->line);
    }

    return s_statement;
}

static int expression_is_token_valid(struct TokenNode *token)
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

static int expression_can_token_follow(struct TokenNode *last2,
                                       struct TokenNode *last,
                                       struct TokenNode *cur)
{
    if (last == NULL)
        return 1;

    int is_last_id_li = last->type == TOK_TYPE_ID || last->type == TOK_TYPE_LI;
    int is_cur_id_li = cur->type == TOK_TYPE_ID || cur->type == TOK_TYPE_LI;

    if (is_last_id_li && is_cur_id_li)
        return 0;

    if (last2 == NULL)
        return 1;

    if (last->id == TOK_OP_INCR || last->id == TOK_OP_DECR)
        return !(
                    (cur->type == TOK_TYPE_ID
                  || cur->type == TOK_TYPE_LI
                  || cur->type == TOK_TYPE_KEY)
                &&
                    (last2->type == TOK_TYPE_ID
                  || last2->type == TOK_TYPE_LI
                  || last2->type == TOK_TYPE_KEY)
                );

    return 1;
}

static short bracket_end_to_start(short bracket)
{
    switch(bracket)
    {
    case TOK_SEP_RBE:
        return TOK_SEP_RBS;
    case TOK_SEP_SBE:
        return TOK_SEP_SBS;
    default:
        return 0;
    }
}

static struct List *expression_to_boundary_list(struct ParserNode *s_cur)
{
    unsigned long size = 0;
    struct List *boundaries = list_new();
    short is_inside_rb = (s_cur->token->id == TOK_SEP_RBS);
    size_t stack_index = 0;
    size_t stack_size  = 16; /*If 16 is to small, stack_size *= 2*/
    unsigned char *stack = malloc(sizeof(unsigned char)*stack_size);
    if (stack == NULL) exit(EXIT_FAILURE);

    struct TokenNode *last2 = NULL;
    struct TokenNode *last1 = NULL;

    while (expression_is_token_valid(s_cur->token)
        && expression_can_token_follow(last2, last1, s_cur->token))
    {
        if (s_cur->token->id == TOK_SEP_RBS || s_cur->token->id == TOK_SEP_SBS)
        {
            stack[stack_index] = s_cur->token->id;
            stack_index++;
        }
        else if (s_cur->token->id == TOK_SEP_RBE || s_cur->token->id == TOK_SEP_SBE)
        {
            stack_index--;
            if (stack_index < 0)
                break;
            if (stack[stack_index]
                != bracket_end_to_start(s_cur->token->id))
                break;
        }
        else if (s_cur->token->id == TOK_SEP_COMMA)
        {
            if (!stack_index)
            {
                /* Some expressions are inside round bracket but it's not the
                 * case anymore: raising error
                 */
                if (is_inside_rb && boundaries->size)
                    return NULL;

                is_inside_rb = 0;
                struct Boundary *new_boundary = NULL;

                if (!boundaries->size)
                {
                    new_boundary = boundary_create(0, size);
                }
                else
                {
                    unsigned int stop
                            = ((struct Boundary*)boundaries->tail->data)->stop;
                    new_boundary = boundary_create(stop + 2, size);
                }

                list_push(boundaries, new_boundary);
            }
            else if (stack_index == 1 && is_inside_rb)
            {
                struct Boundary *new_boundary = NULL;

                if (!boundaries->size)
                {
                    printf("size %ld\n", size);
                    new_boundary = boundary_create(1, size - is_inside_rb);
                }
                else
                {
                    unsigned int stop
                            = ((struct Boundary*)boundaries->tail->data)->stop;
                    new_boundary = boundary_create(stop + 2, size - is_inside_rb);
                }

                list_push(boundaries, new_boundary);
            }
            //else the comma is included in the expression
        }

        last2 = last1;
        last1 = s_cur->token;
        if (!next_node(s_cur))
            break;

        if (stack_index == stack_size-1)
        {
            stack_size *= 2;
            stack = realloc(stack, sizeof(unsigned long)*stack_size);
            if (stack == NULL) exit(EXIT_FAILURE);
        }
        size++;
    }

    struct Boundary *new_boundary = NULL;

    if (!boundaries->size)
    {
        new_boundary = boundary_create(0, size - 1);
    }
    else
    {
        unsigned int stop = ((struct Boundary*)boundaries->tail->data)->stop;
        new_boundary = boundary_create(stop + 2, size - 1 - is_inside_rb);
    }

    list_push(boundaries, new_boundary);

    free(stack);

    if (stack_index)
        return NULL;
    else
        return boundaries;
}

static struct TokenNode **expression_to_array(struct ListNode *node,
                                              unsigned long start,
                                              unsigned long stop)
{
    struct TokenNode **expression_arr = malloc(sizeof(struct TokenNode*)*stop);
    if (expression_arr == NULL) exit(EXIT_FAILURE);

    for (int i = 0; i <= stop; i++)
    {
        if (i >= start)
        {
            expression_arr[i] = (struct TokenNode*)node->data;
            //printf("%s", ((struct TokenNode*)node->data)->token);
        }
        node = node->next;
    }

    //printf("\n");

    return expression_arr;
}

static void parse_expression_block(struct ParserNode *s_cur, struct List *s_list)
{
    unsigned long tmp_line = s_cur->token->line;
    remove_all_token(s_cur, TOK_SEP_COMMA);
    if (s_cur->node == NULL)
        error_printd(ERROR_PARSER_INVALID_EXPRESSION, &tmp_line);

    struct ListNode *copy_node = s_cur->node;

    struct List *expr_boundaries = expression_to_boundary_list(s_cur);
    if (expr_boundaries == NULL || !expr_boundaries->size)
        error_printd(ERROR_PARSER_INVALID_EXPRESSION, &tmp_line);

    unsigned int start = ((struct Boundary*)expr_boundaries->head->data)->start;
    unsigned int stop  = ((struct Boundary*)expr_boundaries->tail->data)->stop;
    struct TokenNode **expression_arr = expression_to_array(copy_node, start, stop);

    list_foreach_(expr_boundaries, node)
    {
        struct Boundary *boundary = (struct Boundary*)node->data;
        struct Expression *s_expression = parse_nested_expression(expression_arr,
                                                                  boundary->start,
                                                                  boundary->stop);
        //if (s_expression == NULL)
        list_push(s_list, s_expression);
    }

    //free boundaries
    free(expression_arr);
}

static void find_lowest_precedence(struct TokenNode **expression_arr,
                                   unsigned long start, unsigned long stop,
                                   short *lowest_precedence, unsigned long *index)
{
    unsigned long bracket_stack = 0;
    for (unsigned long i = start; i <= stop; i++)
    {
        if (expression_arr[i]->id == TOK_SEP_RBS || expression_arr[i]->id == TOK_SEP_SBS)
        {
            bracket_stack++;
        }
        else if (expression_arr[i]->id == TOK_SEP_RBE || expression_arr[i]->id == TOK_SEP_SBE)
        {
            bracket_stack--;
        }
        else if (!bracket_stack)
        {
            if (expression_arr[i]->type == TOK_TYPE_OP)
            {
                short cur_precedence = OPERATOR_PRECEDENCE[expression_arr[i]->id];
                if (cur_precedence < *lowest_precedence)
                {
                    *lowest_precedence = cur_precedence;
                    *index = i;
                }
            }
            else if (expression_arr[i]->id == TOK_SEP_COMMA)
            {
                error_printd(ERROR_PARSER_INVALID_EXPRESSION, &(expression_arr[i]->line));
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
static struct Expression *parse_nested_expression(
                                            struct TokenNode **expression_arr,
                                            unsigned long start,
                                            unsigned long stop)
{
    struct Expression *s_expression = expression_new();

    remove_useless_rb(expression_arr, &start, &stop);

    short lowest_precedence = 100;
    unsigned long lowest_precedence_index = 0;

    find_lowest_precedence(expression_arr, start, stop,
                           &lowest_precedence, &lowest_precedence_index);

    if (lowest_precedence < 100)
    {
        s_expression->type = EXPRESSION_TYPE_OP;
        s_expression->operator = operator_new();
        s_expression->operator->token = expression_arr[lowest_precedence_index];

        if (lowest_precedence_index-1 < start)
        {
            s_expression->operator->left = expression_new();
            s_expression->operator->left->type = EXPRESSION_TYPE_LI;
            s_expression->operator->left->identifier = get_token_null();
        }
        else
        {
            s_expression->operator->left
                    = parse_nested_expression(expression_arr, start, lowest_precedence_index-1);
        }

        if (lowest_precedence_index+1 > stop)
        {
            s_expression->operator->right = expression_new();
            s_expression->operator->right->type = EXPRESSION_TYPE_LI;
            s_expression->operator->right->identifier = get_token_null();
        }
        else
        {
            s_expression->operator->right
                    = parse_nested_expression(expression_arr, lowest_precedence_index+1, stop);
        }
    }
    else if (start == stop) /* only one token in the array */
    {
        if (expression_arr[start]->type == TOK_TYPE_LI
            || expression_arr[start]->id == TOK_KEY_NULL)
        {
            s_expression->type = EXPRESSION_TYPE_LI;
            s_expression->literal = expression_arr[start];
        }
        else if (expression_arr[start]->type == TOK_TYPE_ID)
        {
            s_expression->type = EXPRESSION_TYPE_ID;
            s_expression->identifier = expression_arr[start];
        }
        else
        {
            error_printd(ERROR_PARSER_INVALID_EXPRESSION,
                         &expression_arr[start]->line);
        }
    }
    else if (is_function(expression_arr, start, stop))
    {
        s_expression->type = EXPRESSION_TYPE_FN;
        s_expression->function = function_new();
        s_expression->function->identifier = expression_arr[start];
        parse_function_params(s_expression->function->params,
                              expression_arr, start+2, stop-1);
    }
    else if (is_array(expression_arr, start, stop))
    {
        s_expression->type = EXPRESSION_TYPE_ARRAY;
        s_expression->array = array_new();
        s_expression->array->identifier = expression_arr[start];
        s_expression->array->param
                = parse_nested_expression(expression_arr, start+2, stop-1);
    }
    /*else if (expression_arr[start]->id == TOK_SEP_COMMA)
    {
        printf("comma fail\n");
        exit(EXIT_FAILURE);
    }*/
    else
    {
        s_expression->type = EXPRESSION_TYPE_LI;
        s_expression->identifier = get_token_null();
    }

    return s_expression;
}

static void parse_function_params(struct List *params,
                                  struct TokenNode **expression_arr,
                                  int start, int stop)
{
    int i = start;
    int bracket_stack = 0;
    while (start <= stop)
    {
        if (expression_arr[i]->id == TOK_SEP_RBS)
            bracket_stack++;
        else if (expression_arr[i]->id == TOK_SEP_RBE)
            bracket_stack--;

        if (i == stop
            || (!bracket_stack && expression_arr[i]->id == TOK_SEP_COMMA))
        {
            if (expression_arr[i]->id == TOK_SEP_COMMA)
            {
                list_push(params, parse_nested_expression(expression_arr, start, i-1));
            }
            else
            {
                list_push(params, parse_nested_expression(expression_arr, start, i));
            }
            start = i+1;
        }
        else if (!expression_is_token_valid(expression_arr[i]))
        {
            error_printd(ERROR_PARSER_INVALID_EXPRESSION,
                         &(expression_arr[i]->line));
        }

        i++;
    }
}

static int is_function(struct TokenNode **expression_arr, int start, int stop)
{
    if ((stop - start + 1) < 3)
        return 0;

    return expression_arr[start]->type == TOK_TYPE_ID
        && expression_arr[start+1]->id == TOK_SEP_RBS
        && expression_arr[stop]->id    == TOK_SEP_RBE;
}

static int is_array(struct TokenNode **expression_arr, int start, int stop)
{
    if ((stop - start + 1) < 3)
        return 0;

    return expression_arr[start]->type == TOK_TYPE_ID
        && expression_arr[start+1]->id == TOK_SEP_SBS
        && expression_arr[stop]->id    == TOK_SEP_SBE;
}

static void remove_useless_rb(struct TokenNode *expression[],
                              unsigned long *start, unsigned long *stop)
{
    if (*stop < *start)
        return;

    int lowest_inside = 0;
    int bracket_stack = 0;
    int min = *start;
    int max = *stop;
    while(expression[min]->id == TOK_SEP_RBS
       && expression[max]->id == TOK_SEP_RBE)
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

static struct TokenNode *get_token_null()
{
    //File line start at one so using 0 show that the parser added it to the code
    return token_null;
}

static struct Boundary *boundary_create(unsigned long start, unsigned long stop)
{
    struct Boundary *new = malloc(sizeof(struct Boundary));

    new->start = start;
    new->stop = stop;

    return new;
}



/*
 * From :
 * https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/Operator_Precedence
 */
static const short OPERATOR_PRECEDENCE[127] = {
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

const short OPERATOR_ASSOCIATIVITY[127] = {
/* Single operators */
    [TOK_OP_ASIGN]      = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_NOT]        = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_INF]        = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_SUP]        = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_OR]         = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_AND]        = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_XOR]        = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_ADD]        = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_SUB]        = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_BY]         = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_DIV]        = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_MOD]        = ASSOCIATIVITY_LEFT_TO_RIGHT,

/* Double operators */
    [TOK_OP_INCR]       = ASSOCIATIVITY_NA,
    [TOK_OP_DECR]       = ASSOCIATIVITY_NA,
    [TOK_OP_EXPO]       = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_SQRT]       = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_EQUAL]      = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_NOT_EQUAL]  = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_INF_EQUAL]  = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_SUP_EQUAL]  = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_LOGIC_AND]  = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_LOGIC_OR]   = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_OP_ADD_ASIGN]  = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_SUB_ASIGN]  = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_BY_ASIGN]   = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_DIV_ASIGN]  = ASSOCIATIVITY_RIGHT_TO_LEFT,
    [TOK_OP_MOD_ASIGN]  = ASSOCIATIVITY_RIGHT_TO_LEFT,

/* Separator */
    [TOK_SEP_RBS]       = ASSOCIATIVITY_NA,
    [TOK_SEP_RBE]       = ASSOCIATIVITY_NA,
    [TOK_SEP_SBS]       = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_SEP_SBE]       = ASSOCIATIVITY_LEFT_TO_RIGHT,
    [TOK_SEP_DOT]       = ASSOCIATIVITY_LEFT_TO_RIGHT,
};
