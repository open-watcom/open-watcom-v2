/*
**  Queens.c    --  Find solutions to the Eight-Queens chess problem.
**                  Roberto Sierra  7/25/93  Version 1.1
**                                  3/19/84  Version 1.0
**
**  Description:
**      This program finds all the possible ways that N queens can
**      be placed on an NxN chessboard so that the queens cannot
**      capture one another -- that is, so that no rank, file or
**      diagonal is occupied by more than one queen.  By default,
**      the program prints the first solution it finds.  You can
**      use the -a option to print all solutions, or the -c option
**      just to count them.  The program allows the chess board
**      to be from 1x1 (trivial case) to 100x100.  Warning: the
**      larger the chess board, the longer it typically takes to
**      find each solution, even though there may be more of them.
**
**      This is a terrific example of the utility of recursion. The
**      algorithm uses recursion to drastically limit the number
**      of board positions that are tested.  The program is able
**      to find all 8x8 queen solutions in a fraction of a second
**      (not counting print time).  The code makes no attempt to
**      eliminate symmetrical solutions, so the number of solutions
**      reported will always be higher than the actual number of
**      distinct solutions.
**
**
**  Usage:
**      Queens [-ac] n
**
**      n       number of queens (rows and columns).
**              An integer from 1 to 100.
**      -a      Find (and print) all solutions.
**      -c      Count all solutions, but do not print them.
**
**      The output is sent to stdout.  All errors messages are
**      sent to stderr.  If a problem arises, the return code is -1.
**
**
**  Examples:
**
**      Queens 8        ## Show an 8x8 solution
**      8 queens on a 8x8 board...
**       Q - - - - - - -
**       - - - - Q - - -
**       - - - - - - - Q
**       - - - - - Q - -
**       - - Q - - - - -
**       - - - - - - Q -
**       - Q - - - - - -
**       - - - Q - - - -
**
**      Queens -c 8     ## Count all 8x8 solutions
**      8 queens on a 8x8 board...
**      ...there are 92 solutions.
**
**      Queens -a 4     ## Show all 4x4 solutions
**      4 queens on a 4x4 board...
**      
**      Solution #1:
**       - Q - -
**       - - - Q
**       Q - - -
**       - - Q -
**      
**      Solution #2:
**       - - Q -
**       Q - - -
**       - - - Q
**       - Q - -
**      
**      ...there are 2 solutions.
**
**
**  Build Instructions:
**      You'll need an ANSI C compiler (or the willingness to edit
**      the program a bit).  If you've got Gnu C, then you can
**      compile and load the program as follows:
**
**              gcc Queens.c -ansi -o Queens
**
**      [If you're using MPW on the Mac, define '-d MPW' on the
**      compile line so that background processing will occur.]
**
**
**  Algorithm:
**      In a 1984 Byte article, I ran across an interesting letter
**      from a high school student who was attempting to solve the
**      Eight Queens problem using a BASIC interpreter.  He had
**      developed a program which placed eight queens successively
**      on all sixty-four squares, testing for conflicts at each
**      iteration.  Of course, such a program would require 64^8
**      iterations (about 2.8x10^14 iterations).  Even in C on a,
**      fast CPU, this could take months or years.  Byte's answer was
**      to alter the loops so that the queens resided on separate
**      ranks, thereby reducing the number of iterations required
**      to find all solutions to 8^8 iterations (about 16 million).
**      More reasonable, but still requiring a chunk of CPU time.
**
**      I puzzled about this problem a bit, and came to realize that
**      this was still wasting a lot of CPU cycles.  Though I'm sure
**      others have come up with good algorithms, I decided to come
**      up with my own, with a particular eye on efficiency.  The
**      resulting algorithm finds all 8x8 solutions in a fraction
**      of a second (there are 92 solutions, including rotations).
**      On a Sun 4, it'll find all 365,596 solutions on a 14x14 board
**      in a bit over 2 minutes (printing them out requires extra
**      time, of course).  Even Byte's solution would require 14^14
**      iterations (about 10^16) which would take aeons.
**
**      My algorithm works as follows:
**      (1)  Place a queen in the top left corner.
**      (2)  Place another queen immediately below.
**      (3)  Test for conflicts.  If the second queen conflicts (it
**           does at first), then move it one square to the right.
**      (4)  Loop step 3 until there are no conflicts.  Place
**           the next queen on the board and recurse.
**      (5)  If any queen reaches the right edge of the board,
**           remove it and 'pop' to the previous recursion level.
**      (6)  Now repeat these steps recursively until all eight
**           queens (or however many) have been placed without
**           conflict -- the result is a solution to the problem,
**           which is counted and optionally printed.
**
**      Because conflicts are tested as the recursion proceeds,
**      this has the effect of 'pruning' the recursion so that
**      a large number of board positions are not even attempted.
**      The result is that the algorithm runs in reasonable time.
**
**      I used a few tricks to make the test-for-conflict code
**      extremely efficient -- there is no 'inner' loop to search
**      along ranks, files, or diagonals.  A series of arrays are
**      maintained instead which indicate which queen currently
**      'owns' each rank, file or diagonal.  This makes the
**      algorithm really fly, though the code is a little hard to
**      read.  Lastly, pointer arithmetic is used to reduce the
**      number of implicit multiplications used in array addressing.
**
**
**  Contact:
**      For queries regarding this program, contact Roberto Sierra
**      at any of the following addresses:
**
**              Roberto Sierra
**              bert@netcom.com   (preferred address)
**              73557.2101@compuserve.com
**
**              Tempered MicroDesigns
**              P.O. Box 170638
**              San Francisco, CA  94117
**
**
**  Fine Print:
**      This program is in the public domain and can be used for
**      any purpose whatsoever, including commercial application.
**      [I'd like to hear what you do with it, though.]
**      Absolutely no warranty or liability is implied or extended
**      by the author.
**
**
**  Modification History:
**      PRS  3/19/84  v1.0 -- Original version.
**      PRS  7/25/93  v1.1 -- ANSIfied the code.  More efficient pointers.
**
*/

