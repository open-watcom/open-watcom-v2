/*================================================================\
|                                                                 |
|      OS/2 Physical Device Driver Demonstration Code             |
|                     for Open Watcom C/C++                       |
|                                                                 |
|  TIMER.C Time execution of program                              |
|                                                                 |
|  This program uses the high-resolution timer Physical Device    |
|  Driver (PDD) for OS/2                                          |
|                                                                 |
\================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <os2.h>
#include "hrtimer.h"

HFILE           timerHandle;
TIMESTAMP       overHead;
TIMESTAMP       startTime;
TIMESTAMP       stopTime;

static void calc_time( TIMESTAMP *difference,
                       TIMESTAMP *start,
                       TIMESTAMP *stop,
                       TIMESTAMP *overhead )
{
    ULONG     milli_secs;
    ULONG     nano_secs;

    milli_secs = stop->millisecs - start->millisecs;
    nano_secs = stop->nanosecs - start->nanosecs;
    if( start->nanosecs > stop->nanosecs ) {
        nano_secs += 1000000;
        milli_secs -= 1;
    }
    if( overhead != NULL ) {
        nano_secs -= overhead->nanosecs;
        if( nano_secs & 0x80000000 ) {
            nano_secs += 1000000;
            milli_secs -= 1;
        }
        milli_secs -= overhead->millisecs;
    }
    difference->millisecs = milli_secs;
    difference->nanosecs = nano_secs;
}

static void init( void )
{
    ULONG     rc;
    ULONG     action;
    ULONG     bytes;

    rc = DosOpen( "WTIMER$", &timerHandle, &action, 0,
                   FILE_NORMAL, FILE_OPEN, OPEN_SHARE_DENYNONE, NULL );
    if( rc == 0 ) {
        DosRead( timerHandle, &startTime, sizeof( TIMESTAMP ), &bytes );
        DosRead( timerHandle, &stopTime, sizeof( TIMESTAMP ), &bytes );
        calc_time( &overHead, &startTime, &stopTime, NULL );
        DosRead( timerHandle, &startTime, sizeof( TIMESTAMP ), &bytes );
    } else {
        printf( "Could not open WTIMER$\n" );
    }
}

static void fini( TIMESTAMP *time )
{
    ULONG     rc;
    ULONG     bytes;

    DosRead( timerHandle, &stopTime, sizeof( TIMESTAMP ), &bytes );
    calc_time( time, &startTime, &stopTime, &overHead );
    rc = DosClose( timerHandle );
    if( rc != 0 ) {
        printf( "Could not close WTIMER$\n" );
    }
}

void main( int argc, char **argv )
{
    int         rc;
    TIMESTAMP   time;

    if( argc <= 1 ) {
        printf( "Usage: %s program [program args]\n", argv[0] );
        exit( EXIT_SUCCESS );
    }
    init();
    rc = spawnvp( P_WAIT, argv[1], (const char**)&argv[1] );
    fini( &time );
    printf( "%s: %dms, %dns\n", argv[1], time.millisecs, time.nanosecs );
    exit( rc );
}
