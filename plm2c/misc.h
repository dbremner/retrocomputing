#pragma once
/*
 *	Miscellaneous defines
 */
typedef	unsigned char	BYTE;
typedef	unsigned char	BOOLEAN;


#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif

/*
 *	White space characters
 */
#define	SPACE	' '
#define TAB	9
#define CR	13
#define LF	10

/*
 *	Useful defines
 */
#define is_a_uc_char(char) ((char >= 'A') && (char <= 'Z'))
#define is_a_lc_char(char) ((char >= 'a') && (char <= 'z'))
#define is_a_char(char) (((char & 0x5F) >= 'A') && ((char & 0x5F) <= 'Z'))
#define is_a_digit(char) ((char >= '0') && (char <= '9'))

#define is_a_type(token) ((token->token_class == RESERVED) && \
		(token->token_type >= BYTE) && (token->token_type <= REAL))

#define is_white(ch) ((ch == ' ') || (ch == TAB))

#define NONE	0

//FIXME unsnarl these
#include "cvt.h" 
#include "defs.h"
#include "struct.h"
//FIXME this saves including a protos.h file in *.c

//context.c
int find_member_symbol(TOKEN *symbol, DECL_MEMBER *decl_ptr, DECL_ID **decl_id);
int find_list_symbol(TOKEN *symbol, DECL_MEMBER *decl_ptr, DECL_MEMBER **decl_found, DECL_ID **decl_id);
int find_symbol(TOKEN *symbol, DECL_MEMBER **decl_found, DECL_ID **decl_id);
void add_to_context(DECL_MEMBER *decl);
void add_decl_to_context(DECL *decl);
void new_context(int type, TOKEN *name);
void pop_context(void);
void init_context(void);

//control.c
void parse_control(void);

//convert.c
int parse_statement(TOKEN *first_token);
int parse_new_statement(void);
void parse_file(void);

//declare.c
void element_copy(DECL_MEMBER *src, DECL_MEMBER *dest);
void parse_declare(TOKEN *first_token);
int parse_type(DECL_MEMBER *el_ptr, TOKEN *token);

//declout.c
void out_pointer(TOKEN *name, BOOLEAN use_parens);
void out_bound(TOKEN *bound);
void out_decl_type(DECL_MEMBER *decl_ptr);
void out_struct(DECL_MEMBER *el_ptr);
void out_decl_member(DECL_MEMBER *decl_list, TOKEN *decl_token);
void out_decl(DECL *decl);

//error.c
void parse_error(char *error_string);
void parse_warning(char *warning_string);
void control_error(char *error_string);

//io.c
void out_data(char *string, int length);
void out_white_space(TOKEN *token);
void out_must_white(TOKEN *token);
void out_pre_line(TOKEN *token);
void out_pre_white(TOKEN *token);
void out_token_name(TOKEN *token);
void out_token(TOKEN *token);
void out_must_token(TOKEN *token);
void out_cvt_name(TOKEN *token);
void out_str(char *string);
void out_char(char ch);
void out_to_start(void);
void out_type(int type);
void out_init(void);
void out_str_const(char *str_ptr, int len);
void cvt_octal(TOKEN *token, char octal_string[]);

//main.c
void cvt_file(char *file_name);

//mem.c
void *get_mem(size_t size);
void get_context_ptr(CONTEXT **context);
void get_token_ptr(TOKEN **token);
void get_var_ptr(DECL_ID **var);
void free_var_list(DECL_ID *list_ptr);
void get_element_ptr(DECL_MEMBER **element);
void free_decl_list(DECL_MEMBER *element);
void get_param_ptr(PARAM_LIST **param);
void free_param_list(PARAM_LIST *param_list);
void get_decl_ptr(DECL **decl);
void free_decl(DECL *decl_list);

//parse.c
void get_param_list(PARAM_LIST **param_head);
void parse_param_list(PARAM_LIST *param_list, DECL **decl_list, DECL **extra_decl_list);
void parse_till(int type, TOKEN *token);
void parse_till_end(TOKEN *token);
void parse_to_end(void);
void check_eol(void);
int parse_simple_variable(TOKEN *token, TOKEN *next_token);
void out_ident(TOKEN *ident, DECL_MEMBER *decl, DECL_ID *decl_id);
int parse_member(TOKEN *token, DECL_MEMBER *decl, DECL_ID *decl_id);
int parse_variable(TOKEN *token, DECL_MEMBER **var_decl, DECL_ID **var_decl_id);
int check_cvt_id(TOKEN *token, CVT_ID *cvt_id, char **cvt_string);
int parse_function(TOKEN *token);
int parse_expression(TOKEN *token);
void parse_do(TOKEN *first_token);
void parse_end(TOKEN *first_token);
void parse_if(TOKEN *first_token);
void parse_then(void);
void parse_else(TOKEN *first_token);
void parse_goto(TOKEN *first_token);
void parse_go(TOKEN *first_token);
void parse_call(TOKEN *first_token);
void parse_return(TOKEN *first_token);
void parse_identifier(TOKEN *first_token);
void parse_label(void);
void parse_eol(TOKEN *first_token);
void parse_int_ctl(TOKEN *first_token);
void parse_outport(void);
void parse_output(TOKEN *first_token);
void parse_outword(TOKEN *first_token);
void parse_outhword(TOKEN *first_token);

//token.c
int get_token(TOKEN	*token);
void token_copy(TOKEN *src, TOKEN *dest);