/*-------------------Start heapsort.c program-------------------*/

/****************************************************************/
/*                         HEAPSORT                             */
/*                     C Program Source                         */
/*          Heapsort program for variable sized arrays          */
/*                 Version 1.0, 04 Oct 1992                     */
/*             Al Aburto (aburto@marlin.nosc.mil)               */
/*                      ('ala' on BIX)                          */
/*                                                              */
/* Based on the Heap Sort code in 'Numerical Recipes in C' by   */
/* William H. Press, Brian P. Flannery, Saul A. Teukolsky, and  */
/* William T. Vetterling, Cambridge University Press, 1990,     */
/* ISBN 0-521-35465-X.                                          */
/*                                                              */
/* The MIPS rating is based upon the program run time (runtime) */
/* for one iteration and a gcc 2.1 unoptimized (gcc -DUNIX)     */
/* assembly dump count of instructions per iteration for a i486 */
/* machine (assuming 80386 code).  This is the reference used.  */
/*                                                              */
/* The maximum amount of memory allocated is based on the 'imax'*/
/* variable in main(). Memory size = (2000*sizeof(long))*2^imax.*/
/* imax is currently set to 8, but this value may be increased  */
/* or decreased depending upon your system memory limits. For   */
/* standard Intel PC CPU machines a value of imax = 3 must be   */
/* used else your system may crash or hang up despite code in   */
/* the program to prevent this.                                 */
/****************************************************************/

/****************************************************/
/* Example Compilation:                             */
/* (1) UNIX Systems:                                */
/*     cc -DUNIX -O heapsort.c -o heapsort          */
/*     cc -DUNIX heapsort.c -o heapsort             */
/****************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "timer.h"
#include "report.h"


double nulltime,runtime,sta,stb,dtime();
double emips,hmips,lmips,smips[21];

long bplong,ErrorFlag;

long NLoops[21];


void main()
{

long  i,j,k,p,imax;

bplong = sizeof(long);

printf("\n   Heap Sort C Program\n");
printf("   Version 1.0, 04 Oct 1992\n\n");

printf("   Size of long (bytes): %d\n\n",bplong);

printf("   Array Size    RunTime      Scale    MIPS\n");       
printf("    (bytes)       (sec)\n");

				   /* NLoops[] holds number of loops  */
				   /* (iterations) to conduct. Preset */
				   /* to 1 iteration.                 */
for( i=0 ; i<= 20 ; i++)
{
 NLoops[i] = 1;
}
				   /* Predetermine runtime (sec) for  */
				   /* memory size 2000 * sizeof(long),*/
				   /* and 256 iterations. p = 0 means */
				   /* don't print the result.         */
j = 2000;
k = 256;
p = 0;
HSORT(j,k,p);
				   /* Set number of iterations (loops)*/
				   /* based on runtime above --- so   */
				   /* program won't take forever on   */
				   /* the slower machines.            */
i = 8;
if ( runtime > 0.125 ) i = 1;

NLoops[0] =  32 * i; 
NLoops[1] =  16 * i; 
NLoops[2] =   8 * i;
NLoops[3] =   4 * i;
NLoops[4] =   2 * i;
NLoops[5] =       i;
NLoops[6] =   i / 2;
NLoops[7] =   i / 4;

if ( i == 1 )
{
NLoops[6]  = 1;
NLoops[7]  = 1;
}
				   /* Redo the first run and print    */
				   /* the results.                    */
j = 2000;
k = NLoops[0];
p = 1;
HSORT(j,k,p);
				   /* Save estimated mips result      */
smips[0] = emips;

j = 2000;
ErrorFlag = 0;
				   /* Now do it for memory sizes up to */ 
				   /* (2000*sizeof(long)) * (2 ** imax)*/
				   /* where imax determines maximum    */
				   /* amount of memory allocated.      */
				   /* Currently I set imax = 8, so if  */
				   /* sizeof(long) = 4 program will run*/
				   /* from 8000, 16000, ..., and up to */
				   /* 2048000 byte memory size. You can*/
				   /* increase imax, but imax = 8 is   */
				   /* limit for this test program.     */
