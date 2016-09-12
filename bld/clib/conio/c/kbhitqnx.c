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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <unistd.h>
#include <sys/dev_msg.h>
#include <sys/kernel.h>
#include <termios.h>
#include "rterrno.h"
#include "rtdata.h"

_WCRTLINK int (kbhit)( void )
{
    union {
        struct _dev_waiting         s;
        struct _dev_waiting_reply   r;
    }   msg;
    struct termios  old, new;
    int     error;

    if( _RWD_cbyte != 0 )
        return( 1 );
    tcgetattr( STDIN_FILENO, &old );
    new = old;
    new.c_iflag &= ~(IXOFF | IXON);
    new.c_oflag &= ~OPOST;
    new.c_lflag &= ~(ECHO | ICANON | NOFLSH);
    new.c_lflag |= ISIG;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSADRAIN, &new );
    msg.s.type = _DEV_WAITING;
    msg.s.fd = STDIN_FILENO;
    error = Sendfd( msg.s.fd, &msg.s, &msg.r, sizeof( msg.s ), sizeof( msg.r ) );
    tcsetattr( STDIN_FILENO, TCSADRAIN, &old );
    if( error == -1 )
        return( 0 );
    if( msg.r.status != EOK || msg.r.nbytes == 0 )
        return( 0 );
    return( 1 );
}
