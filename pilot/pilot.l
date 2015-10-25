%{
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

%}

%p	2000
%a	2000
%o	2000
%s	start expression literal

ID		[a-zA-Z][a-zA-Z0-9]*	
NONSPECIALS	[ "&';@A-Z^_`a-z{}~-]+	

%%

<start>t|type		{KEYWORD(keyword = TYPE);}
<start>a|accept		{hastxt = FALSE; KEYWORD(keyword = ACCEPT);}
<start>m|match		{KEYWORD(keyword = MATCH);}
<start>j|jump		{hastxt = FALSE; KEYWORD(keyword = JUMP);}
<start>u|use		{hastxt = FALSE; KEYWORD(keyword = USE);}
<start>c|compute	{hastxt = FALSE; KEYWORD(keyword = COMPUTE);}
<start>g|graphic	{KEYWORD(keyword = GRAPHIC);}
<start>f|file		{KEYWORD(keyword = KFILE);}
<start>r|remark		{KEYWORD(keyword = REMARK);}
<start>e|end		{hastxt = FALSE; KEYWORD(keyword = END);}

<start>y|yes	{
    if (!lexcont)		/* Y command */
	KEYWORD(YES)
    else
    {
	punput('y');
	lexcont = FALSE;
	return(keyword = TYPE);	/* equivalent to TY */
    }
}
<start>n|no	{
    if (!lexcont)		/* N command */
	KEYWORD(NO)
    else
    {
	punput('n');
	lexcont = FALSE;
	return(keyword = TYPE);	/* equivalent to TN */
    }
}

<start>pa|pause		{hastxt = FALSE; KEYWORD(keyword = PAUSE);}
<start>pr|problem	{KEYWORD(keyword = PROBLEM);}
<start>l|link		{KEYWORD(keyword = LINK);}
<start>ch|clearhome	{hastxt = FALSE; KEYWORD(keyword = CLEARHOME);}
<start>ca|cursaddr	{hastxt = FALSE; KEYWORD(keyword = CURSADDR);}
<start>cl|clearline	{hastxt = FALSE; KEYWORD(keyword = CLEARLINE);}
<start>ce|clearend	{hastxt = FALSE; KEYWORD(keyword = CLEAREND);}
<start>jm|jumpmatch	{hastxt = FALSE; KEYWORD(keyword = JUMPMATCH);}
<start>th|typehang	{KEYWORD(keyword = TYPEH);}
<start>xs|system	{KEYWORD(keyword = SYSTEM);}

<expression>[0-9]+		{yylval.number = atoi(yytext); return(NUMBER);}

<start,expression>\*{ID}	{
    strlwr(yytext);
    yylval.var = makevar(LABEL, yytext + 1);
    if (IS_STATE(start))
    {
	yylval.var->v.label.addr = ftell(yyin)-strlen(yysbuf)+(yysptr-yysbuf);
	yylval.var->v.label.lineno = yylineno;
    }
    return(LABEL);
}

<expression>{ID}	{
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

<start,expression>:	{
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
<start,expression>=	{
    if (keyword == COMPUTE && hastxt)
	BEGIN literal;
    else
	BEGIN expression;
    return(yytext[0]);
}

<start,expression>[\ \t]	;

<start,expression>[)*/<>%+-]	{return(yytext[0]);}
<start,expression>\(		{BEGIN expression; return(yytext[0]);}

<start>^\?	{
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

<start>^!.*\n	{
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

<start>^#.*\n	{NONSTANDARD options(yytext + 1);}

<expression>@m	{NONSTANDARD yylval.number = MATCH; return(SPECIAL);}
<expression>@a	{NONSTANDARD yylval.number = ACCEPT; return(SPECIAL);}
<expression>@p	{NONSTANDARD yylval.number = PROBLEM; return(SPECIAL);}

<start,expression>\,	{return(',');}

<start,expression>.	{
    yyerror(BADCHAR);
    yytext[1] = '\0';
    dump_string(yytext, stderr);
    (void) fputc('\n', stderr);
    while (pinput() != '\n')
	continue;
    return(LEXERR);
}

%expression	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%term		{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%factor		{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%uselevel	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%maxuses	{NONSTANDARD; yylval.number = MAXUSES; return(NUMBER);}
%nextstmt	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%return[0-9]+	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%answer		{NONSTANDARD; SYSVAR(STRING_IDENT);}
%matched	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%left		{NONSTANDARD; SYSVAR(STRING_IDENT);}
%match		{NONSTANDARD; SYSVAR(STRING_IDENT);}
%right		{NONSTANDARD; SYSVAR(STRING_IDENT);}
%satisfied	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%relation	{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}
%text		{NONSTANDARD; SYSVAR(STRING_IDENT);}
%status		{NONSTANDARD; SYSVAR(NUMERIC_IDENT);}

\${ID}	{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = TRUE;
    yylval.var = makevar(STRING_IDENT, yytext + 1);
    return(STRING_IDENT);
}
[a-zA-Z][A-Za-z0-9]+\$		{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = TRUE;
    yytext[strlen(yytext) - 1] = '\0';
    yylval.var = makevar(STRING_IDENT, yytext);
    return(STRING_IDENT);
}
#{ID}	{
    strlwr(yytext);
    if (keyword == COMPUTE)
	hastxt = FALSE;
    yylval.var = makevar(NUMERIC_IDENT, yytext+1);
    return(NUMERIC_IDENT);
}

<literal>\\n		{NONSTANDARD yylval.number = '\n'; return(CHAR);}
<literal>\\t		{NONSTANDARD yylval.number = '\t'; return(CHAR);}
<literal>\\b		{NONSTANDARD yylval.number = '\b'; return(CHAR);}
<literal>\\r		{NONSTANDARD yylval.number = '\r'; return(CHAR);}
<literal>\\a		{NONSTANDARD yylval.number = '\007'; return(CHAR);}
<literal>\\x[0-9][0-9]	{NONSTANDARD yylval.number=(16*yytext[2]+yytext[3]); return(CHAR);}
<literal>\\^.		{NONSTANDARD yylval.number = yytext[2] & 0x1f; return(CHAR);}
<literal>\\.		{NONSTANDARD yylval.number = yytext[1]; return(CHAR);}
[ \t]*\/\/	{
    NONSTANDARD {
	while (pinput() != '\n')
	    continue;
	punput('\n');
    }
}
<literal>{NONSPECIALS}	{yylval.string = strsave(yytext); return(ALLOC);}
<literal>.		{yylval.number = yytext[0]; return(CHAR);}

\n	{
    /* reset all error-tracking and syntax-state variables */
    lexcont = TRUE;		/* so we catch omitted keyword */
    hastxt = TRUE;		/* assume we're looking for trailing text */
    BEGIN start;		/* start next line in tokenizing state */
    return(NEWLINE);
}

%%
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