#include <stdio.h>                              /* Need standard I/O functions */
#include <stdlib.h>                             /* Need exit() routine interface */
#include <string.h>                             /* Need strcmp() interface */
#include "timer.h"
#include "report.h"

#define MAXQUEENS   100                         /* Maximum number of queens */
#define MAXRANKS    MAXQUEENS                   /* Maximum number of ranks (rows) */
#define MAXFILES    MAXQUEENS                   /* Maximum number of files (columns) */
#define MAXDIAGS    (MAXRANKS+MAXFILES-1)       /* Maximum number of diagonals */
#define EMPTY       (MAXQUEENS+1)               /* Marks unoccupied file or diagonal */

/* GLOBAL VARIABLES */

int             queens;                         /* Number of queens to place */
int             ranks;                          /* Number of ranks (rows) */
int             files;                          /* Number of files (columns) */
int             printing = 1;                   /* TRUE if printing positions */
int             findall = 0;                    /* TRUE if finding all solutions */

unsigned long   solutions = 0;                  /* Number of solutions found */
int             queen[MAXRANKS];                /* File on which each queen is located */
int             file[MAXFILES];                 /* Which queen 'owns' each file */
int             fordiag[MAXDIAGS];              /* Which queen 'owns' forward diagonals */
int             bakdiag[MAXDIAGS];              /* Which queen 'owns' reverse diagonals */
char            *progname = 0;                  /* The name of this program */





/***********************/
/****   ROUTINES    ****/
/***********************/

/* Internal prototypes */
void    main(int argc,char **argv);             /* Main program */
void    find(int level);                        /* Algorithm to find solutions */
void    pboard(void);                           /* Print a solution */




/*---------------------- main() ---------------------------
**  MAIN program.  The main purpose of this routine is
**  to deal with decoding the command line arguments,
**  initializing the various arrays, and starting the
**  recursive search routine.
*/

