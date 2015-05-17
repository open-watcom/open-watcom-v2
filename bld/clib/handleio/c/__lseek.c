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
#if defined( __OS2__ )
    #define INCL_LONGLONG
#endif
#include "rterrno.h"
#if defined( __DOS__ ) || defined( __WINDOWS__ )
    #include "tinyio.h"
#endif
#include "i64.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "lseek.h"
#include "handleio.h"
#include "thread.h"
#if defined( __OS2__ )
    #include "os2fil64.h"
#endif

#ifndef INVALID_SET_FILE_POINTER
    #define INVALID_SET_FILE_POINTER 0xFFFFFFFF
#endif

#define LODWORD(x) (((unsigned_64 *)&x)->u._32[I64LO32])
#define HIDWORD(x) (((unsigned_64 *)&x)->u._32[I64HI32])

#if defined( __LINUX__ )
_WCRTLINK extern int _llseek( unsigned int, unsigned long, unsigned long, long long *, unsigned int );
#endif


#ifdef __INT64__

_WCRTLINK __int64 __lseeki64( int handle, __int64 offset, int origin )
{
#if defined( __NT__ ) || defined( __OS2__ ) || defined( __LINUX__ )
    __int64         pos;

    __handle_check( handle, -1 );

  #if defined( __OS2__ )
    {
    #if !defined( _M_I86 )
        APIRET          rc;

        if( __os2_DosSetFilePtrL != NULL ) {
            rc = __os2_DosSetFilePtrL( handle, offset, origin, &pos );
            if( rc != 0 ) {
                return( __set_errno_dos( rc ) );
            }
        } else {
    #endif
            if( offset > LONG_MAX || offset < LONG_MIN ) {
                _RWD_errno = EINVAL;
                return( -1LL );
            }
            pos = (unsigned long)__lseek( handle, offset, origin );
            if( pos == INVALID_SET_FILE_POINTER ) {
                pos = -1LL;
            }
    #if !defined( _M_I86 )
        }
    #endif
    }
  #elif defined( __NT__ )
    {
        DWORD           rc;
        LONG            offset_hi;
        int             error;
    
        offset_hi = HIDWORD( offset );
        rc = SetFilePointer( __getOSHandle( handle ), LODWORD( offset ), &offset_hi, origin );
        if( rc == INVALID_SET_FILE_POINTER ) {  // this might be OK so
            error = GetLastError();             // check for sure JBS 04-nov-99
            if( error != NO_ERROR ) {
                return( __set_errno_dos( error ) );
            }
        }
        U64Set( (unsigned_64 *)&pos, rc, offset_hi );
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
        _RWD_errno = EINVAL;
        return( -1LL );
    }
    pos = __lseek( handle, offset, origin );
    if( pos == INVALID_SET_FILE_POINTER ) {
        return( -1LL );
    }
    return( (unsigned long)pos );
#endif
}

#else

_WCRTLINK long __lseek( int handle, long offset, int origin )
{
    long            pos;

    __handle_check( handle, -1 );

#if defined( __OS2__ )
    {
        APIRET          rc;

        rc = DosChgFilePtr( handle, offset, origin, (PULONG)&pos );
        if( rc != 0 ) {
            return( __set_errno_dos( rc ) );
        }
    }
#elif defined( __NT__ )
    pos = SetFilePointer( __getOSHandle( handle ), offset, 0, origin );
    if( pos == INVALID_SET_FILE_POINTER ) {
        return( __set_errno_nt() );
    }
#elif defined( __DOS__ ) || defined( __WINDOWS__ )
    {
        tiny_ret_t      rc;

        rc = TinyLSeek( handle, offset, origin, (u32_stk_ptr)&pos );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
    }
#endif
    return( pos );
}

#endif
