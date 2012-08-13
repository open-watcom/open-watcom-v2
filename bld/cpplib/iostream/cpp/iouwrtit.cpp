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

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <iostream>
#endif
#include "ioutil.h"
#include "lock.h"

std::ios::iostate __WATCOM_ios::writeitem( std::ostream &ostrm,
                                           char const *buffer,
                                           int size,
                                           int fill_offset ) {

    int           put_size;
    long          format_flags;
    int           padding_size;
    char         *padding_buffer;
    int           ret;
    streambuf    *sb;
    std::ios::iostate  state;

    state = std::ios::goodbit;
    __lock_it( ostrm.__i_lock );
    padding_buffer = NULL;
    padding_size   = ostrm.width() - size;
    if( padding_size > 0 ) {
        padding_buffer = new char[padding_size];
        if( padding_buffer == NULL ) {
            padding_size = 0;
        } else {
            ::memset( padding_buffer, ostrm.fill(), padding_size );
        }
    }
    format_flags = ostrm.flags();
    sb           = ostrm.rdbuf();
    __lock_it( sb->__b_lock );
    if( format_flags & std::ios::left ) {
        put_size = size;
        ret      = sb->sputn( buffer, put_size );
        if( ret == put_size ) {
            if( padding_size > 0 ) {
                put_size = padding_size;
                ret      = sb->sputn( padding_buffer, put_size );
            }
        }
    } else if( format_flags & std::ios::internal ) {
        ret = put_size;
        if( fill_offset > 0 ) {
            put_size = fill_offset;
            ret      = sb->sputn( buffer, put_size );
        }
        if( ret == put_size ) {
            if( padding_size > 0 ) {
                put_size = padding_size;
                ret      = sb->sputn( padding_buffer, put_size );
            }
        }
        if( ret == put_size ) {
            put_size = size - fill_offset;
            ret      = sb->sputn( buffer + fill_offset, put_size );
        }
    } else {
        ret = put_size;
        if( padding_size > 0 ) {
            put_size = padding_size;
            ret      = sb->sputn( padding_buffer, put_size );
        }
        if( ret == put_size ) {
            put_size = size;
            ret      = sb->sputn( buffer, put_size );
        }
    }
    if( ret != put_size ) {
        state |= std::ios::failbit;
    }
    if( padding_buffer != NULL ) {
        delete [] padding_buffer;
    }
    ostrm.width( 0 );
    return( state );
}
