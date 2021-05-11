#ifndef F_LEXER_H
#define F_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mwc/forge.h"

#include "list.h"
#include "token.h"
#include "error.h"
#include "warning.h"



void lexer_process(List *tokens, char *file_name,
                   int option_save,
                   int option_print_tokens,
                   int option_print_size);
void lexer_free(List *tokens);



#endif
