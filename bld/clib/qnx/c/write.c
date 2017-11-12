/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  write function implementation for QNX
*
****************************************************************************/


#include <unistd.h>
#include <sys/kernel.h>
#include <sys/io_msg.h>
#include "rterrno.h"


int (write)( int fd, const void *buf, unsigned nbytes )
{
    register unsigned   count = 0;
    union _write {
        struct _io_write        s;
        struct _io_write_reply  r;
    }       msg;
    struct _mxfer_entry mx_entry[2];

    do {
        /*
         *  Set up the message header.
         */
        msg.s.type    = _IO_WRITE;
        msg.s.fd      = fd;
        msg.s.nbytes  = nbytes - count;
        msg.s.zero    = 0;

        _setmx( &mx_entry[0], &msg, sizeof( struct _io_write ) - sizeof( msg.s.data ) );

        /*
         *  Setup the message data description.
         */
        _setmx( &mx_entry[1], (const char *)buf + count, nbytes - count );

        if( Sendfdmx( fd, 2, 1, &mx_entry, &mx_entry ) == -1 ) {
            errno = EINTR;
            return( -1 );
        }

        if( msg.r.status != EOK && msg.r.status != EMORE ) {
            errno = msg.r.status;
            return( -1 );
        }

        count += msg.r.nbytes;
    } while( count < nbytes && msg.r.status == EMORE );

    return( count );
}

