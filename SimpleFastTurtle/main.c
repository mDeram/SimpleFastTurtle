#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[])
{
	struct TokenList *s_list_token = token_list_new();
	struct TokenTree *s_tree_token = token_tree_new();

    lexer_process(s_list_token, "main.sft", 1, 1, 1);

    parser_process(s_tree_token, s_list_token, 1, 1);

    token_tree_delete(s_tree_token);
    token_list_delete(s_list_token);

    return 0;
}
