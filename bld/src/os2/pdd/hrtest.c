/*================================================================\
|                                                                 |
|      OS/2 Physical Device Driver Demonstration Code             |
|                  for Open Watcom C/C++                          |
|                                                                 |
|  HRTEST.C                                                       |
|                                                                 |
|  This program is an Open Watcom C skeleton for using            |
|  HRTIMER.SYS, the high-resolution timer Physical Device Driver  |
|  (PDD) for OS/2                                                 |
|                                                                 |
|  Adapted for Open Watcom C/C++                                  |
|  Original Author: Rick Fishman                                  |
|                   Code Blazers, Inc.                            |
|                   4113 Apricot                                  |
|                   Irvine, CA 92720                              |
|                                                                 |
|  Command Line: hrtest [milliseconds]                            |
|                                                                 |
|   milliseconds is the amount of time that this test program     |
|   will sleep for between calls to the timer. A default is       |
|   provided if no parameter is given.                            |
|                                                                 |
|   A start timestamp will be retrieved from HRTIMER.SYS. Then    |
|   DosSleep will be issued for the milliseconds specified. Then  |
|   a stop timestamp will be retrieved and an elapsed time        |
|   calculated. The elapsed time should be close to the DosSleep  |
|   interval. Keep in mind that DosSleep has a granularity of 32  |
|   milliseconds so a discrepency of elapsed time vs              |
|   milliseconds passed to DosSleep is DosSleep's fault, not the  |
|   timer's.                                                      |
|                                                                 |
|   The overhead of a device driver read is also factored in.     |
|                                                                 |
\================================================================*/

/*********************************************************************/
/*------- Include relevant sections of the OS/2 header files --------*/
/*********************************************************************/

#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS

/*********************************************************************/
/*------------------- APPLICATION DEFINITIONS -----------------------*/
/*********************************************************************/

#define DEFAULT_TIME_PERIOD     5000      // in Milliseconds

/**********************************************************************/
/*----------------------------- INCLUDES -----------------------------*/
/**********************************************************************/

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "hrtimer.h"

/**********************************************************************/
/*---------------------------- STRUCTURES ----------------------------*/
/**********************************************************************/

/**********************************************************************/
/*----------------------- FUNCTION PROTOTYPES ------------------------*/
/**********************************************************************/

BOOL  Initialize     ( int argc, char *argv[] );
VOID  TimeIt         ( VOID );
ULONG CalcElapsedTime( PTIMESTAMP ptsStart, PTIMESTAMP ptsStop, ULONG ulOvHdMs,
                       ULONG ulOvHdNs, PULONG pulNs );
VOID  Terminate      ( VOID );

/**********************************************************************/
/*------------------------ GLOBAL VARIABLES --------------------------*/
/**********************************************************************/

HFILE   hfTimer;

ULONG   ulOverheadMs, ulOverheadNs, ulTimePeriod;

/**********************************************************************/
/*------------------------------ MAIN --------------------------------*/
/*                                                                    */
/*  MAIN DRIVER FOR PROGRAM.                                          */
/*                                                                    */
/*  INPUT: nothing                                                    */
/*                                                                    */
/*  1. Initialize program.                                            */
/*  1. Perform timings.                                               */
/*  1. Terminage program.                                             */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*--------------------------------------------------------------------*/
/**********************************************************************/

int main( int argc, char *argv[] )
{
    if( Initialize( argc, argv ) )
        TimeIt();
    Terminate();
    return( 0 );
}

/**********************************************************************/
/*--------------------------- Initialize -----------------------------*/
/*                                                                    */
/*  INITIALIZE PROGRAM.                                               */
/*                                                                    */
/*  INPUT: command-line argument count,                               */
/*         array of command-line arguments                            */
/*                                                                    */
/*  1. Set the time period that we will try to match with the high    */
/*     resolution timer.                                              */
/*  2. Open the timer device driver.                                  */
/*  3. Read a timestamp from the driver to use as a starting time     */
/*     used to calculate the overhead of a device-driver read.        */
/*  4. Read a timestamp from the driver to use as a stop time for     */
/*     the overhead calculation.                                      */
/*  5. Calculate the overhead in the read.                            */
/*                                                                    */
/*  OUTPUT: TRUE or FALSE if successful or not                        */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

BOOL Initialize( int argc, char *argv[] )
{
    TIMESTAMP   tsStart, tsStop;
    ULONG       ulRC, ulAction, ulBytes;

    if( argc < 2 )
        ulTimePeriod = DEFAULT_TIME_PERIOD;
    else
        ulTimePeriod = atoi( argv[ 1 ] );

    printf( "\nMeasuring a DosSleep of %u milliseconds\n", ulTimePeriod );
    ulRC = DosOpen( "WTIMER$", &hfTimer, &ulAction, 0,
                    FILE_NORMAL, FILE_OPEN, OPEN_SHARE_DENYNONE, NULL );
    if( ulRC )
    {
        printf( "\nFailed to open WTIMER$" );
        printf( "\nDosOpen got a retcode of %u", ulRC );
        return FALSE;
    }
    ulRC = DosRead( hfTimer, &tsStart, sizeof( TIMESTAMP ), &ulBytes );
    if( ulRC )
    {
        printf( "\nDosRead for start got a retcode of %u", ulRC );
        return FALSE;
    }
    ulRC = DosRead( hfTimer, &tsStop, sizeof( TIMESTAMP ), &ulBytes );
    if( ulRC )
    {
        printf( "\nDosRead for stop got a retcode of %u", ulRC );
        return FALSE;
    }
    printf( "\nWTIMER$ Version %u.%u\n",
            tsStop.version, tsStop.revision );
    ulOverheadMs = CalcElapsedTime( &tsStart, &tsStop, 0, 0, &ulOverheadNs );
    printf( "\nOverhead (ms:ns) = %06u:%06u\n", ulOverheadMs, ulOverheadNs );
    return TRUE;
}

