#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include "timer.h"

#define INCL_DOS
#include <os2.h>

#ifdef __WATCOMC__

p5_time ReadP5Timer( void );

#pragma aux ReadP5Timer = \
    ".586" \
    "rdtsc" \
    value [eax edx];
#else
extern p5_time ReadP5Timer( void );
#endif

void UserTimerOn( user_timer *t )
{
    PPIB    ppib;
    PTIB    ptib;

    // Save current priority class
    DosGetInfoBlocks( &ptib, &ppib );
    t->old_priority = (ptib->tib_ptib2->tib2_ulpri >> 8) & 0xFF;
    // Set current thread's priority class to time critical
    DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, 0 );
    t->state = T_STATE_STARTED;
    t->start = ReadP5Timer();
}

void UserTimerOff( user_timer *t )
{
    t->stop = ReadP5Timer();
    assert( t->state == T_STATE_STARTED );
    t->state = T_STATE_STOPPED;
    // Restore initial priority class
    DosSetPriority( PRTYS_THREAD, t->old_priority, 0, 0 );
}

static double processorSpeed( void )
{
    char    *env;
    double  speed;

    speed = 90000000.0; // assume 90 MHz (shrug)
    env = getenv( "PROCESSOR_SPEED" );
    if( env != NULL ) {
    speed = 0.0;
    while( isdigit( env[ 0 ] ) ) {
        speed *= 10.0;
        speed += (double)( env[ 0 ] - '0' );
        env++;
    }
    // assume it's given in MHZ
    speed *= 1000000.0;
    }
    return( speed );
}

double UserTimerElapsed( user_timer *t )
{
    p5_time diff;
    double  secs;

    assert( t->state == T_STATE_STOPPED );
    diff = t->stop - t->start;
    secs = (double)diff / processorSpeed();
    return( secs );
}

static user_timer globalTimer;

void TimerOn() { UserTimerOn( &globalTimer ); }
void TimerOff() { UserTimerOff( &globalTimer ); }
double TimerElapsed() { return( UserTimerElapsed( &globalTimer ) ); }
