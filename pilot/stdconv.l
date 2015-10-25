%{
/*****************************************************************************

NAME
   stdconv.l -- convert program to IEEE PILOT

USAGE
   stdconv <infile >outfile

DESCRIPTION
   Converts string IDs using the bogus postfix-$ form from Capitol PILOT to
prefix form.  I want the postfix form dropped from the standard.
   Also checks for various nonstandard constructs not supported by this
IEEE reference implementation.
   Eventually this may do other conversions from other dialects.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The
author retains copyright on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to
make money with this or any code derived from it, check with the
author first.

******************************************************************************/

static int problems = 0;
static int linenum = 0;

#define BARF(m)	(void) fprintf(stderr, m, linenum); problems++; ECHO
%}
%%
[a-zA-Z][a-zA-Z0-9]*\$[a-zA-Z][a-zA-Z0-9]*	{
	BARF("Prefix-postfix ambiguity, line %d\n");
	}

[a-zA-Z][a-zA-Z0-9]*\$	{
	(void) fprintf(stderr,"Postfix-prefix conversion, line %d\n",linenum);
	yytext[strlen(yytext) - 1] = '\0';
	(void) printf("$%s", yytext);
	}

^[ \t]*[Ff][Oo][Oo][Tt][ \t]*:	{BARF("FOOT is not supported, line %d\n");}
^[ \t]*[Ss][ \t]*:		{BARF("SOUND is not supported, line %d\n");}
^[ \t]*[Vv][ \t]*:		{BARF("VIDEO is not supported, line %d\n");}
^[ \t]*[Ww][ \t]*:		{BARF("WAIT is not supported, line %d\n");}
^[ \t]*[Xx][ \t]*:		{BARF("X is not supported, line %d\n");}
^[ \t]*[Kk][ \t]*:		{BARF("KEEP is not supported, line %d\n");}
^[ \t]*[Dd][ \t]*:		{BARF("DIMENSION is not supported, line %d\n");}
\n				{linenum++;}
%%

int yywrap()
{
    if (problems)
	(void) fputs(
	    "This program won't run under the IEEE reference implementation\n",
	    stderr);
    return(EOF);
}

main()
{
    while (yylex() != 0)
	continue;
}

/* stdconv.l ends here */
