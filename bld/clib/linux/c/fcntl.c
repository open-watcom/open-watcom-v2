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
* Description:  Linux fcntl() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdarg.h>
#include <fcntl.h>
#include "rtdata.h"
#include "linuxsys.h"
#include "errorno.h"

_WCRTLINK int fcntl( int __fildes, int __cmd, ... )
{
    u_long      rest;
    va_list     args;
    u_long      res;

    va_start( args, __cmd );
    rest = va_arg( args, u_long );
    va_end( args );
    res = sys_call3( SYS_fcntl, (u_long)__fildes, (u_long)__cmd, rest );
    __syscall_return( int, res );
}
