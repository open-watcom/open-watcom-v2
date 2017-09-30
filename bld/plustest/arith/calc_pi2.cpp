/*  This program employs the recently discovered digit extraction scheme
    to produce hex digits of pi.  This code is valid up to ic = 2^24 on 
    systems with IEEE arithmetic. */
/*  To compile type : cc -O3 pi.c -lm */
/*  Here ic = 100000 and takes a few seconds on a SGI r4000 machine to run.
    It took 124 seconds for ic = 1000000 */
/*  David H. Bailey     960429 */

#include <stdio.h>
#include <assert.h>
#include <math.h>

void pi_digit( long ic )
{
  double pid, s1, s2, s3, s4;
  extern double series (long m, long n);
  extern void ihex (double x, long m, char c[]);
#define NHX 16
  char chx[NHX];

/*  ic is the hex digit position -- output begins at position ic + 1. */

  s1 = series (1, ic);
  s2 = series (4, ic);
  s3 = series (5, ic);
  s4 = series (6, ic);
  pid = 4. * s1 - 2. * s2 - s3 - s4;
  pid = pid - (long) pid + 1.;
  ihex (pid, NHX, chx);
  printf ("%20.15f %12.12s\n", pid, chx);
}

int main() {
    long i,j;
    for( i = 0; i < 32000; i += 4000 ) {
        for( j = i; j < i+2; ++j ) {
            pi_digit( j );
        }
    }
    return 0;
}

void ihex (double x, long nhx, char chx[])

/*  This returns, in chx, the first nhx hex digits of the fraction of x. */

{
  int i;
  int dig;
  double y;
  char hx[] = "0123456789ABCDEF";

  y = fabs (x);

  for (i = 0; i < nhx; i++){
    y = 16. * (y - floor (y));
    dig = (int) y;
    assert( dig >= 0 && dig <= 15 );
    chx[i] = hx[ dig ];
  }
}

double series (long m, long ic)

/*  This routine evaluates the series  sum_k 16^(ic-k)/(8*k+m) 
    using the modular exponentiation technique. */

{
  long k;
  double ak, eps, p, s, t;
  extern double expm (double x, double y);
#define eps 1e-17

  s = 0.;

/*  Sum the series up to ic. */

  for (k = 0; k < ic; k++){
    ak = 8 * k + m;
    p = ic - k;
    t = expm (p, ak);
    s = s + t / ak;
    s = s - (long) s;
  }

/*  Compute a few terms where k >= ic. */

  for (k = ic; k <= ic + 100; k++){
    ak = 8 * k + m;
    t = pow (16., (double) (ic - k)) / ak;
    if (t < eps) break;
    s = s + t;
    s = s - (long) s;
  }
  return s;
}

double expm (double p, double ak)

/*  expm = 16^p mod ak.  This routine uses the left-to-right binary 
    exponentiation scheme.  It is valid for  ak <= 2^24. */

{
  long i, j;
  double p1, pt, r;
#define ntp 25
  static double tp[ntp];
  static int tp1 = 0;

/*  If this is the first call to expm, fill the power of two table tp. */

  if (tp1 == 0) {
    tp1 = 1;
    tp[0] = 1.;

    for (i = 1; i < ntp; i++) tp[i] = 2. * tp[i-1];
  }

  if (ak == 1.) return 0.;

/*  Find the greatest power of two less than or equal to p. */

  for (i = 0; i < ntp; i++) if (tp[i] > p) break;

  pt = tp[i-1];
  p1 = p;
  r = 1.;

/*  Perform binary exponentiation algorithm modulo ak. */

  for (j = 1; j <= i; j++){
    if (p1 >= pt){
      r = 16. * r;
      r = r - (long) (r / ak) * ak;
      p1 = p1 - pt;
    }
    pt = 0.5 * pt;
    if (pt >= 1.){
      r = r * r;
      r = r - (long) (r / ak) * ak;
    }
  }

  return r;
}

