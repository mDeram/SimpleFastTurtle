#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "list.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"



int main(int argc, char *argv[])
{
    int option_lexer_save       = 1;
    int option_lexer_print      = 1;
    int option_lexer_print_size = 1;
    
    int option_parser_save  = 1;
    int option_parser_print = 1;

    List *tokens     = list_new();
    List *tree_token = list_new();

    lexer_process(tokens, "main.sft", option_lexer_save,
                                      option_lexer_print,
                                      option_lexer_print_size);
    parser_process(tree_token, tokens, option_parser_save,
                                       option_parser_print);

    parser_free(tree_token);
    lexer_free(tokens);

    return 0;
}
