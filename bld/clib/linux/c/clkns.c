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
* Description:  Linux/POSIX clock_nanosleep implementation via syscall
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <time.h>
#include "linuxsys.h"

_WCRTLINK int clock_nanosleep( clockid_t __clk, int __flags, 
                               const struct timespec *__req, 
                               struct timespec *__rmdr )
{
    u_long ures;
    long res;
    ures = sys_call4( SYS_clock_nanosleep,
                      (u_long)__clk, 
                      (u_long)__flags, 
                      (u_long)__req,
                      (u_long)__rmdr );
    
    res = (long)(-ures);
    if(res != 0) {
        _RWD_errno = res;
    }
    
    __syscall_return( int, ures );
}