void main(int argc,char **argv)
{
    register int    i;                          /* Loop variable */
    register char   *p;                         /* Pointer to argument */
    double 	    benchtime;
    char	    buffer[ 80 ];

    progname = argv[0];                         /* The name of the program */

    /****   DECODE COMMAND LINE ARGUMENTS   ****/

    for (i=1; i<argc; ++i) {                    /* Scan through arguments */
	p = argv[i];                            /* Pointer to base of argument */
	if (*p == '-') {                        /* Command line option? */
	    while (*++p) {                      /* Loop through characters */
		switch (*p) {                   /* What is the character */
		case 'a':                       /* '-a' option */
		    findall = 1;                /* Set flag to find all solutions */
		    break;
		case 'c':                       /* '-c' option */
		    printing = 0;               /* Counting, not printing */
		    findall = 1;                /* Also forces findall option */
		    break;
		default:                        /* Illegal option */
		    fprintf(stderr,"%s: Illegal option '%s'\n",progname,argv[i]);
		    fprintf(stderr,"usage: %s [-ac] queens\n",progname);
		    exit(-1);
		}                               /* End of switch */
	    }                                   /* End of loop */
	} else {                                /* End of option test */
	    if (sscanf(p,"%d",&queens) != 1) {  /* Read integer argument */
		fprintf(stderr,"%s: non-integer argument '%s'\n",progname,p);
		exit(-1);
	    }
	    if (queens <= 0) {                  /* N must be positive */
		fprintf(stderr,"%s: queens must be positive integer\n",progname);
		exit(-1);
	    }
	    if (queens > MAXQUEENS) {           /* N can't be too large */
		fprintf(stderr,"%s: can't have more than %d queens\n",
			progname, MAXQUEENS);
		exit(-1);
	    }
	}                                       /* End of argument test */
    }                                           /* End of argument scan loop */
    if (queens == 0) {
	fprintf(stderr,"%s: missing queens argument\n",progname);
	fprintf(stderr,"usage: %s [-ac] queens\n",progname);
	exit(-1);
    }


    ranks = files = queens;                     /* NxN board for N queens */
    printf("%d queen%s on a %dx%d board...\n",
	    queens, queens>1? "s" : "", ranks, files);
    fflush(stdout);


    TimerOn();

    /*  Initialization  */
    solutions = 0;                              /* No solutions yet */
    for (i=0; i<MAXFILES; ++i) file[i] = EMPTY;
    for (i=0; i<MAXDIAGS; ++i) fordiag[i] = bakdiag[i] = EMPTY;

    /* Find all solutions (begin recursion) */
    find(0);
    if (printing && solutions) putchar('\n');

    /* Report results */
    if (solutions == 1) {
	printf("...there is 1 solution\n");
    } else {
	printf("...there are %ld solutions\n", solutions);
    }

    TimerOff();
    sprintf( buffer, "queens(%d,%d)", queens, findall );
    Report( buffer, TimerElapsed() );
    benchtime = TimerElapsed();
    printf("Run Time (sec) = %9.3lf\n\n",benchtime);


    exit(0);                                    /* No errors */
}                                               /* End of main() */



/*-------------------------- find() ----------------------------
**  FIND is the recursive heart of the program, and finds all
**  solutions given a set of level-1 fixed queen positions.
**  The routine moves a single queen through all files (columns)
**  at the current rank (recursion level).  As the queen is moved,
**  conflict tests are made.  If the queen can be placed without
**  conflict, then the routine recurses to the next level.  When
**  all queens have been placed without conflict, a solution is
**  counted and reported.
*/

