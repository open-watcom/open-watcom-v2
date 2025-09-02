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
* Description:  Handle manager routines.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _M_IX86
 #include <i86.h>
#endif
#include <unistd.h>
#if defined(__OS2__)
    #define INCL_DOSFILEMGR
    #include <wos2.h>
#elif defined(__WINDOWS__) || defined(__NT__)
    #include <windows.h>
#elif defined(__UNIX__) && !defined(__QNX__)
    #include <sys/resource.h>
#elif defined(__DOS__)
    #include "tinyio.h"
#endif
#include "roundmac.h"
#include "rtdata.h"
#include "rterrno.h"
#include "liballoc.h"
#include "iomode.h"
#include "fileacc.h"
#include "rtinit.h"
#include "seterrno.h"
#include "handleio.h"
#include "thread.h"


unsigned    __NHandles = 0;

#if defined(__NT__)

static HANDLE   *__FakeHandles = NULL;
static HANDLE   *__OSHandles = NULL;
static int      __topFakeHandle = 0;

unsigned __growPOSIXHandles( unsigned num )
{
    HANDLE      *os_handles;
    unsigned    i;

    if( num > __NHandles ) {
        _AccessFList();
        if( __OSHandles == NULL ) {
            os_handles = lib_malloc( num * sizeof( *os_handles ) );
        } else {
            os_handles = lib_realloc( __OSHandles, num * sizeof( *os_handles ) );
        }
        if( os_handles == NULL ) {
            _RWD_errno = ENOMEM;
        } else {
            for( i = __NHandles; i < num; i++ ) {
                os_handles[i] = INVALID_HANDLE_VALUE;
            }
            __OSHandles = os_handles;
            __NHandles = num;
        }
        _ReleaseFList();
    }
    return( __NHandles );
}

int __allocPOSIXHandle( HANDLE osfh )
{
    int i;

    _AccessFList();
    for( i = 0; i < __NHandles; i++ ) {
        if( __OSHandles[i] == INVALID_HANDLE_VALUE ) {
            __OSHandles[i] = osfh;
            break;
        }
    }
    if( i >= __NHandles ) {
        /*
         * 20 -> (20+10+1) -> 31
         * 31 -> (31+15+1) -> 47
         * 47 -> (47+23+1) -> 71
         * ...
         */
        __growPOSIXHandles( i + (i >> 1) + 1 );
        // keep iomode array in sync
        if( __NFiles < __NHandles )
            __grow_iomode( __NHandles );
        for( ; i < __NHandles; i++ ) {
            if( __OSHandles[i] == INVALID_HANDLE_VALUE ) {
                __OSHandles[i] = osfh;
                break;
            }
        }
        if( i >= __NHandles ) {
            i = -1;
        }
    }
    _ReleaseFList();
    return( i );
}

void __freePOSIXHandle( int handle )
{
    __OSHandles[handle] = INVALID_HANDLE_VALUE;
}


HANDLE __getOSHandle( int handle )
{
    return( __OSHandles[handle] );
}

int __setOSHandle( int handle, HANDLE osfh )
{
    // call the Win32 API for a standard file handle
    switch( handle ) {
    case STDIN_FILENO:
        SetStdHandle( STD_INPUT_HANDLE, osfh );
        break;
    case STDOUT_FILENO:
        SetStdHandle( STD_OUTPUT_HANDLE, osfh );
        break;
    case STDERR_FILENO:
        SetStdHandle( STD_ERROR_HANDLE, osfh );
        break;
    }
    if( handle < __NHandles ) {
        __OSHandles[handle] = osfh;
        return( handle );
    }
    return( -1 );           // this should never happen
}

HANDLE __NTGetFakeHandle( void )
{
    static DWORD    fakeHandle = 0x80000000L;
    HANDLE          osfh;

    _AccessFList();
    osfh = CreateEvent( 0, 0, 0, 0 );
    if( osfh == NULL ) {
        // win32s does not support event handles
        fakeHandle++;
        osfh = (HANDLE)fakeHandle;
    } else {
        __FakeHandles = lib_realloc( __FakeHandles, ( __topFakeHandle + 1 ) * sizeof( *__FakeHandles ) );
        __FakeHandles[__topFakeHandle] = osfh;
        __topFakeHandle++;
    }
    _ReleaseFList();
    return( osfh );
}

// called from library startup code

static void __init_STD_POSIXHandle( DWORD stdhandle )
{
    HANDLE osfh;

    osfh = GetStdHandle( stdhandle );
    if( osfh == NULL
      || osfh == INVALID_HANDLE_VALUE ) {
        osfh = __NTGetFakeHandle();
    }
    __allocPOSIXHandle( osfh );
}

