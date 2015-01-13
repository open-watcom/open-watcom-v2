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
* Description:  Sybase Power Builder check routines for NT host.
*
****************************************************************************/


#include "wwindows.h"
#include "dbgdefn.h"
#include "dbgdata.h"
#ifdef __GUI__
#include "dbgwind.h"
#endif
#include "dbglit.h"

extern void     StartupErr( const char *err );

#ifdef __GUI__
extern a_window *WndMain;
#endif

static DWORD        PidPB;
static bool         AlreadyRunning = false;

BOOL CALLBACK FindPidPB( HWND  hwnd, LPARAM  lParam )
{
    char buff[256];

    GetClassName( hwnd, buff, sizeof( buff ) );
    if( strstr( buff, "PBFRAME" ) != NULL ) {
        GetWindowThreadProcessId(hwnd, &PidPB );
    }
    if( GetWindowText( hwnd, buff, sizeof( buff ) ) != 0 ) {
        if( stricmp( buff, LIT( The_WATCOM_Debugger_for_PowerBuilder ) ) == 0 ) {
            AlreadyRunning = true;
        }
    }
    return( TRUE );
}

const char *CheckForPowerBuilder( const char *name )
{
    static char pid[20];

    if( _IsOff( SW_POWERBUILDER ) )
        return( name );
    EnumWindows( FindPidPB, 0 );
    if( AlreadyRunning ) {
        StartupErr( LIT( PowerBuilder_Debugger_Already_Running ) );
        return( "" );
    }
    if( PidPB == 0 ) {
        StartupErr( LIT( PowerBuilder_Not_Running ) );
        return( "" );
    } else {
        pid[0] = '#';
        itoa( PidPB, pid + 1, 16 );
#ifdef __GUI__
        WndSetTitle( WndMain, LIT( The_WATCOM_Debugger_for_PowerBuilder ) );
#else
        SetConsoleTitle( LIT( The_WATCOM_Debugger_for_PowerBuilder ) );
#endif
        return( pid );
    }
}
