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
#if defined(__OS2__)
    #define INCL_DOSMEMMGR
#endif
#include "rterrno.h"
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    #include "tinyio.h"
#endif
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "defwin.h"
#include "qread.h"
#include "thread.h"

#ifdef __WINDOWS_386__

#define MAXBUFF 0x8000

static tiny_ret_t __TinyRead( int handle, char *buffer, unsigned len )
{
    unsigned    total = 0;
    unsigned    readamt;
    tiny_ret_t  rc;

    while( len > 0 ) {

        if( len > MAXBUFF ) {
            readamt = MAXBUFF;
        } else {
            readamt = len;
        }
        rc = TinyRead( handle, buffer, readamt );
        if( TINY_ERROR( rc ) )
            return( rc );
        total += TINY_LINFO( rc );
        if( TINY_LINFO( rc ) != readamt )
            return( total );

        len -= readamt;
        buffer += readamt;

    }
    return( total );
}
#endif



int __qread( int handle, void *buffer, unsigned len )
{
#if defined( __NT__ )
    DWORD           amount_read;
#elif defined(__OS2__)
    OS_UINT         amount_read;
    APIRET          rc;
#else
    unsigned        amount_read;
    tiny_ret_t      rc;
#endif

    __handle_check( handle, -1 );
#ifdef DEFAULT_WINDOWING
    if( _WindowsStdin != 0 ) {
        LPWDATA res;

        res = _WindowsIsWindowedHandle( handle );
        if( res ) {
            int rt;
            rt = _WindowsStdin( res, buffer, len );
            return( rt );
        }
    }
#endif
#if defined(__NT__)
    if( !ReadFile( __getOSHandle( handle ), buffer, len, &amount_read, NULL ) ) {
        DWORD       err;
        err = GetLastError();
        __set_errno_dos( err );
        if( err != ERROR_BROKEN_PIPE || amount_read != 0 ) {
            return( -1 );
        }
    }
#elif defined(__OS2__)
    rc = DosRead( handle, buffer, len, &amount_read );
    if( rc ) {
        return( __set_errno_dos( rc ) );
    }
#else
  #if defined( __WINDOWS_386__ )
    rc = __TinyRead( handle, buffer, len );
  #else
    rc = TinyRead( handle, buffer, len );
  #endif
    if( TINY_ERROR( rc ) ) {
        return( __set_errno_dos( TINY_INFO( rc ) ) );
    }
    amount_read = TINY_LINFO( rc );
#endif
    return( amount_read );
}
