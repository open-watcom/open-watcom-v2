/*****************************************************/
/* Various timer routines.                           */
/* Al Aburto, aburto@marlin.nosc.mil, 16 Dec 1995    */
/*                                                   */
/* t = dtime() outputs the current time in seconds.  */
/* Use CAUTION as some of these routines will mess   */
/* up when timing across the hour mark!!!            */
/*****************************************************/

/**************************************************************/
/* Timer options. You *MUST* compile with one of              */
/* Amiga, UNIX, UNIX_Old, VMS, BORLAND_C, MSC, MAC, IPSC,     */
/* FORTRAN_SEC, GTODay, CTimer, UXPM, MAC_TMgr, PARIX, or     */
/* POSIX defined! e.g. -DUNIX.                                */
/**************************************************************/

#ifdef Amiga
#include <ctype.h>
#define HZ 50

double dtime()
{
   struct   tt {
	long  days;
	long  minutes;
	long  ticks;
   } tt;

   DateStamp(&tt);
   return ((double)(tt.ticks + (tt.minutes * 60L * 50L))) / (double)HZ;
}
#endif

#ifdef UNIX
#include <sys/time.h>
#include <sys/resource.h>

#ifdef __hpux
#include <sys/syscall.h>
#define getrusage(a,b) syscall(SYS_getrusage,a,b)
#endif

struct rusage rusage;

double dtime()
{
   getrusage(RUSAGE_SELF,&rusage);

   return (double)rusage.ru_utime.tv_sec +
	    (double)rusage.ru_utime.tv_usec * 1.0e-06;
}
#endif

#ifdef UNIX_Old         /* obsolete; HZ may be ill-defined */
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>

#ifndef HZ
#define HZ 60
#endif

struct tms tms;

double dtime()
{
   times(&tms);

   return (double)tms.tms_utime / (double)HZ;
}
#endif

#ifdef VMS   /*  Provided by: RAMO@uvphys.phys.UVic.CA */
#include time

#ifndef HZ
#define HZ 100
#endif

double dtime()
{
  struct tbuffer_t
	 {
	int proc_user_time;
	int proc_system_time;
	int child_user_time;
	int child_system_time;
	 } tms;

   times(&tms);

   return (double)(tms.proc_user_time) / (double)HZ;
}
#endif

#ifdef BORLAND_C
#include <ctype.h>
#include <dos.h>
#include <time.h>

#define HZ 100

double dtime()
{
   struct time tnow;
   gettime(&tnow);

   return 60.0 * (double)tnow.ti_min + (double)tnow.ti_sec
	    + (double)tnow.ti_hund/(double)HZ;
}
#endif

#ifdef MSC   /*  Microsoft C */
#include <time.h>
#include <ctype.h>

#define HZ CLOCKS_PER_SEC

double dtime()
{
   return (double)clock() / (double)HZ;
}
#endif

#ifdef MAC
#include <time.h>

#define HZ 60

double dtime()
{
   return (double)clock() / (double)HZ;
}
#endif

#ifdef IPSC /* iPSC/860, provided by yergeau@gloworm.Stanford.EDU */
extern double dclock();

double dtime()
{
   return dclock();
}
#endif

#ifdef FORTRAN_SEC      /*  FORTRAN dtime() for Cray type systems */

fortran double second();

double dtime()
{
   double q;

   second(&q);
   return q;
}
#endif

#ifdef CTimer     /*  UNICOS C, provided by dority@craysea.cray.com */
#include <time.h>

double dtime()
{
   return (double)clock() / (double)CLOCKS_PER_SEC;
}
#endif

#ifdef GTODay     /* UNIX timer using gettimeofday() */
#include <sys/time.h>

double dtime()
{
   struct timeval tnow;

   gettimeofday(&tnow,NULL);
   return (double)tnow.tv_sec + (double)tnow.tv_usec * 1.0e-6;
}
#endif

#ifdef UXPM /* Fujitsu UXP/M timer, provided by M.Lim@anu.edu.au */
#include <sys/types.h>
#include <sys/timesu.h>

double dtime()
{
   struct tmsu rusage;

   timesu(&rusage);
   return (double)rusage.tms_utime * 1.0e-06;
}
#endif

/**********************************************/
/*    Macintosh (MAC_TMgr) Think C dtime()    */
/*   requires Think C Language Extensions or  */
/*    #include <MacHeaders> in the prefix     */
/*  provided by Francis H Schiffer 3rd (fhs)  */
/*         skipschiffer@genie.geis.com        */
/**********************************************/
#ifdef MAC_TMgr
#include <Timer.h>
#include <stdlib.h>

static TMTask   mgrTimer;
static Boolean  mgrInited = false;
static double   mgrClock;

#define RMV_TIMER RmvTime( (QElemPtr)&mgrTimer )
#define MAX_TIME  1800000000L
/* MAX_TIME limits time between calls to */
/* dtime( ) to no more than 30 minutes   */
/* this limitation could be removed by   */
/* creating a completion routine to sum  */
/* 30 minute segments (fhs 1994 feb 9)   */

static void     Remove_timer( )
{
	RMV_TIMER;
	mgrInited = false;
}
double  dtime( )
{
	if( mgrInited ) {
		RMV_TIMER;
		mgrClock += (MAX_TIME + mgrTimer.tmCount)*1.0e-6;
	} else {
		if( _atexit( &Remove_timer ) == 0 ) mgrInited = true;
		mgrClock = 0.0;
	}
	if( mgrInited ) {
		mgrTimer.tmAddr = NULL;
		mgrTimer.tmCount = 0;
		mgrTimer.tmWakeUp = 0;
		mgrTimer.tmReserved = 0;
		InsTime( (QElemPtr)&mgrTimer );
		PrimeTime( (QElemPtr)&mgrTimer, -MAX_TIME );
	}
	return( mgrClock );
}
#endif

/***********************************************************/
/*  Parsytec GCel timer.                                   */
/*  Provided by: Georg Wambach, gw@informatik.uni-koeln.de */
/***********************************************************/
#ifdef PARIX
#include <sys/time.h>

double dtime()
{
   double q;

   q = (double) (TimeNowHigh()) / (double) CLK_TCK_HIGH;

   return q;
}
#endif

/************************************************/
/*  Sun Solaris POSIX dtime() routine           */
/*  Provided by: Case Larsen, CTLarsen.lbl.gov  */
/************************************************/
#ifdef POSIX
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/rusage.h>

#ifdef __hpux
#include <sys/syscall.h>
#endif

struct rusage rusage;

double dtime()
{
	double q;

	getrusage(RUSAGE_SELF,&rusage);

	q = (double)(rusage.ru_utime.tv_sec);
	q = q + (double)(rusage.ru_utime.tv_nsec) * 1.0e-09;
	
	return q;
}
#endif
