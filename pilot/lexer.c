# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX 200
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
/*****************************************************************************

NAME
    pilot.l -- lexical analysis for IEEE PILOT

DESCRIPTION
   This module provides a yylex() suitable for a PILOT interpreter or
compiler.  Before the first call to yylex(), the initialization
function yyinit() should be invoked.

   It expects to be able to call a makevar() routine that enders an ID
of a given type in a symbol table.  It also expects to be able to call
an options() function to interpret directives.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The author retains
copyright on this implementation on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to make
money with this or any code derived from it, check with the author first.

******************************************************************************/
/*
 *	$Id: pilot.l,v 1.12 1996/10/07 22:38:22 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: pilot.l,v 1.12 1996/10/07 22:38:22 esr Exp $";
#endif /* lint */

/*LINTLIBRARY*/
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "pilot.h"
#include "gencode.h"

#define NOMEM	"out of memory\n"
#define BADCHAR	"lexical error, token buffer is: "
#define NOCONT	"colon continuation is not supported in IEEE 1154-1991\n"
#define SYNERR	"syntax or lexical error in %s\n"
#define NOQUERY	"? not permitted in program files"
#define NOSHELL	"! shell escape not permitted in program files"

#define NONSTANDARD	if (pedantic) \
			{ \
				fputs("pilot: non-IEEE feature\n", stderr); \
				exit(0); \
			}
#define SYSVAR(n)	yylval.var = makevar(n, yytext); return(n)

#define YY_USE_PROTOS	/* for FLEX */

/*******************************************************************
 *
 * Lexical analysis
 *
 ******************************************************************/

bool	multerr;		/* allow multiple errors per parse? */

/* used for error tracking */
int yyerrors;			/* error count */
int yylineno;			/* source line count */
char yyfile[PATH_MAX];		/* current source file */

/*
 * These funky state variables are necessary because PILOT isn't
 * really a free-format, token-based language.
 *
 * In order to handle the continuation syntax properly, the lexer needs to
 * con the parser into thinking it sees the last keyword when none is 
 * supplied.
 */
int keyword;			/* last keyword processed */
bool continuation;		/* syntactic continuation? */

static bool lexcont = TRUE;	/* seen a command keyword on this line? */
/*
 * Things get really weird near strings; the lexer sometimes needs to be told
 * to enter a different state (which, among other things, doesn't skip
 * whitespace) after a colon or gets.
 */
static bool hastxt = TRUE;	/* expecting trailing text after : or = ? */

#ifndef YY_NULL
/* we're in lex */
#define IS_STATE(n) (yybgin == yysvec + 1 + (n))	/* test lexer state */
#undef input
#define input	pinput
#undef unput
#define unput	punput
#define yylex	gettok	/* sneaky way to change yylex()'s name */
#else
/* we're in flex */
#ifndef YYLMAX
#define YYLMAX	20000
#endif
static char yysbuf[YYLMAX], *yysptr = yysbuf;
#define IS_STATE(n)	(YY_START == (n))
#undef YY_INPUT
#define YY_INPUT	pinput
#undef unput
#define unput	punput
#define YY_DECL int gettok(void)
#endif

void strlwr(char *s)
/* force string to lower case */
{
    register char *cp;

    for (cp = s; *cp; cp++)
	*cp = tolower(*cp);
}

/*
 * The production for the second form of string identifier token should
 * really have been written {ID}\$, but this tickles some obscure bug
 * in lex.  We settle for [a-zA-Z][A-Za-z0-9]+\$ instead.
 */

extern void dump_string(char *, FILE *);
extern variable *makevar();
extern char *strsave();

/* actions to take on encountering a keyword */
#define KEYWORD(n)	{lexcont = FALSE; return(n);}

# define start 2
# define expression 4
# define literal 6
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	{KEYWORD(keyword = TYPE);}
break;
case 2:
	{hastxt = FALSE; KEYWORD(keyword = ACCEPT);}
break;
case 3:
	{KEYWORD(keyword = MATCH);}
break;
case 4:
	{hastxt = FALSE; KEYWORD(keyword = JUMP);}
break;
case 5:
	{hastxt = FALSE; KEYWORD(keyword = USE);}
break;
case 6:
{hastxt = FALSE; KEYWORD(keyword = COMPUTE);}
break;
case 7:
{KEYWORD(keyword = GRAPHIC);}
break;
case 8:
	{KEYWORD(keyword = KFILE);}
break;
case 9:
	{KEYWORD(keyword = REMARK);}
break;
case 10:
	{hastxt = FALSE; KEYWORD(keyword = END);}
break;
case 11:
{
    if (!lexcont)		/* Y command */
	KEYWORD(YES)
    else
    {
	punput('y');
	lexcont = FALSE;
	return(keyword = TYPE);	/* equivalent to TY */
    }
}
break;
case 12:
{
    if (!lexcont)		/* N command */
	KEYWORD(NO)
    else
    {
	punput('n');
	lexcont = FALSE;
	return(keyword = TYPE);	/* equivalent to TN */
    }
}
break;
case 13:
	{hastxt = FALSE; KEYWORD(keyword = PAUSE);}
break;
case 14:
{KEYWORD(keyword = PROBLEM);}
break;
case 15:
	{KEYWORD(keyword = LINK);}
