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
#include "rtdata.h"
#include "iomode.h"
#include "fileacc.h"
#include "osver.h"
#include "rtcheck.h"
#include "errorno.h"
#include "seterrno.h"
#include "lseek.h"
#include "thread.h"

// pad with zero bytes
void static __padfile( int hid, long offset, long diff ) {
    int rc;
    unsigned amount;
    auto char buff[512];

    if( __lseek( hid, offset, SEEK_SET ) != offset ) {
        // run away
        return;
    }
    memset( buff, 0, 512 );
    do {
        amount = 512;
        if( diff < 512 ) amount = diff;
        rc = write( hid, buff, amount );
        if( rc != amount ) {
            // run away
            return;
        }
        diff -= amount;
    } while( diff != 0 );
}

_WCRTLINK int chsize( int hid, long size )
{
    long        curOffset;
    long        oldSize;
    long        rc;
    HANDLE      h;

    __handle_check( hid, -1 );
    h = __getOSHandle( hid );

    _AccessFileH( hid );
    curOffset = __lseek( hid, 0L, SEEK_CUR );     /* get current offset */

    // if windows 95 or win32s
    if( !WIN32_IS_NT ) {
        oldSize = __lseek( hid, 0L, SEEK_END );
    }
    /*
        Note that it is not an error to set the file pointer to a position
        beyond the end of the file. The size of the file does not increase
        until you call the SetEndOfFile, WriteFile, or WriteFileEx function. A
        write operation increases the size of the file to the file pointer
        position plus the size of the buffer written, leaving the intervening
        bytes uninitialized.
    */
    rc = SetFilePointer( h, size, 0, FILE_BEGIN );
    if( rc == -1L ) {
        _ReleaseFileH( hid );
        return( __set_errno_nt() );
    }
    if( !SetEndOfFile( h ) ) {
        _ReleaseFileH( hid );
        return( __set_errno_nt() );
    }

    // if windows 95 or win32s
    if( !WIN32_IS_NT ) {
        // if extending file length
        if( size > oldSize ) {
            __padfile( hid, oldSize, size-oldSize );
        }
    }

    if( curOffset > size ) curOffset = size;
    curOffset = __lseek( hid, curOffset, SEEK_SET );
    _ReleaseFileH( hid );
    if( curOffset == -1L ) {
        return( __set_errno_nt() );
    }
    return( 0 );
}
