/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of POSIX tcsetattr
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <termios.h>
#ifdef __LINUX__
    #include <sys/ioctl.h>
#else
#endif
#include "rterrno.h"
#include "seterrno.h"
#include "thread.h"


_WCRTLINK int tcsetattr( int fd, int actions, const struct termios *termios_p )
{
#ifdef __LINUX__
    int cmd;

    if( termios_p == NULL ) {
        return( __set_EINVAL() );
    }

    switch( actions ) {
    case TCSANOW:
        cmd = TCSETS;
        break;
    case TCSADRAIN:
        cmd = TCSETSW;
        break;
    case TCSAFLUSH:
        cmd = TCSETSF;
        break;
    default:
        return( __set_EINVAL() );
    }

    return( ioctl( fd, cmd, termios_p ) );
#else
    return( __set_EINVAL() );
#endif
}
