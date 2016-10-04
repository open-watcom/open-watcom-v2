/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2016 Open Watcom contributors. 
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
* Description:  Implementation of termios tcgetattr for Linux
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <sys/ioctl.h>
#include <termios.h>
#include "linuxsys.h"


_WCRTLINK int tcgetattr( int fd, struct termios *t )
{
syscall_res res;

    if(t == NULL) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    /* OW's struct termios is the same (basically) as the kernel's
     * termios
     */
    res = sys_call3( SYS_ioctl, (u_long)fd, (u_long)TCGETS, (u_long)t);
    __syscall_return( int, res );
}
