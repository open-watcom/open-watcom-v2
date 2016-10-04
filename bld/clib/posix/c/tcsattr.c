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
* Description:  Implementation for tcsetattr() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <termios.h>
#include <sys/ioctl.h>
#include "rterrno.h"
#include "thread.h"


_WCRTLINK int tcsetattr( int __fd, int __optional_actions, const struct termios *__termios_p )
{
    switch(__optional_actions) {
    case TCSANOW:
        return( ioctl( __fd, TCSETS, __termios_p ) );
    case TCSADRAIN:
        return( ioctl( __fd, TCSETSW, __termios_p ) );
    case TCSAFLUSH:
        return( ioctl( __fd, TCSETSF, __termios_p ) );
    default:
        _RWD_errno = EINVAL;
        return( -1 );
    }
}

