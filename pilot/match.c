/******************************************************************************

NAME
   match.c -- match routine for PILOT programs

SYNOPSIS
   int do_match(s, t)
   char *s, *t;

DESCRIPTION
   This routine does the string-matching with ?,*, and or-characters
required for PILOT.  The return value is the number of the alternation
matched, 0 if there was no match.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The
author retains copyright on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to
make money with this or any code derived from it, check with the
author first.

******************************************************************************/
/*
 *	$Id: match.c,v 1.7 1994/09/14 14:41:22 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: match.c,v 1.7 1994/09/14 14:41:22 esr Exp $";
#endif /* lint */

/* LINTLIBRARY */
#include <string.h>
#include <ctype.h>
#include "pilot.h"

int do_match(char *s, char *t)
/* MATCH s in t, with wildcarding; may set %left, %right, and %match */
{
    char	*orpart, *nextpart, *anchor, *sp, *ep;
    int		matchcount;

    /* for each segment bounded by or characters... */
    for (matchcount = 1, orpart = nextpart = s;
	 nextpart;
	 orpart = nextpart, matchcount++)
    {
	/* look for any of the alternation delimiters */
	if ((nextpart = strchr(orpart, ',')) == (char *)0)
	    if ((nextpart = strchr(orpart, '|')) == (char *)0)
		nextpart = strchr(orpart, '!');
	if (nextpart)
	    *nextpart++ = '\0';

	/* ignore whitespace after or-bar characters */
	while (*orpart && isspace(*orpart))
	       orpart++;

	/* look for an anchored match for it at any offset in the target */
	for (anchor = t; *anchor; anchor++)
	{
	    ep = anchor;
	    for (sp = orpart; *sp; sp++)
	    {
		if (*sp == '\\')
		{
		    if (tolower(*++sp) != tolower(*ep))
			goto nextanchor;
		    ep++;
		}
		else if (*sp == '*')
		{
		    while (*ep && *ep != sp[1])
			ep++;
		}
	        else if (*sp == '?' || tolower(*sp) == tolower(*ep))
		    ep++;
		else
		    goto nextanchor;
	    }

	    /* an entire segment matched */
	    (void) memcpy(sys_match, anchor, ep - anchor);
	    sys_match[ep - anchor] = '\0';

	    (void) memcpy(sys_left, t, anchor - t);
	    sys_left[anchor - t] = '\0';

	    (void) strcpy(sys_right, ep);

	    return(matchcount);

	nextanchor:;
	}
    }

    sys_match[0] = sys_left[0] = sys_right[0] = '\0';
    return(FALSE);
}

/* match.c ends here */
