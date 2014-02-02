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
* Description:  Linux sigaction() implementation.
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <errno.h>
#include "linuxsys.h"

_WCRTLINK int sigaction( int __signum, const struct sigaction *__act,
                            struct sigaction *__oldact )
{
    /* given the sigaction layout we must use rt_sigaction
       this requires Linux kernel 2.2 or higher (probably not
       a big deal nowadays) */
    u_long res = sys_call4( SYS_rt_sigaction, __signum, (u_long)__act,
                           (u_long)__oldact, sizeof( sigset_t ) );
    __syscall_return( int, res );
}
