typedef union
{
    int		number;
    char	*string;
    struct rav	*var;
    struct edon	*node;
} YYSTYPE;
#define	TYPE	258
#define	ACCEPT	259
#define	MATCH	260
#define	JUMP	261
#define	USE	262
#define	COMPUTE	263
#define	GRAPHIC	264
#define	KFILE	265
#define	REMARK	266
#define	END	267
#define	YES	268
#define	NO	269
#define	JUMPMATCH	270
#define	TYPEH	271
#define	PROBLEM	272
#define	PAUSE	273
#define	LINK	274
#define	CLEARHOME	275
#define	CURSADDR	276
#define	CLEARLINE	277
#define	CLEAREND	278
#define	SYSTEM	279
#define	NUMBER	280
#define	CHAR	281
#define	SPECIAL	282
#define	STRING	283
#define	ALLOC	284
#define	NUMERIC_IDENT	285
#define	STRING_IDENT	286
#define	LABEL	287
#define	NEWLINE	10
#define	COLON	288
#define	LPAREN	289
#define	RPAREN	290
#define	PLUS	291
#define	MINUS	292
#define	MULTIPLY	293
#define	DIVIDE	294
#define	MODULO	295
#define	EQUAL	296
#define	LESS	297
#define	GREATER	298
#define	BANG	299
#define	ORBAR	300
#define	COMMA	301
#define	NEQUAL	302
#define	NOTGRT	303
#define	NOTLESS	304
#define	UMINUS	305
#define	ANDAND	306
#define	LEXERR	307


extern YYSTYPE yylval;
