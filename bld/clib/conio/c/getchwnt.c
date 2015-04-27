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
* Description:  Win32 getch() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include <conio.h>
#include "ntext.h"
#include "rtdata.h"
#include "fileacc.h"
#include "defwin.h"

enum {
    KS_EMPTY                    = 0,
    KS_HANDLE_FIRST_CALL        = 1,
    KS_HANDLE_SECOND_CALL       = 2,
};

static int do_getch( HANDLE console_in )
{
    INPUT_RECORD ir;
    DWORD n;
    static unsigned repeat;
    static int c;
    static int e;
    static int state = KS_EMPTY;

    switch( state ) {
    case KS_HANDLE_FIRST_CALL:
        --repeat;
        if( c != 0 ) {
            if( repeat == 0 ) {
                state = KS_EMPTY;
            }
        } else {
            state = KS_HANDLE_SECOND_CALL;
        }
        return( c );
    case KS_HANDLE_SECOND_CALL:
        if( repeat == 0 ) {
            state = KS_EMPTY;
        } else {
            state = KS_HANDLE_FIRST_CALL;
        }
        return( e );
    }
    for( ;; ) {
        if( ! ReadConsoleInput( console_in, &ir, 1, &n ) )
            break;
        if( ! __NTRealKey( &ir ) )
            continue;
        repeat = ir.Event.KeyEvent.wRepeatCount - 1;
        c = (unsigned char)ir.Event.KeyEvent.uChar.AsciiChar;
        if( (ir.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY) != 0 || c == 0 ) {
            c = 0;
            e = ir.Event.KeyEvent.wVirtualScanCode;
            state = KS_HANDLE_SECOND_CALL;
        } else {
            if( repeat != 0 ) {
                state = KS_HANDLE_FIRST_CALL;
            }
        }
        return( c );
    }
    return( EOF );
}

_WCRTLINK int getch( void )
{
    int         c;
    HANDLE      h;
    DWORD       mode;

    if( (c = _RWD_cbyte) != 0 ) {
        _RWD_cbyte = 0;
        return( c );
    }
#ifdef DEFAULT_WINDOWING
    if( _WindowsGetch != 0 ) {
        LPWDATA res;
        res = _WindowsIsWindowedHandle( (int) STDIN_FILENO );
        c = _WindowsGetch( res );
    } else {
#endif
        _AccessFileH( STDIN_FILENO );
        h = __NTConsoleInput();
        GetConsoleMode( h, &mode );
        SetConsoleMode( h, 0 );
        c = do_getch( h );
        SetConsoleMode( h, mode );
        _ReleaseFileH( STDIN_FILENO );
#ifdef DEFAULT_WINDOWING
    }
#endif
    return( c );
}
