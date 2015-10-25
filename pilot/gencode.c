/*****************************************************************************

NAME
    gencode.c -- interpreter action code for IEEE PILOT

SYNOPSIS
   variable *makevar(type, var)	-- intern a symbol with given type
   int type; variable *var;

   void gen_label()		-- LABEL entry

   void gen_type()		-- TYPE statement

   void gen_accept()		-- ACCEPT statement

   void gen_match()		-- MATCH statement

   void gen_jump()		-- JUMP statement

   void gen_use()		-- USE statement

   void gen_match()		-- MATCH statement

   void gen_file()		-- FILE statement

   void gen_graphic()		-- GRAPHIC statement

   void gen_end()		-- END statement

   void gen_problem()		-- PROBLEM statement

   void gen_wait()		-- WAIT statement

   void gen_link()		-- LINK statement

   void gen_typeh()		-- TYPEH statement

   void gen_clearhome()		-- CLEARHOME statement

   void gen_cursaddr()		-- CURSADDR statement

   void gen_clearline()		-- CLEARLINE statement

   void gen_clearend()		-- CLEAREND statement

   void gen_system()		-- SYSTEM statement

   void eolhook()		-- action to perform on newline

   void execfile(f)		-- interpret PILOT file
   char *f;

DESCRIPTION
   The guts of the IEEE PILOT interpreter/compiler.  The gen_* functions that
get called by the grammar actions live here.  The structure of this hack is
based on the fact that the lexer and grammar just build node trees; they
neither know nor care whether interpretation or compilation is going on.
   The gen_* functions either interpret PILOT or translate it to C depending
on the value of the compile switch.  The compilation code calls the C
compiler as a back end.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The
author retains copyright on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to
make money with this or any code derived from it, check with the
author first.

******************************************************************************/
/*
 *	$Id: gencode.c,v 1.3 1996/10/07 22:38:21 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: gencode.c,v 1.3 1996/10/07 22:38:21 esr Exp $";
#endif /* lint */

/*LINTLIBRARY*/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "pilot.h"
#include "gencode.h"

extern FILE *yyin;		/* the program text file descriptor */
extern int yylineno;		/* the current source line count */
extern int yydebug;		/* enable YACC instrumentation? */

int verbose;		/* verbosity level of interpreter */
bool pedantic;		/* insist on strict IEEE 1154-1991 conformance */

/*******************************************************************
 *
 * User messages, collected here for internationalization
 *
 ******************************************************************/

#define BADCONT	"can't continue the previous command\n"
#define BADEXT	"source file has incorrect extension\n"
#define BADUSE	"use statements are too deeply nested\n"
#define BADNODE	"internal error, bad type in node\n"
#define INVLAB	"invalid label \"*%s\"\n"
#define INTERR	"internal error, bad type %d in element %d text list\n"
#define JUMPERR	"invalid special jump type\n"
#define NOJUMP	"can't perform JUMP or USE in interactive mode.\n"
#define NOSTDIN	"source file must be specified in order to compile\n"
#define NOTMP	"can't open temp file %s\n"
#define PASS2	"beginning compilation pass 2\n"
#define PROBNG	"PROBLEM has non-portable side-effects in some PILOTS\n"
#define TOOLONG	"id \"%s\" is too long!\n"
#define TOOMANY	"too many variables\n"
#define BADEND	"EOF encountered before final END\n"
#define CANTOPN	"can't open file %s\n"
#define SYNERR	"(line %d) %s\n"
#define SYSVAR	"internal error referencing system variable %s\n"
#define UNUSED	"label %s never JUMPed to or USEd.\n"
#define UNDEFED	"label %s JUMPed to or USEd but never defined.\n"
#define USAGE	"usage: pilot [-v]\n"

/* location of PILOT library */
static char pilotdir[PATH_MAX] = PILOTDIR;

/* mode options */
static int compile;		/* pass number of compile */
static bool keep;		/* don't nuke intermediate C file */

/* compilation globals */
static FILE *yyout;		/* where to send compilation output */
static int indent = 0;		/* current block-indent level */
static int maxret = 0;		/* count of generated return labels */
static int usecount = 0;	/* count of USE statements */
static int matchcount = 0;	/* count of MATCH statements */
static int acceptcount = 0;	/* count of ACCEPT statements */
static int problemcount = 0;	/* count of PROBLEM statements */
static bool needclose = FALSE;		/* need closing '}' for this line */

/* the interpreter's variable table and allocation pointer */
variable variables[MAXVARS], *nextv = variables;

/* other interpretation globals */
static variable *lookfor;	/* label to jump to */
static int specialjump;		/* for @ jumps */
static bool statedump;		/* show state after each command? */
static long startaddr;		/* where jump starts from */

/*******************************************************************
 *
 * Variable lookup and creation
 *
 ******************************************************************/

