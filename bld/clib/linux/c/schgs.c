/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Retrieve or set the current scheduler.
*
****************************************************************************/

#include "variety.h"
#include <sched.h>
#include <sys/types.h>
#include "rtdata.h"
#include "linuxsys.h"
#include "errorno.h"

_WCRTLINK int sched_getscheduler( pid_t pid )
{
    u_long res = sys_call1( SYS_sched_getscheduler, pid );
    __syscall_return( int, res );
}

_WCRTLINK int sched_setscheduler( pid_t pid, int policy,
                                  const struct sched_param *sp )
{
    u_long res = sys_call3( SYS_sched_getscheduler, pid, policy, (u_long)sp );
    __syscall_return( int, res );
}
