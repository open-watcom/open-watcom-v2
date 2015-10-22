/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom contributors. 
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
* Description:  Implementation of mlock/munlock for Linux
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include "linuxsys.h"
#include <sys/mman.h>

_WCRTLINK int mlock( const void *__addr, size_t __len )
{
    syscall_res res = sys_call2( SYS_mlock, (u_long)__addr, (u_long)__len );
    __syscall_return( int, res );
}

_WCRTLINK int munlock( const void *__addr, size_t __len )
{
    syscall_res res = sys_call2( SYS_mlock, (u_long)__addr, (u_long)__len );
    __syscall_return( int, res );
}

_WCRTLINK int mlockall( int __flags )
{
    syscall_res res = sys_call1( SYS_mlockall, (u_long)__flags );
    __syscall_return( int, res );
}

_WCRTLINK int munlockall( )
{
    syscall_res res = sys_call0( SYS_munlockall );
    __syscall_return( int, res );
}
