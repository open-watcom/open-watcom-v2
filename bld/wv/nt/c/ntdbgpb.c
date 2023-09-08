/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#ifdef GUI_IS_GUI
#include "dbgwind.h"
#endif
#include "dbglit.h"
#include "dbgerr.h"
#include "ntdbgpb.h"


extern BOOL         CALLBACK FindPidPB( HWND  hwnd, LPARAM  lParam );

static DWORD        PidPB;
static bool         AlreadyRunning = false;

BOOL CALLBACK FindPidPB( HWND  hwnd, LPARAM  lParam )
{
    char    buff[256];
    int     len;

    lParam=lParam;
    len = GetClassName( hwnd, buff, sizeof( buff ) );
    buff[len] = NULLCHAR;
    if( strstr( buff, "PBFRAME" ) != NULL ) {
        GetWindowThreadProcessId(hwnd, &PidPB );
    }
    if( GetWindowText( hwnd, buff, sizeof( buff ) ) != 0 ) {
        if( stricmp( buff, LIT_DUI( The_WATCOM_Debugger_for_PowerBuilder ) ) == 0 ) {
            AlreadyRunning = true;
        }
    }
    return( true );
}

const char *CheckForPowerBuilder( const char *name )
{
    static char pid[20];

    if( _IsOff( SW_POWERBUILDER ) )
        return( name );
    EnumWindows( FindPidPB, 0 );
    if( AlreadyRunning ) {
        StartupErr( LIT_DUI( PowerBuilder_Debugger_Already_Running ) );
        return( "" );
    }
    if( PidPB == 0 ) {
        StartupErr( LIT_DUI( PowerBuilder_Not_Running ) );
        return( "" );
    } else {
        sprintf( pid, "#%x", PidPB );
#ifdef GUI_IS_GUI
        WndSetTitle( WndMain, LIT_DUI( The_WATCOM_Debugger_for_PowerBuilder ) );
#else
        SetConsoleTitle( LIT_DUI( The_WATCOM_Debugger_for_PowerBuilder ) );
#endif
        return( pid );
    }
}
