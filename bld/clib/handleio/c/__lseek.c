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
*    governing rights and limitations under the License.*
*  ========================================================================
*
* Description:  Low level __lseek() without file extend.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined( __OS2__ )
    #define INCL_LONGLONG
    #include <wos2.h>
    #include "os2fil64.h"
#elif defined( __DOS__ ) || defined( __WINDOWS__ )
    #include "tinyio.h"
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "rterrno.h"
#include "i64.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "lseek.h"
#include "handleio.h"
#include "thread.h"


#define LODWORD(x) (((unsigned_64 *)&x)->u._32[I64LO32])
#define HIDWORD(x) (((unsigned_64 *)&x)->u._32[I64HI32])

#ifdef __INT64__

__int64 _WCNEAR __lseeki64( int handle, __int64 offset, int origin )
{
#if defined( __NT__ ) || defined( __OS2_32BIT__ ) || defined( __LINUX__ )
    __int64         pos;

    __handle_check( handle, -1 );

  #if defined( __OS2__ )
    {
        APIRET          rc;

        if( __os2_DosSetFilePtrL != NULL ) {
            rc = __os2_DosSetFilePtrL( handle, offset, origin, &pos );
            if( rc != 0 ) {
                return( __set_errno_dos( rc ) );
            }
        } else {
            if( offset > LONG_MAX || offset < LONG_MIN ) {
                return( __set_EINVAL() );
            }
            pos = (unsigned long)__lseek( handle, offset, origin );
            if( (long)pos == -1L ) {
                return( -1LL );
            }
        }
    }
  #elif defined( __NT__ )
    {
        DWORD           pos_lo;
        DWORD           error;
        LONG            pos_hi;

        pos_hi = HIDWORD( offset );
        pos_lo = SetFilePointer( __getOSHandle( handle ), LODWORD( offset ), &pos_hi, origin );
        if( pos_lo == INVALID_SET_FILE_POINTER ) {
            // this might be OK so check for error
            error = GetLastError();
            if( error != NO_ERROR ) {
                return( __set_errno_dos( error ) );
            }
        }
        U64Set( (unsigned_64 *)&pos, pos_lo, pos_hi );
    }
  #elif defined( __LINUX__ )
    if( _llseek( handle, LODWORD( offset ), HIDWORD( offset ), &pos, origin ) ) {
        pos = -1LL;
    }
  #endif
    return( pos );
#else
    long            pos;

    if( offset > LONG_MAX || offset < LONG_MIN ) {
        return( __set_EINVAL() );
    }
    pos = __lseek( handle, offset, origin );
    if( pos == -1L ) {
        return( -1LL );
    }
    return( (unsigned long)pos );
#endif
}

#else

long _WCNEAR __lseek( int handle, long offset, int origin )
{
#if defined( __NT__ )
    DWORD               pos;
    DWORD               error;
#elif defined( __DOS__ ) || defined( __WINDOWS__ )
    uint_32             pos;
    tiny_ret_t          rc;
#elif defined( __OS2__ )
    unsigned long       pos;
    APIRET              rc;
#else
    unsigned long       pos;
#endif

    __handle_check( handle, -1 );

#if defined( __OS2__ )
    rc = DosChgFilePtr( handle, offset, origin, &pos );
    if( rc != 0 ) {
        return( __set_errno_dos( rc ) );
    }
#elif defined( __NT__ )
    pos = SetFilePointer( __getOSHandle( handle ), offset, 0, origin );
    if( pos == INVALID_SET_FILE_POINTER ) {
        // this might be OK so check for error
        error = GetLastError();
        if( error != NO_ERROR ) {
            return( __set_errno_dos( error ) );
        }
    }
#elif defined( __DOS__ ) || defined( __WINDOWS__ )
    {
        rc = TinyLSeek( handle, offset, origin, &pos );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
    }
#endif
    return( pos );
}

#endif