break;
case 16:
{hastxt = FALSE; KEYWORD(keyword = CLEARHOME);}
break;
case 17:
{hastxt = FALSE; KEYWORD(keyword = CURSADDR);}
break;
case 18:
{hastxt = FALSE; KEYWORD(keyword = CLEARLINE);}
break;
case 19:
{hastxt = FALSE; KEYWORD(keyword = CLEAREND);}
break;
case 20:
{hastxt = FALSE; KEYWORD(keyword = JUMPMATCH);}
break;
case 21:
{KEYWORD(keyword = TYPEH);}
break;
case 22:
{KEYWORD(keyword = SYSTEM);}
break;
case 23:
	{yylval.number = atoi(yytext); return(NUMBER);}
break;
case 24:
	{
    strlwr(yytext);
    yylval.var = makevar(LABEL, yytext + 1);
    if (IS_STATE(start))
    {
	yylval.var->v.label.addr = ftell(yyin)-strlen(yysbuf)+(yysptr-yysbuf);
	yylval.var->v.label.lineno = yylineno;
    }
    return(LABEL);
}
break;
case 25:
	{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = FALSE;
    if (keyword == JUMPMATCH && IS_STATE(expression))
    {
	yylval.var = makevar(LABEL, yytext);
	return(LABEL);
    }
    else
    {
	yylval.var = makevar(NUMERIC_IDENT, yytext);
	return(NUMERIC_IDENT);
    }
}
break;
case 26:
{
    if (lexcont)	/* no keyword seen on this line; return last one */
    {
	if (pedantic)
	    yyerror(NOCONT);
	lexcont = FALSE;
	punput(':');
	return(keyword);
    }
    else		/* we may need to begin literal parsing */
    {
	if (hastxt)
	    BEGIN literal;
	else
	    BEGIN expression;
	return(':');
    }
}
break;
case 27:
{
    if (keyword == COMPUTE && hastxt)
	BEGIN literal;
    else
	BEGIN expression;
    return(yytext[0]);
}
break;
case 28:
;
break;
case 29:
{return(yytext[0]);}
break;
case 30:
	{BEGIN expression; return(yytext[0]);}
break;
case 31:
{
    NONSTANDARD
    if (yyin == stdin)
	do_help();
    else
    {
	yyerror(NOQUERY);
	while (pinput() != '\n')
	    continue;
	return(LEXERR);
    }
}
break;
case 32:
{
    NONSTANDARD
    if (yyin == stdin)
	system(yytext + 1);
    else
    {
	yyerror(NOSHELL);
	while (pinput() != '\n')
	    continue;
	return(LEXERR);
    }
}
break;
case 33:
{NONSTANDARD options(yytext + 1);}
break;
case 34:
{NONSTANDARD yylval.number = MATCH; return(SPECIAL);}
break;
case 35:
{NONSTANDARD yylval.number = ACCEPT; return(SPECIAL);}
break;
case 36:
{NONSTANDARD yylval.number = PROBLEM; return(SPECIAL);}
break;
case 37:
{return(',');}
break;
case 38:
{
    yyerror(BADCHAR);
    yytext[1] = '\0';
    dump_string(yytext, stderr);
    (void) fputc('\n', stderr);
    while (pinput() != '\n')
	continue;
    return(LEXERR);
}
break;
case 39:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 40:
	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 41:
	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 42:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 43:
{NONSTANDARD; yylval.number = MAXUSES; return(NUMBER);}
break;
case 44:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 45:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 46:
	{NONSTANDARD; SYSVAR(STRING_IDENT);}
break;
case 47:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 48:
	{NONSTANDARD; SYSVAR(STRING_IDENT);}
break;
case 49:
	{NONSTANDARD; SYSVAR(STRING_IDENT);}
break;
case 50:
	{NONSTANDARD; SYSVAR(STRING_IDENT);}
