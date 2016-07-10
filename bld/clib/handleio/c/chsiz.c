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
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include "rterrno.h"
#include "tinyio.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "lseek.h"
#include "msdos.h"

_WCRTLINK int chsize( int handle, long size )
{
    int         rc;
    long        current_offset, diff;
    unsigned    amount;
    auto char   buff[512];
    long        status;
    
    __handle_check( handle, -1 );
    
    current_offset = __lseek( handle, 0L, SEEK_CUR ); /* remember current */
    if( current_offset == -1 )
        return( -1 );
    diff = size - __lseek( handle, 0L, SEEK_END );
    rc = 0;
    if( diff > 0 ) {
        /*** Increase file size ***/
        memset( buff, 0, 512 );
        do {
            amount = 512;
            if( diff < 512 )
                amount = diff;
            rc = write( handle, buff, amount );
            if( rc != amount ) {
                if( _RWD_doserrno == E_access )
                    _RWD_errno = ENOSPC;
                rc = -1;
                break;
            }
            diff -= amount;
        } while( diff != 0 );
        
    } else {
        /*** Shrink the file ***/
        status = __lseek( handle, size, SEEK_SET );
        if( status != -1 ) {
            tiny_ret_t rc1;
            
            rc1 = TinyWrite( handle, buff, 0 );
            if( TINY_ERROR( rc1 ) ) {
                rc = __set_errno_dos( TINY_INFO( rc1 ) );
            }
        } else {
            rc = -1;
        }
        if( current_offset > size ) {
            current_offset = size;
        }
    }
    
    status = __lseek( handle, current_offset, SEEK_SET );
    if( status == -1)
        rc = -1;
    if( rc != -1 )
        rc = 0;
    return( rc );
}
