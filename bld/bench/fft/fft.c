#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "timer.h"
#include "report.h"

#define FFT_SIZE (131072*2)

#define REAL double

 REAL real[FFT_SIZE * 2];
 REAL imag[FFT_SIZE * 2];

void realifft( int n, double *real );
void fft( int n, double *real, double *imag );
void fht( REAL *fz, int n );
void ifft( int n, double *real, double *imag );
void realfft( int n, double *real );

int main(argc,argv)
int argc;
char **argv;
{
 long num=0,i,j;
 /* long status; */
 REAL ssq=0.0;
 REAL scale;
 double runtime;

 if (argc > 1) num = atoi(argv[1]);
 if (num == 0) num = FFT_SIZE;

 if (num > FFT_SIZE)
   {
   printf("\n");
   printf("Error:\n");
   printf("Number of Points %d exceeds alloted array size\n\n",num);
   exit(0);
   }
 
 for (i=0;i<num;i++)
    {
     real[i] = (REAL)i;
     imag[i] = 0.0;
    }
 
 scale = 1.0 / (REAL)num;
 
 printf("\n");
 printf("FFT-Mayer: 04 Oct 1994\n");
 printf("FFT  Size: %6d\n",num);

 fft(num, real, imag);
 ifft(num, real, imag);
 for (j=0;j<num*2;j++)  {real[j]*=scale;imag[j]*=scale;}
 
 TimerOn();
 fft(num, real, imag);
 ifft(num, real, imag);
 TimerOff();
 runtime   = TimerElapsed();
 Report( "fft", runtime );
 for (j=0;j<num*2;j++)  {real[j]*=scale;imag[j]*=scale;}
 
 runtime = runtime / 2.0;
 printf("Run Time (sec) = %11.5f\n",runtime);

 for (ssq=0,i=0;i<num;i++) ssq+=(real[i]-(REAL)i)*(real[i]-(REAL)i);
 printf("ssq errors %#.2g\n\n",ssq);
 exit( EXIT_SUCCESS );
 return( 0 );
}


/*
** FFT and FHT routines
**  Copyright 1988, 1993; Ron Mayer
**  
**  fht(fz,n);
**      Does a hartley transform of "n" points in the array "fz".
**  fft(n,real,imag)
**      Does a fourier transform of "n" points of the "real" and
**      "imag" arrays.
**  ifft(n,real,imag)
**      Does an inverse fourier transform of "n" points of the "real"
**      and "imag" arrays.
**  realfft(n,real)
**      Does a real-valued fourier transform of "n" points of the
**      "real" and "imag" arrays.  The real part of the transform ends
**      up in the first half of the array and the imaginary part of the
**      transform ends up in the second half of the array.
**  realifft(n,real)
**      The inverse of the realfft() routine above.
**      
**      
** NOTE: This routine uses at least 2 patented algorithms, and may be
**       under the restrictions of a bunch of different organizations.
**       Although I wrote it completely myself; it is kind of a derivative
**       of a routine I once authored and released under the GPL, so it
**       may fall under the free software foundation's restrictions;
**       it was worked on as a Stanford Univ project, so they claim
**       some rights to it; it was further optimized at work here, so
**       I think this company claims parts of it.  The patents are
**       held by R. Bracewell (the FHT algorithm) and O. Buneman (the
**       trig generator), both at Stanford Univ.
**       If it were up to me, I'd say go do whatever you want with it;
**       but it would be polite to give credit to the following people
**       if you use this anywhere:
**           Euler     - probable inventor of the fourier transform.
**           Gauss     - probable inventor of the FFT.
**           Hartley   - probable inventor of the hartley transform.
**           Buneman   - for a really cool trig generator
**           Mayer(me) - for authoring this particular version and
**                       including all the optimizations in one package.
**       Thanks,
**       Ron Mayer; mayer@acuson.com
**
*/

