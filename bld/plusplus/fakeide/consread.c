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


// CONSREAD.C -- read from console
//
// This reads characters from the console into a string.
//
// 95/01/26 -- J.W.Welch        -- defined

#include <dos.h>
#include <conio.h>
#include <stdio.h>


unsigned ConsoleRead            // READ FROM CONSOLE
    ( char* buffer              // - buffer
    , unsigned bsize )          // - buffer size
{
    unsigned index;             // - current position in buffer

    fflush( stdout );
    index = 0;
    for( ; ; ) {
        if( kbhit() ) {
            char ch = getche();
            switch( ch ) {
              case '\r' :
                putch( '\n' );
                break;
              case EOF :
                getche();
                continue;
              case '\b' :
                if( index > 0 ) {
                    --index;
                    putch( ' ' );
                    putch( '\b' );
                }
                continue;
              default :
                buffer[ index ] = ch;
                ++index;
                if( index == bsize - 1 ) break;
                continue;
            }
            break;
        } else {
            delay( 50 );
        }
    }
    buffer[ index ] = '\0';
    return index;
}


unsigned ConsoleReadPrefixed    // READ FROM CONSOLE, PREFIXED
    ( char* buffer              // - buffer
    , unsigned bsize            // - buffer size
    , const char* prefix )      // - prefix
{
    fputs( prefix, stdout );
    return ConsoleRead( buffer, bsize );
}
