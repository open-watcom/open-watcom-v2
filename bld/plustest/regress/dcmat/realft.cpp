#include <math.h>
#ifndef PI
#define PI 3.1415926535
#endif

void realft(double data[], int n, int isign)

/*
  Calculates the Fourier Transform of a set of 2n real-valued data points
  stored in array data[1..2n] replacing it by the positive frequency half
  of it's complex Fourier Transform if isign is set to +1.
  The real-valued first and last components of the complex transform
  are returned as elements data[1] and data[2] respectively.
  n must be a power of 2.
  Calculates the inverse transform of a complex data array
  which is the transform of real data if isign is set to -1.
  But the inverse transform array should be multiplied by 1/n.
*/

{
  int	 i, i1, i2, i3, i4, n2p3;
  double c1 = 0.5, c2, h1r, h1i, h2r, h2i;
  double wr, wi, wpr, wpi, wtemp, theta;
  void	 four1(double*, int, int);

  theta	 = PI/(double) n;
  if (isign == 1)
    {
      c2	 = -0.5;
		    four1(data, n, 1);
    }
  else
    {
      c2	 =  0.5;
      theta	 = -theta;
    };
  wtemp	 =  sin(0.5*theta);
  wpr	 = -2.0*wtemp*wtemp;
  wpi	 =  sin(theta);
  wr	 =  1.0 + wpr;
  wi	 =  wpi;
  n2p3	 =  2*n + 3;
  for (i = 2; i <= n/2; i++ )
    {
      i4	 =   1 + (i3 = n2p3 - (i2 = 1 + (i1 = i + i - 1)));
      h1r	 =  c1*(data[i1] + data[i3]);
      h1i	 =  c1*(data[i2] - data[i4]);
      h2r	 = -c2*(data[i2] + data[i4]);
      h2i	 =  c2*(data[i1] - data[i3]);
      data[i1]	 =  h1r + wr*h2r - wi*h2i;
      data[i2]	 =  h1i + wr*h2i + wi*h2r;
      data[i3]	 =  h1r - wr*h2r + wi*h2i;
      data[i4]	 = -h1i + wr*h2i + wi*h2r;
      wr	 = (wtemp = wr)*wpr - wi*wpi + wr;
      wi	 =  wi*wpr + wtemp*wpi + wi;
    }
  if (isign == 1)
    {
      data[1]	 = (h1r = data[1]) + data[2];
      data[2]	 =  h1r - data[2];
    }
  else
    {
      data[1]	 =  c1*((h1r = data[1]) + data[2]);
      data[2]	 =  c1*(h1r - data[2]);
		    four1(data, n, -1);
    };
}