imax = 8;
for( i=1 ; i<= imax ; i++)
{
   j = 2 * j;

   k = NLoops[i];

   HSORT(j,k,p);
   smips[i] = emips;

   if( ErrorFlag > 0L ) break;

}

if( ErrorFlag == 2L )
{
printf("\n   Could Not Allocate Memory for Array Size: %ld\n",j*bplong);
}

hmips = 0.0;
lmips = 1.0e+06;
for( k = 0; k < i; k++)
{
if( smips[k] > hmips ) hmips = smips[k];
if( smips[k] < lmips ) lmips = smips[k];
}

printf("\n   Runtime is the average for 1 iteration.\n");
printf("   High MIPS = %8.2lf\n",hmips);
printf("   Low  MIPS = %8.2lf\n\n",lmips);

exit( EXIT_SUCCESS );

}                                  /* End of main */


/*************************/
/*  Heap Sort Program    */
/*************************/

int HSORT(m,n,p)
long m,n,p;
{

register long *base;
register long i,j,k,l;
register long size;

long  iter,msize,iran,ia,ic,im,ih,ir;
long  count,ca,cb,cc,cd,ce,cf;

char buffer[ 80 ];

msize = m * bplong;
size  = m - 1;
base  = (long *)malloc((unsigned)msize);

ia = 106;
ic = 1283;
im = 6075;
ih = 1001;

   ErrorFlag = 0L;

   if( !base )
     {
     ErrorFlag = 2L;
     return 0;
     }

   count = 0;
   TimerOn();
   for(iter=1 ; iter<=n ; iter++)       /* Do 'n' iterations */             

   {
	iran = 47;                        /* Fill with 'random' numbers */
	for(i=1 ; i<=size ; i++)                      
	{
	iran = (iran * ia + ic) % im;
	*(base+i) = 1 + (ih * iran) / im;
	}
	
	k = (size >> 1) + 1;              /* Heap sort the array */
	l = size;
	ca = 0; cb = 0; cc = 0;
	cd = 0; ce = 0; cf = 0;

	for (;;)
	{
	ca++;
	if (k > 1)
	{
	   cb++;
	   ir = *(base+(--k));
	}
	else
	{  
	   cc++;
	   ir = *(base+l);
	   *(base+l) = *(base+1);
	   if (--l == 1)
	   {
		*(base+1) = ir;
		goto Done;
	   }
	}

	i = k;
	j = k << 1;

	while (j <= l)
	{
	   cd++;
	   if ( (j < l) && (*(base+j) < *(base+j+1)) ) ++j;
	   if (ir < *(base+j))
	   {
		ce++;
		*(base+i) = *(base+j);   
		j += (i=j);
	   }
	   else 
	   {
		cf++;
		j = l + 1;
	   }
	}
	*(base+i) = ir;
	} 
Done:   
   count = count + ca;
   }
   TimerOff();
   runtime = TimerElapsed();
   if( p ) {
       sprintf( buffer, "heapsort(%d,%d)", m, n );
       Report( buffer, runtime );
   }
					/* Scale runtime per iteration */
   runtime = runtime / (double)n;
	
   ir = count / n;
   ir = (ir + ca) / 2;
					/* Estimate MIPS rating */
   emips = 24.0 * (double)size + 10.0 * (double)ir;
   emips = emips + 6.0 * (double)cb + 9.0 * (double)cc;
   emips = emips + 10.0 * (double)cd + 7.0 * (double)ce;
   emips = emips + 4.0 * (double)cf;
   sta   = 1.0e-06 * emips;
   emips = sta / runtime;

   if ( p != 0L )
   {
   printf("   %10ld %10.4lf %10.4lf %7.2lf\n",msize,runtime,sta,emips);
   }
free(base);
return 0;
}
