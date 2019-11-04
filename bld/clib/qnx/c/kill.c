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
* Description:  kill function implementation for QNX
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <errno.h>
#include <sys/kernel.h>
#include <sys/proc_msg.h>


_WCRTLINK int  kill( pid_t pid, int signum )
{
#ifdef _M_I86
    union {
        struct _proc_signal         s;
        struct _proc_signal_reply   r;
    } msg;

    msg.s.type = _PROC_SIGNAL;
    msg.s.subtype = _SIGRAISE;
    msg.s.pid = pid;
    msg.s.signum = signum;

    if( Send( PROC_PID, &msg.s, &msg.r, sizeof(msg.s), sizeof(msg.r) ) == -1 ) {
        return( -1 );
    }

    if( msg.r.status != EOK ) {
        errno = msg.r.status;
        return( -1 );
    }
    return( 0 );
#else
    return( Kill( pid, signum ) );
#endif
}

