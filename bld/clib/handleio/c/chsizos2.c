/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "seterrno.h"
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <wos2.h>
#include "fileacc.h"
#include "iomode.h"
#include "rtcheck.h"
#include "lseek.h"
#include "thread.h"


_WCRTLINK int _chsize( int handle, long size )
{
    long        curOffset, endOffset;
    char        buf[512];
    long        diff;
    unsigned    amount;
    int         retCode;

    __handle_check( handle, -1 );

    /*** Prepare to change the size ***/
    _AccessFileH( handle );
    curOffset = __lseek( handle, 0L, SEEK_CUR );  /* get current offset */
    if( curOffset == -1 ) {
        _ReleaseFileH( handle );
        return( -1 );
    }
    endOffset = __lseek( handle, 0L, SEEK_END );  /* get file size */
    if( endOffset == -1 ) {
        _ReleaseFileH( handle );
        return( -1 );
    }

    if( size > endOffset ) {
        /*** Increase file size ***/
        diff = size - endOffset;                /* number of bytes to pad */
        memset( buf, 0, 512 );                  /* zero buffer */
        do {
            if( diff >= 512 ) {
                amount = 512;
            } else {
                amount = (unsigned)diff;
            }
            retCode = write( handle, buf, amount );
            if( retCode != amount ) {
                lib_set_errno( ENOSPC );
                retCode = -1;
                break;
            }
            diff -= amount;
        } while( diff != 0 );
        if( retCode != -1 ) {
            retCode = 0;
        }
    } else {
        /*** Shrink file ***/
        retCode = DosNewSize( handle, size );
        if( retCode != 0 ) {
            retCode = __set_errno_dos( retCode );
        }
        if( curOffset > size ) {
            curOffset = size;
        }
    }

    /*** Clean up and go home ***/
    curOffset = __lseek( handle, curOffset, SEEK_SET );
    if( curOffset == -1 )
        retCode = -1;
    _ReleaseFileH( handle );
    return( retCode );
}
