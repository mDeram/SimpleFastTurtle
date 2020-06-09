#include "error.h"

/*
 *  Simple error handling:
 *      Display the error message
 *      exit(EXIT_FAILURE)
 */

static void print_id(int error_id);



static const char *c_error_list[100];



/* o = lu */
static const char *c_error_list[100] = {
    "FILE \"%s\" NOT FOUND FOUND", "s",
    "FAILED TO OPEN \"lexer.l\"", "",
    "IDENTIFIER LONGER THAN 90 CHAR :\n%s", "s",
    "A STRING HAS NOT BEEN CLOSED", "",
    "A BLOCK COMMENT HAS NOT BEEN CLOSED", "",

    "INVALID NUMBER OF PARAMETERS IN STATEMENT - LINE : %lu", "o",
    "INVALID STATEMENT BLOCK START - LINE : %lu", "o",
    "INVALID STATEMENT BLOCK END - LINE : %lu", "o",
    "INVALID EXPRESSION - LINE : %lu", "o",
};



static void print_id(int error_id)
{
    fprintf(stderr, "ERROR (id%d): ", error_id);
}

void error_print(int error_id)
{
    print_id(error_id);
    fprintf(stderr, "%s\n", c_error_list[error_id*2]);
}

void error_printd(int error_id, void *data)
{
    print_id(error_id);
    switch(c_error_list[error_id*2+1][0])
    {
    case 's':
        fprintf(stderr, c_error_list[error_id*2], (char *)data);
        break;
    case 'o':
        fprintf(stderr, c_error_list[error_id*2], *(unsigned long *)data);
        break;
    }
    exit(EXIT_FAILURE);
}
