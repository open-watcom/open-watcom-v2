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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "rtdata.h"
#include "liballoc.h"
#include "fileacc.h"
#include "rtinit.h"
#include "seterrno.h"

unsigned __NFiles = _NFILES;            /* maximum # of files we can open */

#if defined(__NETWARE__)
#error NO IO MODE MANAGER UNDER NETWARE
#endif

#if !defined(__QNX__)

unsigned _HUGEDATA __init_mode[_NFILES] = { /* file mode information (flags) */
        _READ,          /* stdin */
        _WRITE,         /* stdout */
        _WRITE,         /* stderr */
        _READ|_WRITE,   /* stdaux */
        _WRITE          /* stdprn */
};

static  unsigned    *_iomode = __init_mode; // initially points to static array
static  unsigned    _init_NFiles;           // original __NFiles value;


void __grow_iomode( int num )
{
    unsigned    *new;

    _AccessIOB();
    if( _iomode == __init_mode ) {
        _init_NFiles = __NFiles;
        new = (unsigned *) lib_malloc( num * sizeof( unsigned ) );
        if( new != NULL ) {
            memcpy( new, __init_mode, __NFiles * sizeof(unsigned) );
        }
    } else {
        #if defined(__NETWARE__)
            new = (unsigned *) lib_realloc( _iomode, num * sizeof( unsigned ), __NFiles * sizeof( unsigned ) );
        #else
            new = (unsigned *) lib_realloc( _iomode, num * sizeof( unsigned ) );
        #endif
    }
    if( new == NULL ) {
        __set_errno( ENOMEM );
    } else {
        memset( &new[__NFiles], 0, (num-__NFiles)*sizeof(unsigned) );
        _iomode = new;
        __NFiles = num;
    }
    _ReleaseIOB();
}

void __shrink_iomode( void )
{
    _AccessIOB();
    // free any malloc'd iomode array
    if( _iomode != __init_mode ) {
        lib_free( _iomode );
        _iomode = __init_mode;
        __NFiles = _init_NFiles;
    }
    _ReleaseIOB();
}

AYI(__shrink_iomode,INIT_PRIORITY_IOSTREAM);

#define _INITIALIZED    _DYNAMIC

unsigned __GetIOMode( unsigned handle )
{
    if( handle >= __NFiles ) {
        return( 0 );
    }
    if( handle < NUM_STD_STREAMS && !(_iomode[handle] & _INITIALIZED) ) {
        _iomode[handle] |= _INITIALIZED;
        if( isatty( handle ) ) {
            _iomode[handle] |= _ISTTY;
        }
    }
    return( _iomode[handle] );
}

// For F77 to call

unsigned __IOMode( unsigned handle )
{
    return( __GetIOMode( handle ) );
}

signed __SetIOMode( unsigned handle, unsigned value )
{
    int         i;

    if( handle >= __NFiles ) {
        i = __NFiles;           // 20 -> (20+10+1) -> 31
                                // 31 -> (31+15+1) -> 47
                                // 47 -> (47+23+1) -> 71
        __grow_iomode( i + (i > 1) + 1 );
    }
    if( handle >= __NFiles ) {
        // return an error indication (errno should be set to ENOMEM)
        return( -1 );
    } else {
        if( value != 0 ) {
            _iomode[handle] = value | _INITIALIZED;
        } else {
            _iomode[handle] = value;    /* we're closing it; smite _INITIALIZED */
        }
        return( handle );
    }
}
#endif
