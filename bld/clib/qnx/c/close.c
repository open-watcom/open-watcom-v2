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
* Description:  close function implementation for QNX
*
****************************************************************************/


#include <unistd.h>
#include <sys/kernel.h>
#include <sys/io_msg.h>
#include <sys/fd.h>
#include "rterrno.h"

int (close)( int fd )
{
    union _close {
        struct _io_close        s;
        struct _io_close_reply  r;
    }       msg;
    struct _mxfer_entry mx_entry[1];
    int                 rc;

    /*
     *  Set up the message header.
     */
    msg.s.type = _IO_CLOSE;
    msg.s.fd   = fd;
    msg.s.zero = 0;
    _setmx( &mx_entry[0], &msg, sizeof( msg ) );
    rc = -1;
    if( Sendfdmx( fd, 1, 1, &mx_entry, &mx_entry ) != -1 ) {
        if( msg.r.status == EOK ) {
            rc = EOK;
        } else {
            errno = msg.r.status;
        }
    }
    qnx_fd_detach( fd );
    return( rc );
}

