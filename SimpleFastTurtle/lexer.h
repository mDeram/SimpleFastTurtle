#ifndef F_LEXER_H
#define F_LEXER_H

void lexer_process(struct List *s_list_token,
                    char *file_name,
                    int option_save,
                    int option_print_tokens,
                    int option_print_size);

#endif
