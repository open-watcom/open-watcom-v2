#include <math.h>
#ifndef PI
#define PI M_PI
#endif

#define SWAP(a, b) tempr = (a); (a) = (b); (b) = tempr

void four1(double data[], int nn, int isign)

/*
Replaces data by its discrete Fourier transform, if isign is input as 1;
or replaces data by nn times its inverse discrete Fourier transform,
if isign is input as -1.
data is a complex array of length nn, input as a real array data[1..2*nn].
nn MUST be an integer power of 2 (this is not checked for!).
*/
{
  int n, mmax, m, j, istep, i;
  /* Double precision for the trigonometric recurrences.		*/
  double wtemp, wr, wpr, wpi, wi, theta;
  double tempr, tempi;

  n = nn << 1;
  j = 1;
  /* This is the bit-reversal section of the routine.			*/
  for (i = 1; i < n; i += 2)
    {
      if (j > i)
	{
	  SWAP(data[j  ],data[i  ]);	// Exchange the two complex numbers.
	  SWAP(data[j+1],data[i+1]);
	};
      m = n >> 1;
      while (m >= 2 && j > m)
	{
	  j  -= m;
	  m >>= 1;
	};
      j += m;
    };
  /* Here begins the Danielson-Lanczos section of the routine.		*/
  mmax = 2;
  while (n > mmax)
    {
      /* Initialize trigonometric recurrence.				*/
      theta	= 6.28318530717959/(isign*mmax);
      wtemp	= sin(0.5*theta);
      wpr	= -2.0*wtemp*wtemp;
      wpi	= sin(theta);
      wr	= 1.0;
      wi	= 0.0;
      /* Here are the two nested inner loops.				*/
      istep	= 2*mmax;
      for (m = 1; m < mmax; m += 2)
	{
	  for (int i = m; i <= n; i += istep)
	    {
	      /* This is the Danielson-Lanczos formula.			*/
	      j		 = i + mmax;
	      tempr	 = wr*data[j  ] - wi*data[j+1];
	      tempi	 = wr*data[j+1] + wi*data[j  ];
	      data[j  ]	 = data[i  ] - tempr;
	      data[j+1]	 = data[i+1] - tempi;
	      data[i  ]	+= tempr;
	      data[i+1]	+= tempi;
	    };					// Trigonometric recurrence.
	  wr = (wtemp = wr)*wpr - wi*wpi + wr;
	  wi = wi*wpr + wtemp*wpi + wi;
	};
      mmax = istep;
    };
  }
