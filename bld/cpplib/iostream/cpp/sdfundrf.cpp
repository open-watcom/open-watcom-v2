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
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdiobuf.h>
#endif
#include "ioutil.h"
#include "sdfhdr.h"

// Handle allocating a buffer, if required. Handle filling the get area
// of the streambuf. Read more stuff from the input device. Return the
// first character read.

int stdiobuf::underflow() {

    char *ptr;
    int   len;

    __lock_it( __b_lock );

    // Flush any output waiting in the buffer:
    if( out_waiting() > 0 ) {
        if( sync() == EOF ) {
            return( EOF );
        }
    }
    // discard put area
    setp( NULL, NULL );

    // Try to allocate a buffer:
    if( base() == NULL ) {
        if( allocate() == EOF ) {
            return( EOF );
        }
        if( base() == NULL ) {
            // unbuffered special case
            ptr = __unbuffered_get_area + DEFAULT_PUTBACK_SIZE;
            setg( __unbuffered_get_area, ptr, ptr );
            int ch;
            ch = fgetc( __file_pointer );
            if( ch == EOF ) {
                return( EOF );
            } else {
                *egptr() = (char)ch;
            }
            setg( eback(), gptr(), egptr() + 1 );
            return( *gptr() );
        } else {
            ptr = base() + DEFAULT_PUTBACK_SIZE;
            setg( base(), ptr, ptr );
        }
    } else if( gptr() >= egptr() ) {
        ptr = base() + DEFAULT_PUTBACK_SIZE;
        setg( base(), ptr, ptr );
    }

    len = (__huge_ptr_int)(ebuf() - egptr());
    if( len > 0 ) {
        len = 1;
        int ch;
        ch = fgetc( __file_pointer );
        if( ch == EOF ) {
            return( EOF );
        } else {
            *egptr() = (char)ch;
        }
    } else {
        len = 0;
    }
    setg( eback(), gptr(), egptr() + len );
    return( gptr() < egptr() ? *gptr() : EOF );
}