variable *makevar(type, name)
/* create a new symbol with given name and type */
int	type;
char	*name;
{
    variable	*vp;

    if ((int)strlen(name) > MAXNAME)
	yyerror(TOOLONG, name);

    for (vp = variables; vp < nextv; vp++)
	if (vp->type == type && strcmp(vp->name, name) == 0)
	    return(vp);

    if (nextv < variables + MAXVARS)
    {
	nextv->type = type;
	(void) strcpy(nextv->name, name);
    }
    else
	yyerror(TOOMANY);

    if (type == LABEL)
    {
	nextv->v.label.addr = BAD_LABEL;
	nextv->v.label.lineno = yylineno;
    }

    initvar(nextv);	/* see nonstd.c */

    return(nextv++);
}

/*******************************************************************
 *
 * Interpretation helpers
 *
 ******************************************************************/

static void dumpvars(int keyword)
/* show state of all system variables */
{
    (void) printf("%%satisfied = %s\n",
		  sys_satisfied ? "TRUE" : "FALSE");

    if (sys_satisfied)
	switch (keyword)
	{
	case TYPE:
	case KFILE:
	case GRAPHIC:
	case YES:
	case NO:
	case LINK:
	    (void) printf("%%text = \"%s\"\n", sys_text);
	    break;

	case ACCEPT:
	    (void) printf("%%answer = \"%s\"\n", sys_answer);
	    break;

	case MATCH:
	    (void) printf("%%text = \"%s\"\n", sys_text);
	    (void) printf("%%answer = \"%s\"\n", sys_answer);
	    (void) printf("%%matched = %d\n", sys_matched);
	    (void) printf("%%match = %s\n", sys_match);
	    (void) printf("%%left = %s\n", sys_left);
	    (void) printf("%%right = %s\n", sys_right);
	    break;

	case USE:
	    (void) printf("%%uselevel = %d\n", sys_uselevel);
	    (void) printf("%%return%d = %d\n",
			  sys_uselevel, sys_nextstmt);
	    /* FALL THROUGH */

	case JUMP:
	    (void) printf("%%nextstmt = %ld\n",sys_nextstmt);
	    break;

	case COMPUTE:
	    (void) printf("%%text = \"%s\"\n", sys_text);
	    (void) printf("%%expression = %d\n",sys_expression);
	    break;

	case END:
	    (void) printf("%%uselevel = %d\n", sys_uselevel);
	    (void) printf("%%nextstmt = %ld\n",
			  sys_return[sys_uselevel + 1]);
	    (void) printf("%%nextstmt = %ld\n",sys_nextstmt);
	    break;

	}
}

void eolhook(void)
/* end-of-line actions for interpreter */
{
    char	*cp;

    if (compile)
	return;

    /* if we have an unknown jump target to find, go do it */
    if ((keyword == JUMP || keyword == USE || keyword == JUMPMATCH)
	&& (lookfor || specialjump))
    {
	if (yyin == stdin)
	    yyerror(NOJUMP);
	else if (lookfor && lookfor->v.label.addr != BAD_LABEL)
	{
	    yylineno = lookfor->v.label.lineno;
	    sys_nextstmt = lookfor->v.label.addr;
	}
	else
	{
	    char	buf[MAXSTR + 1];
	    long	acceptaddr = 0L;

	    if (lookfor || specialjump == ACCEPT)
	    {
		yyseek(0L);
		yylineno = 1;
	    }

	    while (fgets(buf, sizeof(buf)-1, yyin) != (char *)NULL)
	    {
		for (cp = buf; *cp && isspace(*cp); cp++)
		    continue;
		strlwr(cp);
		if (lookfor)
		{
		    if (cp[0] == '*'
			&& !strncmp(lookfor->name, cp+1, strlen(lookfor->name))
			&& isspace(cp[1 + strlen(lookfor->name)]))
		    break;
		}
		else if (specialjump == MATCH)
		{
		    if (cp[0] == 'm')
			break;
		}
		else if (specialjump == PROBLEM)
		{
		    if (cp[0] == 'p')
			break;
		}
		else if (specialjump == ACCEPT)
		{
		    if (ftell(yyin) > startaddr && acceptaddr != 0L)
			break;
		    if (cp[0] == 'a')
			acceptaddr = ftell(yyin) - strlen(cp);
		}
		else
		    yyerror(JUMPERR);

		yylineno++;
	    }

	    if (feof(yyin) || ferror(yyin))	/* fell through loop */
	    {
		yyerror(INVLAB, lookfor->name);
		sys_nextstmt = BAD_LABEL;
	    }
	    else				/* got here from break */
	    {
		sys_nextstmt = ftell(yyin) - strlen(cp);
		if (lookfor)
		    sys_nextstmt += strlen(lookfor->name) + 1;
	    }
	}

	/* reset our jump-to marker and do the actual seek */
	if (verbose)
	    (void) fprintf(stderr, "Seeking to %ld\n", sys_nextstmt);
	yyseek(sys_nextstmt);
    }

    /* O.K, now that sys_nextsmt is calculated, we can do this... */
    if (statedump)
	dumpvars(keyword);
    sys_nextstmt = BAD_LABEL;
    lookfor = (variable *)NULL;
    specialjump = 0;
}

