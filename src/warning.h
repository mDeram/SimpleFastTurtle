#ifndef F_WARNING_H
#define F_WARNING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mwc/forge.h"

#include "token.h"



void warning_print(const int warning_id);
void warning_printd(const int warning_id, const void *data);



enum {
    WARNING_LEXER_COMMENT_NOT_CLOSED,
    WARNING_LEXER_ID_TOO_LONG,
    
    WARNING_PARSER_EMPTY_STATEMENT,
};



#endif
