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


// EXC.CPP -- simple exceptions class
//
// 95/01/30 -- J.W.Welch        -- defined


#include <stdarg.h>
#include <string.h>

#include "Exc.h"

//-----------------------------------------------------------------------
// Exc implementation
//-----------------------------------------------------------------------


Exc::Exc                        // CONSTRUCTOR
    ( char const* prefix        // - prefix
    , ... )                     // - NULL-terminated message list
    : _msg( 0 )
{
    char buffer[1024];          // - construction buffer
    char *bptr;                 // - buffer ptr
    va_list list;               // - list of stuff
    char const* text;           // - text to be inserted

    bptr = buffer;
    va_start( list, prefix );
    text = prefix;
    for( ; ; ) {
        bptr = strcpy( bptr, text );
        bptr += strlen( bptr );
        text = va_arg( list, char const * );
        if( 0 == text ) break;
        *bptr++ = ' ';
    }
    va_end( list );
    _msg = Str( buffer, bptr - buffer );
}
