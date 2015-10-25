/******************************************************************************

NAME
  plib.c -- implementation of non-IEEE primitives

SYNOPSIS
   void do_graphic(t)		-- semantic action for FILE statement
   char *t;

   void do_file(t)		-- semantic action for GRAPHIC statement
   char *t;

   void do_problem(t)		-- semantic action for PROBLEM statement
   char *t;

   void do_pause(n)		-- semantic action for PAUSE statement
   int n;

   void do_link(t)		-- semantic action for LINK statement
   char *t;

   void do_scrinit()		-- screen initialization

   void do_clearhome()		-- semantic action for CLEARHOME statement

   void do_cursaddr(y,x)	-- semantic action for CURSADDR statement
   int y, x;

   void do_clearline()		-- semantic action for CLEARLINE statement

   void do_clearend()		-- semantic action for CLEAREND statement

   void do_scrend()		-- screen de-initialization

DESCRIPTION
   Code for some functions not specified by the IEEE PILOT standard
1154-199 is concentrated here.  These are the ones that may be
required by compiled code.  See also nonstd.c and numconv.c.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The
author retains copyright on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to
make money with this or any code derived from it, check with the
author first.

******************************************************************************/
/*
 *	$Id: plib.c,v 1.11 1994/09/14 19:42:35 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: plib.c,v 1.11 1994/09/14 19:42:35 esr Exp $";
#endif /* lint */

/*LINTLIBRARY*/
#include <stdio.h>
#include <string.h>
#ifdef TERMCAP
#define SYSV		/* guard System V curses.h from brain death */
#include <curses.h>
#endif /* TERMCAP */

#include "pilot.h"

#include <unistd.h>	/* needed for R_OK */

#define FILENG	"pilot: the semantics of the FILE command is not defined\n"
#define GRAPHNG	"pilot: the semantics of the GRAPHICS command is not defined\n"

/*******************************************************************
 *
 * Filename interpretation
 *
 ******************************************************************/

bool namefile(whole, stem, source)
char	*whole, *stem, *source;
{
    char 	*ep;

    if (ep = strchr(whole, '.'))
    {
	if (strcmp(ep, PLT))
	    return(FALSE);
	else
	{
	    (void) strcpy(source, whole);
	    (void) strcpy(stem, whole);
	    *(ep = strchr(stem, '.')) = '\0';
	}
    }
    else
    {
	(void) strcpy(stem, whole);
	(void) strcpy(source, whole);
	(void) strcat(source, PLT);
    }
    return(TRUE);
}

int psearch(whole, stem, source)
/* search for PILOT source or binary */
char	*whole, *stem, *source;
{
    /*
     * The theory here is that we're looking for either a compiled
     * PILOT binary or a source we can interpret.  We look either
     * in the current directory or in the PILOT library
     */
    if (!namefile(whole, stem, source))
	return(EOF);
    else if (access(stem, R_OK) == 0)
	return(FALSE);
    else if (access(source, R_OK) == 0)
	return(TRUE);
    else
    {
	char	withlib[PATH_MAX];

	(void) strcpy(withlib, PILOTDIR);
	(void) strcat(withlib, whole);
	namefile(withlib, stem, source);

	if (access(stem, R_OK) == 0)
	    return(FALSE);
	else if (access(source, R_OK) == 0)
	    return(TRUE);
	else
	    return(EOF);
    }
}

/*******************************************************************
 *
 * Language primitives
 *
 ******************************************************************/

void do_graphic(char *t)
/* perform a GRAPHIC operation */
{
    (void) fprintf(stderr, GRAPHNG);
}

void do_file(char *t)
/* perform a FILE operation */
{
    (void) fprintf(stderr, FILENG);
}

void do_problem(char *t)
/* perform a PROBLEM operation */
{
    /* no-op */
}

void do_pause(int n)
/* perform a PAUSE operation */
{
    /* we assume n is in seconds */
    (void) sleep(n);
}

void do_link(char *t)
/* perform a LINK operation */
{
    char	stem[PATH_MAX], source[PATH_MAX];

    switch (psearch(t, stem, source))
    {
    case TRUE:
	(void) sprintf(stem, "pilot %s\n", source);
	sys_status = system(stem);
	break;

    case FALSE:
	sys_status = system(stem);
	break;

    case EOF:
	sys_status = -1;
	break;
    }
}

/*******************************************************************
 *
 * Screen-control primitives
 *
 ******************************************************************/

#ifdef TERMCAP
static char buf[256];	/* I hope this is big enough for BSD termcap */
#endif /* TERMCAP */

void do_scrinit(void)
/* initialize screen I/O mode */
{
#ifdef TERMCAP
    int	i;

    if (tgetent(buf, getenv("TERM")) == ERR)
    {
	(void) fputs("Sorry, I can't initialize this terminal.\n", stderr);
	exit(2);
    }
#endif /* TERMCAP */
}

static int xputc(c)
int c;
{
    return(putchar(c));
}

void do_clearhome(void)
/* perform a CLEARHOME operation */
{
#ifdef TERMCAP
    /* second arg is only used for padding calculations */
    tputs(tgetstr("cl", (char *)NULL), 24, xputc);
#else
    /*
     * Kluge time.  We just emit a form feed, hoping the output
     * device is ANSI or VT100-like.
     */
    (void) putchar('\014');
#endif /* TERMCAP */
}

void do_cursaddr(int y, int x)
/* perform a CURSADDR operation */
{
#ifdef TERMCAP
    /* second arg is only used for padding calculations */
    tputs(tgoto(tgetstr("cm", (char *)NULL), x-1, y-1), 1, xputc);
#else
    /*
     * Kluge time.  We just emit an ANSI cursor-address sequence, hoping the
     * output device is ANSI or VT100-like.
     */
    (void) printf("\033[%d;%dH", y, x);
#endif /* TERMCAP */
}

void do_clearline(void)
/* perform a CLEARLINE operation */
{
#ifdef TERMCAP
    /* second arg is only used for padding calculations */
    tputs(tgetstr("ce", (char *)NULL), 1, xputc);
#else
    /*
     * Kluge time.  We just emit an ANSI clear-to-eol sequence, hoping the
     * output device is ANSI or VT100-like.
     */
    (void) printf("\033[0K");
#endif /* TERMCAP */
}

void do_clearend(void)
/* perform a CLEAREND operation */
{
#ifdef TERMCAP
    /* second arg is only used for padding calculations */
    tputs(tgetstr("cd", (char *)NULL), 24, xputc);
#else
    /*
     * Kluge time.  We just emit an ANSI clear-to-eos sequence, hoping the
     * output device is ANSI or VT100-like.
     */
    (void) printf("\033[0J");
#endif /* TERMCAP */
}

void do_scrend(void)
/* deinitialize screen I/O mode */
{
    /* no-op at present; reserved for future use */
}

/* plib.c ends here */