void find(register int level)
{
    register int    f;                          /* Indexes through files */
    register int    *fp,*fdp,*bdp;              /* Ptrs to file/diagonal entries */

#ifdef  MPW                                     /* Macintosh MPW ONLY */
    if (level & 7 == 0) {                       /* Periodically break for... */
	SpinCursor(1);                          /* background processing */
    }
#endif

    if (level == queens) {                      /* Placed all queens?  Stop. */
	++solutions;                            /* Congrats, this is a solution! */
	if (printing) pboard();                 /* Print board if printing */
	if (!findall) exit(0);                  /* May stop after first solution */
#ifdef  MPW                                     /* Macintosh MPW ONLY */
	SpinCursor(1);                          /* Allow background processing */
#endif
    } else {                                    /* Not at final level yet */
	
	for (                                   /* MOVE QUEEN THROUGH ALL FILES */
	    f = 0,                              /* Queen starts at left (file 0) */
	    fp = file,                          /* Ptr to base of file array */
	    fdp = &fordiag[level],              /* Ptr to first fwd diag entry */
	    bdp = &bakdiag[level+files-1]       /* Ptr to first bak diag entry */
	;
	    f < files                           /* Loop through all files */
	;
	    ++f,                                /* Advance index */
	    ++fp, ++fdp, --bdp                  /* Advance pointers */
	) {
	    if (*fp >= level &&                 /* No queen on the file? */
		*fdp >= level && *bdp >= level  /* No queens on diagonals? */
	    ) {
		queen[level] = f;               /* Note new position of queen */
		*fp = *fdp = *bdp = level;      /* Place queen on file & diags */
		find(level+1);                  /* This level OK, recurse to next */
		*fp = *fdp = *bdp = EMPTY;      /* Remove queen from file & diags */
	    }                                   /* End of conflict test */
	}                                       /* End of file loop */
    }                                           /* End if (level == queens) */
}                                               /* End of find() */




/*------------------------- pboard() -----------------------
**  This routines prints the board for a particular solution.
**  The output is sent to stdout.
*/

void pboard(void)
{
    register int    i,j;                        /* Rank/File indices */

    if (findall) {                              /* Only if searching for all */
	printf("\nSolution #%lu:\n",solutions); /* Print solution number */
    }
    for (i=0; i<ranks; ++i) {                   /* Loop through all ranks */
	for (j=0; j<files; ++j) {               /* Loop through all files */
	    putchar(' ');                       /* Output a space */
	    if (j==queen[i]) putchar('Q');      /* Output Q for queen... */
	    else putchar('-');                  /* or '-' if empty */
	}
	putchar('\n');                          /* Break line */
    }
    fflush(stdout);                             /* Flush solution to output */
}                                               /* End of pboard() */

/****   End of queens.c ****/


/*****************************************************/
/* Various timer routines.                           */
/* Al Aburto, aburto@marlin.nosc.mil, 16 Dec 1995    */
/*                                                   */
/* t = dtime() outputs the current time in seconds.  */
/* Use CAUTION as some of these routines will mess   */
/* up when timing across the hour mark!!!            */
/*                                                   */
/* For timing I use the 'user' time whenever         */
/* possible. Using 'user+sys' time is a separate     */
/* issue.                                            */
/*                                                   */
/* Example Usage:                                    */
/* [timer options added here]                        */
/* main()                                            */
/* {                                                 */
/* double starttime,benchtime,dtime();               */
/*                                                   */
/* starttime = dtime();                              */ 
/* [routine to time]                                 */
/* benchtime = dtime() - starttime;                  */
/* }                                                 */
/*                                                   */
/* [timer code below added here]                     */
/*****************************************************/

/*******************/
/*  Amiga dtime()  */
/*******************/
#ifdef Amiga
#include <ctype.h>
#define HZ 50

double dtime()
{
	double q;

	struct   tt {
		long  days;
		long  minutes;
		long  ticks;
	} tt;

	DateStamp(&tt);

	q = ((double)(tt.ticks + (tt.minutes * 60L * 50L))) / (double)HZ;

	return q;
}
#endif

/*****************************************************/
/*  UNIX dtime(). This is the preferred UNIX timer.  */
/*  Provided by: Markku Kolkka, mk59200@cc.tut.fi    */
/*  HP-UX Addition by: Bo Thide', bt@irfu.se         */
/*****************************************************/
#ifdef UNIX
#include <sys/time.h>
#include <sys/resource.h>

#ifdef hpux
#include <sys/syscall.h>
#define getrusage(a,b) syscall(SYS_getrusage,a,b)
#endif

struct rusage rusage;

double dtime()
{
	double q;

	getrusage(RUSAGE_SELF,&rusage);

	q = (double)(rusage.ru_utime.tv_sec);
	q = q + (double)(rusage.ru_utime.tv_usec) * 1.0e-06;
	
	return q;
}
#endif

/***************************************************/
/*  UNIX_Old dtime(). This is the old UNIX timer.  */
/*  Use only if absolutely necessary as HZ may be  */
/*  ill defined on your system.                    */
/***************************************************/
#ifdef UNIX_Old
#include <sys/types.h>
#include <sys/times.h>
#include <sys/param.h>

