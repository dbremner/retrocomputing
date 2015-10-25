/******************************************************************************

NAME
  numconv.c -- numeric conversion for PILOT

SYNOPSIS
   int numconv(str)		-- function for implicit numeric conversion
   char *str;

DESCRIPTION
   See also nonstd.c and file.c.

AUTHOR
   Eric S. Raymond <esr@snark.thyrsus.com>, November 1991.  The
author retains copyright on this implementation.  Permission for
nonprofit and educational use is granted to all; if you're planning to
make money with this or any code derived from it, check with the
author first.

******************************************************************************/
/*
 *	$Id: numconv.c,v 1.6 1994/09/14 14:41:45 esr Exp $
 */

#ifndef lint
static char *sccsid = "$Id: numconv.c,v 1.6 1994/09/14 14:41:45 esr Exp $";
#endif /* lint */

/*LINTLIBRARY*/
#include <stdio.h>
#include "pilot.h"
#include "gencode.h"

int numconv(char *str)
/* forced (implicit) numeric-conversion function */
{
    return(atoi(str));
}

/* numconv.c ends here */
