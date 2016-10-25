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
* Description:  Implementations of termios speed functions for Linux
*
* Author: J. Armstrong
*
****************************************************************************/

#include "variety.h"
#include <termios.h>
#include "rterrno.h"
#include "thread.h"


_WCRTLINK speed_t cfgetispeed(const struct termios *tios)
{
    return tios->c_ispeed;
}

_WCRTLINK speed_t cfgetospeed(const struct termios *tios)
{
    return( tios->c_ospeed );
}

static int __valid_speed(speed_t speed)
{
    return ( speed == B0     ||
             speed == B50    ||
             speed == B75    ||
             speed == B110   ||
             speed == B134   ||
             speed == B150   ||
             speed == B200   ||
             speed == B300   ||
             speed == B600   ||
             speed == B1200  ||
             speed == B1800  ||
             speed == B2400  ||
             speed == B4800  ||
             speed == B9600  ||
             speed == B19200 ||
             speed == B38400 );
}

_WCRTLINK int cfsetispeed(struct termios *tios, speed_t speed)
{
    if(tios == NULL || !__valid_speed( speed )) {
        _RWD_errno = EINVAL;
        return( -1 );
    } 

    tios->c_ispeed = speed;

    return( 0 );
}

_WCRTLINK int cfsetospeed(struct termios *tios, speed_t speed)
{
    if(tios == NULL || !__valid_speed( speed )) {
        _RWD_errno = EINVAL;
        return( -1 );
    } 

    tios->c_ospeed = speed;

    return( 0 );
}
