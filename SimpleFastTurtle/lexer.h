#ifndef F_LEXER_H
#define F_LEXER_H

enum Tokens
{
    /*OPerator*/
    TOK_OP_EQUAL    = '=',
    TOK_OP_NOT      = '!',
    TOK_OP_INF      = '<',
    TOK_OP_SUP      = '>',
    TOK_OP_OR       = '|',
    TOK_OP_AND      = '&',
    TOK_OP_XOR      = '^',
    TOK_OP_ADD      = '+',
    TOK_OP_SUB      = '-',
    TOK_OP_BY       = '*',
    TOK_OP_DIV      = '/',
    TOK_OP_MOD      = '%',

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
    TOK_SEP_RBS     = '(',
    TOK_SEP_RBE     = ')',
    TOK_SEP_CBS     = '{',
    TOK_SEP_CBE     = '}',
    TOK_SEP_SBS     = '[',
    TOK_SEP_SBE     = ']',
    TOK_SEP_SEMI    = ';',
    TOK_SEP_COMMA   = ',',
    TOK_SEP_DOT     = '.',
    TOK_SEP_CHAR    = '\'',
    TOK_SEP_STR     = '"',

    /*KEYword*/
    TOK_KEY_FOR     = 65,
    TOK_KEY_FLOAT   = 66,
    TOK_KEY_IF      = 67,
    TOK_KEY_INT     = 68,
    TOK_KEY_WHILE   = 69,
    TOK_KEY_ELSE    = 70,
    TOK_KEY_BOOL    = 71,
    TOK_KEY_BREAK   = 72,
    TOK_KEY_CHAR    = 73,
    TOK_KEY_RETURN  = 74,
    TOK_KEY_STR     = 75,
    TOK_KEY_SWITCH  = 76,
    TOK_KEY_STATIC  = 77,
    TOK_KEY_DEFAULT = 78,
    TOK_KEY_CASE    = 79,
    TOK_KEY_ASSERT  = 80,
    TOK_KEY_ELSEIF  = 81,

    /*LIteral*/
    TOK_LI_NUMBER   = 97,
    TOK_LI_BOOL     = 98,
    TOK_LI_STRING   = 99,
    TOK_LI_CHAR     = 100,//not used, lexer consider every char as strings
};

void lexer_main(int save);

#endif
