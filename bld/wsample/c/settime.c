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


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
//#include <malloc.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "intrptr.h"
#include "os.h"
#include "timermod.h"
#include "wmsg.h"

extern char  FAR_PTR    *MsgArray[ERR_LAST_MESSAGE-ERR_FIRST_MESSAGE+1];
extern void             Output( char FAR_PTR * );
extern unsigned         GetNumber( unsigned, unsigned, char **, unsigned );
extern void             fatal(void);

void InitTimerRate()
{
    TimerMult = DEF_MULT;
}

void SetTimerRate( char **cmd )
{
    unsigned            millisec;

    millisec = GetNumber( MN_MILLISEC, MX_MILLISEC, cmd, 10 );
#if defined(_NEC_PC) || defined(_FMR_PC)
    TimerMult = millisec / MILLISEC_PER_TICK;
    /* Make sure we round the result (i.e., 33/10->3, 35/10->4) */
    if( ( millisec % MILLISEC_PER_TICK ) >= 5 ) {
        ++TimerMult;
    }
#else
    TimerMult = MILLISEC_PER_TICK / millisec;
    if( ( MILLISEC_PER_TICK % millisec ) > ( millisec / 2 ) ) {
        ++TimerMult;
    }
#endif
    TimerMod = TimerMult;
}

unsigned long TimerRate()
{
#if defined(_NEC_PC) || defined(__FMR_PC)
    /*  *10 for millisecs *1000 for microsecs */
    return( (long)TimerMult * 10L * 1000L );
#else
    return( 100000000 / TICKS_PER_HUNDRED );
#endif
}

unsigned SafeMargin()
{
    unsigned    safe_wait;
    unsigned    margin;

#if defined(_NEC_PC) || defined(_FMR_PC)
    /* 20 secs X maximum 100 samples per sec / default rate */
    safe_wait = (20 * 100) / TimerMult;
    /*
      With standard timing there are about 20 samples per second or
      400 samples taken per 20 seconds.
      20 sec is an arbitrary period of time considered safety margin: no
      DOS function should take more time tying-up int21 resources.
    */
#else
    safe_wait = TimerMult * 300;
    /*
      With standard timing there are about 300 samples taken per 20 seconds.
      20 sec is an arbitrary period of time considered safety margin: no
      DOS function should take more time tying-up int21 resources.
    */
#endif
    if( safe_wait > Ceiling / 2 )  safe_wait = Ceiling / 2;
    margin = Ceiling - safe_wait;

#if defined(_NEC_PC) || defined(_FMR_PC)
    /*  *10 for millisecs */
    if( margin < (TimerMult * 10) ) {
#else
    if( margin < (TICKS_PER_HUNDRED / 100) ) {
#endif
        Output( MsgArray[MSG_BUFF_SMALL-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    return( margin );
}

