#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef  WIN32_LEAN_AND_MEAN
#include "timer.h"

p5_time __declspec( naked ) ReadP5Timer( void )
{
    // this is a rdtsc instruction
    __asm _emit 0x0f
    __asm _emit 0x31
    __asm ret
}

void UserTimerOn( user_timer *t )
{
    HANDLE	process;

    process = GetCurrentProcess();
    t->old_priority = GetPriorityClass( process );
    SetPriorityClass( process, HIGH_PRIORITY_CLASS );
    t->state = T_STATE_STARTED;
    t->start = ReadP5Timer();
}

void UserTimerOff( user_timer *t )
{
    t->stop = ReadP5Timer();
    assert( t->state == T_STATE_STARTED );
    t->state = T_STATE_STOPPED;
    SetPriorityClass( GetCurrentProcess(), t->old_priority );
}

static double processorSpeed( void )
{
    char	*env;
    double	speed;

    speed = 90000000.0;	// assume 90 MHZ (shrug)
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
    p5_time	diff;
    double	secs;

    assert( t->state == T_STATE_STOPPED );
    diff = t->stop - t->start;
    secs = (double)diff / processorSpeed();
    return( secs );
}

static user_timer globalTimer;

void TimerOn() { UserTimerOn( &globalTimer ); }
void TimerOff() { UserTimerOff( &globalTimer ); }
double TimerElapsed() { return( UserTimerElapsed( &globalTimer ) ); }
