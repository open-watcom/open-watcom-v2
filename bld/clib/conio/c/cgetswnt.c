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
* Description:  Win32 cgets() implementation.
*
****************************************************************************/


#include "variety.h"
#include <windows.h>
#include <conio.h>
#include <unistd.h>
#include "ntex.h"
#include "fileacc.h"
#include "defwin.h"
#include "qread.h"

#define BACKSPACE       8
#define SPACE           ' '
#define CRLF            13


_WCRTLINK char *cgets( char *buff )
{
    char *p;
    char len;
    DWORD n;
    HANDLE h;
    INPUT_RECORD r;

#ifdef DEFAULT_WINDOWING
    if( _WindowsStdin != 0 ) {  // Default windowing...
        __qread( STDIN_FILENO, buff + 2, *buff - 1 );
        p = buff + 2;
        len = *buff;
        for(;;) {
            if( len <= 1 ) break;
            if( *p == '\r' || *p == '\0' ) break;
            ++p;
            --len;
        }
        *p = '\0';
        buff[1] = p - buff - 2;
        return( buff + 2 );
    }
#endif
    _AccessFileH( STDIN_FILENO );
    h = __NTConsoleInput();     // obtain a console input handle
    for( p = buff + 2, len = *buff; ; ) {
        ReadConsoleInput( h, &r, 1, &n );
        if( __NTRealKey( &r ) ) {       // Only interested in real keys
            if( r.Event.KeyEvent.uChar.AsciiChar == CRLF ) {
                *p = '\0';
                break;
            }
            for( ; r.Event.KeyEvent.wRepeatCount > 0;
                 --r.Event.KeyEvent.wRepeatCount ) {
                // Deal with backspace first...
                if( r.Event.KeyEvent.uChar.AsciiChar == BACKSPACE ) {
                    if( p > buff + 2 ) {
                        putch( BACKSPACE );
                        putch( SPACE );
                        putch( BACKSPACE );
                        --p;
                        ++len;
                    }
                } else if( len > 1 ) { // Other real chars...
                    *p = r.Event.KeyEvent.uChar.AsciiChar;
                    putch( r.Event.KeyEvent.uChar.AsciiChar );
                    ++p;
                    --len;
                } // Otherwise: len <= 1, can't type more.
            }
        }
    }
    _ReleaseFileH( STDIN_FILENO );
    buff[1] = p - buff - 2;
    return( buff + 2 );
}