void __initPOSIXHandles( void )
{
    // __OSHandles = NULL;
    // __NHandles = 0;

    __growPOSIXHandles( __NFiles );
    __init_STD_POSIXHandle( STD_INPUT_HANDLE );
    __init_STD_POSIXHandle( STD_OUTPUT_HANDLE );
    __init_STD_POSIXHandle( STD_ERROR_HANDLE );
}

static void __finiPOSIXHandles( void )
{
    int     i;

    if( __OSHandles != NULL ) {
        lib_free( __OSHandles );
        __OSHandles = NULL;
    }
    if( __FakeHandles != NULL ) {
        for( i = 0 ; i < __topFakeHandle; i++ ) {
            CloseHandle( __FakeHandles[i] );
        }
        lib_free( __FakeHandles );
        __FakeHandles = NULL;
    }
}

AYI( __finiPOSIXHandles, INIT_PRIORITY_LIBRARY-1 )

#endif


void _WCNEAR __set_handles( int num )
{
    __NHandles = num;
}

_WCRTLINK int _grow_handles( int num )
{
    if( num > __NHandles ) {
#if defined(__DOS__)
        /* increase the number of file handles beyond 20 */
        if( _RWD_osmajor > 3
          || ( _RWD_osmajor == 3
          && _RWD_osminor >= 30 ) ) {
            tiny_ret_t  rc;

            /* may allocate a segment of memory! */
            num = __ROUND_UP_SIZE_EVEN( num );  /* make even */
            rc = TinySetMaxHandleCount( num );
            if( TINY_ERROR( rc ) ) {
                __set_errno_dos( TINY_INFO( rc ) );
                num = __NHandles;
            }
        } else {
            /* code for pre DOS 3.3 */
            /* makes use of undocumented area of the psp */
            char            *new_handles;
            char _WCFAR *   _WCFAR *psp_handles;
            char            _WCFAR *psp_num_handles;

            new_handles = (char *)lib_nmalloc( num );
            if( new_handles == 0 ) {
                _RWD_errno = ENOMEM;
                num = __NHandles;
            } else {
                _fmemset( new_handles, 0xFF, num );

                psp_num_handles = _MK_FP( _RWD_psp, 0x32 );
                psp_handles = _MK_FP( _RWD_psp, 0x34 );
                _fmemcpy( new_handles, *psp_handles, *psp_num_handles );

                _disable();
                psp_handles = _MK_FP( _RWD_psp, 0x34 );
                *psp_handles = new_handles;
                psp_num_handles = _MK_FP( _RWD_psp, 0x32 );
                *psp_num_handles = num;
                _enable();
            }
        }
#elif defined( __OS2_16BIT__ )
        {
            APIRET  rc;

            rc = DosSetMaxFH( num );
            if( rc ) {
                if( rc == ERROR_NOT_ENOUGH_MEMORY ) {
                    __set_errno_dos( rc );
                    num = __NHandles;
                } else {
                    int i = num;
                    for( ;; ) {
                        rc = DosSetMaxFH( i );
                        if( rc == ERROR_NOT_ENOUGH_MEMORY )
                            break;
                        if( rc == 0 )
                            break;
                        if( i > ( num + 256 ) )
                            break;
                        i++;
                    }
                    if( rc != 0 ) {
                        __set_errno_dos( rc );
                        num = __NHandles;
                    } else {
                        num = i;
                    }
                }
            }
        }
#elif defined( __OS2_32BIT__ )
        {
            APIRET  rc;
            LONG    req_count;
            ULONG   curr_max_fh;

            req_count = num - __NHandles;
            rc = DosSetRelMaxFH( &req_count, &curr_max_fh );
            num = curr_max_fh;
        }
#elif defined(__WINDOWS__)
        {
            num = SetHandleCount( num );
        }
#elif defined(__NT__)
        {
            num = __growPOSIXHandles( num );
        }
#elif defined(__NETWARE__)
        {
            #error NO HANDLE MANAGER UNDER NETWARE
        }
#else
        {
            // nothing to do
        }
#endif
#if defined(__UNIX__) && !defined(__QNX__)
        {
            struct rlimit   rl;
            int             old_num;

            if( getrlimit( RLIMIT_NOFILE, &rl ) == 0 ) {
                old_num = rl.rlim_cur;
                if( old_num < num ) {
                    rl.rlim_cur = num;
                    if( setrlimit( RLIMIT_NOFILE, &rl ) == 0 ) {
                        old_num = num;
                    }
                }
                __NHandles = old_num;
            }
        }
#else
        if( num > __NFiles ) {
            __grow_iomode( num );   // sets new __NFiles if successful
        }
        __NHandles = num;
#endif
    }
    return( __NHandles );
}
