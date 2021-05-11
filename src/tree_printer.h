#ifndef F_TREE_PRINTER_H
#define F_TREE_PRINTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

#include "mwc/forge.h"

#include "list.h"
#include "token.h"



void token_tree_fprintf(FILE *output, List *s_tree_token);
/* CHANGEME: change to local */
void token_statement_fprintf(FILE *output, void *data, ListPos pos);
void token_expression_fprintf(FILE *output, void *data, ListPos pos);
void token_expression_tree_fprintf(FILE *output, void *data);



#endif
