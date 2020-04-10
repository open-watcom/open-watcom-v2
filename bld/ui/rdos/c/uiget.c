/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <rdos.h>
#include <stdio.h>
#include "uidef.h"
#include "uiforce.h"


typedef ui_event (*event_proc)(void);

extern int              WaitHandle;

static rdos_uitimer_callback *Callback = NULL;
static int              TimerPeriodMs = 0;
static bool             HasEscape = false;

void UIAPI rdos_uitimer( rdos_uitimer_callback *proc, int ms )
{
    Callback = proc;
    TimerPeriodMs = ms;
}

MOUSETIME UIAPI uiclock( void )
/*****************************
 * this routine get time in platform dependant units,
 * used for mouse & timer delays
 */
{
    /* use 1 ms precission (timers) */
    return( RdosGetLongSysTime() / 1192L );
}

unsigned UIAPI uiclockdelay( unsigned milli )
/*******************************************
 * this routine converts milli-seconds into platform
 * dependant units - used to set mouse & timer delays
 */
{
    /* use 1 ms precission (timers) */
    return( milli);
}

void UIAPI uiflush( void )
{
    uiflushevent();
    flushkey();
}

ui_event UIAPI uieventsource( bool update )
{
    static int      ReturnIdle = 1;
    ui_event        ui_ev;
    MOUSETIME       start;
    event_proc          proc;

    start = uiclock();
    for( ;; ) {
        if( HasEscape ) {
            HasEscape = false;
            ui_ev = EV_ESCAPE;
            break;
        }
        ui_ev = forcedevent();
        if( ui_ev > EV_NO_EVENT )
            break;

        if( Callback != NULL && TimerPeriodMs ) {
            proc = (event_proc)RdosWaitTimeout( WaitHandle, TimerPeriodMs );
            if( proc == NULL ) {
                (*Callback)();
            } else {
                ui_ev = (*proc)();
                if( ui_ev > EV_NO_EVENT ) {
                    break;
                }
            }
        } else {
            proc = (event_proc)RdosWaitTimeout( WaitHandle, 25 /* ms */ );
            if( proc != NULL ) {
                ui_ev = (*proc)();
                if( ui_ev > EV_NO_EVENT ) {
                    break;
                }
            }

            if( ReturnIdle ) {
                ReturnIdle--;
                return( EV_IDLE );
            } else {
                if( update )
                    uirefresh();
                if( uiclock() - start >= UIData->tick_delay ) {
                    return( EV_CLOCK_TICK );
                } else if( UIData->busy_wait ) {
                    return( EV_SINK );
                }
            }

            proc = (event_proc)RdosWaitTimeout( WaitHandle, 250 /* ms */ );
            if( proc != NULL ) {
                ui_ev = (*proc)();
                if( ui_ev > EV_NO_EVENT ) {
                    break;
                }
            }
        }
    }
    ReturnIdle = 1;
    return( ui_ev );
}


ui_event UIAPI uiget( void )
{
    return( uieventsource( true ) );
}

void UIAPI rdos_uisendescape( void )
{
    HasEscape = true;
}
