/*
 * Focal, 1981.
 * Expression evaluation.
 */
#include "focal.h"

/*
 * Evaluate an expression.
 */
double
eval(void)
{
	double		val;
	double		rop;
	int	c;

	if ((c=getnb())=='+' || c=='-') {
		val = primary();
		if (c == '-')
			val = -val;
	} else {
		--ctp;
		val = primary();
	}
	while ((c=getnb())=='+' || c=='-') {
		rop = primary();
		if (c == '+')
			val = val + rop;
		else
			val = val - rop;
	}
	--ctp;
	return (val);
}

double
primary(void)
{
	double		val;
	double		rop;
	int	c;

	val = term();
	while ((c = getnb())=='*' || c=='/' || c=='^') {
		rop = term();
		if (c == '*')
			val = val * rop;
		else if (c == '/')
			val = val / rop;
		else 
			val = pow(val, rop);
	}
	--ctp;
	return (val);
}

double
term(void)
{
	int	c;
	char	*cp;
	double		val;
	int	fsign;
	int	fdot;
	int	fexp;
	int	type;
	int	subs = 0;
	struct	sym	*sp;
	char		id[NID];
	char		nbuf[20];

	if ((c = getnb())=='(' || c=='[' || c=='<') {
		val = eval();
		if (++c != ')')
			++c;
		if (c != getnb())
			diag("Mismatched enclosures");
		return (val);
	}
	if (c=='.' || isdigit(c)) {
		fsign = 1;
		fexp = 0;
		fdot = 0;
		if (c == '.')
			++fdot;
		cp = &nbuf[0];
		for (;;) {
			if (cp >= &nbuf[19])
				diag("Number too long");
			*cp++ = c;
			if ((c = *ctp++) == '.') {
				if (fdot++)
					break;
			} else if (c == 'e') {
				if (fexp++)
					break;
				fsign = 0;
				fdot = 1;
			} else if (c=='+' || c=='-') {
				if (fsign++)
					break;
			} else if (!isdigit(c))
				break;
		}
		--ctp;
		*cp = '\0';
		return (atof(nbuf));
	}
	if (!isalpha(c))
		diag("Expression syntax");
	cp = &id[0];
	do {
		if (cp < &id[NID-1])
			*cp++ = c;
		c = *ctp++;
	} while (isalnum(c));
	*cp = 0;
	if (id[0]=='f' && (sp=lookup(id, S_FUNC, 0))!=NULL) {
		while (c==' ' || c=='\t')
			c = *ctp++;
		if (c != '(')
			diag("Missing `(' for function");
		val = eval();
		if (getnb() != ')')
			diag("Missing `)' for function");
		return ((*sp->s_un.s_fp)(val));
	}
	type = S_SCAL;
	while (c==' ' || c=='\t')
		c = *ctp++;
	if (c == '(') {
		type = S_ARRAY;
		subs = (int) eval();
		if (getnb() != ')')
			diag("Missing ) in subscript");
	} else
		--ctp;
	if ((sp=lookup(id, type, subs)) == NULL)
		diag("Undefined variable");
	return (sp->s_un.s_value);
}


struct	sym *
lookup(const char *id, int type, int subs)
{
	struct sym *sp;
	int	ix;
	
	ix = hashsym(id, type, subs);
	sp = sym[ix];
	while (sp != NULL) {
		if (sp->s_type == type
		&& (type!=S_ARRAY || sp->s_subs==subs)
		&&  strcmp(id, sp->s_id) == 0)
			break;
		sp = sp->s_fp;
	}
	return (sp);
}

struct sym *
allocsym(const char *id)
{
    struct sym *sp = calloc(sizeof(*sp),1);
    if(sp) {
        sp->s_id = strdup(id);
        if(!sp->s_id) {
            free(sp);
        }
    }
    return sp;    
}

void
freesym(struct sym *s)
{
    if(s) //this case shouldn't occur
        free(s->s_id);
    free(s);
}

struct sym *
getsym(void)
{
	int c;
	char id[NID];
	char *cp;
	struct sym *sp;
	int subs = 0, type;
	int	ix;
	
	if (isalpha(c = getnb()) == 0)
		diag("Missing variable");
	cp = &id[0];
	do {
		if (cp < &id[NID-1])
			*cp++ = c;
		c = *ctp++;
	} while (isalnum(c));
	*cp = 0;
	type = S_SCAL;
	while (c==' ' || c=='\t')
		c = *ctp++;
	if (c == '(') {
		type = S_ARRAY;
		subs = (int) eval();
		if (getnb() != ')')
			diag("Bad subscript");
	} else
		--ctp;
	if ((sp=lookup(id, type, subs)) == NULL) {
		sp = allocsym(id);
		if (sp == NULL)
			diag("Out of space (symbols)");
		ix = hashsym(id, type, subs);
		sp->s_fp = sym[ix];
		sym[ix] = sp;
		sp->s_type = type;
		sp->s_subs = subs;
	}
	return (sp);
}

double
fsin(double arg)
{
	return (sin(arg));
}

double
fcos(double arg)
{
	return (cos(arg));
}

double
fexp(double arg)
{
	return (exp(arg));
}

double
flog(double arg)
{
	return (log(arg));
}

double
fatn(double arg)
{
	return (atan(arg));
}

double
fsqt(double arg)
{
	if (arg < 0.0)
		diag("Fsqt < 0.0");
	return (sqrt(arg));
}

double
fabt(double arg)
{
	if (arg < 0)
		return (-arg);
	return (arg);
}

double
fsgn(double arg)
{
	if (arg < 0)
		return (-1.0);
	return (1.0);
}

double
fitr(double arg)
{
	if (arg < 0)
		return (-floor(-arg));
	if (arg == 0)
		return (0.0);
	return (floor(arg));
}

double
fran(double arg)
{
	(void)arg;
    return (drand48());
}

