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
* Description:  Win32 putch() implementation.
*
****************************************************************************/


#include "variety.h"
#include <conio.h>
#include <windows.h>
#include "ntconio.h"
#include "fileacc.h"
#include "defwin.h"

/*
    BOOL WriteConsole(
      HANDLE hConsoleOutput,           // handle to screen buffer
      CONST VOID *lpBuffer,            // write buffer
      DWORD nNumberOfCharsToWrite,     // number of characters to write
      LPDWORD lpNumberOfCharsWritten,  // number of characters written
      LPVOID lpReserved                // reserved
    );
 */

_WCRTLINK int putch( int c )
{
    char        ch;
    DWORD       written;
    HANDLE      h;

    ch = c;
#ifdef DEFAULT_WINDOWING
    if( _WindowsPutch != 0 ) {
        LPWDATA res;
        res = _WindowsIsWindowedHandle( STDOUT_FILENO );
        _WindowsPutch( res, c );
    } else {
#endif
        written = 0;
        h = __NTConsoleOutput();            // obtain a console output handle
        if( h != INVALID_HANDLE_VALUE ) {
            WriteConsole( h, &ch, 1, &written, NULL );
        }
        if( written == 0 ) return( -1 );
#ifdef DEFAULT_WINDOWING
    }
#endif
    return( c );
}
