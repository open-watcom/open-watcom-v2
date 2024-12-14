/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2019 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  RDOS/POSIX timer implementations
*
* Author: L. Ekblad
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <process.h>
#include "rtinit.h"
#include "rdos.h"
#include "_rdos.h"

/* timer structures are shared with kernel so do not modify the following types! */

#define TIMER_ENTRIES 252

/* timer entry */

struct RdosTimerEntry
{
    long long  timeout;
    void     (*callback)(void *param);
    void      *param;
} RdosTimerEntry;

/* active timers (read-only) */

struct RdosActiveTimers
{
    int                    PendingBitmap[8];
    int                    CompletedBitmap[8];
    struct RdosTimerEntry  EntryArr[TIMER_ENTRIES];
} RdosActiveTimers;

/* requested timers (read-write) */

struct RdosReqTimers
{
    int                    ReqBitmap[8];
    int                    DoneBitmap[8];
    struct RdosTimerEntry  EntryArr[TIMER_ENTRIES];
} RdosReqTimers;

struct RdosTimer
{
    struct RdosReqTimers    Req;
    struct RdosActiveTimers Active;
};

static struct RdosTimer *timer = 0;
static int               timer_no = 0;
static struct RdosFutex  timer_futex;
static int               id_arr[TIMER_ENTRIES];

static void UpdateTimers(  )
{
    int                    i;
    int                    bit;
    int                    mask;
    int                    index;
    struct RdosTimerEntry *entry;

    for( i = 0; i < 8; i++ ) {
        if( timer->Active.CompletedBitmap[i] ) {
            mask = 1;
            for( bit = 0; bit < 32; bit++ ) {
                if( timer->Active.CompletedBitmap[i] & mask ) {
                    if( ( timer->Req.DoneBitmap[i] & mask ) == 0 ) {
                        timer->Req.DoneBitmap[i] |= mask;
                        index = 32 * i + bit - 4;
                        entry = &timer->Req.EntryArr[index];
                        ( *entry->callback )( entry->param );
                    }
                }
                mask = mask << 1;
            }
        }
    }
}

static void TimerThread( void *param )
{
    timer = (struct RdosTimer *)param;

    for( ;; ) {
        if( __wait_timer_event( ) ) {
            RdosEnterFutex( &timer_futex );
            UpdateTimers();
            RdosLeaveFutex( &timer_futex );
        } else
            break;
    }

    timer = 0;
}

static void CreateTimerThread( )
{
    int retry;

    _begintimerthread( &TimerThread );
    for( retry = 0; retry < 100 && !timer; retry++ )
        RdosWaitMilli( 10 );
}

static int StartTimer( void (*callback)( void *param ), void *param, long long timeout )
{
    int                    i;
    int                    bit;
    int                    active;
    int                    done = 0;
    int                    index = 0;
    int                    mask;
    struct RdosTimerEntry *entry;

    for( i = 0; i < 8 && !done ; i++ ) {
        active = timer->Req.ReqBitmap[i] | timer->Active.PendingBitmap[i] | timer->Active.CompletedBitmap[i];
        if( active != -1 ) {
            mask = 1;
            for( bit = 0; bit < 32; bit++ ) {
                if( ( active & mask ) == 0 ) {
                    index = 32 * i + bit - 4;
                    entry = &timer->Req.EntryArr[index];
                    entry->timeout = timeout;
                    entry->callback = callback;
                    entry->param = param;
                    timer->Req.ReqBitmap[i] |= mask;
                    done = 1;
                    break;
                }
                mask = mask << 1;
            }
        }
    }
    return( index + 4 );
}

static int StopTimer( int index )
{
    int                    i;
    int                    bit;
    int                    mask;

    if( index < 4 || index >= 256 )
        return( 0 );

    i = index / 32;
    bit = index % 32;
    mask = 1 << bit;
    if( timer->Req.DoneBitmap[i] & mask )
        return( 0 );
    else {
        if( timer->Req.ReqBitmap[i] | timer->Active.PendingBitmap[i] | timer->Active.CompletedBitmap[i] ) {
            timer->Req.DoneBitmap[i] |= mask;
            return( 1 );
        } else
            return( 0 );
    }
}

int RdosStartAppTimer(void (*Start)(void *Param), void *Param, long long Timeout)
{
    int index;
    int id;

    RdosEnterFutex( &timer_futex );

    if( !timer )
        CreateTimerThread( );

    index = StartTimer( Start, Param, Timeout );

    if( index ) {
        timer_no++;
        id = timer_no;
        id_arr[index] = id;
    }

    RdosLeaveFutex( &timer_futex );

    return( id );
}

int RdosStopAppTimer(int id)
{
    int i;
    int res = 0;

    RdosEnterFutex( &timer_futex );

    for( i = 0; i < TIMER_ENTRIES; i++ ) {
        if( id_arr[i] == id ) {
            res = StopTimer( i );
            break;
        }
    }

    RdosLeaveFutex( &timer_futex );

    return( res );
}

_WCRTLINK int timer_create( clockid_t __clk, struct sigevent *__sevp, timer_t *__tmr )
{
    /* unused parameters */ (void)__clk; (void)__sevp, (void)__tmr;

    return( 0 );
}


_WCRTLINK int timer_delete( timer_t __tmr )
{
    /* unused parameters */ (void)__tmr;

    return( 0 );
}

_WCRTLINK int timer_getoverrun( timer_t __tmr )
{
    /* unused parameters */ (void)__tmr;

    return( 0 );
}

_WCRTLINK int timer_gettime( timer_t __tmr, struct itimerspec *__v )
{
    /* unused parameters */ (void)__tmr, (void)__v;

    return( 0 );
}

_WCRTLINK int timer_settime( timer_t __tmr, int flags, struct itimerspec *__new, struct itimerspec *__old )
{
    /* unused parameters */ (void)__tmr, (void)flags, (void)__new, (void)__old;

    return( 0 );
}

static void init( void )
{
    int i;

    RdosInitFutex( &timer_futex, "App.Timer" );

    for( i = 0; i < TIMER_ENTRIES; i++ )
        id_arr[i] = 0;
}

AXI( init, INIT_PRIORITY_RUNTIME )