static char *evaltext(textparts, tn)
/* fill sys_text with the results of evaluating the current text parts */
part	*textparts;
int	tn;
{
    part	*tp;
    char	*ep, *np;

    /* associate string system variable names with locations */
    static struct
    {
	char	*name;
	char	*loc;
    }
    *spp, sysstrings[] =
    {
	"answer",	sys_answer,
	"left",		sys_left,
	"match",	sys_match,
	"right",	sys_right,
	"text",		sys_text,
    };

    /* associate numeric system variable names with locations */
    static struct
    {
	char	*name;
	int	*loc;
    }
    *npp, sysnums[] =
    {
	"expression",	&sys_expression,
	"term",		&sys_term,
	"factor",	&sys_factor,
	"uselevel",	&sys_uselevel,
	"nextstmt",	&sys_nextstmt,
	"matched",	&sys_matched,
	"satisfied",	&sys_satisfied,
	"relation",	&sys_relation,
	"status",	&sys_status,
	(char *)NULL,
    };

    sys_text[0] = '\0';
    for (tp = textparts; tp < textparts + tn; tp++)
	switch(tp->type)
	{
	case STRING:
	    (void) strcat(sys_text, tp->part.string);
	    break;

	case ALLOC:
	    (void) strcat(sys_text, tp->part.string);
	    free(tp->part.string);
	    break;

	case NUMBER:
	    (void) sprintf(sys_text + strlen(sys_text), "%d", tp->part.number);
	    break;

	case CHAR:
	    ep = sys_text + strlen(sys_text);
	    *ep++ = tp->part.number;
	    *ep = '\0';
	    break;

	case STRING_IDENT:
	    if (tp->part.var->name[0] != '%')
		(void) strcat(sys_text, tp->part.var->v.string);
	    else
	    {
		for (spp = sysstrings; spp->name; spp++)
		    if (strcmp(spp->name, tp->part.var->name + 1) == 0)
		    {
			(void) strcat(sys_text, spp->loc);
			break;
		    }
		if (spp->name == (char *)NULL)
		    yyerror(SYSVAR, tp->part.var->name);
	    }
	    break;

	case NUMERIC_IDENT:
	    if (strncmp(tp->part.var->name, "%return", 7) == 0)
		(void) sprintf(sys_text + strlen(sys_text),
			       "%ld",
			       sys_return[atoi(tp->part.var->name + 8)]);
	    else if (tp->part.var->name[0] != '%')
		(void) sprintf(sys_text + strlen(sys_text),
			      "%d",
			      tp->part.var->v.number);
	    else
	    {
		for (npp = sysnums; npp->name; npp++)
		    if (strcmp(npp->name, tp->part.var->name + 1) == 0)
		    {
			(void) sprintf(sys_text + strlen(sys_text),
				       "%d",
				       *(npp->loc));
			break;
		    }
		if (npp->name == (char *)NULL)
		    yyerror(SYSVAR, tp->part.var->name);
	    }
	    break;

	default:
	    yyerror(INTERR, tp->type, tp - textparts);
	    break;
	}

    return(sys_text);
}

static int eval(np)
/* evaluate a node tree */
node	*np;
{
    int	left, right;

    /* null node corresponds to empty condition expression */
    if (np == (node *)NULL)
    {
	if (!continuation)
	    sys_satisfied = TRUE;
	return(sys_satisfied);
    }

    /* if the node is an atom, return its value */
    if (np->type == NUMBER)
	return(sys_factor = np->value.number);
    else if (np->type == NUMERIC_IDENT)
	return(sys_factor = np->value.var->v.number);
    else if (np->type == YES)
	return(sys_satisfied = sys_matched);
    else if (np->type == NO)
	return(sys_satisfied = !sys_matched);

    /* the magic recursion */
    left = eval(np->left);
    right = eval(np->right);

    switch(np->type)
    {
    case LESS:		return(sys_relation = (left < right));
    case GREATER:	return(sys_relation = (left > right));
    case EQUAL:		return(sys_relation = (left == right));
    case NEQUAL:	return(sys_relation = (left != right));
    case NOTGRT:	return(sys_relation = (left <= right));
    case NOTLESS:	return(sys_relation = (left >= right));
    case PLUS:		return(sys_expression = (left + right));
    case MINUS:		return(sys_expression = (left - right));
    case UMINUS:	return(sys_term = (-right));
    case MULTIPLY:	return(sys_term = (left * right));
    case DIVIDE:	return(sys_term = (left / right));
    case MODULO:	return(sys_term = (left % right));
    case ANDAND:	return(left && right);

    default:
	return(yyerror(BADNODE));
	break;
    }
}

