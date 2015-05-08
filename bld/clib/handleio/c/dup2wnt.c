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
#include <windows.h>
#include "rtdata.h"
#include "fileacc.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"

_WCRTLINK int dup2( int hid1, int hid2 )
{
    HANDLE      ph;
    HANDLE      fh;
    HANDLE      fh2;

    __handle_check( hid1, -1 );

    if( hid1 == hid2 ) {
        return( hid2 );
    }
    // This is required for Win32.
    // if 2nd handle is out of range then try to grow the handle/iomode arrays
    if( hid2 >= __NHandles )  {
        __growPOSIXHandles( hid2 );
    }
    // if 2nd handle is still out of range then no more memory
    if( hid2 >= __NHandles )  {
        return( -1 );
    }

    ph = GetCurrentProcess();
    fh = __getOSHandle( hid1 );

    _AccessFileH( hid1 );

    if( !DuplicateHandle( ph, fh, ph, &fh2, 0, TRUE, DUPLICATE_SAME_ACCESS ) ) {
        _ReleaseFileH( hid1 );
        return( __set_errno_nt() );
    }

    close( hid2 );

    __setOSHandle( hid2, fh2 );
    __SetIOMode( hid2, __GetIOMode( hid1 ) );

    _ReleaseFileH( hid1 );
    return( 0 );
}
