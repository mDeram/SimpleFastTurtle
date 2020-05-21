#ifndef F_LEXER_H
#define F_LEXER_H

struct TokenList *lexer_process(char *file_name, int option_save, int option_print_tokens, int option_print_size);

#endif
