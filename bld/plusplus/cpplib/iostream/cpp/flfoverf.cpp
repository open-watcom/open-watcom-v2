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


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %     Copyright (C) 1992, by WATCOM International Inc.  All rights    %
// %     reserved.  No part of this software may be reproduced or        %
// %     used in any form or by any means - graphic, electronic or       %
// %     mechanical, including photocopying, recording, taping or        %
// %     information storage and retrieval systems - except with the     %
// %     written permission of WATCOM International Inc.                 %
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//  Modified    By              Reason
//  ========    ==              ======
//  92/02/19    Steve McDowell  Initial implementation.
//  92/02/28    ...             Modified to delay allocation of buffers
//                              until overflow/underflow called.
//  92/09/08    Greg Bentz      Cleanup.
//  93/03/22    Greg Bentz      modify filebuf::open() and filebuf::attach()
//                              to assume ios::binary unless ios::text is
//                              found in the fstat() so that QNX is supported.
//  93/07/22    Greg Bentz      Make sure overflow() function sets up the
//                              put area
//  93/09/13    Greg Bentz      filebuf::~filebuf() must close if !__attached
//  93/10/15    Greg Bentz      let __plusplus_open() determine if default
//                              file mode is TEXT or BINARY
//  93/10/15    Raymond Tang    Modify filebuf::open() to return NULL if both
//                              ios::noreplace and ios::nocreate are specified
//  93/10/22    Raymond Tang    Split into separate files.
//  93/11/09    Greg Bentz      - modify behaviour of ios::app to always
//                                write at end of file without affecting the
//                                current file position
//                              - make sure that the get area is discarded
//                                in the right order
//  94/04/06    Greg Bentz      combine header files
//  94/10/07    Greg Bentz      ::write may not dump all requested data

#ifdef __SW_FH
#include "iost.h"
#else
#include "variety.h"
#include <unistd.h>
#include <string.h>
#include <fstream.h>
#include <limits.h>
#endif
#include "ioutil.h"

int filebuf::overflow( int c ) {
/******************************/
// Handle allocating a buffer, if required.
// Handle overflow of the output streambuf buffer.
// Take the contents of the buffer and send them to the POSIX device
// found in handle.
// Also send the character "c", unless it's EOF.

    unsigned int    waiting;
    int             written;
    streamoff       offset;

    __lock_it( __b_lock );

    // Flush any input waiting in the buffer:
    if( in_avail() > 0 ) {
        // note that sync() will discard get area
        if( sync() == EOF ) {
            return( EOF );
        }
    } else {
        // discard get area
        setg( NULL, NULL, NULL );
    }

    // Try to allocate a buffer:
    if( base() == NULL ) {
        if( allocate() == EOF ) {
            return( EOF );
        }
        if( base() == NULL ) {
            if( c != EOF ) {
                char charbuf = (char)c;
                written = ::write( fd(), &charbuf, sizeof( charbuf ) );
                return( written == sizeof( charbuf ) ? __NOT_EOF : EOF );
            }
            return( __NOT_EOF );
        } else {
            setp( base(), ebuf() );
        }
    } else if( pptr() <= pbase() ) {
        setp( base(), ebuf() );
    }

    // if appending, remember current offset and seek to end
    if( __file_mode & ios::app ) {
        offset = tell( fd() );
        if( offset < 0 ) {
            return( EOF );
        }
        if( lseek( fd(), 0, ios::end ) < 0 ) {
            return( EOF );
        }
    }

    // Now write the output:
    if( (c != EOF) && (pptr() < epptr()) ) {
        *(pptr()) = (char)c;
        pbump( 1 );
        c = EOF;
    }
    waiting = (__huge_ptr_int)(pptr() - pbase());
    while( waiting > 0 ) {
        written = ::write( fd(), pbase(), (waiting>INT_MAX?INT_MAX:waiting) );
        if( written == -1 ) {
            return( EOF );
        } else if( written == 0 ) {
            break;
        }
        waiting -= written;
        if( waiting > 0 ) {
            ::memmove( pbase(), pbase() + written, waiting );
            setp( pbase(), epptr() );
            pbump( waiting );
        } else {
            setp( pbase(), epptr() );
        }
    }
    if( c != EOF ) {
        if( pptr() < epptr() ) {
            *(pptr()) = (char)c;
            pbump( 1 );
        } else {
            return( EOF );
        }
    }

    // if appending, restore current offset
    if( __file_mode & ios::app ) {
        if( lseek( fd(), offset, ios::beg ) < 0 ) {
            return( EOF );
        }
    }

    if( waiting ) {
        return( EOF );
    }
    return( __NOT_EOF );
}
