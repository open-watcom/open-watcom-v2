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
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include "vi.h"
#include "fcbmem.h"
#define _WINSOCKAPI_
#include <windows.h>

extern HANDLE   OutputHandle,InputHandle;

void ResetSpawnScreen( void )
{
    #ifndef __WIN__
        SetConsoleActiveScreenBuffer( OutputHandle );
        SetConsoleMode( InputHandle, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT
            | ENABLE_PROCESSED_INPUT | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT );
    #endif
}

int MySpawn( char *cmd )
{
    char                path[MAX_PATH+128];
    cmd_struct          cmds;
    DWORD               rc;
    DWORD               mode;
    HANDLE              inph;
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;


    GetSpawnCommandLine( path, cmd, &cmds );
    cmds.cmd[ cmds.len ] = 0x00;
    strcat( path, " " );
    strcat( path, cmds.cmd );

    SetConsoleActiveScreenBuffer( GetStdHandle( STD_OUTPUT_HANDLE ) );
    inph = GetStdHandle( STD_INPUT_HANDLE );
    GetConsoleMode( inph, &mode );
    SetConsoleMode( inph, mode );

    memset( &sinfo, 0, sizeof( sinfo ) );
    sinfo.wShowWindow = SW_NORMAL;
    sinfo.cb = sizeof( sinfo );
    if( !CreateProcess( NULL, path, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL,
                        &sinfo, &pinfo ) ) {
        rc = -1;
    } else {
        if( WaitForSingleObject( pinfo.hProcess, -1 ) == 0 ) {
            GetExitCodeProcess( pinfo.hProcess, &rc );
        } else {
            rc = -1;
        }
    }
    return( rc );
}