#define GOOD_TRIG
#include "trigtbl.h"
char fht_version[] = "Brcwl-Hrtly-Ron-dbld";

#define SQRT2_2   0.70710678118654752440084436210484
#define SQRT2   2*0.70710678118654752440084436210484

void fht( REAL *fz, int n )
{
 /* REAL a,b; */
 /* REAL c1,s1,s2,c2,s3,c3,s4,c4; */
 /* REAL f0,g0,f1,g1,f2,g2,f3,g3; */
 int i,k,k1,k2,k3,k4,kx;
 REAL *fi,*fn,*gi;
 TRIG_VARS;

 for (k1=1,k2=0;k1<n;k1++)
    {
     REAL a;
     for (k=n>>1; (!((k2^=k)&k)); k>>=1);
     if (k1>k2)
	{
	     a=fz[k1];fz[k1]=fz[k2];fz[k2]=a;
	}
    }
 for ( k=0 ; (1<<k)<n ; k++ );
 k  &= 1;
 if (k==0)
    {
	 for (fi=fz,fn=fz+n;fi<fn;fi+=4)
	    {
	     REAL f0,f1,f2,f3;
	     f1     = fi[0 ]-fi[1 ];
	     f0     = fi[0 ]+fi[1 ];
	     f3     = fi[2 ]-fi[3 ];
	     f2     = fi[2 ]+fi[3 ];
	     fi[2 ] = (f0-f2);  
	     fi[0 ] = (f0+f2);
	     fi[3 ] = (f1-f3);  
	     fi[1 ] = (f1+f3);
	    }
    }
 else
    {
	 for (fi=fz,fn=fz+n,gi=fi+1;fi<fn;fi+=8,gi+=8)
	    {
	     REAL s1,c1,s2,c2,s3,c3,s4,c4,g0,f0,f1,g1,f2,g2,f3,g3;
	     c1     = fi[0 ] - gi[0 ];
	     s1     = fi[0 ] + gi[0 ];
	     c2     = fi[2 ] - gi[2 ];
	     s2     = fi[2 ] + gi[2 ];
	     c3     = fi[4 ] - gi[4 ];
	     s3     = fi[4 ] + gi[4 ];
	     c4     = fi[6 ] - gi[6 ];
	     s4     = fi[6 ] + gi[6 ];
	     f1     = (s1 - s2);        
	     f0     = (s1 + s2);
	     g1     = (c1 - c2);        
	     g0     = (c1 + c2);
	     f3     = (s3 - s4);        
	     f2     = (s3 + s4);
	     g3     = SQRT2*c4;         
	     g2     = SQRT2*c3;
	     fi[4 ] = f0 - f2;
	     fi[0 ] = f0 + f2;
	     fi[6 ] = f1 - f3;
	     fi[2 ] = f1 + f3;
	     gi[4 ] = g0 - g2;
	     gi[0 ] = g0 + g2;
	     gi[6 ] = g1 - g3;
	     gi[2 ] = g1 + g3;
	    }
    }
 if (n<16) return;

 do
    {
     REAL s1,c1;
     k  += 2;
     k1  = 1  << k;
     k2  = k1 << 1;
     k4  = k2 << 1;
     k3  = k2 + k1;
     kx  = k1 >> 1;
	 fi  = fz;
	 gi  = fi + kx;
	 fn  = fz + n;
	 do
	    {
	     REAL g0,f0,f1,g1,f2,g2,f3,g3;
	     f1      = fi[0 ] - fi[k1];
	     f0      = fi[0 ] + fi[k1];
	     f3      = fi[k2] - fi[k3];
	     f2      = fi[k2] + fi[k3];
	     fi[k2]  = f0         - f2;
	     fi[0 ]  = f0         + f2;
	     fi[k3]  = f1         - f3;
	     fi[k1]  = f1         + f3;
	     g1      = gi[0 ] - gi[k1];
	     g0      = gi[0 ] + gi[k1];
	     g3      = SQRT2  * gi[k3];
	     g2      = SQRT2  * gi[k2];
	     gi[k2]  = g0         - g2;
	     gi[0 ]  = g0         + g2;
	     gi[k3]  = g1         - g3;
	     gi[k1]  = g1         + g3;
	     gi     += k4;
	     fi     += k4;
	    } while (fi<fn);
     TRIG_INIT(k,c1,s1);
     for (i=1;i<kx;i++)
	{
	 REAL c2,s2;
	 TRIG_NEXT(k,c1,s1);
	 c2 = c1*c1 - s1*s1;
	 s2 = 2*(c1*s1);
	     fn = fz + n;
	     fi = fz +i;
	     gi = fz +k1-i;
	     do
		{
		 REAL a,b,g0,f0,f1,g1,f2,g2,f3,g3;
		 b       = s2*fi[k1] - c2*gi[k1];
		 a       = c2*fi[k1] + s2*gi[k1];
		 f1      = fi[0 ]    - a;
		 f0      = fi[0 ]    + a;
		 g1      = gi[0 ]    - b;
		 g0      = gi[0 ]    + b;
		 b       = s2*fi[k3] - c2*gi[k3];
		 a       = c2*fi[k3] + s2*gi[k3];
		 f3      = fi[k2]    - a;
		 f2      = fi[k2]    + a;
		 g3      = gi[k2]    - b;
		 g2      = gi[k2]    + b;
		 b       = s1*f2     - c1*g3;
		 a       = c1*f2     + s1*g3;
		 fi[k2]  = f0        - a;
		 fi[0 ]  = f0        + a;
		 gi[k3]  = g1        - b;
		 gi[k1]  = g1        + b;
		 b       = c1*g2     - s1*f3;
		 a       = s1*g2     + c1*f3;
		 gi[k2]  = g0        - a;
		 gi[0 ]  = g0        + a;
		 fi[k3]  = f1        - b;
		 fi[k1]  = f1        + b;
		 gi     += k4;
		 fi     += k4;
		} while (fi<fn);
	}
     TRIG_RESET(k,c1,s1);
    } while (k4<n);
}


