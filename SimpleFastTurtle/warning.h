#ifndef F_WARNING_H
#define F_WARNING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



void warning_print(int warning_id);
void warning_printd(int warning_id, void *data);



enum {
    WARNING_PARSER_EMPTY_STATEMENT,
};



#endif
