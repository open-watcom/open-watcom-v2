
/*
** misc.c
** BYTEmark (tm)
** BYTE's Native Mode Benchmarks
** Rick Grehan, BYTE Magazine
*/

#include <stdio.h>
#include "misc.h"

/***********************************************************
**     MISCELLANEOUS BUT OTHERWISE NECESSARY ROUTINES     **
***********************************************************/

/****************************
** RANDOM NUMBER GENERATOR **
*****************************
** This is a second-order linear congruential random number
** generator.  Its advantage is (of course) that it can be
** seeded and will thus produce repeatable sequences of
** random numbers.
*/

/****************************
*         randwc()          *
*****************************
** Returns signed long random modulo num.
*/
long randwc(long num)
{
	return(randnum(0L)%num);
}

/***************************
**      abs_randwc()      **
****************************
** Same as randwc(), only this routine returns only
** positive numbers.
*/
unsigned long abs_randwc(unsigned long num)
{
long temp;		/* Temporary storage */

temp=randwc(num);
if(temp<0) temp=0L-temp;

return((unsigned long)temp);
}

/****************************
*        randnum()          *
*****************************
** Second order linear congruential generator.
** Constants suggested by J. G. Skellam.
** If val==0, returns next member of sequence.
**    val!=0, restart generator.
*/
long randnum(long lngval)
{
	register long interm;
	static long randw[2] = { 13L , 117L };

	if (lngval!=0L)
	{	randw[0]=13L; randw[1]=117L; }

	interm=(randw[0]*254754L+randw[1]*529562L)%999563L;
	randw[1]=randw[0];
	randw[0]=interm;
	return(interm);
}

