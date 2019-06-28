/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2018-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of sendfile() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <sys/types.h>
#include <sys/sendfile.h>
#include "linuxsys.h"


_WCRTLINK ssize_t sendfile( int to_fd, int from_fd, off_t *offset, size_t bytes )
{
    syscall_res res = sys_call4( SYS_sendfile, (u_long)to_fd, (u_long)from_fd, (u_long)offset, (u_long)bytes );
    __syscall_return( ssize_t, res );
}
