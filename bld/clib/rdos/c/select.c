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
* Description:  RDOS select() implementation.
*
****************************************************************************/


#include "variety.h"
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include "liballoc.h"
#include "rdos.h"

_WCRTLINK int select( int __width, fd_set * __readfds, fd_set * __writefds, fd_set * __exceptfds, struct timeval * __timeout )
{
    int count;
    int timeout = 0x7FFFFFFF;
    char masks[3 * FD_SETSIZE / 8];
    char *ptr;
    int size;
    int width = __width;

    if( width > FD_SETSIZE) {
        width = FD_SETSIZE;
        size = FD_SETSIZE / 8;
    } else
        size = ( width - 1 ) / 8 + 1;

    if( __timeout ) {
        timeout = __timeout->tv_usec  / 1000;
        timeout += __timeout->tv_sec * 1000;
    }

    ptr = masks;
    if( __readfds )
        memcpy(ptr, __readfds->fds_bits, size);
    else
        memset(ptr, 0, size);

    ptr += size;
    if( __writefds )
        memcpy(ptr, __writefds->fds_bits, size);
    else
        memset(ptr, 0, size);

    ptr += size;
    if( __exceptfds )
        memcpy(ptr, __exceptfds->fds_bits, size);
    else
        memset(ptr, 0, size);

    count = RdosSelect( masks, width, timeout );

    ptr = masks;
    if( __readfds )
        memcpy(__readfds->fds_bits, ptr, size);

    ptr += size;
    if( __writefds )
        memcpy(__writefds->fds_bits, ptr, size);

    ptr += size;
    if( __exceptfds )
        memcpy(__exceptfds->fds_bits, ptr, size);

    return( count );
}
