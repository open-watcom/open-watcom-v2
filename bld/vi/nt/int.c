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
#include <dos.h>
#include <signal.h>
#include <setjmp.h>
#include "win.h"

static volatile int     exit_thread;

static DWORD WINAPI TimerThread( LPVOID param )
{
    char        date[80];

    param = param;
    while( !exit_thread ) {
        Sleep( 55 );
        ClockTicks++;
        if( EditFlags.ClockActive && EditFlags.SpinningOurWheels &&
            EditFlags.Spinning ) {
            SpinLoc->cinfo_char = SpinData[SpinCount];
            MyVioShowBuf( SpinLoc - Scrn, 1 );
            SpinCount++;
            if( SpinCount >= 4 ) {
                SpinCount = 0;
            }
        }
        if( (ClockTicks % 9) != 0 ) {
            continue;
        }
        GetDateTimeString( date );
        if( EditFlags.ClockActive && EditFlags.Clock ) {
            int nchars = 5;

            if( EditFlags.DisplaySeconds ) {
                ClockStart[7].cinfo_char = date[DATE_LEN - 1];
                ClockStart[6].cinfo_char = date[DATE_LEN - 2];
                ClockStart[5].cinfo_char = ':';
                nchars = 8;
            }
            ClockStart[4].cinfo_char = date[DATE_LEN - 4];
            ClockStart[3].cinfo_char = date[DATE_LEN - 5];
            ClockStart[2].cinfo_char = ':';
            ClockStart[1].cinfo_char = date[DATE_LEN - 7];
            ClockStart[0].cinfo_char = date[DATE_LEN - 8];
            MyVioShowBuf( ClockStart - Scrn, nchars );
        }
    }
    return( 0 );
}

static BOOL WINAPI BreakHandler( DWORD type )
{
    switch( type ) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        if( EditFlags.WatchForBreak ) {
            EditFlags.BreakPressed = true;
        }
        return( TRUE );
    default:
        return( FALSE );
    }
}

void SetInterrupts( void )
{
    DWORD       tid;

    exit_thread = false;
    CreateThread( NULL, 1024, TimerThread, NULL, 0, &tid );
    SetConsoleCtrlHandler( BreakHandler, TRUE );
}

void RestoreInterrupts( void )
{
    exit_thread = true;
    SetConsoleCtrlHandler( BreakHandler, FALSE );
}
