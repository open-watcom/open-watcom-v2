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
#include <stdio.h>
#include <dos.h>
#include <windows.h>
#include "ntex.h"
#include "rtdata.h"
#include "fileacc.h"
#include "defwin.h"

_WCRTLINK int kbhit( void )
{
    DWORD n;
    HANDLE h;
    INPUT_RECORD r;

    if( _WindowsKbhit != 0 ) {
        LPWDATA res;
        res = _WindowsIsWindowedHandle( (int) STDIN_FILENO );
        return( _WindowsKbhit( res ) );
    }
    _AccessFileH( STDIN_FILENO );
    h = __NTConsoleInput();
    for(;;) {
        PeekConsoleInput( h, &r, 1, &n );
        if( n == 0 ) break;
        if( __NTRealKey( &r ) ) break;
        // flush out mouse, window, and key up events
        ReadConsoleInput( h, &r, 1, &n );
    }
    // n != 0 if there is a key waiting
    _ReleaseFileH( STDIN_FILENO );
    return( n != 0 );
}
