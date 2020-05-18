#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lexer.h"

/*
 *Lexer: do the lexical analyze of SFT
 *
 *Add each token to a simple linked list
 *is save option is true, create a file and write all token
 *
 */

static void lexer_open_recur(char *file_name);
static void lexer_write_token(FILE *f, int line, char name[], int id, char token[]);
static void lexer_analyze(FILE *f);
static void lexer_test(FILE *output, int current_line, int token_index, char current_token[]);
static int lexer_write_strings(FILE *f, FILE *output, const char current_char, int *current_line);
static int lexer_delete_comments(FILE *f, const char current_char, int *current_line);
static int lexer_is_literal(char token[]);
static int lexer_is_keyword(char token[]);
static int lexer_is_operator(char token);
static int lexer_is_separator(char token);
static char lexer_escape_to_char(char c);

void lexer_main(int save)
{
    lexer_open_recur("main.sft");
}

static void lexer_open_recur(char *file_name)
{
    FILE *f = NULL;
    f = fopen(file_name, "r");

    if (f == NULL)
    {
        printf("ERROR : FILE \"%s\" NOT FOUND FOUND", file_name);
        exit(EXIT_FAILURE);
    }

    lexer_analyze(f);

    fclose(f);
}

static void lexer_write_token(FILE *f, int line, char name[], int id, char token[])
{
    fprintf(f, "[Line: %d\t\tType: %s\t\tId: %d\t\tSymbol: %s\t\t]\n", line, name, id, token);
}

static void lexer_analyze(FILE *f) //temp
{
    FILE *new_f = NULL;
    new_f = fopen("lexer.l", "w+");

    if (new_f == NULL)
    {
        printf("ERROR : CREATION OF \"lexer.l\" IMPOSSIBLE");
        exit(EXIT_FAILURE);
    }

    int current_line = 1;
    int token_index = 0;
    char current_token[100] = "";

    char c = fgetc(f);
    while (c != EOF)
    {
        if (c == '\n' || c == '\t' || c == ' ')
        {
            lexer_test(new_f, current_line, token_index, current_token);
            token_index = 0;

            if (c == '\n')
                current_line++;
        }
        else
        {
            int is_operator = lexer_is_operator(c);
            int is_separator = 0;
            if (!is_operator)
                is_separator = lexer_is_separator(c);

            if (is_operator || is_separator)
            {
                lexer_test(new_f, current_line, token_index, current_token);
                token_index = 0;

                current_token[0] = c;
                current_token[1] = '\0';

                if (is_operator)
                {
                    if (!lexer_delete_comments(f, c, &current_line))
                        lexer_write_token(new_f, current_line, "OP", c, current_token);
                }
                else
                {
                    if (!lexer_write_strings(f, new_f, c, &current_line))
                        lexer_write_token(new_f, current_line, "SEP", c, current_token);
                }
            }
            else if (c != '\r')
            {
                current_token[token_index] = c;
                token_index++;
            }
        }
        c = fgetc(f);
    }

    fclose(new_f);
    printf("Number of line : %d\n", current_line);
}

static int lexer_write_strings(FILE *f, FILE *output, const char current_char, int *current_line)
{   
    if (current_char != '\'' && current_char != '"')
        return 0;
    
    //Should be able to deal with any string size single liked list with chuck that are 100-1 char ?
    char new_str[1000] = "";
    int str_index = 0;
    char c = fgetc(f);
    /*
     * Every time there is a backslash, the next letter is inserted with the backslash as one char only
     */
    while (c != EOF)
    {
        if (c == '\n')
        {
            (*current_line)++;
            break;
        }

        if (c == '\\')
        {
            char next_char = fgetc(f);
            if (next_char == EOF)
            {
                printf("ERROR : A STRING HAS NOT BEEN CLOSED");
                exit(EXIT_FAILURE);
            }
            new_str[str_index] = lexer_escape_to_char(next_char);
            str_index++;
        }
        else if (c == current_char)
        {
            break;
        }
        else
        {
            new_str[str_index] = c;
            str_index++;
        }
        c = fgetc(f);
    }
    
    new_str[str_index] = '\0';
    lexer_write_token(output, *current_line, "LI", TOK_LI_STRING, new_str);

    return 1;
}

static int lexer_delete_comments(FILE *f, const char current_char, int *current_line)
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
            printf("ERROR : A BLOCK COMMENT HAS NOT BEEN CLOSED");
            exit(EXIT_FAILURE);
        }
        return 1;
    }

    if (next_char != EOF)
        fseek(f, -1, SEEK_CUR);
    return 0;
}

static void lexer_test(FILE *output, int current_line, int token_index, char current_token[])
{
    if (token_index == 0)
        return;

    current_token[token_index] = '\0';

    int result = lexer_is_keyword(current_token);
    if (result)
    {
        lexer_write_token(output, current_line, "KEY", result, current_token);
        return;
    }

    result = lexer_is_literal(current_token);
    if (result)
    {
        lexer_write_token(output, current_line, "LI", result, current_token);
        return;
    }

    lexer_write_token(output, current_line, "ID", 0, current_token);
}

/*Note that string and char are not taken into account here since they require special state*/
static int lexer_is_literal(char token[])
{
    int first_char = token[0];
    if (first_char >= 48 && first_char <= 57)
        return TOK_LI_NUMBER;
    if ((first_char == 't' || first_char == 'f') && (!strcmp("true", token) || !strcmp("false", token)))
    {
        return TOK_LI_BOOL;
    }
    return 0;
}

static int lexer_is_keyword(char token[])
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
    }
    return 0;
}

static int lexer_is_operator(char token)
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

static int lexer_is_separator(char token)
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
static char lexer_escape_to_char(char c)
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