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


#include <windows.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "dummy.h"
#include "ldstr.h"

char    ShutUp;

/*
 * WinMain - this is a dummy process called by the user interface
 *           portion of Dr WATCOM NT to create a debug event in order
 *           to wake up the task control thread
 *
 *           It must be called with a command line of the form:
 *              __drnt.exe  [signature] [semaphorename]
 */

int PASCAL WinMain( HANDLE currinst, HANDLE previnst, LPSTR cmdline, int cmdshow)
{
    char        *ptr;
    char        *sig;
    char        buf[100];
    char        rcstr[256];
    unsigned    len;
    HANDLE      event_sem;

    currinst = currinst;
    previnst = previnst;
    cmdshow = cmdshow;

    len = 0;
    ptr = cmdline;
    sig = DUMMY_SIG;
    while( isspace( *ptr ) ) ptr++;
    while( *sig != '\0' ) {
        if( *sig != *ptr ) {
            LoadString( currinst, STR_DONT_RUN_ME, rcstr, sizeof( rcstr ) );
            sprintf( buf, rcstr, APP_NAME );
            MessageBox( NULL, buf, DUMMY_NAME, MB_OK | MB_ICONEXCLAMATION );
            return( 1 );
        }
        sig++;
        ptr++;
    }
    while( isspace( *ptr ) ) ptr++;
    event_sem = OpenSemaphore( SYNCHRONIZE, FALSE, ptr );
    if( event_sem == NULL ) {
        LoadString( currinst, STR_CANT_GET_SEMAPHORE, rcstr, sizeof( rcstr ) );
        sprintf( buf, rcstr, GetLastError()  );
        MessageBox( NULL, buf, DUMMY_NAME, MB_ICONEXCLAMATION | MB_OK );
        return( 1 );
    }
    for( ;; ) {
        WaitForSingleObject( event_sem, INFINITE );
        OutputDebugString( "" );
        if( ShutUp ) break; /* this is an elaborate way to make the compiler
                               not report that the return statement is
                               unreachable so this will build clean */
    }
    return( 0 );
}