/*******************************************************************
 *
 * Compilation helpers
 *
 ******************************************************************/

static void genindent(n)
/* generate proper indent level */
int	n;
{
    int i = 0;

    for (i = 0; i < n * 4; i++)
	(void) fputc(' ', yyout);
}

void solhook(s)
/* start-of-line actions for compiler */
char	*s;
{
    if (compile == 2)
    {
	if (needclose && (!continuation || s == (char *)NULL))
	{
	    genindent(--indent);
	    (void) fputs("}\n", yyout);
	    needclose = FALSE;
	}
  
	if (s && s[0] && s[0] != '\n')
	{
	    variable	*vp;
	    char	*cp;

	    if (!keep)
		(void) fprintf(yyout, "#line %d \"%s\"\n",
			       yylineno+1, yyfile);

	    /* list the PILOT statement we're compiling */
	    s[strlen(s) - 1] = '\0';
	    (void) fprintf(yyout, "    /*-%s-*/\n", s);
	    s[strlen(s)] = '\n';

	    /* emit this statement's label if it has one */
	    for (vp = variables; vp < nextv; vp++)
		if (vp->type == LABEL
		    && vp->v.label.addr <= ftell(yyin)
		    && vp->v.label.addr >= ftell(yyin) - (int)strlen(s))
		    (void) fprintf(yyout, "plt_%s:\n", vp->name);

	    /* emit label for @[MAP] jumps */
	    for (cp = s; *cp && isspace(*cp); cp++)
		if (*cp == 'm')
		    (void) fprintf(yyout, "match_%d:\n", matchcount++);
		else if (*cp == 'a')
		    (void) fprintf(yyout, "accept_%d:\n", acceptcount++);
		else if (*cp == 'p')
		    (void) fprintf(yyout, "problem_%d:\n", problemcount++);

	    if (continuation
		    && keyword != TYPE && keyword != TYPEH
		    && keyword != YES && keyword != NO
		    && keyword != REMARK)
		yyerror(BADCONT);
	}
	else if (s)
	    (void) fputc('\n', yyout);
    }
}

static void genexpr();	/* this is just a forward */

static binop(np, var, op)
/* write expression wrapper for given binary op and system variable */
node	*np;
char	*var, *op;
{
    (void) fputs("(", yyout);
    if (statedump)
	(void) fprintf(yyout, "%s = ", var);
    if (np->left)
	genexpr(np->left);
    (void) fputs(op, yyout);
    if (np->right)
	genexpr(np->right); 
    (void) fputs(")", yyout);
}

static void genexpr(np)
/* prettyprint a PILOT expression in generated C */
node	*np;
{
    int	left, right;

    /* null node corresponds to empty condition expression */
    if (np == (node *)NULL)
	return;
    else if (np->type == NUMBER)
    {
	if (statedump)
	    (void) fprintf(yyout, "(sys_factor = %d)", np->value.number);
	else
	    (void) fprintf(yyout, "%d", np->value.number);
    }
    else if (np->type == NUMERIC_IDENT)
    {
	if (statedump)
	    (void) fprintf(yyout,"(sys_factor = num_%s)", np->value.var->name);
	else
	    (void) fprintf(yyout, "num_%s", np->value.var->name);
    }
    else if (np->type == YES)
    {
	if (statedump)
	    (void) fprintf(yyout, "(sys_satisfied = sys_matched)");
	else
	    (void) fprintf(yyout, "sys_matched");
    }
    else if (np->type == NO)
    {
	if (statedump)
	    (void) fprintf(yyout, "(sys_satisfied = !sys_matched)");
	else
	    (void) fprintf(yyout, "!sys_matched");
    }
    else
	switch(np->type)
	{
	case LESS:	binop(np, "sys_relation", "<"); break;
	case GREATER:	binop(np, "sys_relation", ">"); break;
	case EQUAL:	binop(np, "sys_relation", "=="); break;
	case NEQUAL:	binop(np, "sys_relation", "!="); break;
	case NOTGRT:	binop(np, "sys_relation", "<="); break;
	case NOTLESS:	binop(np, "sys_relation", ">="); break;
	case PLUS:	binop(np, "sys_expression", "+"); break;
	case MINUS:	binop(np, "sys_expression", "-"); break;
	case MULTIPLY:	binop(np, "sys_term", "*"); break;
	case DIVIDE:	binop(np, "sys_term", "/"); break;
	case MODULO:	binop(np, "sys_term", "%"); break;
	case UMINUS:	binop(np, "sys_term", "-"); break;
	case ANDAND:	binop(np, "sys_satistied", "&&"); break;
	default:
	    (void) yyerror(BADNODE);
	    break;
	}
}

