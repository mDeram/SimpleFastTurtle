#include "lexer.h"

/*
 *  Do the lexical analysis of a .sft file
 *  Take a file path
 *  Return a list of token
 */

#define LEXER_TOKEN_LENGTH 91
#define KEYWORD_NUMBER 11



typedef struct {
    ulong line;
    uchar index;
    char c;
    char token[LEXER_TOKEN_LENGTH];
} LexerToken;

typedef struct {
    TokId id;
    char *name;
} KeywordLookup;



static const KeywordLookup keyword_list[KEYWORD_NUMBER];



static void handle_args(List *tokens,
                        int option_save,
                        int option_print_tokens,
                        int option_print_size);
static void save_in_file(List *tokens);
static void tokenize(FILE *f, List *tokens);
static void tokenize_core(FILE *f, List *tokens, LexerToken *current);
static void handle_operator(List *tokens, LexerToken *current);
static int handle_strings(FILE *f, List *tokens, LexerToken *current);
static void handle_inline_comment(FILE *f, LexerToken *current);
static void handle_block_comment(FILE *f, LexerToken *current);
static bool handle_comments(FILE *f, LexerToken *current);
static void general_test(List *tokens, LexerToken *current);
static TokId get_literal(const char token[]);
static TokId get_keyword(const char token[]);
static bool is_operator(const char token);
static bool is_token_simple_operator(const TokenNode node);
static TokId get_double_operator(ListNode *last, char op2);
static bool is_separator(const char token);
static char escape_to_char(const char c);



void lexer_process(List *tokens, char *file_name,
                   int option_save,
                   int option_print_tokens,
                   int option_print_size)
{
    FILE *f = fopen(file_name, "r");
    if (f == NULL)
        error_printd(ERROR_LEXER_FILE_NOT_FOUND, file_name);

    tokenize(f, tokens);

    fclose(f);

    handle_args(tokens, option_save, option_print_tokens, option_print_size);
}

void lexer_free(List *tokens)
{
    list_free_foreach(tokens, token_free);
}

static void handle_args(List *tokens,
                        int option_save,
                        int option_print_tokens,
                        int option_print_size)
{
    if (option_save)
        save_in_file(tokens);

    if (option_print_tokens)
        list_fprintf(tokens, stdout, token_fprintf);

    if (option_print_size)
        printf("Token Number: %ld\n", tokens->size);
}

static void save_in_file(List *tokens)
{
    FILE *output = fopen("output.l", "w+");
    if (output == NULL)
        error_print(ERROR_LEXER_FILE_OUTPUT_FAILURE);

    list_fprintf(tokens, output, token_fprintf);

    fclose(output);
}

static void tokenize(FILE *f, List *tokens)
{
    LexerToken current = {
        .line = 1,
        .index = 0,
        .c = ' ',
        .token = ""
    };

    while ((current.c = fgetc(f)) != EOF)
    {
        if (current.c == '\n' || current.c == '\t' || current.c == ' ')
        {
            general_test(tokens, &current);

            if (current.c == '\n')
                current.line++;
        }
        else if (current.c != '\r')
        {
            tokenize_core(f, tokens, &current);
        }

        if (current.index == LEXER_TOKEN_LENGTH-1)
            current.index--;
    }

    general_test(tokens, &current);
}

static void tokenize_core(FILE *f, List *tokens, LexerToken *current)
{
    if (is_operator(current->c))
    {
        general_test(tokens, current);
        
        current->token[0] = current->c;
        current->token[1] = '\0';

        if (!handle_comments(f, current))
        {
            handle_operator(tokens, current);
        }
    }
    else if (is_separator(current->c))
    {
        general_test(tokens, current);

        current->token[0] = current->c;
        current->token[1] = '\0';

        if (!handle_strings(f, tokens, current))
        {
            list_push(tokens, token_create(current->line,
                                           TOK_TYPE_SEP,
                                           current->c,
                                           current->token));
        }
    }
    else
    {
        current->token[current->index] = current->c;
        current->index++;
    }
}

