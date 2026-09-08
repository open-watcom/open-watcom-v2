/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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


#include <cassert>
#include <cstdarg>
#include <cstring>

#include "wresstr.hpp"

WEXPORT WResStr::WResStr( WResourceId str_id ) {
/**********************************************/

    str_id = str_id;
    assert( GUILoadString( str_id, _buffer, BUFFERSIZE ) );
}

const char * GetParm( char idxChar, std::va_list args ) {
/*******************************************************/

    const char * ret = NULL;
    unsigned     i;
    unsigned     index;

    assert( '0' <= idxChar && idxChar <= '9' );

    index = idxChar - '0' - 1;

    for( i = 0; i <= index; i += 1 ) {
        ret = va_arg( args, char * );
    }

    return( ret );
}

char * WEXPORT WResStr::formats( char * buf, unsigned len, WResourceId format_id, ... ) {
/***************************************************************************************/

    WResStr         format( format_id );
    const char *    fmts = format;
    const char *    parm;
    std::size_t     src = 0;
    std::size_t     dest = 0;
    bool            done;

    /* unused parameters */ (void)len;

    for( done = false; !done; ) {
        assert( dest < len );

        switch( fmts[src] ) {
        case '%':
            src++;
            if( fmts[src] == '%' ) {  // escaped % (%%)
                buf[dest] = fmts[src];
            } else {
                std::va_list     args;

                va_start( args, format_id );
                parm = GetParm( fmts[src], args );

                assert( std::strlen( parm ) + dest < len );

                std::strcpy( buf + dest, parm );
                dest += std::strlen( parm );
                dest--;                     // it will be incremented at bottom

                va_end( args );
            }
            break;
        case '\0':
            done = true;
            // fall through
        default:
            buf[dest] = fmts[src];
        }

        src++;
        dest++;
    }

    return( buf );
}
