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
* Description:  qnx_fd_detach function implementation for QNX
*
****************************************************************************/


#include <string.h>
#include <sys/kernel.h>
#include <sys/proc_msg.h>
#include <sys/fd.h>
#include "rterrno.h"


int (qnx_fd_detach)( int fd )
{
    union sfd {
        struct _proc_fd         s;
        struct _proc_fd_reply1  r;
    }       msg;

    memset( &msg.s, 0, sizeof( msg.s ) );
    msg.s.type = _PROC_FD;
    msg.s.subtype = _PROC_SUB_DETACH;
    msg.s.fd = fd;

    if( Send( PROC_PID, &msg.s, &msg.r, sizeof( msg.s ), sizeof( msg.r ) ) == -1 ) {
        return( -1 );
    }

    if( msg.r.status ) {
        errno = msg.r.status;
        return( -1 );
    }

    return( 0 );
}

