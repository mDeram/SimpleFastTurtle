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
	struct List *s_list_token = list_new();
	struct List *s_tree_token = list_new();

    lexer_process(s_list_token, "main.sft", 1, 1, 1);

    parser_process(s_tree_token, s_list_token, 1, 1);

    //list_free_foreach(s_tree_token, token_branch_free);
    list_free_foreach(s_list_token, token_free);

    return 0;
}
