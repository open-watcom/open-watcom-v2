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
#include "uidef.h"
#include "uiforce.h"
#include "biosui.h"

extern EVENT                Event;
volatile unsigned long      clock_ticks;
volatile int                clock_int;


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


unsigned global uiclockdelay( unsigned milli )
/***********************************************************************
 uiclockdelay:
 ------------
    Input:      NONE
    Output:     NONE

    this routine converts milli-seconds into platform
    dependant units - used to set mouse & timer delays
************************************************************************/
{
    return( milli / 10 );
}


void BIOSGetClock( unsigned off, unsigned seg );
#pragma aux BIOSGetClock =      \
    "push ds"                   \
    "push di"                   \
    "mov di,ax"                 \
    "mov ds,dx"                 \
    "mov ah,1"                  \
    "int 96h"                   \
    "pop di"                    \
    "pop ds"                    \
    parm [ax][dx]               \
    modify [ax];

unsigned long uiclock()
/***********************************************************************
 uiclock:
 -------
    Input:      NONE
    Output:     current clock ticks (100ths of a second resolution)

************************************************************************/
{
    auto struct date_time_block {
            unsigned short year;            /* 1980 - 2079 */
            unsigned char  month;           /* 1 - 12 */
            unsigned char  day;             /* 1 - 31 */
            unsigned char  day_of_week;     /* 0 - 6 */
            unsigned char  hours;           /* 0 - 23 */
            unsigned char  minutes;         /* 0 - 59 */
            unsigned char  seconds;         /* 0 - 59 */
            unsigned char  hundreds;        /* 1/100 second 0 - 99 */
            unsigned char  fill;            /* always 0 */
    } dt;
    BIOSGetClock( FP_OFF(&dt), FP_SEG(&dt) );
    return( dt.hundreds +
            (dt.seconds + ((dt.minutes + (dt.hours * 60)) * 60L)) * 100L );
}


EVENT global uieventsource( int update )
/**************************************/
{
    register    EVENT                   ev;
    static      int                     ReturnIdle = 1;
    unsigned long                       start;

    start = uiclock();
    for( ; ; ) {
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
