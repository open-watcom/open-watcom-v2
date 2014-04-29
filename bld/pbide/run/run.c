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


// USAGE:
// RUN <CLASS NAME> "<CAPTION>" <PATH> <PARAMETERS>

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char ClassName[256];
char Caption[256];
WORD CaptionLen;
HWND TheWindow;

BOOL CALLBACK EnumProc( HWND hwnd, LPARAM lparam ) {
    char        buf[256];

    lparam = lparam;
    GetClassName( hwnd, buf, sizeof( buf ) );
    if( !strcmp( buf, ClassName ) ) {
        GetWindowText( hwnd, buf, sizeof( buf ) );
        if( !strncmp( buf, Caption, CaptionLen ) ) {
            TheWindow = hwnd;
            return( FALSE );
        }
    }
    return( TRUE );
}

int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    char        *ptr;
    char        *dst;
    FARPROC     enumproc;

    currinst = currinst;
    previnst = previnst;
    cmdshow = cmdshow;

    ptr = cmdline;

    // read the path
//    while( isspace( *ptr ) ) ptr++;
//    dst = AppName;
//    while( !isspace( *ptr ) ) {
//      *dst = *ptr;
//      dst++;
//      ptr++;
//    }
//    *dst = '\0';

    // read the classname
    while( isspace( *ptr ) ) ptr++;
    dst = ClassName;
    while( !isspace( *ptr ) ) {
        *dst = *ptr;
        dst++;
        ptr++;
    }
    *dst = '\0';

    // read the Window Caption
    while( isspace( *ptr ) ) ptr++;
    if( *ptr == '"' ) {
        ptr++;
        dst = Caption;
        while( *ptr != '"' ) {
            *dst = *ptr;
            dst++;
            ptr++;
        }
        *dst = '\0';
    }
    ptr++;
    CaptionLen = strlen( Caption );
    while( isspace( *ptr ) ) ptr++;

    TheWindow = NULL;
    enumproc = MakeProcInstance( (FARPROC)EnumProc, currinst );
    EnumWindows( (WNDENUMPROC)enumproc, 0 );
    FreeProcInstance( enumproc );

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
