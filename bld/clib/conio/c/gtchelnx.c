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
* Description:  Implementation of getche() for Linux.
*
****************************************************************************/


#include "variety.h"
#include <conio.h>
#include <unistd.h>
#include <termios.h>
#include "rtdata.h"

_WCRTLINK int (getche)( void )
{
    unsigned char   buf[ 1 ];
    int             c;
    struct termios  old, new;

    c = _RWD_cbyte;
    _RWD_cbyte = 0;
    if( c == 0 ) {
        tcgetattr( STDIN_FILENO, &old );
        new = old;
        new.c_iflag &= ~(IXOFF | IXON);
        new.c_lflag &= ~(ICANON | NOFLSH);
        new.c_lflag |= ISIG | ECHO;
        new.c_cc[VMIN] = 1;
        new.c_cc[VTIME] = 0;
        tcsetattr( STDIN_FILENO, TCSADRAIN, &new );
        read( STDIN_FILENO, buf, 1 );  /* must be read with no echo */
        c = buf[0];
        tcsetattr( STDIN_FILENO, TCSADRAIN, &old );
    }
    return( c );
}
