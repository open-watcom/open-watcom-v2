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
* Description:  RDOS cgets() implementation.
*
****************************************************************************/


#include "variety.h"
#include <rdos.h>
#include <conio.h>
#include <unistd.h>

#define BACKSPACE       8
#define SPACE           ' '
#define CRLF            13


_WCRTLINK char *cgets( char *buff )
{
    char *p;
    char len;
    char ch;

    for( p = buff + 2, len = *buff; ; ) {
        ch = (char)RdosReadKeyboard();
        if( ch ) {       // Only interested in real keys
            if ( ch == CRLF ) {
                *p = '\0';
                break;
            }
            if( ch == BACKSPACE ) {
                if( p > buff + 2 ) {
                    putch( BACKSPACE );
                    putch( SPACE );
                    putch( BACKSPACE );
                    --p;
                    ++len;
                }
            } else if( len > 1 ) { // Other real chars...
                *p = ch;
                putch( ch );
                ++p;
                --len;
            } // Otherwise: len <= 1, can't type more.
        }
    }
    buff[1] = p - buff - 2;
    return( buff + 2 );
}
