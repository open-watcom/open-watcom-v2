/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Set handle to binary mode __set_binary function prototype
*                (used by Fortran)
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#if defined(__NT__)
    #include <windows.h>
#endif
#include "iomode.h"

#if defined( __DOS__ )

#include "tinyio.h"
#include "seterrno.h"

int __set_binary( int handle )
{
    unsigned        iomode_flags;

    __ChkTTYIOMode( handle );
    iomode_flags = __GetIOMode( handle );
    iomode_flags |= _BINARY;
    __SetIOMode( handle, iomode_flags );
    if( iomode_flags & _ISTTY ) {
        tiny_ret_t rc;

        rc = TinyGetDeviceInfo( handle );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
        rc = TinySetDeviceInfo( handle, TINY_INFO(rc) | TIO_CTL_RAW );
        if( TINY_ERROR( rc ) ) {
            return( __set_errno_dos( TINY_INFO( rc ) ) );
        }
    }
    return( 0 );
}

#else

int __set_binary( int handle )
{
    unsigned        iomode_flags;

    __ChkTTYIOMode( handle );
    iomode_flags = __GetIOMode( handle );
    iomode_flags |= _BINARY;
    __SetIOMode( handle, iomode_flags );
    return( 0 );
}

#endif
