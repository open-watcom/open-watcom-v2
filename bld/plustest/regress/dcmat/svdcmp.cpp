#include <math.h>
#include <libc.h>
#include <iostream.h>
#ifndef PI
#define PI 3.1415926535
#endif

/*
PYTHAG computes sqrt(a^{2} + b^{2}) without destructive overflow or underflow.
*/
static double at, bt, ct;
#define PYTHAG(a, b) ((at = fabs(a)) > (bt = fabs(b)) ? \
(ct = bt/at, at*sqrt(1.0+ct*ct)): (bt ? (ct = at/bt, bt*sqrt(1.0+ct*ct)): 0.0))

static double maxarg1, maxarg2;
#define MAX(a, b) (maxarg1 = (a), maxarg2 = (b), (maxarg1) > (maxarg2) ? \
(maxarg1) : (maxarg2))

#define SIGN(a, b) ((b) < 0.0 ? -fabs(a): fabs(a))

void error(char error_text[])
    /* Numerical Recipes standard error handler.			*/
  {

    cerr	<< "Numerical Recipes run-time error...\n"
		<< error_text << "\n"
		<< "...now exiting to system...\n";
    exit(1);
  }

/*
Given a matrix a[m][n], this routine computes its singular value
decomposition, A = U*W*V^{T}.  The matrix U replaces a on output.
The diagonal matrix of singular values W is output as a vector w[n].
The matrix V (not the transpose V^{T}) is output as v[n][n].
m must be greater or equal to n;  if it is smaller, then a should be
filled up to square with zero rows.
*/
void svdcmp(double* a[], int m, int n, double w[], double* v[])
  {
    int flag, i, its, j, jj, k, l, nm;
    double c, f, h, s, x, y, z;
    double anorm = 0.0, g = 0.0, scale = 0.0;

    if (m < n)
      error("SVDCMP: Matrix A must be augmented with extra rows of zeros.");
    double* rv1 = new double [n];

    /* Householder reduction to bidiagonal form.			*/
    for (i = 0; i < n; i++)
      {
	l = i + 1;
        rv1[i] = scale*g;
        g = s = scale = 0.0;
        if (i < m)
	  {
	    for (k = i; k < m; k++)
	      scale += fabs(a[k][i]);
	    if (scale)
	      {
	        for (k = i; k < m; k++)
	          {
		    a[k][i] /= scale;
		    s += a[k][i]*a[k][i];
	          };
	        f = a[i][i];
	        g = -SIGN(sqrt(s), f);
	        h = f*g - s;
	        a[i][i] = f - g;
	        if (i != n - 1)
	          {
		    for (j = l; j < n; j++)
		      {
		        for (s  = 0.0, k = i; k < m; k++)
		          s += a[k][i]*a[k][j];
		        f = s/h;
		        for ( k = i; k < m; k++)
			  a[k][j] += f*a[k][i];
		      };
	          };
	        for (k = i; k < m; k++)
	          a[k][i] *= scale;
	      };
          };
        w[i] = scale*g;
        g = s= scale = 0.0;
        if (i < m && i != n - 1)
          {
	    for (k = l; k < n; k++)
	      scale += fabs(a[i][k]);
	    if (scale)
	      {
	        for (k = l; k < n; k++)
	          {
		    a[i][k] /= scale;
		    s += a[i][k]*a[i][k];
	          };
	        f = a[i][l];
	        g = -SIGN(sqrt(s), f);
	        h = f*g - s;
	        a[i][l] = f - g;
	        for (k = l; k < n; k++)
	          rv1[k] = a[i][k]/h;
	        if (i != m - 1)
	          {
		    for (j = l; j < m; j++)
		      {
		        for (s = 0.0, k = l; k < n; k++)
		          s += a[j][k]*a[i][k];
		        for (k = l; k < n; k++)
		          a[j][k] += s*rv1[k];
		      };
	          };
	        for (k = l; k < n; k++)
	          a[i][k] *= scale;
	      };
	  };
        anorm = MAX(anorm, (fabs(w[i]) + fabs(rv1[i])));
      };
    /* Accumulation of right-hand transformations.			*/
    for (i = n - 1; 0 <= i; i--)
      {
	if (i < n - 1)
	  {
	    if (g)
	      {
		for (j = l; j < n; j++)
		  v[j][i] = (a[i][j]/a[i][l])/g;
		  /* Double division to avoid possible underflow:	*/
		for (j = l; j < n; j++)
		  {
		    for (s = 0.0, k = l; k < n; k++)
		      s += a[i][k]*v[k][j];
		    for (k = l; k < n; k++)
		      v[k][j] += s*v[k][i];
		  };
	      };
	    for (j = l; j < n; j++)
	      v[i][j] = v[j][i] = 0.0;
	  };
	v[i][i] = 1.0;
	g = rv1[i];
	l = i;
      };
    /* Accumulation of left-hand transformations.			*/
    for (i = n - 1; 0 <= i; i--)
      {
	l = i + 1;
	g = w[i];
	if (i < n - 1)
	  for (j = l; j < n; j++)
	    a[i][j] = 0.0;
	if (g)
	  {
	    g = 1.0/g;
	    if (i != n - 1)
	      {
		for (j = l; j < n; j++)
		  {
		    for (s = 0.0, k = l; k < m; k++)
		      s += a[k][i]*a[k][j];
		    f = (s/a[i][i])*g;
		    for (k = i; k < m; k++)
		      a[k][j] += f*a[k][i];
		  };
	       };
	    for (j = i; j < m; j++)
	      a[j][i] *= g;
	  }
	else
	  for (j = i; j < m; j++)
	    a[j][i] = 0.0;
	++a[i][i];
      };
    /* Diagonalization of the bidiagonal form.				*/
    for (k = n - 1; 0 <= k; k--)	/* Loop over singular values.	*/
      {
	for (its = 0; its < 30; its++)	/* Loop over allowed iterations.*/
	  {
	    flag = 1;
	    for (l = k; 0 <= l; l--)	/* Test for splitting:		*/
	      {
		nm = l - 1;		/* Note that rv1[0] is always zero.*/
		if (fabs(rv1[l]) + anorm == anorm)
		  {
		    flag = 0;
		    break;
		  };
		if (fabs(w[nm]) + anorm == anorm)
		  break;
	      };
	    if (flag)
	      {
		c = 0.0;		/* Cancellation of rv1[l], if l>0:*/
		s = 1.0;
		for (i = l; i <= k; i++) {
		    f = s*rv1[i];
		    if (fabs(f) + anorm != anorm)
		      {
			g = w[i];
			h = PYTHAG(f, g);
			w[i] = h;
			h = 1.0/h;
			c = g*h;
			s = (-f*h);
			for (j = 0; j < m; j++)
			  {
			    y = a[j][nm];
			    z = a[j][i];
			    a[j][nm] = y*c + z*s;
			    a[j][i]  = z*c - y*s;
			  };
		      };
		  };
	      };
	    z = w[k];
	    if (l == k)		/* Convergence.				*/
	      {
		if (z < 0.0)	/* Singular value is made non-negative.	*/
		  {
		    w[k] = -z;
		    for (j = 0; j < n; j++)
		      v[j][k] = (-v[j][k]);
		  };
		break;
	      };
	    if (its == 29)
	      error("No convergence in 30 SVDCMP iterations.");
	    x = w[l];		/* Shift from bottom 2-by-2 minor.	*/
	    nm = k - 1;
	    y = w[nm];
	    g = rv1[nm];
	    h = rv1[k];
	    f = ((y - z)*(y + z) + (g - h)*(g + h))/(2.0*h*y);
	    g = PYTHAG(f, 1.0);
	    f = ((x - z)*(x + z) + h*((y/(f + SIGN(g, f))) - h))/x;
	    /* Next QR transformation:					*/
	    c = s = 1.0;
	    for (j = l; j <= nm; j++)
	      {
		i = j + 1;
		g = rv1[i];
		y = w[i];
		h = s*g;
		g = c*g;
		z = PYTHAG(f, h);
		rv1[j] = z;
		c = f/z;
		s = h/z;
		f = x*c + g*s;
		g = g*c - x*s;
		h = y*s;
		y = y*c;
		for (jj = 0; jj < n;  jj++)
		  {
		    x = v[jj][j];
		    z = v[jj][i];
		    v[jj][j] = x*c + z*s;
		    v[jj][i] = z*c - x*s;
		  };
		z = PYTHAG(f, h);
		w[j] = z;	/* Rotation can be arbitrary if z = 0.	*/
		if (z)
		  {
		    z = 1.0/z;
		    c = f*z;
		    s = h*z;
		  };
		f = (c*g) + (s*y);
		x = (c*y) - (s*g);
		for (jj = 0; jj < m; jj++)
		  {
		    y = a[jj][j];
		    z = a[jj][i];
		    a[jj][j] = y*c + z*s;
		    a[jj][i] = z*c - y*s;
		  };
	      };
	    rv1[l] = 0.0;
	    rv1[k] = f;
	    w[k] = x;
	  };
      };
    delete rv1;
  }
