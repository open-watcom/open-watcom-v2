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
* Description:  Set up sampling timer rate.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "os.h"
#include "wmsg.h"

#define global_timer_data
#include "timermod.h"

void InitTimerRate( void )
{
    TimerMult = DEF_MULT;
    TimerRestoreValue = 0;
}

void SetTimerRate( char **cmd )
{
    unsigned            millisec;

    millisec = GetNumber( MN_MILLISEC, MX_MILLISEC, cmd, 10 );
    TimerMult = MILLISEC_PER_TICK / millisec;
    if( ( MILLISEC_PER_TICK % millisec ) > ( millisec / 2 ) ) {
        ++TimerMult;
    }
    TimerMod = TimerMult;
}

#ifdef __NETWARE__
extern void SetRestoreRate( char **cmd )
{
    /*
    //  If someone codes a value of 1 then they are in HUGE trouble!
    //  Just set to 0, which is DOS default!
    */
    TimerRestoreValue = GetNumber( 0, 0xFFFF, cmd, 10 );
    if( 1 == TimerRestoreValue ) {
        TimerRestoreValue = 0;
    }
}

extern void ResolveRateDifferences( void )
{
    /*
    //  On NetWare 5 and 6, the default interrupt will be at approx 145 Hz. We
    //  should calculate this but it is quicker to just use the restore rate if
    //  specified. We need to overhaul the sampler full stop.
    //  It's not as if it's much use anyway. The sampling granularity, even at
    //  the maximum of 1mS, is not enough to see what the process is doing
    //  except at a very high level. Also you end up with loads of junk reported
    //  when you're not even the primary application!!!!!
    //
    //  here we need to resolve and differences between expected operation
    //  (18.2 ints/sec) and what we are really using as default (144/s on NW5+)
    //  for now, I haven't decided how to do this so I'll just ignore the
    //  problems    :)
    */
}

#endif

unsigned long TimerRate( void )
{
    return( 100000000 / TICKS_PER_HUNDRED );
}

unsigned SafeMargin( void )
{
    unsigned    safe_wait;
    unsigned    margin;

    safe_wait = TimerMult * 300;
    /*
      With standard timing there are about 300 samples taken per 20 seconds.
      20 sec is an arbitrary period of time considered safety margin: no
      DOS function should take more time tying-up int21 resources.
    */
    if( safe_wait > Ceiling / 2 )
        safe_wait = Ceiling / 2;
    margin = Ceiling - safe_wait;

    if( margin < (TICKS_PER_HUNDRED / 100) ) {
        Output( MsgArray[MSG_BUFF_SMALL - ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    return( margin );
}

