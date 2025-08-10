/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Win32 cgets() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <conio.h>
#include <unistd.h>
#include <windows.h>
#include "ntconio.h"
#include "fileacc.h"
#include "defwin.h"
#include "qread.h"


_WCRTLINK char *cgets( char *buff )
{
    char *p;
    int len;
    DWORD n;
    HANDLE conin;
    INPUT_RECORD r;

    len = *(unsigned char *)buff;
    p = buff + 2;
#ifdef DEFAULT_WINDOWING
    if( _WindowsStdin != NULL ) {   // Default windowing...
        for( len = __qread( STDIN_FILENO, p, len - 1 ); len > 0; --len ) {
            if( *p == '\r'
              || *p == '\0' )
                break;
            ++p;
        }
        *p = '\0';
        buff[1] = p - ( buff + 2 );
        return( buff + 2 );
    }
#endif
    _AccessFileH( STDIN_FILENO );
    conin = __NTConsoleInput();             // obtain a console input handle
    for( ; len > 1; ) {
        ReadConsoleInput( conin, &r, 1, &n );
        if( !__NTRealKey( &r ) )        // Only interested in real keys
            continue;
        if( r.Event.KeyEvent.uChar.AsciiChar == '\r' ) {
            break;
        }
        for( ; r.Event.KeyEvent.wRepeatCount > 0; --r.Event.KeyEvent.wRepeatCount ) {
            // Deal with backspace first...
            if( r.Event.KeyEvent.uChar.AsciiChar == '\b' ) {
                if( p > buff + 2 ) {
                    putch( '\b' );
                    putch( ' ' );
                    putch( '\b' );
                    --p;
                    ++len;
                }
            } else if( len > 1 ) {  // Other real chars...
                *p = r.Event.KeyEvent.uChar.AsciiChar;
                putch( r.Event.KeyEvent.uChar.AsciiChar );
                ++p;
                --len;
            } else {
                // Otherwise: len <= 1, can't type more.
                break;
            }
        }
    }
    _ReleaseFileH( STDIN_FILENO );
    *p = '\0';
    buff[1] = p - ( buff + 2 );
    return( buff + 2 );
}
