/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


// USAGE:
// RUN <CLASS NAME> "<CAPTION>" <PATH> <PARAMETERS>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "winexprt.h"
#include "wclbproc.h"


WINEXPORT BOOL CALLBACK EnumProc( HWND hwnd, LPARAM lparam );

char ClassName[256];
char Caption[256];
WORD CaptionLen;
HWND TheWindow;

WINEXPORT BOOL CALLBACK EnumProc( HWND hwnd, LPARAM lparam )
{
    char        buf[256];
    int         len;

    /* unused parameters */ (void)lparam;

    len = GetClassName( hwnd, buf, sizeof( buf ) );
    buf[len] = '\0';
    if( strcmp( buf, ClassName ) == 0 ) {
        GetWindowText( hwnd, buf, sizeof( buf ) );
        if( strncmp( buf, Caption, CaptionLen ) == 0 ) {
            TheWindow = hwnd;
            return( FALSE );
        }
    }
    return( TRUE );
}

int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow )
{
    const char  *ptr;
    char        *dst;
    WNDENUMPROC wndenumproc;

    /* unused parameters */ (void)previnst; (void)cmdshow;

    ptr = cmdline;

    // read the path
//    while( isspace( *ptr ) )
//        ptr++;
//    dst = AppName;
//    while( !isspace( *ptr ) ) {
//      *dst++ = *ptr++;
//    }
//    *dst = '\0';

    // read the classname
    while( isspace( *ptr ) )
        ptr++;
    dst = ClassName;
    while( !isspace( *ptr ) ) {
        *dst++ = *ptr++;
    }
    *dst = '\0';

    // read the Window Caption
    while( isspace( *ptr ) )
        ptr++;
    if( *ptr == '"' ) {
        ptr++;
        dst = Caption;
        while( *ptr != '"' ) {
            *dst++ = *ptr++;
        }
        *dst = '\0';
    }
    ptr++;
    CaptionLen = strlen( Caption );
    while( isspace( *ptr ) )
        ptr++;

    TheWindow = NULL;
    wndenumproc = MakeProcInstance_WNDENUM( EnumProc, currinst );
    EnumWindows( wndenumproc, 0 );
    FreeProcInstance_WNDENUM( wndenumproc );

    if( TheWindow == NULL ) {
        WinExec( ptr, SW_SHOW );
    } else {
#ifdef __NT__
        SetForegroundWindow( TheWindow );
#else
        ShowWindow( TheWindow, SW_RESTORE );
        SetWindowPos( TheWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
#endif
    }
    return( 1 );
}
