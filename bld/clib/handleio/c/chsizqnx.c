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
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "rtdata.h"
#include "iomode.h"
#include "rtcheck.h"

_WCRTLINK int chsize( int handle, long size )
{
    long        current_offset;
    long        diff;
    int         flags;
    unsigned    amount;
    char        buff[512];

    __handle_check( handle, -1 );

    current_offset = lseek( handle, 0L, SEEK_CUR ); /* remember current */
    if( current_offset == -1 ) return( -1 );
    diff = size - lseek( handle, 0L, SEEK_END );
    if( diff > 0 ) {
        /* pad file */
        if( (flags = fcntl( handle, F_GETFL )) == -1 ) {
            return( -1 );
        }
        if( flags & O_APPEND ) {
            memset( buff, 0, sizeof( buff ) );
            while( diff > sizeof( buff ) ) {
                amount = write( handle, buff, sizeof( buff ) );
                switch( amount ) {
                case -1:
                    return( -1 );
                case 0:
                    _RWD_errno = ENOSPC;
                    return( -1 );
                }
                diff -= amount;
            }
            amount = diff;
        } else {
            if( lseek( handle, (size-1), SEEK_SET ) != (size-1) ) {
                return( -1 );
            }
            buff[0] = 0;
            amount = 1;
        }
        if( write( handle, &buff, amount ) != amount ) return( -1 );
    } else {
        /* truncate file */
        if( ltrunc( handle, size, SEEK_SET ) != size ) return( -1 );
        if( current_offset > size ) current_offset = size;
    }
    lseek( handle, current_offset, SEEK_SET );
    return( 0 );
}
