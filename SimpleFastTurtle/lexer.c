#include "lexer.h"

/*
 *  Do the lexical analysis of a .sft file
 *  Take a file path
 *  Return a list of token
 */

static void handle_args(struct List *s_list_token, int option_save,
                        int option_print_tokens, int option_print_size);
static void save_in_file(struct List *s_list_token);
static void tokenize(FILE *f, struct List *s_list_token);
static void tokenize_core(FILE *f, struct List *s_list_token,
                          struct LexerToken *s_cur);
static void handle_operator(struct List *s_list_token, struct LexerToken *s_cur);
static int handle_strings(FILE *f, struct List *s_list_token,
                          struct LexerToken *s_cur);
static int handle_comments(FILE *f, struct LexerToken *s_cur);
static void general_test(struct List *s_list_token, struct LexerToken *s_cur);
static int is_literal(const char token[]);
static int is_keyword(const char token[]);
static int is_operator(const char token);
static int is_double_operator(const char op1, const char op2);
static int is_separator(const char token);
static char escape_to_char(const char c);



void lexer_process(struct List *s_list_token,
                   char *file_name,
                   int option_save,
                   int option_print_tokens,
                   int option_print_size)
{
    FILE *f = NULL;
    f = fopen(file_name, "r");
    if (f == NULL)
        error_printd(ERROR_LEXER_FILE_NOT_FOUND, file_name);

    tokenize(f, s_list_token);

    fclose(f);

    handle_args(s_list_token, option_save,
                option_print_tokens, option_print_size);
}

void lexer_free(struct List *s_list_token)
{
    list_free_foreach(s_list_token, token_free);
}

static void handle_args(struct List *s_list_token, int option_save,
                        int option_print_tokens, int option_print_size)
{
    if (option_save)
        save_in_file(s_list_token);

    if (option_print_tokens)
        list_fprintf(s_list_token, stdout, token_fprintf);

    if (option_print_size)
        printf("Token Number: %ld\n", s_list_token->size);
}

static void save_in_file(struct List *s_list_token)
{
    FILE *output = NULL;
    output = fopen("lexer.l", "w+");
    if (output == NULL)
        error_print(ERROR_LEXER_FILE_OUTPUT_FAILURE);

    list_fprintf(s_list_token, output, token_fprintf);

    fclose(output);
}

static void tokenize(FILE *f, struct List *s_list_token)
{
    struct LexerToken s_cur = {
        .line = 1,
        .index = 0,
        .c = ' ',
        .token = ""
    };

    while ((s_cur.c = fgetc(f)) != EOF)
    {
        if (s_cur.c == '\n' || s_cur.c == '\t' || s_cur.c == ' ')
        {
            general_test(s_list_token, &s_cur);

            if (s_cur.c == '\n')
                s_cur.line++;
        }
        else if (s_cur.c != '\r')
        {
            tokenize_core(f, s_list_token, &s_cur);
        }

        if (s_cur.index == LEXER_TOKEN_LENGTH-1)
            s_cur.index--;
    }

    general_test(s_list_token, &s_cur);
}

static void tokenize_core(FILE *f, struct List *s_list_token,
                          struct LexerToken *s_cur)
{
    if (is_operator(s_cur->c))
    {
        general_test(s_list_token, s_cur);

        s_cur->token[0] = s_cur->c;
        s_cur->token[1] = '\0';

        if (!handle_comments(f, s_cur))
        {
            handle_operator(s_list_token, s_cur);
        }
    }
    else if (is_separator(s_cur->c))
    {
        general_test(s_list_token, s_cur);

        s_cur->token[0] = s_cur->c;
        s_cur->token[1] = '\0';

        if (!handle_strings(f, s_list_token, s_cur))
        {
            list_push(s_list_token,
                      token_new(s_cur->line, TOK_TYPE_SEP,
                                s_cur->c, s_cur->token));
        }
    }
    else
    {
        s_cur->token[s_cur->index] = s_cur->c;
        s_cur->index++;
    }
}

static void handle_operator(struct List *s_list_token, struct LexerToken *s_cur)
{
    /*
     *  If the last token is a simple operator
     *  Delete it and create a double operator
     */
    struct TokenNode *s_last_token
            = (struct TokenNode *)s_list_token->tail->data;
    char id = 0;

    if (s_last_token->type == TOK_TYPE_OP
        && s_last_token->id > 20
        && (id = is_double_operator(s_last_token->token[0], s_cur->token[0])))
    {
        s_cur->token[1] = s_cur->token[0];
        s_cur->token[0] = s_last_token->token[0];
        s_cur->token[2] = '\0';
        token_free(s_last_token);

        struct TokenNode *s_new_token
                = token_new(s_cur->line, TOK_TYPE_OP, id, s_cur->token);
        s_list_token->tail->data = s_new_token;
    }
    else
    {
        list_push(s_list_token,
                  token_new(s_cur->line, TOK_TYPE_OP, s_cur->c, s_cur->token));
    }
}

static int handle_strings(FILE *f, struct List *s_list_token,
                          struct LexerToken *s_cur)
{
    if (s_cur->c != '\'' && s_cur->c != '"')
        return 0;

