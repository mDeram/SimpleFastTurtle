#ifndef F_TOKEN_H
#define F_TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "mwc/forge.h"

#include "list.h"



typedef struct TokenNode TokenNode;
typedef struct Statement Statement;
typedef struct Expression Expression;
typedef struct Operator Operator;
typedef struct Function Function;
typedef struct ExprArray ExprArray;



void token_fprintf(FILE *output, void *data);
TokenNode *token_create(const ulong current_line,
                        const uint8 type,
                        const uint8 id,
                        const char *token);
void token_free(void *data);

Statement *statement_new();
void statement_free(void *data);

Expression *expression_new();
void expression_free(void *data);

Operator *operator_new();
void operator_free(void *data);

Function *function_new();
void function_free(void *data);

ExprArray *expr_array_new();
void expr_array_free(void *data);



typedef enum {
    EXPR_TYPE_OP,
    EXPR_TYPE_ID,
    EXPR_TYPE_LI,
    EXPR_TYPE_FN,
    EXPR_TYPE_ARRAY
} ExprType;

typedef enum {
    TOK_TYPE_OP,
    TOK_TYPE_SEP,
    TOK_TYPE_KEY,
    TOK_TYPE_LI,
    TOK_TYPE_ID
} TokType;

/* TODO I don't need to put default value myself */
typedef enum {
    /*
     *  All the literal number are choosen depending on the space
     *  available in the ascii table, that does not interfere with symbols
     *  (like a-z and A-Z areas and 1 to 30)
     */
    TOK_NULL            = 0,
/* OPerator */
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

    MIN_TOK_OP          = 1,    /*------*/
    TOK_OP_INCR         = 2,    /*  ++  */
    TOK_OP_DECR         = 3,    /*  --  */
    TOK_OP_EXPO         = 4,    /*  **  */
    TOK_OP_SQRT         = 5,    /*  //  */
    TOK_OP_EQUAL        = 6,    /*  ==  */
    TOK_OP_NOT_EQUAL    = 7,    /*  !=  */
    TOK_OP_INF_EQUAL    = 8,    /*  <=  */
    TOK_OP_SUP_EQUAL    = 9,    /*  >=  */
    TOK_OP_LOGIC_AND    = 10,    /*  &&  */
    TOK_OP_LOGIC_OR     = 11,   /*  ||  */
    TOK_OP_ADD_ASIGN    = 12,   /*  +=  */
    TOK_OP_SUB_ASIGN    = 13,   /*  -=  */
    TOK_OP_BY_ASIGN     = 14,   /*  *=  */
    TOK_OP_DIV_ASIGN    = 15,   /*  /=  */
    TOK_OP_MOD_ASIGN    = 16,   /*  %=  */
    MAX_TOK_OP          = 17,   /*------*/

/* SEParator */
    /*
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

/* KEYword */
    TOK_KEY_FOR         = 65,   /* in implementation*/
    TOK_KEY_IF          = 66,   /* in implementation*/
    TOK_KEY_WHILE       = 67,   /* in implementation*/
    TOK_KEY_ELSE        = 68,   /* in implementation*/
    TOK_KEY_BREAK       = 69,   /* in implementation*/
    TOK_KEY_RETURN      = 70,   /* in implementation*/
    TOK_KEY_ASSERT      = 71,
    TOK_KEY_ELIF        = 72,   /* in implementation*/
    TOK_KEY_FN          = 73,   /* in implementation*/
    TOK_KEY_VAR         = 74,   /* in implementation*/
    TOK_KEY_NULL        = 75,   /* in implementation*/

/* LIteral */
    TOK_LI_NUMBER       = 97,
    TOK_LI_BOOL         = 98,
    TOK_LI_STRING       = 99,

/* IDentifier */
    TOK_ID              = 100,
} TokId;



struct TokenNode {
    ulong line;
    TokType type;
    TokId id;
    char *token;
};

struct Statement {
    TokenNode *token;
    List *expressions;
    List *statements;
};

struct Expression {
    ExprType type;
    union {
        Operator *operator;
        TokenNode *identifier;
        TokenNode *literal;
        Function *function;
        ExprArray *expr_array;
    };
};

struct Operator {
    TokenNode *token;
    Expression *left;
    Expression *right;
};

struct Function {
    TokenNode *identifier;
    List *params;
};

struct ExprArray {
    TokenNode *identifier;
    Expression *param;
};



#endif
