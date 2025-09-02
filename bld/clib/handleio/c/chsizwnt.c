/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include <windows.h>
#include "rterrno.h"
#include "iomode.h"
#include "fileacc.h"
#include "osver.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "lseek.h"
#include "thread.h"


#define PAD_SIZE        512

// pad with zero bytes
void static _WCNEAR __padfile( int handle, long offset, long diff )
{
    int rc;
    unsigned amount;
    char buff[PAD_SIZE];

    if( __lseek( handle, offset, SEEK_SET ) != offset ) {
        // run away
        return;
    }
    memset( buff, 0, PAD_SIZE );
    do {
        amount = PAD_SIZE;
        if( diff < PAD_SIZE )
            amount = diff;
        rc = write( handle, buff, amount );
        if( rc != amount ) {
            // run away
            return;
        }
        diff -= amount;
    } while( diff != 0 );
}

_WCRTLINK int _chsize( int handle, long size )
{
    long        curOffset;
    long        oldSize;
    HANDLE      osfh;
    DWORD       error;

    __handle_check( handle, -1 );
    osfh = __getOSHandle( handle );

    _AccessFileH( handle );
    curOffset = __lseek( handle, 0L, SEEK_CUR );     /* get current offset */

    // if windows 95 or win32s
    if( !WIN32_IS_NT ) {
        oldSize = __lseek( handle, 0L, SEEK_END );
    }
    /*
        Note that it is not an error to set the file pointer to a position
        beyond the end of the file. The size of the file does not increase
        until you call the SetEndOfFile, WriteFile, or WriteFileEx function. A
        write operation increases the size of the file to the file pointer
        position plus the size of the buffer written, leaving the intervening
        bytes uninitialized.
    */
    if( SetFilePointer( osfh, size, 0, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
        // this might be OK so need to check error
        error = GetLastError();
        if( error != NO_ERROR ) {
            _ReleaseFileH( handle );
            return( __set_errno_dos( error ) );
        }
    }
    if( SetEndOfFile( osfh ) == 0 ) {
        _ReleaseFileH( handle );
        return( __set_errno_nt() );
    }

    // if windows 95 or win32s
    if( !WIN32_IS_NT ) {
        // if extending file length
        if( size > oldSize ) {
            __padfile( handle, oldSize, size-oldSize );
        }
    }

    if( curOffset > size )
        curOffset = size;
    curOffset = __lseek( handle, curOffset, SEEK_SET );
    _ReleaseFileH( handle );
    if( curOffset == -1L ) {
        return( __set_errno_nt() );
    }
    return( 0 );
}
