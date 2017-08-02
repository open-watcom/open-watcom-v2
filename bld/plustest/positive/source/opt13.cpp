#include "fail.h"
#include <math.h>
#include <assert.h>

double drand48() {
    double x;

    x = rand();
    x /= RAND_MAX;
    return x;
}

/* Return random integer in [1,maxint], perturbed by [-1,1]*noise */
double randnoisyint(unsigned long maxint, double noise)
{
	double d = 1.0 + (rand()%maxint) + (2*drand48()-1)*noise;
	assert( d != 0 );
	return d;
}

int main()
{
	unsigned long maxint;
	int samplesize=1e3;
	int b, i, cnt;
	double num, den, tol, noise;

	for (tol = 1e-5; tol >= 1e-11; tol *= .01) {
		for (maxint = 10; maxint <= 1e5; maxint *= 10) {
			for (noise = .999, b = 0; b < 14; noise *= .1, b++) {
				for (i = 0, cnt = 0; i < samplesize; i++) {
					num = randnoisyint(maxint, noise);
					assert( num != 0 );
					den = randnoisyint(maxint, noise);
					assert( den != 0 );
					cnt += (((num-(num/den)*den)/num)>tol);
				}
				if( cnt != 0 ) fail(__LINE__);
			}
		}
	}
	_PASS;
}