#ifndef HZ
#define HZ 60
#endif

struct tms tms;

double dtime()
{
	double q;

	times(&tms);

	q = (double)(tms.tms_utime) / (double)HZ;
	
	return q;
}
#endif

/*********************************************************/
/*  VMS dtime() for VMS systems.                         */
/*  Provided by: RAMO@uvphys.phys.UVic.CA                */
/*  Some people have run into problems with this timer.  */
/*********************************************************/
#ifdef VMS
#include time

#ifndef HZ
#define HZ 100
#endif

struct tbuffer_t
		 {
	int proc_user_time;
	int proc_system_time;
	int child_user_time;
	int child_system_time;
		 };
struct tbuffer_t tms;

double dtime()
{
	double q;

	times(&tms);

	q = (double)(tms.proc_user_time) / (double)HZ;
	
	return q;
}
#endif

/******************************/
/*  BORLAND C dtime() for DOS */
/******************************/
#ifdef BORLAND_C
#include <ctype.h>
#include <dos.h>
#include <time.h>

#define HZ 100
struct time tnow;

double dtime()
{
	double q;

	gettime(&tnow);

	q = 60.0 * (double)(tnow.ti_min);
	q = q + (double)(tnow.ti_sec);
	q = q + (double)(tnow.ti_hund)/(double)HZ;
	
	return q;
}
#endif

/**************************************/
/*  Microsoft C (MSC) dtime() for DOS */
/**************************************/
#ifdef MSC
#include <time.h>
#include <ctype.h>

#define HZ CLOCKS_PER_SEC
clock_t tnow;

double dtime()
{
	double q;

	tnow = clock();

	q = (double)tnow / (double)HZ;
	
	return q;
}
#endif

/*************************************/
/*  Macintosh (MAC) Think C dtime()  */
/*************************************/
#ifdef MAC
#include <time.h>

#define HZ 60

double dtime()
{
	double q;

	q = (double)clock() / (double)HZ;
	
	return q;
}
#endif

/************************************************************/
/*  iPSC/860 (IPSC) dtime() for i860.                       */
/*  Provided by: Dan Yergeau, yergeau@gloworm.Stanford.EDU  */
/************************************************************/
#ifdef IPSC
extern double dclock();

double dtime()
{
	double q;

	q = dclock();
	
	return q;
}
#endif

/**************************************************/
/*  FORTRAN dtime() for Cray type systems.        */
/*  This is the preferred timer for Cray systems. */
/**************************************************/
#ifdef FORTRAN_SEC

fortran double second();

double dtime()
{
	double q;

	second(&q);
	
	return q;
}
#endif

/***********************************************************/
/*  UNICOS C dtime() for Cray UNICOS systems.  Don't use   */
/*  unless absolutely necessary as returned time includes  */
/*  'user+system' time.  Provided by: R. Mike Dority,      */
/*  dority@craysea.cray.com                                */
/***********************************************************/
#ifdef CTimer
#include <time.h>

double dtime()
{
	double    q;
	clock_t   clock(void);

		 q = (double)clock() / (double)CLOCKS_PER_SEC;

		 return q;
}
#endif

/********************************************/
/* Another UNIX timer using gettimeofday(). */
/* However, getrusage() is preferred.       */
/********************************************/
#ifdef GTODay
#include <sys/time.h>

struct timeval tnow;

double dtime()
{
	double q;

	gettimeofday(&tnow,NULL);
	q = (double)tnow.tv_sec + (double)tnow.tv_usec * 1.0e-6;

	return q;
}
#endif

/*****************************************************/
/*  Fujitsu UXP/M timer.                             */
/*  Provided by: Mathew Lim, ANUSF, M.Lim@anu.edu.au */
/*****************************************************/
#ifdef UXPM
#include <sys/types.h>
#include <sys/timesu.h>
struct tmsu rusage;

double dtime()
{
	double q;

	timesu(&rusage);

	q = (double)(rusage.tms_utime) * 1.0e-06;
	
	return q;
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