void ifft( int n, double *real, double *imag )
{
 double a,b,c,d;
 double q,r,s,t;
 int i,j,k;
 fht(real,n);
 fht(imag,n);
 for (i=1,j=n-1,k=n/2;i<k;i++,j--) {
  a = real[i]; b = real[j];  q=a+b; r=a-b;
  c = imag[i]; d = imag[j];  s=c+d; t=c-d;
  imag[i] = (s+r)*0.5;  imag[j] = (s-r)*0.5;
  real[i] = (q-t)*0.5;  real[j] = (q+t)*0.5;
 }
}

void realfft( int n, double *real )
{
 double a,b /* ,c,d */;
 int i,j,k;
 fht(real,n);
 for (i=1,j=n-1,k=n/2;i<k;i++,j--) {
  a = real[i];
  b = real[j];
  real[j] = (a-b)*0.5;
  real[i] = (a+b)*0.5;
 }
}

void fft( int n, double *real, double *imag )
{
 double a,b,c,d;
 double q,r,s,t;
 int i,j,k;
 for (i=1,j=n-1,k=n/2;i<k;i++,j--) {
  a = real[i]; b = real[j];  q=a+b; r=a-b;
  c = imag[i]; d = imag[j];  s=c+d; t=c-d;
  real[i] = (q+t)*.5; real[j] = (q-t)*.5;
  imag[i] = (s-r)*.5; imag[j] = (s+r)*.5;
 }
 fht(real,n);
 fht(imag,n);
}

void realifft( int n, double *real )
{
 double a,b /* ,c,d */;
 int i,j,k;
 for (i=1,j=n-1,k=n/2;i<k;i++,j--) {
  a = real[i];
  b = real[j];
  real[j] = (a-b);
  real[i] = (a+b);
 }
 fht(real,n);
}
