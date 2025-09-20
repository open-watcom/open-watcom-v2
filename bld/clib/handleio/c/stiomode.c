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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#if defined(__OS2__)
    #define INCL_DOSFILEMGR
    #include <wos2.h>
#elif defined(__NT__)
    #include <windows.h>
#endif
#include "liballoc.h"
#include "fileacc.h"
#include "rtinit.h"
#include "iomode.h"
#include "handleio.h"
#include "thread.h"


#if defined(__NETWARE__)
#error NO IO MODE MANAGER UNDER NETWARE
#endif

extern  unsigned _HUGEDATA __init_mode[_NFILES];

static  unsigned _init_NFiles;          // original __NFiles value;

void _WCNEAR __grow_iomode( int num )
{
    unsigned    *new;

    _AccessIOB();
    if( __io_mode == __init_mode ) {
        _init_NFiles = __NFiles;
        new = lib_malloc( num * sizeof( *new ) );
        if( new != NULL ) {
            memcpy( new, __init_mode, __NFiles * sizeof( *new ) );
        }
    } else {
        new = lib_realloc( __io_mode, num * sizeof( *new ) );
    }
    if( new == NULL ) {
        lib_set_errno( ENOMEM );
    } else {
        memset( &new[__NFiles], 0, ( num - __NFiles ) * sizeof( *new ) );
        __io_mode = new;
        __NFiles = num;
    }
    _ReleaseIOB();
}

static void _WCNEAR __shrink_iomode( void )
{
    _AccessIOB();
    // free any malloc'd iomode array
    if( __io_mode != __init_mode ) {
        lib_free( __io_mode );
        __io_mode = __init_mode;
        __NFiles = _init_NFiles;
    }
    _ReleaseIOB();
}

AYIN( __shrink_iomode, INIT_PRIORITY_IOSTREAM );


#if defined(__OS2_32BIT__)

static void __preinit_iomode_os2( void )
{
    LONG    req_count;
    ULONG   curr_max_fh;
    APIRET  rc;

    // Ensure that the clib and OS file handle limits match
    req_count = 0;
    rc = DosSetRelMaxFH( &req_count, &curr_max_fh );
    if( rc == 0 ) {
        __grow_iomode( curr_max_fh );
        __NHandles = curr_max_fh;   // same as __set_handles
    }
}

AXI( __preinit_iomode_os2, INIT_PRIORITY_RUNTIME );

#endif

#define _INITIALIZED    _DYNAMIC

int _WCNEAR __SetIOMode_grow( int handle, unsigned value )
{
    int         i;

    if( handle >= __NFiles ) {
        i = __NFiles;
        /*
         * 20 -> (20+10+1) -> 31
         * 31 -> (31+15+1) -> 47
         * 47 -> (47+23+1) -> 71
         * ...
         */
        __grow_iomode( i + (i >> 1) + 1 );
    }
    if( handle >= __NFiles ) {
        // return an error indication (errno should be set to ENOMEM)
        return( -1 );
    }
    if( value != 0 ) {
        __ChkTTYIOMode( handle );
        __io_mode[handle] = value | _INITIALIZED;
    } else {
        __io_mode[handle] = value;    /* we're closing it; smite _INITIALIZED */
    }
    return( handle );
}
