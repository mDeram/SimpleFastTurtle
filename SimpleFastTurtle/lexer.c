#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "token.h"
#include "error.h"

#include "lexer.h"

/*
 *Lexer:
 *  Do the lexical analyze of SFT
 *  Add each token to a linked list
 *
 *Options:
 *  Save: Write all tokens in lexer.l
 */

static void lexer_save(struct TokenList *s_list_token);
static void lexer_tokenize(FILE *f, struct TokenList *s_list_token);
static int lexer_write_strings(FILE *f, struct TokenList *s_list_token, const char current_char, unsigned long int *current_line);
static int lexer_delete_comments(FILE *f, const char current_char, unsigned long int *current_line);
static void lexer_test(struct TokenList *s_list_token, const unsigned long int current_line, const unsigned char token_index, char current_token[]);
static int lexer_is_literal(const char token[]);
static int lexer_is_keyword(const char token[]);
static int lexer_is_operator(const char token);
static int lexer_is_separator(const char token);
static char lexer_escape_to_char(const char c);


void lexer_process(struct TokenList *s_list_token, char *file_name, int option_save, int option_print_tokens, int option_print_size)
{
    FILE *f = NULL;
    f = fopen(file_name, "r");

    if (f == NULL)
        error_printd(ERROR_LEXER_FILE_NOT_FOUND, file_name);

    lexer_tokenize(f, s_list_token);

    fclose(f);

    /*OPTIONS*/
    if (option_save)
        lexer_save(s_list_token);

    if (option_print_tokens)
        token_list_fprintf(stdout, s_list_token);

    if (option_print_size)
        printf("Size: %d\n", s_list_token->size);
}

static void lexer_save(struct TokenList *s_list_token)
{
    FILE *output = NULL;
    output = fopen("lexer.l", "w+");

    if (output == NULL)
        error_print(ERROR_LEXER_FILE_OUTPUT_FAILURE);

    token_list_fprintf(output, s_list_token);


    fclose(output);
}

static void lexer_tokenize(FILE *f, struct TokenList *s_list_token)
{
    unsigned long int current_line = 1;
    unsigned char token_index = 0;
    char current_token[91] = "";

    char c;
    while (1)
    {
        c = fgetc(f);
        if (c == '\n' || c == '\t' || c == ' ' || c == EOF)
        {
            lexer_test(s_list_token, current_line, token_index, current_token);
            token_index = 0;

            if (c == '\n')
                current_line++;
            else if (c == EOF)
                break;
        }
        else
        {
            int is_operator = lexer_is_operator(c);
            int is_separator = 0;
            if (!is_operator)
                is_separator = lexer_is_separator(c);

            if (is_operator || is_separator)
            {
                lexer_test(s_list_token, current_line, token_index, current_token);
                token_index = 0;

                current_token[0] = c;
                current_token[1] = '\0';

                if (is_operator)
                {
                    if (!lexer_delete_comments(f, c, &current_line))
                        token_list_push(s_list_token, current_line, TOK_TYPE_OP, c, current_token);
                }
                else
                {
                    if (!lexer_write_strings(f, s_list_token, c, &current_line))
                        token_list_push(s_list_token, current_line, TOK_TYPE_SEP, c, current_token);
                }
            }
            else if (c != '\r')
            {
                current_token[token_index] = c;
                token_index++;
            }
        }
        if (token_index > 90)
        {
            current_token[90] = '\0';
            error_printd(ERROR_LEXER_ID_TOO_LONG, current_token);
        }
    }
}