static void gencond(np)
/* generate C for a condition expression corresponding to the given node */
node	*np;
{
    if (np != (node *)NULL && !continuation)
    {
	genindent(indent);
	(void) fprintf(yyout, "if (");
	genexpr(np);
	(void) fprintf(yyout, ") {\n");
	needclose = TRUE;
	indent++;
    }
    else if (statedump)
    {
	genindent(indent);
	(void) fputs("sys_satisfied = TRUE;\n", yyout);
    }
}

static void gentext(textparts, tn)
/* generate sprintf to assemble text from current parts */
part	*textparts;
int	tn;
{
    char	fmtbuf[MAXSTR * 2];	/* allow for escaped " and % */
    char	argbuf[MAXSTR];
    part	*tp;
    char	*ep, *cp, *np;

    fmtbuf[0] = argbuf[0] = '\0';
    for (tp = textparts; tp < textparts + tn; tp++)
	switch(tp->type)
	{
	case STRING:
	    ep = fmtbuf + strlen(fmtbuf);
	    for (cp = tp->part.string; *cp; cp++)
	    {
		if (*cp == '"' || *cp == '\\')
		    *ep++ = '\\';
		else if (*cp == '%')
		    *ep++ = '%';
		*ep++ = *cp;
	    }
	    *ep = '\0';
	    break;

	case ALLOC:
	    ep = fmtbuf + strlen(fmtbuf);
	    for (cp = tp->part.string; *cp; cp++)
	    {
		if (*cp == '"' || *cp == '\\')
		    *ep++ = '\\';
		else if (*cp == '%')
		    *ep++ = '%';
		*ep++ = *cp;
	    }
	    *ep = '\0';
	    free(tp->part.string);
	    break;

	case NUMBER:
	    (void) sprintf(fmtbuf + strlen(fmtbuf), "%d", tp->part.number);
	    break;

	case CHAR:
	    ep = fmtbuf + strlen(fmtbuf);
	    if (tp->part.number == '"')
		*ep++ = '\\';
	    else if (tp->part.number == '%')
		*ep++ = '%';
	    *ep++ = tp->part.number;
	    *ep = '\0';
	    break;

	case STRING_IDENT:
	    (void) strcat(fmtbuf, "%s");
	    if ((np = tp->part.var->name)[0] == '%')
		(void) sprintf(argbuf + strlen(argbuf), ", sys_%s", ++np);
	    else
		(void) sprintf(argbuf + strlen(argbuf), ", str_%s", np);
	    break;

	case NUMERIC_IDENT:
	    (void) strcat(fmtbuf, "%d");
	    if (strncmp(tp->part.var->name, "%return", 7) == 0)
		(void) sprintf(argbuf + strlen(argbuf),
			       ", sys_return[%d]", atoi(tp->part.var->name+8));
	    else if ((np = tp->part.var->name)[0] == '%')
		(void) sprintf(argbuf + strlen(argbuf), ", sys_%s", ++np);
	    else
		(void) sprintf(argbuf + strlen(argbuf), ", num_%s", np);
	    break;

	default:
	    yyerror(INTERR, tp->type, tp - textparts);
	    break;
	}

    genindent(indent);
    (void) fprintf(yyout,
		   "(void) %s(sys_text, \"%s\"%s);\n",
		   argbuf[0] ? "sprintf" : "strcpy", fmtbuf, argbuf);
}

/*******************************************************************
 *
 * Interpretation/compilation functions
 *
 ******************************************************************/

void gen_label(v)
/* enter location of a LABEL */
variable	*v;
{
    /* no longer necessary to do anything here */
}

void gen_type(cond, text, pn)
/* perform TYPE statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("(void) puts(sys_text);\n", yyout);
    }
    else if (eval(cond))
	(void) puts(evaltext(text, pn));
}

void gen_accept(cond, var, type)
/* perform ACCEPT statement */
node *cond;
variable *var;
int type;
{
    if (compile)
    {
	gencond(cond);
	genindent(indent);
	(void) fputs("(void) gets(sys_answer);\n", yyout);
	if (var)
	{
	    genindent(indent);
	    if (var->type == STRING_IDENT)
		(void)fprintf(yyout, 
			  "(void) strcpy(str_%s, sys_answer);\n",
			  var->name);
	    else
		(void)fprintf(yyout,
			 "num_%s = numconv(sys_answer);\n",
			 var->name); 
	}
    }
    else if (eval(cond))
    {
	(void) fgets(sys_answer, sizeof(sys_answer)-1, stdin);
	sys_answer[strlen(sys_answer) - 1] = '\0';
	switch(type)
	{
	case STRING_IDENT:
	    (void) strcpy(var->v.string, sys_answer);
	    break;

	case NUMERIC_IDENT:
	    var->v.number = numconv(sys_answer);
	    break;
	}
    }
}

