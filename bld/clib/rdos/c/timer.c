/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2025 The Open Watcom Contributors. All Rights Reserved.
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

static struct RdosTimer      *timer = 0;
static int                    timer_no = 0;
static struct RdosFutex       timer_futex;
static int                    id_arr[TIMER_ENTRIES];
static int                    FCurrIndex = 0;
static struct RdosTimerEntry *FCurrEntry = 0;

static void _WCNEAR UpdateTimers( void )
{
    int                    i;
    int                    bit;
    int                    mask;

    for( i = 0; i < 8; i++ ) {
        if( timer->Active.CompletedBitmap[i] ) {
            mask = 1;
            for( bit = 0; bit < 32; bit++ ) {
                if( timer->Active.CompletedBitmap[i] & mask ) {
                    if( (timer->Req.DoneBitmap[i] & mask) == 0 ) {
                        timer->Req.DoneBitmap[i] |= mask;
                        FCurrIndex = 32 * i + bit;
                        FCurrEntry = &timer->Req.EntryArr[FCurrIndex - 4];
                        ( *FCurrEntry->callback )( FCurrEntry->param );
                        FCurrEntry = 0;
                        FCurrIndex = 0;
                    }
                }
                mask = mask << 1;
            }
        }
    }
}

static void _WCNEAR TimerThread( void *param )
{
    timer = (struct RdosTimer *)param;

    for( ;; ) {
        if( __wait_timer_event() ) {
            RdosEnterFutex( &timer_futex );
            UpdateTimers();
            RdosLeaveFutex( &timer_futex );
        } else
            break;
    }

    timer = 0;
}

static void _WCNEAR CreateTimerThread( void )
{
    int retry;

    _begintimerthread( &TimerThread );
    for( retry = 0; retry < 100 && !timer; retry++ ) {
        RdosWaitMilli( 10 );
    }
}

static int _WCNEAR StartTimer( void (*callback)( void *param ), void *param, long long timeout )
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
                if( (active & mask) == 0 ) {
                    index = 32 * i + bit;
                    entry = &timer->Req.EntryArr[index - 4];
                    entry->timeout = timeout;
                    entry->callback = callback;
                    entry->param = param;
                    __locked_set_bit( timer->Req.ReqBitmap, index );
                    done = 1;
                    break;
                }
                mask = mask << 1;
            }
        }
    }

    if( done ) {
        return( index );
    } else {
        return( 0 );
    }
}

static int _WCNEAR StopTimer( int index )
{
    int                    i;
    int                    bit;
    int                    mask;

    if( index < 4
      || index >= 256 )
        return( 0 );

    i = index / 32;
    bit = index % 32;
    mask = 1 << bit;
    if( timer->Req.DoneBitmap[i] & mask ) {
        return( 0 );
    } else {
        if( (timer->Req.ReqBitmap[i] & mask)
          || (timer->Active.PendingBitmap[i] & mask)
          || (timer->Active.CompletedBitmap[i] & mask) ) {
            timer->Req.DoneBitmap[i] |= mask;
            return( 1 );
        } else {
            return( 0 );
        }
    }
}

static int _WCNEAR RestartCurrTimer( long long timeout )
{
    int                    i;
    int                    bit;
    int                    mask;

    if( FCurrEntry == 0 )
        return( 0 );

    i = FCurrIndex / 32;
    bit = FCurrIndex % 32;
    mask = 1 << bit;
    timer->Req.DoneBitmap[i] &= ~mask;

    FCurrEntry->timeout = timeout;
    __locked_set_bit( timer->Req.ReqBitmap, FCurrIndex );
    return( 1 );
}

static int _WCNEAR ResetTimer( int index, long long timeout )
{
    int                    i;
    int                    bit;
    int                    mask;
    struct RdosTimerEntry *entry;

    if( index < 4
      || index >= 256 )
        return( 0 );

    i = index / 32;
    bit = index % 32;
    mask = 1 << bit;
    if( timer->Req.DoneBitmap[i] & mask ) {
        return( 0 );
    } else {
        if( timer->Active.PendingBitmap[i] & mask ) {
            entry = &timer->Req.EntryArr[index - 4];
            entry->timeout = timeout;
            return( 1 );
        } else {
            return( 0 );
        }
    }
}

int RdosStartAppTimer( void (*Start)(void *Param), void *Param, int Ms )
{
    int index;
    int id = 0;
    long long timeout = RdosUserGetLongSysTime() + 1193 * Ms;

    RdosEnterFutex( &timer_futex );

    if( timer == 0 )
        CreateTimerThread( );

    index = StartTimer( Start, Param, timeout );

    if( index ) {
        timer_no++;
        if( timer_no == 0 )
            timer_no++;
        id = timer_no;
        id_arr[index] = id;
    }

    RdosLeaveFutex( &timer_futex );

    if( Ms < 100 )
        __signal_timer( );

    return( id );
}

int RdosStopAppTimer( int id )
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

int RdosRestartCurrentAppTimer( int Ms )
{
    int  res;
    long long timeout = RdosUserGetLongSysTime() + 1193 * Ms;

    RdosEnterFutex( &timer_futex );
    res = RestartCurrTimer( timeout );
    RdosLeaveFutex( &timer_futex );

    return( res );
}

int RdosResetAppTimer( int id, int Ms )
{
    int  i;
    int  res = 0;
    long long timeout = RdosUserGetLongSysTime() + 1193 * Ms;

    RdosEnterFutex( &timer_futex );

    for( i = 0; i < TIMER_ENTRIES; i++ ) {
        if( id_arr[i] == id ) {
            res = ResetTimer( i, timeout );
            break;
        }
    }

    RdosLeaveFutex( &timer_futex );

    return( res );
}

static void _WCNEAR init( void )
{
    int i;

    RdosInitFutex( &timer_futex, "App.Timer" );

    for( i = 0; i < TIMER_ENTRIES; i++ ) {
        id_arr[i] = 0;
    }
}

AXIN( init, INIT_PRIORITY_RUNTIME )
