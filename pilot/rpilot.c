/*****************************************************************************

NAME
   rpilot.c -- basic runtime support for PILOT

DESCRIPTION
   System variables live here.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The
author retains copyright on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to
make money with this or any code derived from it, check with the
author first.

******************************************************************************/
/*
 *	$Id: rpilot.c,v 1.5 1992/07/10 18:17:05 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: rpilot.c,v 1.5 1992/07/10 18:17:05 esr Exp $";
#endif /* lint */

/*LINTLIBRARY*/
#include "pilot.h"

int sys_expression;		/* value of latest numeric sum */
int sys_term;			/* value of latest numeric product */
int sys_factor;			/* value of literal, value, or expression */
int sys_nextstmt;		/* location of next statement */
int sys_uselevel;		/* number of USE statements executed */
char sys_answer[MAXSTR];	/* latest user response line */
int sys_matched;		/* true iff last match succeeded */
char sys_left[MAXSTR];		/* the part of %answer before %match */
char sys_match[MAXSTR];		/* the part of %anser matched */
char sys_right[MAXSTR];		/* the part of %answer before %match */
bool sys_satisfied;		/* true iff current guard is satisfied */
bool sys_relation;		/* true iff current rel_expr is satisfied */
char sys_text[MAXSTR];		/* value of current text buffer */
int sys_return[MAXUSES];	/* use return stack */
int sys_status;			/* status of last SYSTEM command */

/* rpilot.c ends here */
