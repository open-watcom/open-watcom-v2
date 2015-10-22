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
* Description:  Implementation of mmap/munmap for Linux
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include "linuxsys.h"
#include <stdint.h>
#include <sys/mman.h>

struct mmap_arg_struct {
        u_long address;
        u_long len;
        u_long prot;
        u_long flags;
        u_long fd;
        u_long offset;
};

_WCRTLINK void *mmap(void *__address, size_t __len, int __prot, 
                     int __flags, int __fd, off_t __offset)
{
syscall_res res;
struct mmap_arg_struct arg;
    
    arg.address = (u_long)__address;
    arg.len =     (u_long)__len;
    arg.prot =    (u_long)__prot; 
    arg.flags =   (u_long)__flags;
    arg.fd =      (u_long)__fd;
    arg.offset =  (u_long)__offset;

    /* We're using "old" mmap since syscall6 is a problem */
    res = sys_call1( SYS_mmap, (u_long)&arg);

    __syscall_return( void *, res );
}

_WCRTLINK int munmap(void *__address, size_t __len)
{
    syscall_res res = sys_call2( SYS_munmap, (u_long)__address, (u_long)__len );

    __syscall_return( int, res );
}