    size_t index = 0;
    size_t size = 16; /*If 16 is to small, size *= 2*/
    char *str = malloc(sizeof(char)*size);

    char c;
    while ((c = fgetc(f)) != EOF && c != s_cur->c && c != '\n')
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
        s_cur->line++;
    else if (c == EOF)
        error_print(ERROR_LEXER_STRING_NOT_CLOSED);

    str[index] = '\0';
    list_push(s_list_token,
              token_new(s_cur->line, TOK_TYPE_LI, TOK_LI_STRING, str));

    free(str);

    return 1;
}

static int handle_comments(FILE *f, struct LexerToken *s_cur)
{
    if (s_cur->c != '/')
        return 0;

    char next_char = fgetc(f);
    if (next_char == '/') /*Inline comment*/
    {
        char c;
        while ((c = fgetc(f)) != EOF && c != '\n')
            ;

        s_cur->line++;

        return 1;
    }

    if (next_char == '*') /*Block comment*/
    {
        char last = fgetc(f);
        char c;
        while ((c = fgetc(f)) != EOF && !(last == '*' && c == '/'))
        {
            if (last == '\n')
                s_cur->line++;

            last = c;
        }

        if (c == EOF)
            warning_print(WARNING_LEXER_COMMENT_NOT_CLOSED);

        return 1;
    }

    if (next_char != EOF)
        fseek(f, -1, SEEK_CUR);

    return 0;
}

static void general_test(struct List *s_list_token, struct LexerToken *s_cur)
{
    if (!s_cur->index)
        return;

    s_cur->token[s_cur->index] = '\0';
    s_cur->index = 0;

    int result;
    if ((result = is_literal(s_cur->token)))
    {
        list_push(s_list_token,
                  token_new(s_cur->line, TOK_TYPE_LI, result, s_cur->token));
    }
    else if ((result = is_keyword(s_cur->token)))
    {
        list_push(s_list_token,
                  token_new(s_cur->line, TOK_TYPE_KEY, result, s_cur->token));
    }
    else
    {
        list_push(s_list_token,
                  token_new(s_cur->line, TOK_TYPE_ID, TOK_ID, s_cur->token));
    }
}

static int is_literal(const char token[])
{
    char first_char = token[0];
    if (first_char >= '0' && first_char <= '9')
        return TOK_LI_NUMBER;
    if ((first_char == 't' && !strcmp("true", token))
     || (first_char == 'f' && !strcmp("false", token)))
        return TOK_LI_BOOL;
    return 0;
}

static int is_keyword(const char token[])
{
    if (token[0] < 'a' && token[0] > 'w')
        return 0;

    switch(token[0])
    {
    case 'f':
        if (!strcmp("for", token))
            return TOK_KEY_FOR;
        if (!strcmp("float", token))
            return TOK_KEY_FLOAT;
        if (!strcmp("fn", token))
            return TOK_KEY_FN;
        break;
    case 'v':
        if (!strcmp("var", token))
            return TOK_KEY_VAR;
        break;
    case 'i':
        if (!strcmp("if", token))
            return TOK_KEY_IF;
        if (!strcmp("int", token))
            return TOK_KEY_INT;
        break;
    case 'w':
        if (!strcmp("while", token))
            return TOK_KEY_WHILE;
        break;
    case 'e':
        if (!strcmp("else", token))
            return TOK_KEY_ELSE;
        if (!strcmp("elif", token))
            return TOK_KEY_ELIF;
        break;
    case 'b':
        if (!strcmp("bool", token))
            return TOK_KEY_BOOL;
        if (!strcmp("break", token))
            return TOK_KEY_BREAK;
        break;
    case 'c':
        if (!strcmp("char", token))
            return TOK_KEY_CHAR;
        if (!strcmp("case", token))
            return TOK_KEY_CASE;
        if (!strcmp("class", token))
            return TOK_KEY_CLASS;
        break;
    case 'r':
        if (!strcmp("return", token))
            return TOK_KEY_RETURN;
        break;
    case 's':
        if (!strcmp("str", token))
            return TOK_KEY_STR;
        if (!strcmp("switch", token))
            return TOK_KEY_SWITCH;
        if (!strcmp("static", token))
            return TOK_KEY_STATIC;
        break;
    case 'd':
        if (!strcmp("default", token))
            return TOK_KEY_DEFAULT;
        break;
    case 'a':
        if (!strcmp("assert", token))
            return TOK_KEY_ASSERT;
        break;
    case 'n':
        if (!strcmp("new", token))
            return TOK_KEY_NEW;
        if (!strcmp("null", token))
            return TOK_KEY_NULL;
        break;
    }
    return 0;
}

static int is_operator(const char token)
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
        return 1;
    default:
        return 0;
    }
}

static int is_double_operator(const char op1, const char op2)
{
    /*
     * Evaluating op2 first reduce the number of cases
     */
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
        case TOK_OP_DIV:
            return TOK_OP_SQRT;
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
        }
    }
    return 0;
}

static int is_separator(const char token)
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
        return 1;
    default:
        return 0;
    }
}

/*  Convert "\t" to '\t'*/
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
