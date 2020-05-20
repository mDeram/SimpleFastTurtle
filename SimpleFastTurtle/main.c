#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//libs
#include "list.h"

//program
#include "lexer.h"

int main(int argc, char *argv[])
{

    struct List *s_list_token = lexer_process("main.sft", 1);

    list_delete(s_list_token);

    return 0;
}