break;
case 51:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 52:
{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 53:
	{NONSTANDARD; SYSVAR(STRING_IDENT);}
break;
case 54:
	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
break;
case 55:
	{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = TRUE;
    yylval.var = makevar(STRING_IDENT, yytext + 1);
    return(STRING_IDENT);
}
break;
case 56:
	{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = TRUE;
    yytext[strlen(yytext) - 1] = '\0';
    yylval.var = makevar(STRING_IDENT, yytext);
    return(STRING_IDENT);
}
break;
case 57:
	{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = FALSE;
    yylval.var = makevar(NUMERIC_IDENT, yytext+1);
    return(NUMERIC_IDENT);
}
break;
case 58:
	{NONSTANDARD yylval.number = '\n'; return(CHAR);}
break;
case 59:
	{NONSTANDARD yylval.number = '\t'; return(CHAR);}
break;
case 60:
	{NONSTANDARD yylval.number = '\b'; return(CHAR);}
break;
case 61:
	{NONSTANDARD yylval.number = '\r'; return(CHAR);}
break;
case 62:
	{NONSTANDARD yylval.number = '\007'; return(CHAR);}
break;
case 63:
{NONSTANDARD yylval.number=(16*yytext[2]+yytext[3]); return(CHAR);}
break;
case 64:
	{NONSTANDARD yylval.number = yytext[2] & 0x1f; return(CHAR);}
break;
case 65:
	{NONSTANDARD yylval.number = yytext[1]; return(CHAR);}
break;
case 66:
{
    NONSTANDARD {
	while (pinput() != '\n')
	    continue;
	punput('\n');
    }
}
break;
case 67:
	{yylval.string = strsave(yytext); return(ALLOC);}
break;
case 68:
	{yylval.number = yytext[0]; return(CHAR);}
break;
case 69:
{
    /* reset all error-tracking and syntax-state variables */
    lexcont = TRUE;		/* so we catch omitted keyword */
    hastxt = TRUE;		/* assume we're looking for trailing text */
    BEGIN start;		/* start next line in tokenizing state */
    return(NEWLINE);
}
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */
#include <ctype.h>

void yyinit(char *file)
/* make sure the lexer begins in tokenizing state */
{
    if (file)
	(void) strcpy(yyfile, file);	
    BEGIN start;
    hastxt = TRUE;
    yylineno = 0;
}

/*******************************************************************
 *
 * lex(1) library surrogates
 *
 ******************************************************************/

#undef yylex
int yylex(void)
/* sneaky way of adding instrumentation... */
{
    int	class = gettok();

    if (verbose > 1)
	switch (class)
	{
	case TYPE:	(void) printf("TYPE keyword\n"); break;
	case ACCEPT:	(void) printf("ACCEPT keyword\n"); break;
	case MATCH:	(void) printf("MATCH keyword\n"); break;
	case JUMP:	(void) printf("JUMP keyword\n"); break;
	case USE:	(void) printf("USE keyword\n"); break;
	case COMPUTE:	(void) printf("COMPUTE keyword\n"); break;
	case GRAPHIC:	(void) printf("GRAPHIC keyword\n"); break;
	case KFILE:	(void) printf("FILE keyword\n"); break;
	case REMARK:	(void) printf("REMARK keyword\n"); break;
	case END:	(void) printf("END keyword\n"); break;
	case YES:	(void) printf("YES keyword\n"); break;
	case NO:	(void) printf("NO keyword\n"); break;
	case JUMPMATCH:	(void) printf("JUMPMATCH keyword\n"); break;
	case TYPEH:	(void) printf("TYPEH keyword\n"); break;
	case PROBLEM:	(void) printf("PROBLEM keyword\n"); break;
	case PAUSE:	(void) printf("PAUSE keyword\n"); break;
	case LINK:	(void) printf("LINK keyword\n"); break;
	case CLEARHOME:	(void) printf("CLEARHOME keyword\n"); break;
	case CURSADDR:	(void) printf("CURSADDR keyword\n"); break;
	case CLEARLINE:	(void) printf("CLEARLINE keyword\n"); break;
	case CLEAREND:	(void) printf("CLEAREND keyword\n"); break;
	case SYSTEM:	(void) printf("SYSTEM keyword\n"); break;

	case LABEL:
	    (void) printf("LABEL: %s, line %d\n",
			  yylval.var->name, yylval.var->v.label.lineno);
	    break;

	case NUMBER:
	    (void) printf("NUMBER: %d\n", yylval.number);
	    break;

	case CHAR:
	    (void) printf("CHAR: '%c'\n", yylval.number);
	    break;

	case STRING:
	    (void) printf("STRING: \"%s\"\n", yylval.string);
	    break;

	case ALLOC:
	    (void) printf("ALLOC: \"%s\"\n", yylval.string);
	    break;

	case NUMERIC_IDENT:
	    (void) printf("NUMERIC_IDENT: %s\n", yylval.var->name);
	    break;

	case STRING_IDENT:
	    (void) printf("STRING_IDENT: %s\n", yylval.var->name);
	    break;

	case NEWLINE:
	    (void) printf("NEWLINE\n");
	    break;

	case COLON:
	    (void) printf("COLON\n");
	    break;

	case LEXERR:
	    (void) printf("LEXERR\n");
	    break;

	default:
	    (void) printf("Randomness (class %d): \"%c\"\n", class, class);
	    break;
	}

    return(class);
}

int pinput()
/* redefine input to smash case when appropriate */
{
    if (*yysptr == '\0')
	if (fgets(yysbuf, YYLMAX, yyin) == (char *)NULL)
	    return(0);
	else
	{
	    if (verbose >= 1 && yyin != stdin)
		(void) fprintf(stderr, "%03d: %s", yylineno, yysbuf);
	    
	    for (yysptr = yysbuf; *yysptr && *yysptr == ' '; yysptr++)
		continue;
	    continuation = (*yysptr == ':');
	    solhook(yysbuf);
	    yylineno++;
	}

    if (!IS_STATE(literal))
	return(tolower(*yysptr++));
    else
	return(*yysptr++);
}

int punput(int c)
{
    if (yysptr > yysbuf)
	return(*--yysptr = c);
    else
	return(ungetc(c, yyin));
}

int yyseek(long offset)
/* seek in the source file, nuking the pushback buffer */
{
    yysptr = yysbuf;
    *yysptr = '\0';
    return(fseek(yyin, offset, 0));
}

int yyerror(message, s1, s2, s3, s4)
/* emit syntax error message */
char	*message, *s1, *s2, *s3, *s4;
{
    yyerrors++;
    if (yyin == stdin)
    {
	int	i;

	for (i = 0; i < yysptr - yysbuf - 1; i++)
	    (void) putc(' ', stderr);
	(void) putc('^', stderr);
	(void) putc('\n', stderr);
	(void) fprintf(stderr, "pilot: ");
    }
    else
	(void) fprintf(stderr, "\"%s\", line %d, char %d: ",
		       yyfile, yylineno, yysptr - yysbuf);
    if (strcmp(message, "syntax error"))
	(void) fprintf(stderr, message, s1, s2, s3, s4);
    else
	(void) fprintf(stderr, SYNERR,
	       IS_STATE(start) ? "keyword part" : (IS_STATE(expression) ? "expression" : "literal"));
    if (!multerr)
	exit(0);
}

int yywrap()
{
    if (yyin == stdin)
	(void) putchar('\n');
    return(EOF);
}

char *strsave(char *s)
/* save an allocated copy of a string */
{
    char	*m = malloc(strlen(s) + 1);

    if (m == (char *)NULL)
    {
	(void) fprintf(stderr, NOMEM);
	exit(1);
    }
    else
    {
	(void) strcpy(m, s);
	return(m);
    }
}

/*******************************************************************
 *
 * Error reporting
 *
 ******************************************************************/

void dump_string(char *str, FILE *fp)
{
    register char *cp;

    for (cp = str; *cp; cp++)
	if (isprint(*cp))
	    (void) putchar(*cp);
	else if (*cp == '"')
	    (void) fputs("\\\"", fp);
	else if (*cp == '\b')
	    (void) fputs("\\b", fp);
	else if (*cp == '\033')
	    (void) fputs("\\e", fp);
	else if (*cp == '\n')
	    (void) fputs("\\n", fp);
	else if (*cp == '\r')
	    (void) fputs("\\r", fp);
	else if (*cp == '\t')
	    (void) fputs("\\t", fp);
	else if (*cp == '\v')
	    (void) fputs("\\v", fp);
	else
	    (void) fprintf(fp, "\\x%02x", *cp);
}

/*
 The following sets edit modes for GNU EMACS
 Local Variables:
 mode:c
 End:
*/
/* pilot.l ends here */
int yyvstop[] = {
0,

69,
0,

38,
0,

28,
38,
0,

38,
0,

38,
0,

29,
38,
0,

30,
38,
0,

29,
38,
0,

29,
38,
0,

37,
38,
0,

29,
38,
0,

26,
38,
0,

27,
38,
0,

38,
0,

2,
38,
0,

6,
38,
0,

10,
38,
0,

8,
38,
0,

7,
38,
0,

4,
38,
0,

15,
38,
0,

3,
38,
0,

12,
38,
0,

38,
0,

9,
38,
0,

38,
0,

1,
38,
0,

5,
38,
0,

38,
0,

11,
38,
0,

38,
0,

38,
0,

31,
38,
0,

23,
38,
0,

38,
0,

25,
38,
0,

68,
0,

68,
0,

67,
68,
0,

67,
68,
0,

68,
0,

68,
0,

68,
0,

68,
0,

67,
68,
0,

68,
0,

57,
0,

55,
0,

66,
0,

24,
0,

17,
0,

19,
0,

16,
0,

18,
0,

20,
0,

12,
0,

13,
0,

14,
0,

21,
0,

22,
0,

32,
0,

33,
0,

57,
0,

23,
0,

35,
0,

34,
0,

36,
0,

25,
0,

67,
0,

67,
0,

67,
0,

65,
0,

65,
0,

62,
65,
0,

60,
65,
0,

58,
65,
0,

61,
65,
0,

59,
65,
0,

65,
0,

56,
0,

10,
0,

5,
0,

11,
0,

64,
0,

8,
0,

4,
0,

15,
0,

1,
0,

63,
0,

48,
0,

40,
0,

53,
0,

3,
0,

13,
0,

49,
0,

50,
0,

2,
0,

9,
0,

46,
0,

41,
0,

54,
0,

6,
0,

7,
0,

14,
0,

47,
0,

43,
0,

45,
0,

44,
0,

52,
0,

42,
0,

18,
0,

51,
0,

39,
0,
0};
# define YYTYPE int
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,9,	1,10,	
9,9,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,9,	0,0,	9,9,	
1,11,	1,12,	1,13,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
1,14,	14,73,	9,14,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
74,133,	77,133,	78,133,	79,133,	
86,133,	0,0,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
1,15,	1,15,	1,15,	1,15,	
3,16,	29,76,	31,83,	32,84,	
33,85,	8,55,	8,56,	34,86,	
3,17,	3,10,	35,88,	8,51,	
8,51,	8,51,	36,89,	34,87,	
37,90,	8,58,	39,93,	40,94,	
38,91,	41,95,	42,97,	43,98,	
44,99,	63,121,	64,122,	65,123,	
66,124,	67,125,	8,51,	3,17,	
8,51,	3,16,	3,18,	3,19,	
3,20,	38,92,	41,96,	3,21,	
3,22,	3,23,	3,22,	3,24,	
3,22,	68,126,	3,25,	3,16,	
69,127,	71,131,	70,129,	72,132,	
69,128,	80,133,	76,133,	81,133,	
88,133,	3,26,	83,133,	3,22,	
3,27,	3,22,	8,60,	4,45,	
3,28,	4,46,	4,19,	90,133,	
49,106,	70,130,	4,21,	4,22,	
4,23,	4,22,	4,24,	30,77,	
82,133,	4,25,	95,133,	30,78,	
49,107,	84,133,	30,79,	49,108,	
89,133,	87,133,	30,80,	85,133,	
4,26,	30,81,	4,22,	4,27,	
4,22,	4,47,	91,133,	30,82,	
3,29,	98,133,	3,30,	92,133,	
3,31,	3,32,	3,33,	93,133,	
96,133,	3,34,	97,133,	3,35,	
3,36,	3,37,	102,102,	3,38,	
104,104,	3,39,	3,40,	3,41,	
3,42,	76,134,	80,135,	3,43,	
3,44,	109,133,	83,138,	121,153,	
122,154,	123,155,	5,16,	4,29,	
81,136,	4,30,	88,142,	4,31,	
4,32,	4,33,	5,17,	5,10,	
4,34,	124,156,	4,35,	4,36,	
4,37,	126,159,	4,38,	94,133,	
4,39,	4,40,	4,41,	4,42,	
85,140,	128,162,	4,43,	4,44,	
99,133,	84,139,	82,137,	127,160,	
129,163,	5,17,	87,141,	5,16,	
5,18,	5,19,	5,20,	127,161,	
89,143,	5,21,	5,22,	5,23,	
5,22,	5,24,	5,22,	97,149,	
5,25,	5,48,	92,145,	91,144,	
93,146,	125,157,	130,164,	132,167,	
96,148,	125,158,	131,165,	5,26,	
138,133,	5,22,	5,27,	5,22,	
131,166,	5,49,	5,50,	6,18,	
6,19,	7,51,	135,133,	146,133,	
6,21,	6,22,	6,23,	6,22,	
6,24,	7,52,	7,10,	6,25,	
48,105,	48,105,	48,105,	48,105,	
48,105,	48,105,	48,105,	48,105,	
48,105,	48,105,	6,26,	149,133,	
6,22,	6,27,	6,22,	150,133,	
6,49,	134,133,	94,147,	139,133,	
7,53,	145,133,	7,54,	7,55,	
7,56,	7,57,	153,183,	99,150,	
154,184,	7,51,	7,51,	7,51,	
155,185,	7,54,	156,186,	7,58,	
7,51,	61,61,	61,61,	61,61,	
61,61,	61,61,	61,61,	61,61,	
61,61,	61,61,	61,61,	157,187,	
7,51,	158,188,	7,51,	135,169,	
146,179,	7,59,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
7,60,	159,189,	134,168,	145,178,	
139,172,	160,190,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
11,61,	11,61,	11,61,	11,61,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	143,133,	148,133,	
161,191,	162,192,	163,193,	164,194,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	12,62,	12,62,	
12,62,	12,62,	13,63,	165,195,	
166,196,	167,197,	13,64,	13,65,	
171,133,	172,133,	175,133,	183,210,	
184,211,	13,66,	13,67,	13,68,	
185,212,	187,213,	188,214,	13,69,	
13,70,	13,71,	13,72,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	143,176,	189,215,	190,216,	
148,181,	191,217,	192,218,	193,219,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	194,220,	197,221,	
204,133,	171,201,	205,133,	210,233,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	15,74,	15,74,	
15,74,	15,74,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
142,133,	136,133,	137,133,	144,133,	
207,133,	173,133,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
23,75,	23,75,	23,75,	23,75,	
45,100,	177,133,	180,133,	211,234,	
46,102,	212,235,	213,236,	214,237,	
45,100,	45,101,	215,238,	216,239,	
46,102,	46,103,	62,62,	62,62,	
62,62,	62,62,	62,62,	62,62,	
62,62,	62,62,	62,62,	62,62,	
140,133,	174,133,	217,240,	176,133,	
181,133,	219,241,	202,133,	45,100,	
206,133,	45,100,	220,242,	46,102,	
45,100,	46,102,	221,243,	142,175,	
46,102,	173,202,	222,133,	207,231,	
45,100,	136,170,	231,133,	45,100,	
46,102,	137,171,	144,177,	46,102,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	234,253,	236,254,	
45,100,	237,255,	177,205,	180,208,	
46,104,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	176,204,	
181,209,	206,230,	174,203,	202,228,	
140,173,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	50,109,	
50,109,	50,109,	50,109,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,9,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,110,	53,0,	
53,111,	53,0,	53,0,	53,0,	
141,133,	147,133,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,111,	
53,0,	53,14,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	53,0,	53,0,	53,0,	
53,0,	203,133,	53,0,	53,0,	
53,0,	53,0,	209,133,	53,111,	
75,75,	75,75,	75,75,	75,75,	
75,75,	75,75,	75,75,	75,75,	
75,75,	75,75,	120,152,	120,152,	
120,152,	120,152,	120,152,	120,152,	
120,152,	120,152,	120,152,	120,152,	
225,133,	238,256,	239,257,	241,259,	
243,260,	53,0,	53,0,	53,0,	
228,133,	247,133,	54,111,	249,133,	
54,111,	251,133,	253,264,	256,265,	
54,111,	54,111,	257,266,	259,267,	
260,268,	264,270,	267,271,	54,111,	
269,133,	270,272,	168,133,	0,0,	
141,174,	0,0,	0,0,	0,0,	
0,0,	147,180,	203,229,	0,0,	
0,0,	54,111,	53,0,	209,232,	
0,0,	53,0,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	225,246,	0,0,	228,249,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	54,111,	54,111,	
54,111,	54,111,	168,198,	54,111,	
54,111,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,0,	59,0,	59,0,	
59,111,	59,0,	59,111,	59,0,	
59,0,	59,0,	169,133,	170,133,	
59,0,	59,0,	59,0,	59,0,	
59,0,	59,111,	59,0,	59,0,	
59,74,	178,133,	198,133,	179,133,	
208,133,	200,133,	199,133,	250,133,	
229,133,	223,133,	59,0,	201,133,	
59,0,	59,0,	59,0,	59,0,	
60,113,	59,112,	224,133,	230,133,	
114,151,	226,133,	232,133,	227,133,	
60,113,	60,0,	244,133,	252,133,	
114,151,	114,0,	152,182,	152,182,	
152,182,	152,182,	152,182,	152,182,	
152,182,	152,182,	152,182,	152,182,	
248,133,	245,133,	246,133,	59,0,	
59,0,	59,0,	261,133,	60,113,	
0,0,	60,113,	262,133,	114,151,	
60,113,	114,151,	263,133,	0,0,	
114,151,	0,0,	0,0,	0,0,	
60,113,	0,0,	0,0,	60,113,	
114,151,	0,0,	0,0,	114,151,	
169,199,	0,0,	250,263,	199,223,	
170,200,	178,206,	199,224,	201,227,	
59,0,	208,98,	199,225,	59,0,	
60,113,	179,207,	198,222,	223,244,	
114,151,	200,226,	226,247,	227,248,	
229,250,	0,0,	244,78,	0,0,	
230,251,	224,245,	252,95,	0,0,	
232,252,	240,258,	240,258,	240,258,	
240,258,	240,258,	240,258,	240,258,	
240,258,	240,258,	240,258,	0,0,	
0,0,	60,114,	0,0,	261,79,	
60,115,	60,116,	245,261,	262,269,	
246,262,	0,0,	248,77,	0,0,	
0,0,	0,0,	263,86,	0,0,	
0,0,	60,117,	0,0,	0,0,	
0,0,	60,118,	0,0,	60,119,	
0,0,	0,0,	0,0,	60,120,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
0,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	0,0,	
110,0,	0,0,	110,0,	110,0,	
110,0,	0,0,	0,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
0,0,	110,0,	110,14,	110,0,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	110,0,	110,0,	
110,0,	110,0,	0,0,	110,0,	
110,0,	110,0,	110,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	110,0,	110,0,	
110,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	110,0,	
0,0,	0,0,	110,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	112,0,	
112,0,	112,0,	0,0,	112,0,	
0,0,	112,0,	112,133,	112,0,	
0,0,	0,0,	112,0,	112,0,	
112,0,	112,0,	112,0,	0,0,	
112,0,	112,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
112,0,	0,0,	112,0,	112,0,	
112,0,	112,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	112,0,	112,0,	112,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	112,0,	0,0,	
0,0,	112,0,	0,0,	0,0,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+1,	0,		0,	
yycrank+0,	yysvec+1,	0,	
yycrank+-123,	0,		0,	
yycrank+-154,	yysvec+3,	0,	
yycrank+-249,	0,		0,	
yycrank+-280,	yysvec+5,	0,	
yycrank+-316,	0,		0,	
yycrank+-94,	yysvec+7,	0,	
yycrank+3,	0,		0,	
yycrank+0,	0,		yyvstop+1,
yycrank+317,	0,		0,	
yycrank+375,	0,		0,	
yycrank+401,	0,		0,	
yycrank+2,	0,		0,	
yycrank+471,	0,		0,	
yycrank+0,	0,		yyvstop+3,
yycrank+0,	yysvec+9,	yyvstop+5,
yycrank+0,	yysvec+11,	yyvstop+8,
yycrank+0,	yysvec+12,	yyvstop+10,
yycrank+0,	yysvec+13,	yyvstop+12,
yycrank+0,	0,		yyvstop+15,
yycrank+0,	0,		yyvstop+18,
yycrank+529,	0,		yyvstop+21,
yycrank+0,	0,		yyvstop+24,
yycrank+0,	yysvec+14,	yyvstop+27,
yycrank+0,	0,		yyvstop+30,
yycrank+0,	0,		yyvstop+33,
yycrank+0,	yysvec+15,	yyvstop+36,
yycrank+26,	yysvec+15,	yyvstop+38,
yycrank+102,	yysvec+15,	yyvstop+41,
yycrank+16,	yysvec+15,	yyvstop+44,
yycrank+22,	yysvec+15,	yyvstop+47,
yycrank+14,	yysvec+15,	yyvstop+50,
yycrank+22,	yysvec+15,	yyvstop+53,
yycrank+29,	yysvec+15,	yyvstop+56,
yycrank+41,	yysvec+15,	yyvstop+59,
yycrank+29,	yysvec+15,	yyvstop+62,
yycrank+47,	yysvec+15,	yyvstop+65,
yycrank+41,	yysvec+15,	yyvstop+67,
yycrank+22,	yysvec+15,	yyvstop+70,
yycrank+41,	yysvec+15,	yyvstop+72,
yycrank+31,	yysvec+15,	yyvstop+75,
yycrank+32,	yysvec+15,	yyvstop+78,
yycrank+47,	yysvec+15,	yyvstop+80,
yycrank+-651,	0,		yyvstop+83,
yycrank+-655,	0,		yyvstop+85,
yycrank+0,	0,		yyvstop+87,
yycrank+280,	0,		yyvstop+90,
yycrank+95,	0,		yyvstop+93,
yycrank+656,	0,		yyvstop+95,
yycrank+0,	0,		yyvstop+98,
yycrank+0,	yysvec+9,	yyvstop+100,
yycrank+-778,	0,		yyvstop+102,
yycrank+842,	0,		yyvstop+105,
yycrank+0,	yysvec+11,	yyvstop+108,
yycrank+0,	yysvec+12,	yyvstop+110,
yycrank+0,	yysvec+13,	yyvstop+112,
yycrank+0,	yysvec+14,	yyvstop+114,
yycrank+-968,	0,		yyvstop+116,
yycrank+-1031,	0,		yyvstop+119,
yycrank+317,	yysvec+11,	yyvstop+121,
yycrank+618,	yysvec+12,	yyvstop+123,
yycrank+39,	0,		0,	
yycrank+30,	0,		0,	
yycrank+54,	0,		0,	
yycrank+51,	0,		0,	
yycrank+56,	0,		0,	
yycrank+68,	0,		0,	
yycrank+71,	0,		0,	
yycrank+77,	0,		0,	
yycrank+72,	0,		0,	
yycrank+60,	0,		0,	
yycrank+0,	0,		yyvstop+125,
yycrank+56,	yysvec+15,	0,	
yycrank+796,	yysvec+23,	yyvstop+127,
yycrank+142,	yysvec+15,	0,	
yycrank+57,	yysvec+15,	yyvstop+129,
yycrank+58,	yysvec+15,	yyvstop+131,
yycrank+59,	yysvec+15,	yyvstop+133,
yycrank+141,	yysvec+15,	yyvstop+135,
yycrank+143,	yysvec+15,	0,	
yycrank+164,	yysvec+15,	0,	
yycrank+146,	yysvec+15,	0,	
yycrank+169,	yysvec+15,	0,	
yycrank+175,	yysvec+15,	0,	
yycrank+60,	yysvec+15,	yyvstop+137,
yycrank+173,	yysvec+15,	0,	
yycrank+144,	yysvec+15,	0,	
yycrank+172,	yysvec+15,	0,	
yycrank+155,	yysvec+15,	yyvstop+139,
yycrank+182,	yysvec+15,	yyvstop+141,
yycrank+187,	yysvec+15,	yyvstop+143,
yycrank+191,	yysvec+15,	0,	
yycrank+231,	yysvec+15,	0,	
yycrank+166,	yysvec+15,	yyvstop+145,
yycrank+192,	yysvec+15,	0,	
yycrank+194,	yysvec+15,	0,	
yycrank+185,	yysvec+15,	yyvstop+147,
yycrank+240,	yysvec+15,	0,	
yycrank+0,	yysvec+45,	0,	
yycrank+0,	0,		yyvstop+149,
yycrank+-169,	yysvec+46,	0,	
yycrank+0,	0,		yyvstop+151,
yycrank+-188,	yysvec+46,	yyvstop+153,
yycrank+0,	yysvec+48,	yyvstop+155,
yycrank+0,	0,		yyvstop+157,
yycrank+0,	0,		yyvstop+159,
yycrank+0,	0,		yyvstop+161,
yycrank+209,	yysvec+50,	yyvstop+163,
yycrank+-1151,	yysvec+53,	yyvstop+165,
yycrank+0,	yysvec+54,	yyvstop+167,
yycrank+-1278,	yysvec+59,	yyvstop+169,
yycrank+0,	0,		yyvstop+171,
yycrank+-1035,	0,		yyvstop+173,
yycrank+0,	0,		yyvstop+175,
yycrank+0,	0,		yyvstop+178,
yycrank+0,	0,		yyvstop+181,
yycrank+0,	0,		yyvstop+184,
yycrank+0,	0,		yyvstop+187,
yycrank+806,	0,		yyvstop+190,
yycrank+132,	0,		0,	
yycrank+136,	0,		0,	
yycrank+150,	0,		0,	
yycrank+159,	0,		0,	
yycrank+185,	0,		0,	
yycrank+145,	0,		0,	
yycrank+171,	0,		0,	
yycrank+170,	0,		0,	
yycrank+164,	0,		0,	
yycrank+205,	0,		0,	
yycrank+192,	0,		0,	
yycrank+202,	0,		0,	
yycrank+0,	0,		yyvstop+192,
yycrank+309,	yysvec+15,	0,	
yycrank+282,	yysvec+15,	0,	
yycrank+585,	yysvec+15,	0,	
yycrank+586,	yysvec+15,	0,	
yycrank+272,	yysvec+15,	yyvstop+194,
yycrank+311,	yysvec+15,	0,	
yycrank+640,	yysvec+15,	0,	
yycrank+780,	yysvec+15,	0,	
yycrank+584,	yysvec+15,	0,	
yycrank+430,	yysvec+15,	0,	
yycrank+587,	yysvec+15,	0,	
yycrank+313,	yysvec+15,	0,	
yycrank+283,	yysvec+15,	0,	
yycrank+781,	yysvec+15,	0,	
yycrank+431,	yysvec+15,	0,	
yycrank+303,	yysvec+15,	yyvstop+196,
yycrank+307,	yysvec+15,	yyvstop+198,
yycrank+0,	0,		yyvstop+200,
yycrank+998,	0,		0,	
yycrank+235,	0,		0,	
yycrank+242,	0,		0,	
yycrank+244,	0,		0,	
yycrank+246,	0,		0,	
yycrank+276,	0,		0,	
yycrank+260,	0,		0,	
yycrank+293,	0,		0,	
yycrank+316,	0,		0,	
yycrank+351,	0,		0,	
yycrank+365,	0,		0,	
yycrank+365,	0,		0,	
yycrank+355,	0,		0,	
yycrank+390,	0,		0,	
yycrank+384,	0,		0,	
yycrank+393,	0,		0,	
yycrank+854,	yysvec+15,	0,	
yycrank+970,	yysvec+15,	0,	
yycrank+971,	yysvec+15,	0,	
yycrank+468,	yysvec+15,	0,	
yycrank+469,	yysvec+15,	yyvstop+202,
yycrank+589,	yysvec+15,	0,	
yycrank+641,	yysvec+15,	yyvstop+204,
yycrank+470,	yysvec+15,	yyvstop+206,
yycrank+643,	yysvec+15,	0,	
yycrank+617,	yysvec+15,	0,	
yycrank+981,	yysvec+15,	0,	
yycrank+983,	yysvec+15,	0,	
yycrank+618,	yysvec+15,	0,	
yycrank+644,	yysvec+15,	yyvstop+208,
yycrank+0,	0,		yyvstop+210,
yycrank+406,	0,		0,	
yycrank+407,	0,		0,	
yycrank+401,	0,		0,	
yycrank+0,	0,		yyvstop+212,
yycrank+409,	0,		0,	
yycrank+399,	0,		0,	
yycrank+415,	0,		0,	
yycrank+415,	0,		0,	
yycrank+419,	0,		0,	
yycrank+418,	0,		0,	
yycrank+420,	0,		0,	
yycrank+445,	0,		0,	
yycrank+0,	0,		yyvstop+214,
yycrank+0,	0,		yyvstop+216,
yycrank+462,	0,		0,	
yycrank+982,	yysvec+15,	0,	
yycrank+986,	yysvec+15,	0,	
yycrank+985,	yysvec+15,	0,	
yycrank+991,	yysvec+15,	0,	
yycrank+646,	yysvec+15,	0,	
yycrank+801,	yysvec+15,	0,	
yycrank+528,	yysvec+15,	yyvstop+218,
yycrank+530,	yysvec+15,	yyvstop+220,
yycrank+648,	yysvec+15,	0,	
yycrank+588,	yysvec+15,	0,	
yycrank+984,	yysvec+15,	0,	
yycrank+806,	yysvec+15,	0,	
yycrank+453,	0,		0,	
yycrank+540,	0,		0,	
yycrank+543,	0,		0,	
yycrank+557,	0,		yyvstop+222,
yycrank+558,	0,		0,	
yycrank+546,	0,		0,	
yycrank+558,	0,		0,	
yycrank+568,	0,		0,	
yycrank+0,	0,		yyvstop+224,
yycrank+579,	0,		0,	
yycrank+571,	0,		0,	
yycrank+572,	0,		0,	
yycrank+658,	yysvec+15,	yyvstop+226,
yycrank+989,	yysvec+15,	0,	
yycrank+998,	yysvec+15,	0,	
yycrank+828,	yysvec+15,	0,	
yycrank+1001,	yysvec+15,	0,	
yycrank+1003,	yysvec+15,	0,	
yycrank+836,	yysvec+15,	0,	
yycrank+988,	yysvec+15,	0,	
yycrank+999,	yysvec+15,	0,	
yycrank+662,	yysvec+15,	yyvstop+228,
yycrank+1002,	yysvec+15,	0,	
yycrank+0,	0,		yyvstop+230,
yycrank+599,	0,		0,	
yycrank+0,	0,		yyvstop+232,
yycrank+615,	0,		0,	
yycrank+602,	0,		0,	
yycrank+756,	0,		0,	
yycrank+755,	0,		0,	
yycrank+1065,	0,		0,	
yycrank+762,	0,		0,	
yycrank+0,	0,		yyvstop+234,
yycrank+767,	0,		0,	
yycrank+1006,	yysvec+15,	0,	
yycrank+1021,	yysvec+15,	0,	
yycrank+1022,	yysvec+15,	0,	
yycrank+837,	yysvec+15,	yyvstop+236,
yycrank+1020,	yysvec+15,	0,	
yycrank+839,	yysvec+15,	yyvstop+238,
yycrank+987,	yysvec+15,	0,	
yycrank+841,	yysvec+15,	yyvstop+240,
yycrank+1007,	yysvec+15,	0,	
yycrank+773,	0,		0,	
yycrank+0,	0,		yyvstop+242,
yycrank+0,	0,		yyvstop+244,
yycrank+763,	0,		0,	
yycrank+772,	0,		0,	
yycrank+0,	yysvec+240,	yyvstop+246,
yycrank+782,	0,		0,	
yycrank+776,	0,		0,	
yycrank+1026,	yysvec+15,	0,	
yycrank+1030,	yysvec+15,	0,	
yycrank+1034,	yysvec+15,	0,	
yycrank+774,	0,		0,	
yycrank+0,	0,		yyvstop+248,
yycrank+0,	0,		yyvstop+250,
yycrank+786,	0,		0,	
yycrank+0,	0,		yyvstop+252,
yycrank+852,	yysvec+15,	yyvstop+254,
yycrank+779,	0,		0,	
yycrank+0,	0,		yyvstop+256,
yycrank+0,	0,		yyvstop+258,
0,	0,	0};
struct yywork *yytop = yycrank+1405;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
00  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,011 ,012 ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
01  ,01  ,01  ,01  ,01  ,01  ,01  ,01  ,
040 ,01  ,'"' ,01  ,01  ,'%' ,'"' ,'"' ,
01  ,'%' ,'%' ,'%' ,01  ,'-' ,01  ,'%' ,
'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,'0' ,
'0' ,'0' ,01  ,'"' ,'%' ,01  ,'%' ,01  ,
'"' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,01  ,01  ,01  ,'"' ,'"' ,
'"' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,'A' ,
'A' ,'A' ,'A' ,'"' ,01  ,'"' ,'"' ,01  ,
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
/*	@(#)ncform	1.2	*/
int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
