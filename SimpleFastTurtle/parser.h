#ifndef F_PARSER_H
#define F_PARSER_H



void parser_process(struct List *s_tree_token,
					struct List *s_list_token,
					int option_save,
					int option_print_tree);



extern short OPERATOR_PRECEDENCE[127];
extern short OPERATOR_ASSOCIATIVITY[127];



enum {
	ASSOCIATIVITY_NA,
	ASSOCIATIVITY_LEFT_TO_RIGHT,
	ASSOCIATIVITY_RIGHT_TO_LEFT,
};



#endif
