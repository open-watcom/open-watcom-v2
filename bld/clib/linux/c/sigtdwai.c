/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of sigtimedwait() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include "rtdata.h"
#include "linuxsys.h"

_WCRTLINK int sigtimedwait( const sigset_t *__set, siginfo_t *__info,
                            const struct timespec *__timeout )

{
    u_long  res = sys_call4( SYS_rt_sigtimedwait, (u_long)__set, (u_long)__info, (u_long)__timeout, sizeof( sigset_t ) );
    if( res >= -125 ) {
        _RWD_errno = -res;
        res = -1;
    }
    /* glibc does this, but no other libc */
//    if ((res != -1) && __info && (__info->si_code == SI_TKILL)) {
//        __info->si_code = SI_USER;
//    }
    return( (int)res );
}
