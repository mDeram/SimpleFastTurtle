
 ├─if┐
 | | └─==
 | |   ├─L e[
 | |   |   /
 | |   |   ├─L 4
 | |   |   └─R +
 | |   |     ├─L 5
 | |   |     └─R 3
 | |   | ]
 | |   └─R 2
 | └─var┐
 |     └─=
 |       ├─L i
 |       └─R start
 ├─var┐
 |   └─=
 |     ├─L bracket_stack
 |     └─R 0
 └─while┐
   | └─<=
   |   ├─L start
   |   └─R stop
   ├─if┐
   | | └─==
   | |   ├─L expression_arr
   | |   └─R TOK_SEP_RBS
   | └─expr┐
   |     └─++
   |       ├─L bracket_stack
   |       └─R null
   ├─else
   | └─if┐
   |   | └─==
   |   |   ├─L expression_arr
   |   |   └─R TOK_SEP_RBE
   |   └─expr┐
   |       └─--
   |         ├─L bracket_stack
   |         └─R null
   ├─if┐
   | | └─||
   | |   ├─L ==
   | |   | ├─L i
   | |   | └─R stop
   | |   └─R &&
   | |     ├─L !
   | |     | ├─L null
   | |     | └─R bracket_stack
   | |     └─R ==
   | |       ├─L expression_arr
   | |       └─R TOK_SEP_COMMA
   | ├─if┐
   | | | └─==
   | | |   ├─L expression_arr
   | | |   └─R TOK_SEP_COMMA
   | | └─expr┐
   | |     └─list_push
   | |       └┐
   | |        ├─params
   | |        └─parse_nested_expression
   | |          └┐
   | |           ├─expression_arr
   | |           ├─start
   | |           └─-
   | |             ├─L i
   | |             └─R 1
   | ├─else
   | | └─expr┐
   | |     └─list_push
   | |       └┐
   | |        ├─params
   | |        └─parse_nested_expression
   | |          └┐
   | |           ├─expression_arr
   | |           ├─start
   | |           └─i
   | └─expr┐
   |     └─=
   |       ├─L start
   |       └─R +
   |         ├─L i
   |         └─R 1
   ├─else
   | └─if┐
   |   | └─!
   |   |   ├─L null
   |   |   └─R is_token_valid_in_expression
   |   |     └┐
   |   |      ├─expression_arr
   |   |      └─bracket_stack
   |   └─expr┐
   |       └─error_printd
   |         └┐
   |          ├─ERROR_PARSER_INVALID_EXPRESSION
   |          └─expression_arr
   └─expr┐
       └─++
         ├─L i
         └─R null

