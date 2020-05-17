#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//libs
#include "list.h"

//program
#include "lexer.h"

int main(int argc, char *argv[])
{

    lexer_main(1);
    //token_list = lexer_main();

    return 0;
}
