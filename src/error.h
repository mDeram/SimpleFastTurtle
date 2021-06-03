#ifndef F_ERROR_H
#define F_ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mwc/forge.h"

#include "token.h"



void error_print(const int error_id);
void error_printd(const int error_id, const void *data);



enum {
    ERROR_LEXER_FILE_NOT_FOUND,
    ERROR_LEXER_FILE_OUTPUT_FAILURE,
    ERROR_LEXER_ID_TOO_LONG,
    ERROR_LEXER_STRING_NOT_CLOSED,

    ERROR_PARSER_FILE_OUTPUT_FAILURE,
    ERROR_PARSER_INVALID_NUMBER_PARAMETERS,
    ERROR_PARSER_INVALID_EXPRESSION,
    ERROR_PARSER_INVALID_EXPRESSION_START,
    ERROR_PARSER_INVALID_EXPRESSION_END,
    ERROR_PARSER_INVALID_STATEMENT,
    ERROR_PARSER_INVALID_STATEMENT_START,
    ERROR_PARSER_INVALID_STATEMENT_BLOCK_START,
    ERROR_PARSER_INVALID_STATEMENT_BLOCK,
    ERROR_PARSER_INVALID_STATEMENT_BLOCK_END,
    ERROR_PARSER_INVALID_VAR_ASSIGNMENT,
    ERROR_PARSER_NOT_HANDLED_KEYWORD,
};



#endif