void gen_match(cond, text, pn)
/* perform MATCH statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("sys_matched = do_match(sys_text, sys_answer);\n", yyout);
    }
    else if (eval(cond))
	sys_matched = do_match(evaltext(text, pn), sys_answer);
}

void gen_jump(cond, var, target)
/* perform JUMP statement */
node *cond;
variable *var;
int target;
{
    if (var)
	var->v.label.refcount++;
    if (compile)
    {
	gencond(cond);
	genindent(indent);
	if (target == MATCH)
	    (void) fprintf(yyout, "goto match_%d;\n", matchcount + 1);
	else if (target == ACCEPT)
	    (void) fprintf(yyout, "goto accept_%d;\n", acceptcount);
	else if (target == PROBLEM)
	    (void) fprintf(yyout, "goto problem_%d;\n", problemcount + 1);
	else
	    (void) fprintf(yyout, "goto plt_%s;\n", var->name);
    }
    else if (eval(cond))
    {
	if (target != LABEL)
	{
	    startaddr = ftell(yyin);
	    specialjump = target;
	    lookfor = (variable *)NULL;
	}
	else
	    lookfor = var;
    }
}

void gen_use(cond, var)
/* perform USE statement */
node *cond;
variable *var;
{
    var->v.label.refcount++;
    if (compile)
    {
	char	buf[10];

	gencond(cond);
	genindent(indent);
	(void) fprintf(yyout,
	       "sys_return[sys_uselevel++] = %d; goto plt_%s; retlab%d:;\n",
	       maxret, var->name, maxret);
	++maxret;
    }
    else if (eval(cond))
    {
	if (sys_uselevel >= MAXUSES)
	    yyerror(BADUSE);
	else
	    sys_return[sys_uselevel++] = (int)ftell(yyin);
	lookfor = var;
    }
}

void gen_compute(cond, var, expr, text, pn)
/* perform COMPUTE statement */
bool	*cond;
variable	*var;
node	*expr;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	if (expr == (node *)NULL)
	{
	    gentext(text, pn);
	    genindent(indent);
	    (void) fprintf(yyout,
			   "(void) strcpy(str_%s, sys_text);\n",
			   var->name);
	}
	else
	{
	    genindent(indent);
	    (void) fprintf(yyout, "num_%s = ", var->name);
	    genexpr(expr);
	    (void) fputs(";\n", yyout);
	}
    }
    else if (eval(cond))
    {
	if (pn)
	    (void) strncpy(var->v.string, evaltext(text, pn), MAXSTR);
	else
	    var->v.number = eval(expr);
    }
}

void gen_file(cond, text, pn)
/* perform FILE statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("(void) do_file(sys_text);\n", yyout);
    }
    else if (eval(cond))
	(void) do_file(evaltext(text, pn));
}

void gen_graphic(cond, text, pn)
/* perform GRAPHIC statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("(void) do_graphic(sys_text);\n", yyout);
    }
    else if (eval(cond))
	(void) do_graphic(evaltext(text, pn));
}

void gen_end(cond, val)
/* perform END statement */
bool	*cond;
int	val;
{
    if (compile)
    {
	int	i;

	/*
	 * I puke, big time, whenever I think about the code this generates.
	 */
	gencond(cond);
	genindent(indent);
	(void) fprintf(yyout, "if (sys_uselevel-- == 0) exit(%d);\n", val);
	genindent(indent);
	if (usecount == 0)
	    (void) fputs("exit(0);\n", yyout);
	else
	{
	    (void) fputs("switch(sys_return[sys_uselevel]) {\n", yyout);
	    for (i = 0; i < usecount; i++)
	    {
		genindent(indent);
		/*
		 * There is potential for optimization here.  If the
		 * first pass kept information on the location and targets
		 * of all JUMPs and USEs, a fairly simple graph-coloring
		 * album could be used to compute which USEs could lead
		 * to any given END.  Then, only those would need to be
		 * included in the case list for that END.
		 */
		(void) fprintf(yyout, "case %d: goto retlab%d;\n", i, i);
	    }
	    genindent(indent);
	    (void) fputs("default: exit(0);\n", yyout);
	    genindent(indent);
	    (void) fputs("}\n", yyout);
	}
    }
    else if (eval(cond))
    {
	if (sys_uselevel == 0)
	    exit(0);
	else
	    --sys_uselevel;
	(void) fseek(yyin, sys_return[sys_uselevel], 0);
    }
}

void gen_problem(cond, text, pn)
/* perform PROBLEM statement */
bool	*cond;
part	*text;
int	pn;
{
    if (pedantic)
	yyerror(PROBNG);
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("(void) do_problem(sys_text);\n", yyout);
    }
    else if (eval(cond))
	(void) do_problem(evaltext(text, pn));
}

