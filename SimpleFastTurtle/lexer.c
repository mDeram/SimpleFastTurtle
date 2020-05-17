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
static int lexer_is_keyword(char token[]);
static int lexer_is_operator(char token);
static int lexer_is_separator(char token);

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


    int current_line = 0;
    int token_index = 0;
    char current_token[100] = "";
    char c = fgetc(f);
    while (c != EOF)
    {
        if (c == '\n' || c == '\t' || c == ' ')
        {
            if (token_index > 0)
            {
                current_token[token_index] = '\0';
                if (current_token[0] > 97 && current_token[0] < 120)
                {
                    int result = lexer_is_keyword(current_token);
                    if (result)
                        lexer_write_token(new_f, current_line, "KEY", result, current_token);
                    else
                        lexer_write_token(new_f, current_line, "ID", 0, current_token);
                }
                else
                {
                    lexer_write_token(new_f, current_line, "ID", 0, current_token);
                }
                token_index = 0;
            }
            if (c == '\n')
                current_line++;
        }
        else
        {
            //can be optimized
            int is_operator = lexer_is_operator(c);
            int is_separator = 0;
            if (!is_operator)
                is_separator = lexer_is_separator(c);

            if (is_operator || is_separator)
            {
                if (token_index > 0)
                {
                    current_token[token_index] = '\0';
                    if (current_token[0] > 97 && current_token[0] < 120)
                    {
                        int result = lexer_is_keyword(current_token);
                        if (result)
                            lexer_write_token(new_f, current_line, "KEY", result, current_token);
                        else
                            lexer_write_token(new_f, current_line, "ID", 0, current_token);
                    }
                    else
                    {
                        lexer_write_token(new_f, current_line, "ID", 0, current_token);
                    }
                }

                token_index = 0;
                current_token[0] = c;
                current_token[1] = '\0';

                if (is_operator)
                    lexer_write_token(new_f, current_line, "OP", c, current_token);
                else
                    lexer_write_token(new_f, current_line, "SEP", c, current_token);
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
    printf("Number of line : %d\n", current_line+1);
}

static int lexer_is_keyword(char token[])
{
    switch(token[0])
    {
        case 'f':
            if (!strcmp("for", token))
                return TOK_KEY_FOR;
            else if (!strcmp("float", token))
                return TOK_KEY_FLOAT;
            break;
        case 'i':
            if (!strcmp("if", token))
                return TOK_KEY_IF;
            else if (!strcmp("int", token))
                return TOK_KEY_INT;
            break;
        case 'w':
            if (!strcmp("while", token))
                return TOK_KEY_WHILE;
            break;
        case 'e':
            if (!strcmp("else", token))
                return TOK_KEY_ELSE;
            break;
        case 'b':
            if (!strcmp("bool", token))
                return TOK_KEY_BOOL;
            else if (!strcmp("break", token))
                return TOK_KEY_BREAK;
            break;
        case 'c':
            if (!strcmp("char", token))
                return TOK_KEY_CHAR;
            break;
        case 'r':
            if (!strcmp("return", token))
                return TOK_KEY_RETURN;
            break;
        case 's':
            if (!strcmp("str", token))
                return TOK_KEY_STR;
            else if (!strcmp("switch", token))
                return TOK_KEY_SWITCH;
            else if (!strcmp("static", token))
                return TOK_KEY_STATIC;
            break;
        case 'd':
            if (!strcmp("default", token))
                return TOK_KEY_DEFAULT;
            break;
        default:
            return 0;
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
            return 1;
        default:
            return 0;
    }
}
