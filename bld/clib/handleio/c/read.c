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
#include <stddef.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined(__OS2__)
    #define INCL_DOSMEMMGR
    #include <wos2.h>
#elif defined( __WINDOWS__ )
    #include <windows.h>
    #include "tinyio.h"
#elif defined( __DOS__ )
    #include "tinyio.h"
#endif
#include "iomode.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "defwin.h"
#include "lseek.h"
#include "thread.h"


#if defined(__WINDOWS_386__)
static int __read( int handle, void *buf, unsigned len )
#else
_WCRTLINK int read( int handle, void *buf, unsigned len )
#endif
{
    unsigned    read_len;
    unsigned    total_len;
    unsigned    reduce_idx;
    unsigned    finish_idx;
    unsigned    iomode_flags;
    char        *buffer = buf;
#if defined(__NT__)
    DWORD       amount_read;
    DWORD       error;
    HANDLE      osfh;
#elif defined(__OS2__)
    OS_UINT     amount_read;
    APIRET      rc;
#else
    unsigned    amount_read;
    tiny_ret_t  rc;
#endif
#ifdef DEFAULT_WINDOWING
    LPWDATA     res;
#endif

    __handle_check( handle, -1 );
    __ChkTTYIOMode( handle );
    iomode_flags = __GetIOMode( handle );
    if( iomode_flags == 0 ) {
#if defined( __WINDOWS__ ) || defined( __WINDOWS_386__ )
        return( _lread( handle, buffer, len ) );
#else
        lib_set_errno( EBADF );
        return( -1 );
#endif
    }
    if( (iomode_flags & _READ) == 0 ) {
        lib_set_errno( EACCES );     /* changed from EBADF to EACCES 23-feb-89 */
        return( -1 );
    }
#ifdef __NT__
    osfh = __getOSHandle( handle );
#endif
    if( iomode_flags & _BINARY ) {       /* if binary mode */
#ifdef DEFAULT_WINDOWING
        if( _WindowsStdin != NULL
          && (res = _WindowsIsWindowedHandle( handle )) != NULL ) {
            total_len = _WindowsStdin( res, buffer, len );
        } else {
#endif
#if defined(__NT__)
            if( ReadFile( osfh, buffer, len, &amount_read, NULL ) == 0 ) {
                error = GetLastError();
                if( error == ERROR_BROKEN_PIPE )
                    return( amount_read );
                return( __set_errno_dos( error ) );
            }
#elif defined( __OS2__ )
            rc = DosRead( handle, buffer, len, &amount_read );
            if( rc ) {
                return( __set_errno_dos( rc ) );
            }
#else
            rc = TinyRead( handle, buffer, len );
            if( TINY_ERROR( rc ) ) {
                return( __set_errno_dos( TINY_INFO( rc ) ) );
            }
            amount_read = TINY_LINFO( rc );
#endif
            total_len = amount_read;
#ifdef DEFAULT_WINDOWING
        }
#endif
    } else {
        _AccessFileH( handle );
        total_len = 0;
        read_len = len;
        do {
#ifdef DEFAULT_WINDOWING
            if( _WindowsStdin != NULL
              && (res = _WindowsIsWindowedHandle( handle )) != NULL ) {
                amount_read = _WindowsStdin( res, buffer, read_len );
            } else {
#endif
#if defined(__NT__)
                if( ReadFile( osfh, buffer, read_len, &amount_read, NULL ) == 0 ) {
                    _ReleaseFileH( handle );
                    error = GetLastError();
                    if( error == ERROR_BROKEN_PIPE )
                        return( total_len );
                    return( __set_errno_dos( error ) );
                }
#elif defined( __OS2__ )
                rc = DosRead( handle, buffer, read_len, &amount_read );
                if( rc ) {
                    _ReleaseFileH( handle );
                    return( __set_errno_dos( rc ) );
                }
#else
                rc = TinyRead( handle, buffer, read_len );
                if( TINY_ERROR( rc ) ) {
                    _ReleaseFileH( handle );
                    return( __set_errno_dos( TINY_INFO( rc ) ) );
                }
                amount_read = TINY_LINFO( rc );
#endif
#ifdef DEFAULT_WINDOWING
            }
#endif
            if( amount_read == 0 ) {                    /* EOF */
                break;
            }
            finish_idx = 0;
            for( reduce_idx = 0; reduce_idx < amount_read; ++reduce_idx ) {
                if( buffer[reduce_idx] == 0x1a ) {    /* EOF */
                    __lseek( handle,
                           ((long)reduce_idx - (long)amount_read) + 1L,
                           SEEK_CUR );
                    total_len += finish_idx;
                    _ReleaseFileH( handle );
                    return( total_len );
                }
                if( buffer[reduce_idx] != '\r' ) {
                    buffer[finish_idx++] = buffer[reduce_idx];
                }
            }
            total_len += finish_idx;
            buffer += finish_idx;
            read_len -= finish_idx;
            if( iomode_flags & _ISTTY ) {
                break;
            }
        } while( read_len != 0 );
        _ReleaseFileH( handle );
    }
    return( total_len );
}

#if defined( __WINDOWS_386__ )

#define MAXBUFF 0x8000

_WCRTLINK int read( int handle, void *buffer, unsigned len )
{
    unsigned    total;
    unsigned    amount;
    unsigned    readamt;

    __handle_check( handle, -1 );
    total = 0;
    amount = MAXBUFF;
    while( len > 0 ) {
        if( len < MAXBUFF )
            amount = len;
        readamt = __read( handle, buffer, amount );
        if( (int)readamt == -1 )
            return( -1 );
        total += readamt;
        if( readamt != amount )
            break;
        len -= readamt;
        buffer = (char *)buffer + readamt;
    }
    return( total );
}
#endif
