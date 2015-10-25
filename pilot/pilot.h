/* pilot.h -- types and #defines for IEEE PILOT interpreter */
/*
 *	$Id: pilot.h,v 1.8 1994/10/17 01:53:08 esr Exp $
 */

/* maximum file size */
#ifdef USG
#include <limits.h>
#else
#define PATH_MAX	1024
#endif

#define MAXUSES		64	/* depth of use stack */
#define MAXVARS		1024	/* maximum variables */
#define MAXNAME		10	/* maximum length of variable names */
#define MAXSTR		256	/* maximum string size */
#define MAXJUMPS	80	/* maximum jumps per JUMPMATCH */

#ifndef PILOTDIR
#define PILOTDIR	"/usr/lib/pilot/"
#endif /* PILOTDIR */

#define PLT	".p"		/* PILOT source file extension */

#ifndef TRUE
typedef int	bool;
#define TRUE	1
#define FALSE	0
#endif /* TRUE */

/* system variables */
extern int sys_expression;	/* value of latest numeric sum */
extern int sys_term;		/* value of latest numeric product */
extern int sys_factor;		/* value of literal, value, or expression */
extern int sys_nextstmt;	/* location of next statement */
extern int sys_uselevel;	/* number of USE statements executed */
extern char sys_answer[MAXSTR];	/* latest user response line */
extern int sys_matched;		/* true iff last match succeeded */
extern char sys_left[MAXSTR];	/* the part of %answer before %match */
extern char sys_match[MAXSTR];	/* the part of %answer matched */
extern char sys_right[MAXSTR];	/* the part of %answer before %match */
extern bool sys_satisfied;	/* true iff current guard true */
extern bool sys_relation;	/* true iff current rel_expr true */
extern char sys_text[MAXSTR];	/* value of current text buffer */
extern int sys_return[MAXUSES];	/* return stack */
extern int sys_status;		/* status of last SYSTEM command */

/* PILOT library functions */
extern void do_scrinit(void);		/* screen I/O initialization */
extern void do_scrend(void);		/* screen I/O de-initialization */
extern int do_match(char *, char *);	/* MATCH */
extern void do_graphic(char *);		/* GRAPHIC */
extern void do_file(char *);		/* FILE */
extern void do_problem(char *);		/* PROBLEM */
extern void do_pause(int);		/* PAUSE */
extern void do_link(char *);		/* LINK */
extern void do_clearhome(void);		/* CLEARHOME */
extern void do_cursaddr(int, int);	/* CURSADDR */
extern void do_clearline(void);		/* CLEARLINE */
extern void do_clearend(void);		/* CLEAREND */
extern bool namefile(char*,char*,char*);/* make source & stem fr filename */
extern int numconv(char *);		/* numeric conversion code */

/* pilot.h ends here */
