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
* Description:  Handle manager routines.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include "liballoc.h"
#include <string.h>
#ifdef _M_IX86
 #include <i86.h>
#endif
#include <unistd.h>
#include <errno.h>
#if defined(__OS2__)
 #define INCL_DOSFILEMGR
 #include <wos2.h>
#elif defined(__WINDOWS__) || defined(__NT__)
 #include <windows.h>
#elif defined(__DOS__)
 #include "tinyio.h"
#endif
#include "iomode.h"
#include "fileacc.h"
#include "rtinit.h"
#include "seterrno.h"
#include "handleio.h"

#undef __getOSHandle

#if defined(__DOS__)
    extern tiny_ret_t _Set_File_Handle_Count( unsigned );
  #if defined(__386__)
    #pragma aux _Set_File_Handle_Count = \
        "mov        ah,67h"             \
        "int        0x21"               \
        "rcl        eax,1"              \
        "ror        eax,1"              \
        value [eax]                     \
        parm caller [ebx]
  #else
    #pragma aux _Set_File_Handle_Count = \
        "mov        ah,67h"             \
        "int        0x21"               \
        "sbb        dx,dx"              \
        value [dx ax]                   \
        parm caller [bx]
  #endif
#endif

extern  unsigned    __NFiles;       // the size of the iomode array
extern  void        __grow_iomode( int num );
        unsigned    __NHandles = 0;

#if defined(__NT__)

HANDLE *__OSHandles = NULL;

unsigned __growPOSIXHandles( unsigned num )
{
    HANDLE *new2;
    unsigned i;

    if( num > __NHandles ) {
        _AccessFList();
        if( __OSHandles == NULL ) {
            new2 = lib_malloc( num * sizeof( int ) );
        } else {
            new2 = lib_realloc( __OSHandles, num * sizeof( int ) );
        }
        if( new2 == NULL ) {
            __set_errno( ENOMEM );
            num = __NHandles;
        } else {
            for( i = __NHandles; i < num; i++ ) {
                new2[ i ] = NULL_HANDLE;
            }
            __OSHandles = new2;
            __NHandles = num;
        }
        _ReleaseFList();
    }
    return( __NHandles );
}

int __allocPOSIXHandle( HANDLE hdl )
{
    int i;

    _AccessFList();
    for( i = 0; i < __NHandles; i++ ) {
        if( __OSHandles[i] == NULL_HANDLE ) break;
    }
    if( i >= __NHandles ) {
                                // 20 -> (20+10+1) -> 31
                                // 31 -> (31+15+1) -> 47
                                // 47 -> (47+23+1) -> 71
        __growPOSIXHandles( i + (i >> 1) + 1 );
        // keep iomode array in sync
        if( __NHandles > __NFiles ) __grow_iomode( __NHandles );
        for( ; i < __NHandles; i++ ) {
            if( __OSHandles[i] == NULL_HANDLE ) break;
        }
    }
    if( i >= __NHandles ) {
        i = -1;
    } else {
        __OSHandles[i] = hdl;
    }
    _ReleaseFList();
    return( i );
}

void __freePOSIXHandle( int hid )
{
    __OSHandles[ hid ] = NULL_HANDLE;
}


HANDLE __getOSHandle( int hid )
{
    return( __OSHandles[ hid ] );
}

int __setOSHandle( unsigned hid, HANDLE hdl )
{
    // call the Win32 API for a standard file handle
    switch( hid ) {
    case STDIN_FILENO:
        SetStdHandle( STD_INPUT_HANDLE, hdl );
        break;
    case STDOUT_FILENO:
        SetStdHandle( STD_OUTPUT_HANDLE, hdl );
        break;
    case STDERR_FILENO:
        SetStdHandle( STD_ERROR_HANDLE, hdl );
        break;
    }
    if( hid < __NHandles ) {
        __OSHandles[ hid ] = hdl;
    } else {
        hid = (unsigned)-1;     // this should never happen
    }
    return( hid );
}

HANDLE *__FakeHandles = 0;
static int __topFakeHandle = 0;

HANDLE __NTGetFakeHandle( void )
{
    HANDLE os_handle;

    _AccessFList();
    os_handle = CreateEvent( 0, 0, 0, 0 );
    if( os_handle == NULL ) {
        // win32s does not support event handles
        static DWORD fakeHandle = 0x80000000L;
        fakeHandle++;
        os_handle = (HANDLE)fakeHandle;
    } else {
        __FakeHandles = lib_realloc( __FakeHandles, (__topFakeHandle+1) * sizeof( HANDLE ) );
        __FakeHandles[ __topFakeHandle ] = os_handle;
        __topFakeHandle++;
    }
    _ReleaseFList();
    return( os_handle );
}

