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


#include "vi.h"
#include <signal.h>
#include <setjmp.h>
#include "win.h"
#define INCL_DOSSIGNALS
#define INCL_DOSPROCESS
#include <os2.h>

static void BrkHandler( int sig_num )
{
    sig_num = sig_num;
    signal( SIGINT, BrkHandler );
    signal( SIGBREAK, BrkHandler );
    if( EditFlags.WatchForBreak ) {
        EditFlags.BreakPressed = TRUE;
    }
    KeyAdd( 3 );
}

static volatile int     exitThread;

#ifndef __OS2V2__
#define TSTACK_SIZE     1024
static unsigned char    __far thread_stack[TSTACK_SIZE];
#else
#define TSTACK_SIZE     10240
#endif

void TimerThread( void )
{
    char        date[80];
    char_info   *clk;

    while( !exitThread ) {
        DosSleep( 55 );
        ClockTicks++;
        if( EditFlags.ClockActive && EditFlags.SpinningOurWheels && EditFlags.Spinning ) {
            (*(char_info *)SpinLoc).ch = SpinData[SpinCount];
            SpinCount++;
            if( SpinCount >= 4 ) {
                SpinCount = 0;
            }
            MyVioShowBuf( SpinLoc - Scrn, 1 );
        }
        if( (ClockTicks % 9) != 0 ) {
            continue;
        }
        GetDateTimeString( date );
        if( EditFlags.ClockActive && EditFlags.Clock ) {
            int bytes = 5;
            clk = (char_info *) ClockStart;
            if( EditFlags.DisplaySeconds ) {
                clk[7].ch = date[DATE_LEN - 1];
                clk[6].ch = date[DATE_LEN - 2];
                clk[5].ch = ':';
                bytes = 8;
            }
            clk[4].ch = date[DATE_LEN - 4];
            clk[3].ch = date[DATE_LEN - 5];
            clk[2].ch = ':';
            clk[1].ch = date[DATE_LEN - 7];
            clk[0].ch = date[DATE_LEN - 8];
            MyVioShowBuf( ClockStart - Scrn, bytes );
        }
    }
    exitThread = FALSE;
    DosExit( EXIT_THREAD, 0 );
}

static TID timerTID;

void SetInterrupts( void )
{
    signal( SIGINT, BrkHandler );
    signal( SIGBREAK, BrkHandler );
    exitThread = FALSE;
#ifdef __OS2V2__
    DosCreateThread( &timerTID, (PFNTHREAD)TimerThread, 0, FALSE, TSTACK_SIZE );
#else
    DosCreateThread( TimerThread, &timerTID, &thread_stack[TSTACK_SIZE - 2] );
#endif
}

void RestoreInterrupts( void )
{
    exitThread = TRUE;
#ifdef __OS2V2__
    DosWaitThread( &timerTID, DCWW_WAIT );
#else
    while( exitThread ) {
        DosSleep( 1 );
    }
#endif

    signal( SIGINT, SIG_DFL );
    signal( SIGBREAK, SIG_DFL );
}
