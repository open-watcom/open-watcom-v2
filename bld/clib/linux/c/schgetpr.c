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
* Description:  Retrieve the max and min priorities allowed by the Linux
*               scheduler.
*
****************************************************************************/

#include "variety.h"
#include <sched.h>
#include "rtdata.h"
#include "linuxsys.h"

_WCRTLINK int sched_get_priority_max( int policy )
{
    u_long res = sys_call1( SYS_sched_get_priority_max, policy );
    __syscall_return( int, res );
}

_WCRTLINK int sched_get_priority_min( int policy )
{
    u_long res = sys_call1( SYS_sched_get_priority_min, policy );
    __syscall_return( int, res );
}
