#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//libs
#include "token.h"

//program
#include "lexer.h"

int main(int argc, char *argv[])
{

    struct TokenList *s_list_token = lexer_process("main.sft", 1, 0, 0);

    token_list_delete(s_list_token);

    return 0;
}
