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

typedef struct TOKEN TOKEN;
typedef struct DECL_MEMBER DECL_MEMBER;
typedef struct DECL DECL;
//FIXME this saves including a protos.h file in *.c

//context.c
void init_context(void);

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

//mem.c
void *getmem(size_t);
void get_token_ptr(TOKEN **token);

//token.c
int get_token(TOKEN	*token);
void token_copy(TOKEN *src, TOKEN *dest);