void gen_pause(cond, val)
/* perform PAUSE statement */
node *cond;
node *val;
{
    if (compile)
    {
	gencond(cond);
	genindent(indent);
	(void) fprintf(yyout, "do_pause(");
	genexpr(val);
	(void) fputs(");\n", yyout);
    }
    else if (eval(cond))
	do_pause(eval(val));
}

void gen_link(cond, text, pn)
/* perform LINK statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	genindent(indent);
	(void) fprintf(yyout, "do_link(%s);\n", sys_text);
    }
    else if (eval(cond))
	/*
	 * Ideally, we'd call do_link() here.  Trouble is, we don't want
	 * to recurse the interpreter every time it does a link; the
	 * idea is to simulate textual inclusion.
	 */
	sys_status = execfile(evaltext(text, pn));
}

void gen_jumpmatch(cond, njumps, jumpvec)
/* perform JUMPMATCH statement */
bool		*cond;
int		njumps;
variable	*jumpvec[];
{
    int	i;

    for (i = 0; i < njumps; i++)
	jumpvec[i]->v.label.refcount++;
    if (compile)
    {
	gencond(cond);
	genindent(indent);
	(void) fprintf(yyout, "switch(sys_matched) {\n");

	for (i = 0; i < njumps; i++)
	{
	    genindent(indent);
	    (void) fprintf(yyout,"case %d: ", i + 1);
	    if (jumpvec[i]->type == MATCH)
		(void) fprintf(yyout, "goto match_%d;\n", matchcount + 1);
	    else if (jumpvec[i]->type == ACCEPT)
		(void) fprintf(yyout, "goto accept_%d;\n", acceptcount);
	    else if (jumpvec[i]->type == PROBLEM)
		(void) fprintf(yyout, "goto problem_%d;\n", problemcount + 1);
	    else
		(void) fprintf(yyout, "goto plt_%s;\n", jumpvec[i]->name);
	}

	genindent(indent);
	(void) fputs("}\n", yyout);
    }
    else if (sys_matched > 0 && sys_matched <= njumps)
	gen_jump(cond, jumpvec[sys_matched-1], jumpvec[sys_matched-1]->type);
} 

void gen_typeh(cond, text, pn)
/* perform TYPEH statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("(void) fputs(sys_text, stdout);\n", yyout);
    }
    else if (eval(cond))
	(void) fputs(evaltext(text, pn), stdout);
}

void gen_clearhome(cond)
/* perform CLEARHOME statement */
bool	*cond;
{
    if (compile)
    {
	gencond(cond);
	(void) fprintf(yyout, "do_clearhome();\n");
    }
    else if (eval(cond))
	do_clearhome();
}

void gen_cursaddr(cond, y, x)
/* perform CURSADDR statement */
bool	*cond;
{
    if (compile)
    {
	gencond(cond);
	(void) fprintf(yyout, "do_cursaddr(%d, %d);\n", y, x);
    }
    else if (eval(cond))
	do_cursaddr(y, x);
}

void gen_clearline(cond)
/* perform CLEARLINE statement */
bool	*cond;
{
    if (compile)
    {
	gencond(cond);
	(void) fprintf(yyout, "do_clearline();\n");
    }
    else if (eval(cond))
	do_clearline();
}

void gen_clearend(cond)
/* perform CLEAREND statement */
bool	*cond;
{
    if (compile)
    {
	gencond(cond);
	(void) fprintf(yyout, "do_clearend();\n");
    }
    else if (eval(cond))
	do_clearend();
}

void gen_system(cond, text, pn)
/* perform SYSTEM statement */
bool	*cond;
part	*text;
int	pn;
{
    if (compile)
    {
	gencond(cond);
	gentext(text, pn);
	genindent(indent);
	(void) fputs("sys_status = system(sys_text);\n", yyout);
    }
    else if (eval(cond))
	sys_status = system(evaltext(text, pn));
}

/*******************************************************************
 *
 * Sequencing
 *
 ******************************************************************/

void options(str)
/* interpret options string */
char	*str;
{
    char	*p;

    if (p = strchr(str, 'c'))
	multerr = (p[-1] == '-');
    if (p = strchr(str, 'd'))
	statedump = (p[-1] == '-');
    if (p = strchr(str, 'k'))
	keep = (p[-1] == '-');
    if (p = strchr(str, 'm'))
	multerr = (p[-1] == '-');
    if (p = strchr(str, 'p'))
	pedantic = (p[-1] == '-');
    if (p = strchr(str, 'v'))
    {
	verbose = atoi(p + 1);
	if (strchr(p, 'y'))
	    yydebug = (p[-1] == '-');
    }
}

