#ifndef F_PARSER_H
#define F_PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "token.h"
#include "list.h"
#include "error.h"
#include "warning.h"



void parser_process(struct List *s_tree_token,
                    struct List *s_list_token,
                    int option_save,
                    int option_print_tree);



extern const short OPERATOR_ASSOCIATIVITY[127];



struct ParserNode {
    struct ListNode *node;
    struct TokenNode *token;
};



enum {
    ASSOCIATIVITY_NA,
    ASSOCIATIVITY_LEFT_TO_RIGHT,
    ASSOCIATIVITY_RIGHT_TO_LEFT,
};



#endif