/**********************************************************************/
/*----------------------------- TimeIt -------------------------------*/
/*                                                                    */
/*  DO THE TIMING.                                                    */
/*                                                                    */
/*  INPUT: nothing                                                    */
/*                                                                    */
/*  1. Until Ctrl-Break is pressed:                                   */
/*     A. Read a starting timestamp from the timer.                   */
/*     B. Sleep for a specified period of time.                       */
/*     C. Read a stop timestamp from the timer.                       */
/*     D. Calculate the elapsed time.                                 */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

VOID TimeIt()
{
    TIMESTAMP   tsStart, tsStop;
    ULONG       ulRC, ulMsecs, ulNsecs, ulBytes;

    printf( "\nHit Ctrl-C to terminate this test program...\n" );
    for( ; ; )
    {
        printf( "\nSleeping for %u milliseconds...", ulTimePeriod );
        fflush( stdout );
        ulRC = DosRead( hfTimer, &tsStart, sizeof( TIMESTAMP ), &ulBytes );
        if( ulRC )
        {
            printf( "\nDosRead for start got a retcode of %u", ulRC );
            return;
        }
        DosSleep( ulTimePeriod );
        ulRC = DosRead( hfTimer, &tsStop, sizeof( TIMESTAMP ), &ulBytes );
        if( ulRC )
        {
            printf( "\nDosRead for stop got a retcode of %u", ulRC );
            return;
        }
        ulMsecs = CalcElapsedTime( &tsStart, &tsStop,
                                   ulOverheadMs, ulOverheadNs, &ulNsecs );
        printf( " elapsed time (ms:ns) = %06u:%06u", ulMsecs, ulNsecs );
    }
}

/**********************************************************************/
/*------------------------- CalcElapsedTime --------------------------*/
/*                                                                    */
/*  CALCULATE ELAPSED TIME GIVEN TWO TIMESTAMPS                       */
/*                                                                    */
/*  INPUT: pointer to Start timestamp structure,                      */
/*         pointer to Stop  timestamp structure,                      */
/*         number of overhead milliseconds,                           */
/*         number of overhead nanoseconds,                            */
/*         address of variable to return remainder nanoseconds        */
/*                                                                    */
/*  OUTPUT: Number of elapsed milliseconds                            */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/

ULONG CalcElapsedTime( PTIMESTAMP ptsStart, PTIMESTAMP ptsStop,
                       ULONG ulOverhdMs, ULONG ulOverhdNs, PULONG pulNs )
{
    ULONG       ulMsecs, ulNsecs;
    TIMESTAMP   tsStart, tsStop;

    // De-reference timestamp structures for speed
    tsStart = *ptsStart;
    tsStop  = *ptsStop;

    // Calculate elapsed milliseconds
    ulMsecs = tsStop.millisecs - tsStart.millisecs;

    // If nanosecond overflow ...
    if( tsStart.nanosecs > tsStop.nanosecs )
    {
        // Adjust nanoseconds by adding in 1 millisecond
        ulNsecs = (1000000 + tsStop.nanosecs) - tsStart.nanosecs;
        // Adjust milliseconds down by 1
        ulMsecs--;
    }
    else
    {
        // No overflow so calculate elapsed nanoseconds
        ulNsecs = tsStop.nanosecs - tsStart.nanosecs;
    }

    // If caller wants overhead factored in ...
    if( ulOverhdMs || ulOverhdNs )
    {
        if( ulOverhdNs > ulNsecs )
        {
            ulNsecs = (1000000 + ulNsecs) - ulOverhdNs;
            ulMsecs--;
        }
        else
        {
            ulNsecs -= ulOverhdNs;
        }
        ulMsecs -= ulOverhdMs;
    }
    *pulNs = ulNsecs;
    return( ulMsecs );
}

/**********************************************************************/
/*--------------------------- Terminate ------------------------------*/
/*                                                                    */
/*  TERMINATE PROGRAM                                                 */
/*                                                                    */
/*  INPUT: nothing                                                    */
/*                                                                    */
/*  1. Close the device driver.                                       */
/*                                                                    */
/*  OUTPUT: nothing                                                   */
/*                                                                    */
/*--------------------------------------------------------------------*/
/**********************************************************************/
VOID Terminate()
{
    ULONG ulRC;

    if( hfTimer )
    {
        ulRC = DosClose( hfTimer );
        if( ulRC )
        {
            printf( "\nDosClose got a retcode of %u", ulRC );
        }
    }
}
