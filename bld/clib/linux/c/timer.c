/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom Contributors.
*    All Rights Reserved.
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
* Description:  Linux/POSIX timer implementations
*
* Author: J. Armstrong
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include "linuxsys.h"

/* Incomplete kernel sigevent type that provides "just enough"
 * to properly create a timer
 */
typedef struct ksigevent {
    union sigval    sigev_value;
    int             sigev_signo;
    int             sigev_notify;
    int             sigev_tid;
};

_WCRTLINK int timer_create( clockid_t __clk, struct sigevent *__sevp, timer_t *__tmr )
{
    syscall_res res;
    struct ksigevent ksev;
    int id;

    if( __tmr == NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    memset( &ksev, 0, sizeof( struct ksigevent ) );
    if( __sevp != NULL ) {
        ksev.sigev_value = __sevp->sigev_value;
        ksev.sigev_signo = __sevp->sigev_signo;
        ksev.sigev_notify = __sevp->sigev_notify;
    } else {
#ifdef SIGEV_SIGNAL
        ksev.sigev_notify = SIGEV_SIGNAL;
#endif
        ksev.sigev_signo = SIGALRM;
    }

    res = sys_call3( SYS_timer_create, 
                      (u_long)__clk, 
                      (u_long)&ksev, 
                      (u_long)&id );
    if( !__syscall_iserror( res ) ) {
        *__tmr = (timer_t)(intptr_t)id;
    }
    __syscall_return( int, res );
}


_WCRTLINK int timer_delete( timer_t __tmr )
{
    syscall_res res = sys_call1( SYS_timer_delete, (u_long)__tmr );
    __syscall_return( int, res );
}

_WCRTLINK int timer_getoverrun( timer_t __tmr )
{
    syscall_res res = sys_call1( SYS_timer_getoverrun, (u_long)__tmr );
    __syscall_return( int, res );
}

_WCRTLINK int timer_gettime( timer_t __tmr, struct itimerspec *__v )
{
    syscall_res res = sys_call2( SYS_timer_gettime, (u_long)__tmr, (u_long)__v );
    __syscall_return( int, res );
}

_WCRTLINK int timer_settime( timer_t __tmr, int flags, struct itimerspec *__new, struct itimerspec *__old )
{
    syscall_res res = sys_call4( SYS_timer_settime, (u_long)__tmr, (u_long)flags, (u_long)__new, (u_long)__old );
    __syscall_return( int, res );
}
