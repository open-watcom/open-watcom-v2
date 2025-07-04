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
#include <stdio.h>
#include <unistd.h>
#include <windows.h>
#include "rterrno.h"
#include "fileacc.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "thread.h"


_WCRTLINK int dup2( int handle1, int handle2 )
{
    HANDLE      osph;
    HANDLE      osfh1;
    HANDLE      osfh2;

    __handle_check( handle1, -1 );

    if( handle1 == handle2 ) {
        return( handle2 );
    }
    // This is required for Win32.
    // if 2nd handle is out of range then try to grow the handle/iomode arrays
    if( handle2 >= __NHandles )  {
        __growPOSIXHandles( handle2 );
    }
    // if 2nd handle is still out of range then no more memory
    if( handle2 >= __NHandles )  {
        return( -1 );
    }

    osph = GetCurrentProcess();
    osfh1 = __getOSHandle( handle1 );

    _AccessFileH( handle1 );

    if( DuplicateHandle( osph, osfh1, osph, &osfh2, 0, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) {
        _ReleaseFileH( handle1 );
        return( __set_errno_nt() );
    }

    close( handle2 );

    __setOSHandle( handle2, osfh2 );
    __SetIOMode( handle2, __GetIOMode( handle1 ) );

    _ReleaseFileH( handle1 );
    return( 0 );
}