static void handle_operator(List *tokens, LexerToken *current)
{
    /*
     *  If the last token is a simple operator
     *  Delete it and create a double operator
     */
    TokId id = get_double_operator(tokens->tail, current->token[0]);
    if (id != TOK_NULL)
    {
        TokenNode *last = (TokenNode *)tokens->tail->data;

        char tmp = current->token[0];
        current->token[0] = last->token[0];
        current->token[1] = tmp;
        current->token[2] = '\0';

        token_free(last);

        TokenNode *new_token = token_create(current->line,
                                            TOK_TYPE_OP,
                                            id,
                                            current->token);
        tokens->tail->data = new_token;
    }
    else
    {
        list_push(tokens, token_create(current->line,
                                       TOK_TYPE_OP,
                                       current->c,
                                       current->token));
    }
}

static int handle_strings(FILE *f, List *tokens, LexerToken *current)
{
    if (current->c != '\'' && current->c != '"')
        return 0;

    size_t index = 0;
    size_t size  = 16;  /* If 16 is to small, size *= 2 */
    char   *str  = malloc(sizeof(char)*size);

    char c;
    while ((c = fgetc(f)) != EOF && c != current->c && c != '\n')
    {
        if (c == '\\')
        {
            char next_char = fgetc(f);
            if (next_char == EOF)
                error_print(ERROR_LEXER_STRING_NOT_CLOSED);

            str[index] = escape_to_char(next_char);
            index++;
        }
        else
        {
            str[index] = c;
            index++;
        }

        if (index == size-1)
        {
            size *= 2;
            str = realloc(str, sizeof(char)*size);
            if (str == NULL) exit(EXIT_FAILURE);
        }
    }

    if (c == '\n')
        current->line++;
    else if (c == EOF)
        error_print(ERROR_LEXER_STRING_NOT_CLOSED);

    str[index] = '\0';
    list_push(tokens, token_create(current->line,
                                   TOK_TYPE_LI,
                                   TOK_LI_STRING,
                                   str));

    free(str);

    return 1;
}

static void handle_inline_comment(FILE *f, LexerToken *current)
{
    char c;
    while ((c = fgetc(f)) != EOF && c != '\n')
        ;

    current->line++;
}

static void handle_block_comment(FILE *f, LexerToken *current)
{
    char last = fgetc(f);
    char c;
    while ((c = fgetc(f)) != EOF && !(last == '*' && c == '/'))
    {
        if (last == '\n')
            current->line++;

        last = c;
    }

    if (c == EOF)
        warning_print(WARNING_LEXER_COMMENT_NOT_CLOSED);
}

static bool handle_comments(FILE *f, LexerToken *current)
{
    if (current->c != '/')
        return FALSE;

    char next_char = fgetc(f);

    if (next_char == '/')
    {
        handle_inline_comment(f, current);
        return TRUE;
    }
    else if (next_char == '*')
    {
        handle_block_comment(f, current);
        return TRUE;
    }

    if (next_char != EOF)
        fseek(f, -1, SEEK_CUR);

    return FALSE;
}

static void general_test(List *tokens, LexerToken *current)
{
    if (!current->index)
        return;

    current->token[current->index] = '\0';

    TokId result;
    if      ((result = get_literal(current->token)) != TOK_NULL)
    {
        list_push(tokens, token_create(current->line,
                                       TOK_TYPE_LI,
                                       result,
                                       current->token));
    }
    else if ((result = get_keyword(current->token)) != TOK_NULL)
    {
        list_push(tokens, token_create(current->line,
                                       TOK_TYPE_KEY,
                                       result,
                                       current->token));
    }
    else
    {
        if (current->index > TOKEN_TYPE_ID_LENGTH)
                warning_printd(WARNING_LEXER_ID_TOO_LONG, &current->token);

        list_push(tokens, token_create(current->line,
                                       TOK_TYPE_ID,
                                       TOK_ID,
                                       current->token));
    }

    current->index = 0;
}

static TokId get_literal(const char token[])
{
    char first_char = token[0];
    // TODO check if the number is valid
    if (first_char >= '0' && first_char <= '9')
        return TOK_LI_NUMBER;
    if (!strcmp("true", token) || !strcmp("false", token))
        return TOK_LI_BOOL;
    return TOK_NULL;
}

static TokId get_keyword(const char token[])
{
    if (token[0] < 'a' && token[0] > 'w')
        return TOK_NULL;

    for (int i = 0; i < KEYWORD_NUMBER; i++)
    {
        if (!strcmp(keyword_list[i].name, token))
            return keyword_list[i].id;
    }

    return TOK_NULL;
}

