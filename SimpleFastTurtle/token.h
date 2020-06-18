#ifndef F_TOKEN_H
#define F_TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "list.h"



void token_fprintf(FILE *output, void *data);
struct TokenNode *token_new(const unsigned long int current_line,
                            const char type,
                            const char id,
                            const char *token);
void token_free(void *data);

struct Statement *statement_new();
void statement_free(void *data);

struct Expression *expression_new();
void expression_free(void *data);

struct Function *function_new();
void function_free(void *data);

void token_tree_fprintf(FILE *output, struct List *s_tree_token);
void token_statement_fprintf(FILE *output, void *data, char ident[], char e_pos);
void token_expression_fprintf(FILE *output, void *data, char ident[], char e_pos);
void token_expression_tree_fprintf(FILE *output, void *data, char ident[]);
#if 0
void token_tree_clear(struct TokenTree *tree);
void token_tree_delete(struct TokenTree *tree);
#endif



struct TokenNode {
    unsigned long int line;
    unsigned char type;
    unsigned char id;
    char *token;
};



struct Statement {
    struct TokenNode *token;
    struct List *expressions;
    struct List *statements;
};

struct Expression {
    unsigned char type;
    union {
        struct Operator *operator;
        struct TokenNode *identifier;
        struct TokenNode *literal;
        struct Function *function;
    };
};

struct Function {
    struct TokenNode *name;
    struct List *params;
};

struct Operator {
    struct TokenNode *token;
    struct Expression *left;
    struct Expression *right;
};

enum {
    EXPRESSION_TYPE_OP,
    EXPRESSION_TYPE_ID,
    EXPRESSION_TYPE_LI,
    EXPRESSION_TYPE_FN
};



enum {
    /*
     *  All the literal number are choosen depending on the space
     *  available in the ascii table, that does not interfere with symbols
     *  (like a-z and A-Z areas and 1 to 30)
     */
    TOK_TYPE_OP         = 48,
    TOK_TYPE_SEP        = 49,
    TOK_TYPE_KEY        = 50,
    TOK_TYPE_LI         = 51,
    TOK_TYPE_ID         = 52,

    /*OPerator*/
    TOK_OP_ASIGN        = '=',
    TOK_OP_NOT          = '!',
    TOK_OP_INF          = '<',
    TOK_OP_SUP          = '>',
    TOK_OP_OR           = '|',
    TOK_OP_AND          = '&',
    TOK_OP_XOR          = '^',
    TOK_OP_ADD          = '+',
    TOK_OP_SUB          = '-',
    TOK_OP_BY           = '*',
    TOK_OP_DIV          = '/',
    TOK_OP_MOD          = '%',

    TOK_OP_INCR         = 1,    /*  ++  */
    TOK_OP_DECR         = 2,    /*  --  */
    TOK_OP_EXPO         = 3,    /*  **  */
    TOK_OP_SQRT         = 4,    /*  //  */
    TOK_OP_EQUAL        = 5,    /*  ==  */
    TOK_OP_NOT_EQUAL    = 6,    /*  !=  */
    TOK_OP_INF_EQUAL    = 7,    /*  <=  */
    TOK_OP_SUP_EQUAL    = 8,    /*  >=  */
    TOK_OP_LOGIC_AND    = 9,    /*  &&  */
    TOK_OP_LOGIC_OR     = 10,   /*  ||  */
    TOK_OP_ADD_ASIGN    = 11,   /*  +=  */
    TOK_OP_SUB_ASIGN    = 12,   /*  -=  */
    TOK_OP_BY_ASIGN     = 13,   /*  *=  */
    TOK_OP_DIV_ASIGN    = 14,   /*  /=  */
    TOK_OP_MOD_ASIGN    = 15,   /*  %=  */

    /*SEParator
     *
     * R=round
     * C=curly
     * S=square
     *
     * B=bracket
     * S=start
     * E=end
     */
    TOK_SEP_RBS         = '(',
    TOK_SEP_RBE         = ')',
    TOK_SEP_CBS         = '{',
    TOK_SEP_CBE         = '}',
    TOK_SEP_SBS         = '[',
    TOK_SEP_SBE         = ']',
    TOK_SEP_SEMI        = ';',
    TOK_SEP_COMMA       = ',',
    TOK_SEP_DOT         = '.',
    TOK_SEP_CHAR        = '\'',
    TOK_SEP_STR         = '"',

    /*KEYword*/
    TOK_KEY_FOR         = 65,   /* in implementation*/
    TOK_KEY_FLOAT       = 66,
    TOK_KEY_IF          = 67,   /* in implementation*/
    TOK_KEY_INT         = 68,
    TOK_KEY_WHILE       = 69,   /* in implementation*/
    TOK_KEY_ELSE        = 70,   /* in implementation*/
    TOK_KEY_BOOL        = 71,
    TOK_KEY_BREAK       = 72,   /* in implementation*/
    TOK_KEY_CHAR        = 73,
    TOK_KEY_RETURN      = 74,   /* in implementation*/
    TOK_KEY_STR         = 75,
    TOK_KEY_SWITCH      = 76,
    TOK_KEY_STATIC      = 77,
    TOK_KEY_DEFAULT     = 78,
    TOK_KEY_CASE        = 79,
    TOK_KEY_ASSERT      = 80,
    TOK_KEY_ELIF        = 81,   /* in implementation*/
    TOK_KEY_NEW         = 82,
    TOK_KEY_CLASS       = 83,
    TOK_KEY_FN          = 84,   /* in implementation*/
    TOK_KEY_VAR         = 85,   /* in implementation*/
    TOK_KEY_NULL        = 86,   /* in implementation*/

    /*LIteral*/
    TOK_LI_NUMBER       = 97,
    TOK_LI_BOOL         = 98,
    TOK_LI_STRING       = 99,

    TOK_ID              = 100,
};

#endif
