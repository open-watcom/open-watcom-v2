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
* Description:  Implementation of set errno routines called from assembled modules.
*
****************************************************************************/


#include "variety.h"
#include "seterrno.h"
#include <stdlib.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#elif defined( __NETWARE__ ) \
  && !defined( _THIN_LIB )
    #include "nw_lib.h"
#endif
#include "clibsupp.h"
#include "thread.h"


#if defined( __NETWARE__ ) \
  && defined( _THIN_LIB )
/*
 * unused by OW thin libraries
 */
#else

_WCRTLINK void __set_EDOM( void )
{
    lib_set_errno( EDOM );
}

_WCRTLINK void __set_ERANGE( void )
{
    lib_set_errno( ERANGE );
}

int _WCNEAR __set_EINVAL( void )
{
    lib_set_errno( EINVAL );
    return( -1 );
}

#endif
