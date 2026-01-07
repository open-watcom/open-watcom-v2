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
* Description:  Implementation of CRTL internal set errno routines.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <stdlib.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __NETWARE__ )
    #include "nw_lib.h"
#endif
#include "clibsupp.h"
#include "thread.h"


#if defined( __NETWARE__ )
#if !defined( _THIN_LIB )
int _WCNEAR __get_errno( void )
{
#if defined( _NETWARE_LIBC )
    return( *___errno() );
#else
    return( *__get_errno_ptr() );
#endif
}
void _WCNEAR __set_errno( int err )
{
#if defined( _NETWARE_LIBC )
    *___errno() = err;
#else
    *__get_errno_ptr() = err;
#endif
}
#endif
#elif defined(__QNX__)
#elif defined(__RDOSDEV__)
#elif defined(__MT__)
int _WCNEAR __get_errno( void )
{
    return( __THREADDATAPTR->__errnoP );
}
void _WCNEAR __set_errno( int err )
{
    __THREADDATAPTR->__errnoP = err;
}
#else
#endif


int _WCNEAR __set_EINVAL( void )
{
#if defined( __NETWARE__ )
#if defined( _NETWARE_LIBC )
    *___errno() = EINVAL;
#else
    *__get_errno_ptr() = EINVAL;
#endif
#else
    lib_set_errno( EINVAL );
#endif
    return( -1 );
}


#if defined( __NETWARE__ )

#else

_WCRTLINK void __set_EDOM( void )
{
    lib_set_errno( EDOM );
}

_WCRTLINK void __set_ERANGE( void )
{
    lib_set_errno( ERANGE );
}

#endif
