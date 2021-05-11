#ifndef F_PARSER_H
#define F_PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "mwc/forge.h"

#include "list.h"
#include "token.h"
#include "tree_printer.h"
#include "error.h"
#include "warning.h"



typedef enum {
    ASSO_NA,
    ASSO_LEFT_TO_RIGHT,
    ASSO_RIGHT_TO_LEFT,
} Associativity;




void parser_process(List *tree_token, List *tokens,
                    int option_save, int option_print);
void parser_free(List *tree_token);



extern const short OPERATOR_ASSOCIATIVITY[127];



#endif
