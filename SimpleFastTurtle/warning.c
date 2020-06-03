#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "warning.h"

static void warning_print_id(int warning_id);

/* o = lu */
const char *c_warning_list[100] = {
	"EMPTY STATEMENT - LINE : %lu", "o",
};

static void warning_print_id(int warning_id)
{
	fprintf(stderr, "Warning (id%d): ", warning_id);
}

void warning_print(int warning_id)
{
	warning_print_id(warning_id);
	fprintf(stderr, "%s\n", c_warning_list[warning_id*2]);
}

void warning_printd(int warning_id, void *data)
{
	warning_print_id(warning_id);
	switch(c_warning_list[warning_id*2+1][0])
	{
		case 's':
			fprintf(stderr, c_warning_list[warning_id*2], (char *)data);
			break;
		case 'o':
			fprintf(stderr, c_warning_list[warning_id*2], *(unsigned long *)data);
			break;
	}
	fprintf(stderr, "\n");
}
