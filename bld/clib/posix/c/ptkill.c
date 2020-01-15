/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  POSIX thread kill signal implementation
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "thread.h"
#include "rterrno.h"
#include "_ptint.h"
#ifdef __LINUX__
#include "tgkill.h"
#endif


_WCRTLINK int pthread_kill(pthread_t thread, int sig)
{
    pid_t tpid;
    pid_t ppid;

#ifndef __LINUX__

    /* unused parameters */ (void)sig;

#endif
    tpid = __get_thread_id( thread );
    ppid = getpid();
    if(tpid != 0 && ppid != 0) {
#ifdef __LINUX__
        return( __tgkill(ppid, tpid, sig) );
#else
        _RWD_errno = ENOSYS;
        return( -1 );
#endif
    }

    _RWD_errno = EINVAL;

    return( -1 );
}

