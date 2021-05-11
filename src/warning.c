#include "warning.h"

/*
 *  Simple warning handling that just display warnings messages
 */

static void print_id(const int warning_id);



static const char *c_warning_list[100];



/* o = lu */
static const char *c_warning_list[100] = {
    /*  Lexer   */
    "A BLOCK COMMENT HAS NOT BEEN CLOSED", "",

    /*  Parser  */
    "EMPTY STATEMENT - LINE : %lu", "o",
};



static void print_id(const int warning_id)
{
    fprintf(stderr, "Warning (id%d): ", warning_id);
}

void warning_print(const int warning_id)
{
    print_id(warning_id);
    fprintf(stderr, "%s\n", c_warning_list[warning_id*2]);
}

void warning_printd(const int warning_id, const void *data)
{
    print_id(warning_id);
    switch(c_warning_list[warning_id*2+1][0])
    {
    case 's':
        fprintf(stderr, c_warning_list[warning_id*2], (char *)data);
        break;
    case 'o':
        fprintf(stderr, c_warning_list[warning_id*2], *(ulong *)data);
        break;
    }
    fprintf(stderr, "\n");
}
