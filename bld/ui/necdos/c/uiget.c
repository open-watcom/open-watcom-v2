/****************************************************************************
*
*                            Open Watcom Project
*
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


#include <dos.h>
#include <time.h>
#include "biosui.h"
#include "uidef.h"
#include "uiforce.h"

extern EVENT                    Event;

#ifdef __386__
extern unsigned long            uiclock();
extern void                     uirefreshclock();
#else
static volatile unsigned long   clock_ticks = 0;
static volatile int             clock_int = 1;
#endif

void global uiflush()
/***********************************************************************
 uiflush:
 -------
    Input:      NONE
    Output:     NONE

    clear up keyboard buffer for any unused keys
************************************************************************/
{
    Event = EV_NO_EVENT;
    flushkey();
}

void global uiexpendclock()
{
#if !defined( __386__ )
    while( !clock_int );
#endif
}

#if !defined( __386__ )
void __interrupt __far timer_rtn()
{
    ++clock_ticks;
    clock_int = 1;
}

unsigned long uiclock()
{
    return( clock_ticks );
}
#endif

EVENT global uieventsource( int update )
/***********************************************************************
 uieventsource:
 -------------
    Input:      update
    Output:     NONE

    return any BIOS event ( if there is any )
************************************************************************/
{
    register    EVENT                   ev;
    static      int                     ReturnIdle = 1;
    unsigned    long                    start;

    start = uiclock();
    for(;;) {
        /*  since NEC doesn't provide 100th of sec in time request we
            have to start our own timer and every time timer expire
            we increment our internal clock tick counter to keep track
            internal time
         */
        #if defined( __386__ )
        uirefreshclock();
        #else
        if( clock_int == 1 ) {
            union       REGPACK                 r;

            clock_int = 0;
            r.h.ah = 0x02;              /* set interval timer */
            r.w.cx = 2;                 /* duration n x 10 msec */
            r.w.bx = FP_OFF( (void far *)timer_rtn );
            r.w.es = FP_SEG( (void far *)timer_rtn );
            intr( 0x1c, &r );
        }
        #endif

        ev = forcedevent();
        if( ev > EV_NO_EVENT ) break;
        ev = mouseevent();
        if( ev > EV_NO_EVENT ) {
            break;
        }
        ev = keyboardevent();
        if( ev > EV_NO_EVENT ) {
            uihidemouse();
            break;
        }
        if( ReturnIdle ) {
            ReturnIdle--;
            return( EV_IDLE );
        } else {
            if( !UIData->no_idle_int ) IdleInterrupt();
            if( update  &&  !UIData->no_refresh ) uirefresh();
            if( uiclock() - start >= UIData->tick_delay ) {
                return( EV_CLOCK_TICK );
            } else if( UIData->busy_wait ) {
                return( EV_SINK );
            }
        }
    }
    ReturnIdle = 1;
    UIData->no_refresh = FALSE;
    return( ev );
}


EVENT global uiget()
/***********************************************************************
 uiget:
 -----
    Input:      NONE
    Output:     NONE

    call uieventsource with update on
************************************************************************/
{
    return( uieventsource( 1 ) );
}
