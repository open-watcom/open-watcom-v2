#include "life.h"

static unsigned TimerDelay = 0;
static BOOL     NeedNewTimer = FALSE;
static unsigned FastEnoughCount = 0;


extern BOOL InitTimer()
/**********************

    Initialize the timer callbacks
*/
{
    TimerDelay = ONE_SECOND/100;
    if( !SetTimer( WinHandle, TIMER_ID, TimerDelay, 0L ) ) {
        Error( "Too many timers in use" );
        return( FALSE );
    }
    return( TRUE );
}


extern void FiniTimer()
/*********************/
{
    KillTimer( WinHandle, TIMER_ID );
}


extern void TimerTurbo()
/***********************

    Turn the timer up as fast as it goes. It'll slow down on its own
*/
{
    TimerDelay = ONE_SECOND/100;
    NeedNewTimer = TRUE;
    NewTimer();
}


extern void NewTimer()
/*********************

    Set a new timer rate if required.
*/
{
    if( NeedNewTimer ) {
        KillTimer( WinHandle, TIMER_ID );
        SetTimer( WinHandle, TIMER_ID, TimerDelay, 0L );
        NeedNewTimer = FALSE;
    }
}


extern void RelinquishControl()
/******************************

    Allow another process to take control, so the system doesn't grind to a
    halt.
*/
{
    MSG         peek;

    PeekMessage( &peek, WinHandle, WM_TIMER, WM_TIMER, PM_NOREMOVE );
}


extern void CheckTimerRate()
/***************************

    Check to see if we should speed up or slow down the timer, based upon
    whether there's already another WM_TIMER event in the queue. We speed
    up if we made it before the new timer event n times, and slow down
    the first time we don't make it.
*/
{
    MSG         peek;
    BOOL        removed;

    removed = FALSE;
    while( PeekMessage( &peek, WinHandle, WM_TIMER, WM_TIMER, PM_REMOVE ) ) {
        removed = TRUE;
    }
    if( removed ) {
        FastEnoughCount = 0;
        TimerDelay += TimerDelay / 8;
        NeedNewTimer = TRUE;
    } else if( ++FastEnoughCount > 4 ) {
        if( FastEnoughCount*TimerDelay > 4*ONE_SECOND ) {
            TimerDelay -= TimerDelay / 8;
            NeedNewTimer = TRUE;
            FastEnoughCount = 0;
        }
    }
}
