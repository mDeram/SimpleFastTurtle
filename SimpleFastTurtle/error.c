#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "error.h"

const char *c_error_list[100] = {
	"ERROR : FILE \"%s\" NOT FOUND FOUND", "s",
	"ERROR : FAILED TO OPEN \"lexer.l\"", "",
	"ERROR : IDENTIFIER LONGER THAN 90 CHAR :\n%s", "s",
	"ERROR : A STRING HAS NOT BEEN CLOSED", "",
	"ERROR : A BLOCK COMMENT HAS NOT BEEN CLOSED", "",
};

void error_print(int error_id)
{
	printf("%s", c_error_list[error_id*2]);
}

void error_printd(int error_id, void *data)
{
	switch(c_error_list[error_id*2+1][0])
	{
		case 's':
			printf(c_error_list[error_id*2], (char *)data);
			break;
	}
	exit(EXIT_FAILURE);
}
