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
#include <conio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>
#include "winserv.h"

#define TMPSLEN 256

static char     tmpStr[TMPSLEN];
static short    tmpOff=0;

void Output( char *str )
{
    int         i;
    int         len;

    len = strlen( str );
    for( i=0;i<=len;i++ ) {
        if( str[i] == 0 ) {
            return;
        }
        if( str[i] == '\r' ) {
            continue;
        }
        if( str[i] == '\n' ) {
            tmpStr[ tmpOff ] = 0;
            if( DebugWindow != NULL ) {
                SendMessage( DebugWindow, LB_ADDSTRING, 0,
                     (LONG) (LPSTR) tmpStr );
            }
            tmpOff = 0;
        } else {
            tmpStr[tmpOff++] = str[i];
            if( tmpOff >= TMPSLEN-1 ) {
                tmpOff--;
            }
        }
    }

} /* Output */

void SayGNiteGracey( int return_code )
{
    return_code = return_code;
}

void StartupErr( char *err )
{
    MessageBox( NULL, err, "", MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
    if( MainWindowHandle != NULL ) {
        ShowWindow( MainWindowHandle, SW_SHOWNORMAL );
    }
}

int KeyPress( void )
{
    return( FALSE );
}

int KeyGet( void )
{
    return( 0 );
}

int  WantUsage( char *foo ) {
    foo = foo;
    return( FALSE );
}
