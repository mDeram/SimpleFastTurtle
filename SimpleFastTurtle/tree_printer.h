#ifndef F_TREE_PRINTER_H
#define F_TREE_PRINTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "list.h"
#include "token.h"


// change names
void token_tree_fprintf(FILE *output, struct List *s_tree_token);
void token_statement_fprintf(FILE *output, void *data, char e_pos);
void token_expression_fprintf(FILE *output, void *data, char e_pos);
void token_expression_tree_fprintf(FILE *output, void *data);



struct AstIdent {
    size_t index;
    size_t size;
    char *ident;
};

#endif
