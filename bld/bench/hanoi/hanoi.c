/************************************************/
/*  hanoi.c, Version 1.0, 18 Sep 1992  --- AAA  */
/*           Version 1.1, 06 Jul 1993  --- AAA  */
/*           Added print out for all disks      */
/*           Version 1.2, 28 May 1994  --- AAA  */
/*           Calculate average Moves/25usec     */
/*           Version 1.2, 27 Oct 1994  --- AAA  */
/*           Only avg for RunTime > 0.1 sec     */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "report.h"

#define other(i,j) (6-(i+j))

int num[4];
long count;

void mov(int n,int f,int t);

void main()
{
	double RunTime = 0.0, sum_mps = 0.0, TLimit, mps;
	int disk, Loops = 0;
	double TotalTime = 0.0;
	
	printf("\n");
	printf("Towers of Hanoi Puzzle Test Program (27 Oct 94)\n\n");
	printf("Disks     Moves     Time(sec)   Moves/25usec\n");

	TLimit  = 30.0;
	disk    = 15;
 
	while ( RunTime < TLimit )
	{
	 disk++;
	 num[0] = 0;
	 num[1] = disk;
	 num[2] = 0;
	 num[3] = 0;
	 count  = 0;

	 TimerOn();
	 mov(disk,1,3);
	 TimerOff();
	 
	 RunTime = TimerElapsed();
	 TotalTime += RunTime;
	 mps = 2.5E-05 * ( (double)count/RunTime );
	 
	 if ( RunTime > 0.1)
	 {
	 Loops = Loops + 1;
	 sum_mps = sum_mps + mps;
	 }

	 printf("%3ld  %10ld  %12.5lf  %10.4lf\n",disk,count,RunTime,mps);
	 
	 if ( disk == 30 ) break;
	}
	
	Report( "hanoi", TotalTime );
	sum_mps = sum_mps / (double)Loops;
	printf("\nAverage Moves Per 25 usec = %10.4lf\n",sum_mps);
	printf("\n");

	exit( EXIT_SUCCESS );
}

void mov(n,f,t)
{
   int o;
   if(n == 1) 
   {
	num[f]--;
	num[t]++;
	count++;
	return;
   }
   o = other(f,t);
   mov(n-1,f,o);
   mov(1,f,t);
   mov(n-1,o,t);
   return;
}
