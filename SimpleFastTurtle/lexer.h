#ifndef F_LEXER_H
#define F_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "list.h"
#include "token.h"
#include "error.h"
#include "warning.h"



#define LEXER_TOKEN_LENGTH 91


void lexer_process(struct List *s_list_token,
                   char *file_name,
                   int option_save,
                   int option_print_tokens,
                   int option_print_size);



struct LexerToken {
    unsigned long int line;
    unsigned char index;
    char c;
    char token[LEXER_TOKEN_LENGTH];
};

#endif