static int lexer_write_strings(FILE *f, struct TokenList *s_list_token, const char current_char, unsigned long int *current_line)
{
    if (current_char != '\'' && current_char != '"')
        return 0;

    size_t index = 0;
    size_t size = 10; /* >1 */
    char *str = malloc(sizeof(char)*size);

    /*Every time there is a backslash, the next letter is inserted with the backslash as one char only*/
    char c = fgetc(f);
    while (c != EOF && c != current_char && c != '\n')
    {
        if (c == '\\')
        {
            char next_char = fgetc(f);
            if (next_char == EOF)
                error_print(ERROR_LEXER_STRING_NOT_CLOSED);

            str[index] = lexer_escape_to_char(next_char);
            index++;
        }
        else
        {
            str[index] = c;
            index++;
        }

        if (index == size-1)
        {
            str = realloc(str, sizeof(char)*size*2);
            size *= 2;
        }

        c = fgetc(f);
    }

    if (c == '\n')
        (*current_line)++;
    else if (c == EOF)
        error_print(ERROR_LEXER_STRING_NOT_CLOSED);

    str[index] = '\0';
    token_list_push(s_list_token, *current_line, TOK_TYPE_LI, TOK_LI_STRING, str);

    free(str);

    return 1;
}

static int lexer_delete_comments(FILE *f, const char current_char, unsigned long int *current_line)
{
    if (current_char != '/')
        return 0;

    char next_char = fgetc(f);
    if (next_char == '/') /*Inline comment*/
    {
        char c = fgetc(f);
        while (c != EOF && c != '\n')
        {
            c = fgetc(f);
        }
        (*current_line)++;

        return 1;
    }

    if (next_char == '*') /*Block comment*/
    {
        char last = fgetc(f);
        char c = fgetc(f);
        while (c != EOF && !(last == '*' && c == '/'))
        {
            if (last == '\n')
                (*current_line)++;

            last = c;
            c = fgetc(f);
        }
        if (c == EOF)
        {
            error_print(ERROR_LEXER_COMMENT_NOT_CLOSED);
        }
        return 1;
    }

    if (next_char != EOF)
        fseek(f, -1, SEEK_CUR);
    return 0;
}

static void lexer_test(struct TokenList *s_list_token, const unsigned long int current_line, const unsigned char token_index, char current_token[])
{
    if (token_index == 0)
        return;

    current_token[token_index] = '\0';

    int result = lexer_is_keyword(current_token);
    if (result)
    {
        token_list_push(s_list_token, current_line, TOK_TYPE_KEY, result, current_token);
        return;
    }

    result = lexer_is_literal(current_token);
    if (result)
    {
        token_list_push(s_list_token, current_line, TOK_TYPE_LI, result, current_token);
        return;
    }

    token_list_push(s_list_token, current_line, TOK_TYPE_ID, 0, current_token);
}

/*Note that string and char are not taken into account here since they require a special state*/
static int lexer_is_literal(const char token[])
{
    char first_char = token[0];
    if (first_char >= 48 && first_char <= 57)
        return TOK_LI_NUMBER;
    if ((first_char == 't' || first_char == 'f') && (!strcmp("true", token) || !strcmp("false", token)))
        return TOK_LI_BOOL;
    return 0;
}

static int lexer_is_keyword(const char token[])
{
    if (token[0] < 97 && token[0] > 119)
        return 0;

    switch(token[0])
    {
        case 'f':
            if (!strcmp("for", token))
                return TOK_KEY_FOR;
            if (!strcmp("float", token))
                return TOK_KEY_FLOAT;
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
            if (!strcmp("elseif", token))
                return TOK_KEY_ELSEIF;
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
            break;
        case 'p':
            if (!strcmp("public", token))
                return TOK_KEY_PUBLIC;
            if (!strcmp("private", token))
                return TOK_KEY_PRIVATE;
            if (!strcmp("protected", token))
                return TOK_KEY_PROTECT;
            break;
    }
    return 0;
}

static int lexer_is_operator(const char token)
{
    switch(token)
    {
        case TOK_OP_EQUAL:
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

static int lexer_is_separator(const char token)
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

/*That's dope : convert "\t" to '\t'*/
static char lexer_escape_to_char(const char c)
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
