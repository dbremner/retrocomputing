#include "apl.h"
#include <stdlib.h>
#include <limits.h>


ex_deal()
{
	struct item *p;
	int m, n;
	double f;
	data d1, d2;

	m = topfix();
	n = topfix();
	if(m < 0 || m > n) error("deal D");
	p = newdat(DA, 1, m);
	datum = thread.iorg;
	for(; n!=0; n--) {
		f = m;
		f /= n;
		if(random()/(float)INT_MAX < f) {
			putdat(p, datum);
			m--;
		}
		datum += one;
	}
	m = p->size;
	while(m > 0) {
		f = random()/(float)INT_MAX;
		n = m * f;
		m--;
		if(n != m) {
			p->index = n;
			d1 = getdat(p);
			p->index = m;
			d2 = getdat(p);
			p->index = n;
			putdat(p, d2);
			p->index = m;
			putdat(p, d1);
		}
	}
	*sp++ = p;
}

data
ex_rand(d)
data d;
{
	double f;

	f = (random()/(float)INT_MAX) * d;
	d = floor(f) + thread.iorg;
	return(d);
}