static bool is_operator(const char token)
{
    switch(token)
    {
    case TOK_OP_ASIGN:
    case TOK_OP_NOT:
    case TOK_OP_INF:
    case TOK_OP_SUP:
    case TOK_OP_OR:
    case TOK_OP_AND:
    case TOK_OP_XOR:
    case TOK_OP_ADD:
    case TOK_OP_SUB:
    case TOK_OP_BY:
    case TOK_OP_DIV:
    case TOK_OP_MOD:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool is_token_simple_operator(const TokenNode node)
{
    return node.type == TOK_TYPE_OP
       && (node.id < MIN_TOK_OP || node.id > MAX_TOK_OP);
}

static TokId get_double_operator(ListNode *last, char op2)
{
    if (last == NULL)
        return TOK_NULL;
    
    TokenNode last_node = *(TokenNode *)last->data;

    if (!is_token_simple_operator(last_node))
        return TOK_NULL;
    
    /* Evaluating op2 first reduce the number of cases */
    char op1 = last_node.token[0];
    if (op2 == op1)
    {
        switch(op1)
        {
        case TOK_OP_ASIGN:
            return TOK_OP_EQUAL;
        case TOK_OP_ADD:
            return TOK_OP_INCR;
        case TOK_OP_SUB:
            return TOK_OP_DECR;
        case TOK_OP_BY:
            return TOK_OP_EXPO;
        case TOK_OP_AND:
            return TOK_OP_LOGIC_AND;
        case TOK_OP_OR:
            return TOK_OP_LOGIC_OR;
        }
    }
    else if (op2 == TOK_OP_ASIGN)
    {
        switch(op1)
        {
        case TOK_OP_NOT:
            return TOK_OP_NOT_EQUAL;
        case TOK_OP_INF:
            return TOK_OP_INF_EQUAL;
        case TOK_OP_SUP:
            return TOK_OP_SUP_EQUAL;
        case TOK_OP_ADD:
            return TOK_OP_ADD_ASIGN;
        case TOK_OP_SUB:
            return TOK_OP_SUB_ASIGN;
        case TOK_OP_BY:
            return TOK_OP_BY_ASIGN;
        case TOK_OP_DIV:
            return TOK_OP_DIV_ASIGN;
        case TOK_OP_MOD:
            return TOK_OP_MOD_ASIGN;
        }
    }
    return TOK_NULL;
}

static bool is_separator(const char token)
{
    switch(token)
    {
    case TOK_SEP_RBS:
    case TOK_SEP_RBE:
    case TOK_SEP_CBS:
    case TOK_SEP_CBE:
    case TOK_SEP_SBS:
    case TOK_SEP_SBE:
    case TOK_SEP_SEMI:
    case TOK_SEP_COMMA:
    case TOK_SEP_DOT:
    case TOK_SEP_CHAR:
    case TOK_SEP_STR:
        return TRUE;
    default:
        return FALSE;
    }
}

/*  Convert "\x" to '\x' if needed*/
static char escape_to_char(const char c)
{
    switch(c)
    {
    case 'a':
        return '\a';
    case 'b':
        return '\b';
    case 't':
        return '\t';
    case 'n':
        return '\n';
    case 'v':
        return '\v';
    case 'f':
        return '\f';
    case 'r':
        return '\r';
    default:
        return c;
    }
}

static const KeywordLookup keyword_list[KEYWORD_NUMBER] = {
    {.id = TOK_KEY_FOR,     .name = "for"},
    {.id = TOK_KEY_IF,      .name = "if"},
    {.id = TOK_KEY_WHILE,   .name = "while"},
    {.id = TOK_KEY_ELSE,    .name = "else"},
    {.id = TOK_KEY_BREAK,   .name = "break"},
    {.id = TOK_KEY_RETURN,  .name = "return"},
    {.id = TOK_KEY_ASSERT,  .name = "assert"},
    {.id = TOK_KEY_ELIF,    .name = "elif"},
    {.id = TOK_KEY_FN,      .name = "fn"},
    {.id = TOK_KEY_VAR,     .name = "var"},
    {.id = TOK_KEY_NULL,    .name = "null"},
};
