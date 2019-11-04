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
* Description:  Accessors to errno variable.
*
****************************************************************************/


#include "variety.h"
#if defined( __NT__ )
    #include <windows.h>
#elif defined(__QNX__)
    #include <sys/magic.h>
    #include "slibqnx.h"
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rterrno.h"
#include "thread.h"


#if defined(__QNX__)

_WCRTLINK int (*__get_errno_ptr( void ))
{
#if defined(_M_I86)
    return( SLIB2CLIB( int, &__MAGIC.Errno ) );
#else
    void        *err_ptr;

    __getmagicvar( &err_ptr, _m_errno_ptr );
    return( err_ptr );
#endif
}

#else

#undef errno

#if !defined( __SW_BM ) || defined( __RDOSDEV__ )

_WCRTDATA int       errno;

#endif

_WCRTLINK int (*__get_errno_ptr( void ))
{
    return( &_RWD_errno );
}

#endif