// called from library startup code

void __initPOSIXHandles( void )
{
    HANDLE h;

    // __OSHandles = NULL;
    // __NHandles = 0;

    __growPOSIXHandles( __NFiles );
    h = GetStdHandle( STD_INPUT_HANDLE );
    if( h == 0 || h == INVALID_HANDLE_VALUE ) {
        h = (HANDLE)__NTGetFakeHandle();
    }
    __allocPOSIXHandle( h );        // should return 0==STDIN_FILENO
    h = GetStdHandle( STD_OUTPUT_HANDLE );
    if( h == 0 || h == INVALID_HANDLE_VALUE ) {
        h = (HANDLE)__NTGetFakeHandle();
    }
    __allocPOSIXHandle( h );        // should return 1==STDOUT_FILENO
    h = GetStdHandle( STD_ERROR_HANDLE );
    if( h == 0 || h == INVALID_HANDLE_VALUE ) {
        h = (HANDLE)__NTGetFakeHandle();
    }
    __allocPOSIXHandle( h );        // should return 3==STDERR_FILENO
}

static void __finiPOSIXHandles( void )
{
    if( __OSHandles != NULL ) {
        lib_free( __OSHandles );
        __OSHandles = NULL;
    }
    if( __FakeHandles != NULL ) {
        int i;
        for( i = 0 ; i < __topFakeHandle ; i++ ) {
            CloseHandle( __FakeHandles[i] );
        }
        lib_free( __FakeHandles );
        __FakeHandles = 0;
    }
}

AYI( __finiPOSIXHandles, INIT_PRIORITY_LIBRARY-1 )

#endif


void __set_handles( int num )
{
    __NHandles = num;
}

_WCRTLINK int _grow_handles( int num )
{
    if( num > __NHandles ) {
        #if defined(__DOS__)
            /* increase the number of file handles beyond 20 */

            if( _osmajor > 3 || ( _osmajor == 3 && _osminor >= 30 ) ) {
                tiny_ret_t  rc;

                /* may allocate a segment of memory! */
                num = (num+1) & ~1; /* make even */
                rc = _Set_File_Handle_Count( num );
                if( TINY_ERROR( rc ) ) {
                    __set_errno_dos( TINY_INFO( rc ) );
                    num = __NHandles;
                }
            } else {
                /* code for pre DOS 3.3 */
                /* makes use of undocumented area of the psp */
                char             *new_handles;
                char _WCFAR * _WCFAR *psp_handles;
                char        _WCFAR *psp_num_handles;

                new_handles = (char *)lib_nmalloc( num );
                if( new_handles == 0 ) {
                    __set_errno( ENOMEM );
                    num = __NHandles;
                } else {
                    _fmemset( new_handles, 0xff, num );

                    psp_num_handles = MK_FP( _psp, 0x32 );
                    psp_handles = MK_FP( _psp, 0x34 );
                    _fmemcpy( new_handles, *psp_handles, *psp_num_handles );

                    _disable();
                    psp_handles = MK_FP( _psp, 0x34 );
                    *psp_handles = new_handles;
                    psp_num_handles = MK_FP( _psp, 0x32 );
                    *psp_num_handles = num;
                    _enable();
                }
            }
        #elif defined( __OS2_286__ )
        {
            int     rc;

            rc = DosSetMaxFH( num );
            if( rc != 0 ) {
                if( rc == ERROR_NOT_ENOUGH_MEMORY ) {
                    __set_errno_dos( rc );
                    num = __NHandles;
                } else {
                    int i = num;
                    for( ;; ) {
                        rc = DosSetMaxFH( i );
                        if( rc == ERROR_NOT_ENOUGH_MEMORY ) break;
                        if( rc == 0 ) break;
                        if( i > (num+256) ) break;
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
        #elif defined( __WARP__ )
        {
            LONG    req_count;
            ULONG   curr_max_fh;
            APIRET  rc;

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
        #elif defined(__UNIX__)
        {
            // nothing to do
        }
        #endif

        if( num > __NFiles ) {
            __grow_iomode( num );   // sets new __NFiles if successful
        }
        __NHandles = num;
    }
    return( __NHandles );
}
