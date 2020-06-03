#ifndef F_WARNING_H
#define F_WARNING_H

enum {
	WARNING_PARSER_EMPTY_STATEMENT,
};

extern const char *c_warning_list[100];

void warning_print(int warning_id);
void warning_printd(int warning_id, void *data);

#endif
