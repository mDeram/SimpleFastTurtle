#ifndef F_ERROR_H
#define F_ERROR_H

enum {
	ERROR_LEXER_FILE_NOT_FOUND,
	ERROR_LEXER_FILE_OUTPUT_FAILURE,
	ERROR_LEXER_ID_TOO_LONG,
	ERROR_LEXER_STRING_NOT_CLOSED,
	ERROR_LEXER_COMMENT_NOT_CLOSED,
};

extern const char *c_error_list[100];

void error_print(int error_id);
void error_printd(int error_id, void *data);

#endif
