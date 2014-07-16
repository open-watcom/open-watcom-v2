#include <math.h>
#ifndef PI
#define PI 3.1415926535
#endif

void cosft(double y[], int n, int isign)

/*
  Calculates the cosine transform of a set y[1..n] of real-valued data points.
  The transformed data replaces the original data in array y.
  n must be a power of 2.
  Set isign to +1 for a transform and to -1 for an inverse transform.
  But the inverse transform array should be multiplied by 2/n.
*/

{
  int	 j, m, n2;
  double enf0, even, odd, sum, sume, sumo, y1, y2;
  double theta, wi = 0.0, wr = 1.0, wpi, wpr, wtemp;
  void	 realft(double*, int, int);

  theta	=  PI/(double) n;
  wtemp	=  sin(0.5*theta);
  wpr	= -2.0*wtemp*wtemp;
  wpi	=  sin(theta);
  sum	=  y[1];
  m	=  n >> 1;
  n2	=  n + 2;
  for (j = 2; j <= m; j++)
    {
      wr	 = (wtemp = wr)*wpr - wi*wpi + wr;
      wi	 =  wi*wpr + wtemp*wpi + wi;
      y1	 =  0.5*(y[j] + y[n2-j]);
      y2	 = (y[j] - y[n2-j]);
      y[j]	 =  y1 - wi*y2;
      y[n2-j]	 =  y1 + wi*y2;
      sum	+=  wr*y2;
    };
	    realft(y, m, 1);
  y[2]	 =  sum;
  for (j = 4; j <= n; j += 2)
    {
      sum	+=  y[j];
      y[j]	 =  sum;
    };
  if (isign  ==  -1)
    {
      even	 =  y[1];
      odd	 =  y[2];
      for (j = 3; j <= n-1; j += 2)
	{
	  even	+=  y[j];
	  odd	+=  y[j+1];
	};
      enf0	 =  2.0*(even - odd);
      sumo	 =  y[1] - enf0;
      sume	 = (2.0*odd/n) - sumo;
      y[1]	 =  0.5*enf0;
      y[2]	-=  sume;
      for (j = 3; j <= n-1; j += 2)
	{
	  y[j]		-=  sumo;
	  y[j+1]	-=  sume;
	};
    };
}
