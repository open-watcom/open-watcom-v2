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


#include "variety.h"
#include <io.h>
#include <windows.h>
#include "ntexc.h"
#include "rtdata.h"
#include "fileacc.h"

static HANDLE console_in = (HANDLE)-1;
static HANDLE console_out = (HANDLE)-1;

int __NTRealKey( INPUT_RECORD *k )
{
    if( k->EventType == KEY_EVENT ) {
        if( k->Event.KeyEvent.bKeyDown ) {
            switch( k->Event.KeyEvent.wVirtualKeyCode ) {
            case VK_SHIFT:
            case VK_CONTROL:
            case VK_MENU:       /* Alt */
                return( 0 );
            }
            return( 1 );
        }
    }
    return( 0 );
}

static void initConsoleHandles( void )
{
    _AccessFileH( STDIN_FILENO );
    if( console_in == (HANDLE)-1 ) {
        console_in = CreateFile( "conin$",
                                 GENERIC_READ, FILE_SHARE_READ, NULL,
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    }
    if( console_out == (HANDLE)-1 ) {
        console_out = CreateFile( "conout$",
                                  GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
    }
    _ReleaseFileH( STDIN_FILENO );
}

HANDLE __NTConsoleInput( void )
{
    initConsoleHandles();
    return( console_in );
}

HANDLE __NTConsoleOutput( void )
{
    initConsoleHandles();
    return( console_out );
}
