int	main (void);
void	builtin (char *cp, double (*fp)(double));
void	process (void);
void	ask (void);
void	badline (void);
void	pushcntl (void);
void	popcntl (void);
void	popdo (void);
void	popfor (void);
void	clearfors (struct sym *sp);
void	inject (int c);
int	focal_getline (char *cp, FILE *fp);
void	type (void);
void	save (struct lno *lnop, FILE *fp);
void	erasesyms (void);

double	eval (void);
double	primary (void);
double	term (void);
struct sym	*lookup (char *id, int type, int subs);
struct sym	*getsym (void);

#ifdef FUNCTIONS
double	fsin (double arg);
double	fcos (double arg);
double	fexp (double arg);
double	flog (double arg);
double	fatn (double arg);
double	fsqt (double arg);
double	fabt (double arg);
double	fsgn (double arg);
double	fitr (double arg);
double	fran (double arg);
#endif

void	library (void);
void	catchcc (void);
void	onintr (void);
void	putline (struct line *lp, FILE *fp);
void	getlno (struct lno *lnop, int c);
int	getnum (int c);
struct line	*alocline (char *cp);
void	diag (char *s);
int	getnb (void);
double	drand48 (void);