int execfile(t)
/* execute or compile a PILOT file in the current directory */
char	*t;
{
    FILE	*oldyyin = yyin;
    char	oldyyfile[PATH_MAX];
    char	stem[PATH_MAX], source[PATH_MAX], outfile[PATH_MAX];
    int		pass2level;
    variable	*vp;

    (void) strcpy(oldyyfile, yyfile);

    if (t)
    {
	if (!namefile(t, stem, source))
	    yyerror(BADEXT);
	if ((yyin = fopen(source, "r")) == (FILE *)NULL)
	{
	    (void) fprintf(stderr, CANTOPN, t);
	    return(1);
	}
    }

    if (!compile)
	do_scrinit();
    else
    {
	variable	*vp;
	int		class;

	if (t == (char *)NULL)
	    yyerror(NOSTDIN);

	(void) strcpy(outfile, stem);
	(void) strcat(outfile, ".c");
	if ((yyout = fopen(outfile, "w")) == (FILE *)NULL)
	    yyerror(NOTMP, t);

	/*
	 * First pass -- accumulate symbol table, label, and jump info.
	 * To avoid parser overhead, we only do lexical analysis.
	 */
	yyinit(source);
	pass2level = verbose;
	usecount = 0;
	if (verbose == 1)
	    verbose = 0;
	while (class = yylex())
	    if (class == USE)
		usecount++;
	verbose = pass2level;

	/* check for undefined labels */
	for (vp = variables; vp < nextv; vp++)
	    if (vp->type == LABEL && vp->v.label.addr == BAD_LABEL)
	    {
		yylineno = vp->v.label.lineno;
		yyerror(UNDEFED, vp->name);
	    }


	(void) fprintf(yyout,
		       "/* %s -- generated C code for %s */\n",
		       outfile, source);
	(void) fputs("#include <stdio.h>\n", yyout);
	(void) fputs("#include \"pilot.h\"\n", yyout);

	/* generate declarations for all non-system variables */
	for (vp = variables; vp < nextv; vp++)
	    if (vp->name[0] != '%')
		switch (vp->type)
		{
		case NUMERIC_IDENT:
		    (void) fprintf(yyout,
				   "static int num_%s;\n",
				   vp->name);
		    break;

		case STRING_IDENT:
		    (void) fprintf(yyout,
				   "static char str_%s[MAXSTR + 1];\n",
				   vp->name);
		    break;
		}

	compile = 2;
	yyseek(0L);

	if (verbose)
	    (void) fputs(PASS2, stderr);

	(void) fputs("\nmain()\n{\n    do_scrinit();\n", yyout);
	indent = 1;
    }

    yyinit(t ? source : t);
    yyparse();
    solhook((char *)NULL);

    if (sys_uselevel != 0)
	(void) fprintf(stderr, BADEND);

    /* check for unreferenced labels */
    for (vp = variables; vp < nextv; vp++)
	if (vp->type == LABEL && vp->v.label.refcount == 0)
	{
	    yylineno = vp->v.label.lineno;
	    yyerror(UNUSED, vp->name);
	}

    if (!compile)
	do_scrend();
    else
    {
	(void) fputs("    do_scrend();\n}\n", yyout);
	(void) fprintf(yyout, "\n/* Generated code for %s ends here */\n", t);
	(void) fclose(yyout);
    }

    if (sys_uselevel)
	(void) fprintf(stderr, BADEND);

    if (t)
	(void) fclose(yyin);

    if (compile && yyerrors == 0)
    {
	char	sysbuf[200];

	(void) sprintf(sysbuf,
		       "cc -I. -I%s -L%s %s.c -lpilot -ltermcap -o %s\n",
		       pilotdir, pilotdir, stem, stem);
	if (verbose)
	    (void) fputs(sysbuf, stderr);
	if (system(sysbuf) == 0 && !keep)
	{
	    (void) strcat(stem, ".c");
	    (void) unlink(stem);
	}
    }

    yyin = oldyyin;
    (void) strcpy(yyfile, oldyyfile);
    return(0);
}

main(argc, argv)
int	argc;
char	*argv[];
{
    extern int	    optind;		/* getopt() sets this */
    extern char	    *optarg;		/* and this */
    extern char	    *getenv();
    int	c;

    if (optarg = getenv("PILOTDIR"))
	(void) strcpy(pilotdir, optarg);

    while ((c = getopt(argc, argv, "ckmdv:")) != EOF)
	switch (c)
	{
	case 'c':
	    compile = 1;	/* start them on compilation pass 1 */
	    multerr = TRUE;
	    break;

	case 'm':
	    multerr = TRUE;
	    break;

	case 'd':
	    statedump = TRUE;
	    break;

	case 'k':
	    keep = TRUE;
	    break;

	case 'p':
	    pedantic = TRUE;
	    break;

	case 'v':
	    verbose = atoi(optarg);
	    if (strchr(optarg, 'y'))
		yydebug = 1;
	    break;

	default:
	    (void) fprintf(stderr, USAGE);
	    break;
	}

    if (optind == argc)
	return(execfile((char *)NULL));
    else
	for (; optind < argc; optind++)
	    if (execfile(argv[optind]))
		break;
}

/* gencode.c ends here */
