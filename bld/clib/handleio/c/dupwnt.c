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
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "thread.h"


_WCRTLINK int dup( int old_handle )
{
    HANDLE      osfh;
    int         handle;
    HANDLE      cprocess;

    __handle_check( old_handle, -1 );
    /*
     * First try to get the required slot.
     * No point in creating a file only to not use it.
     */
    handle = __allocPOSIXHandleDummy();
    if( handle == -1 ) {
        return( -1 );
    }

    cprocess = GetCurrentProcess();

    if( DuplicateHandle( cprocess, __getOSHandle( old_handle ), cprocess, &osfh, 0, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) {
        /*
         * Give back the slot we got
         */
        __freePOSIXHandle( handle );
        return( __set_errno_nt() );
    }
    /*
     * Now use the slot we got.
     */
    __setOSHandle( handle, osfh );
    __SetIOMode( handle, __GetIOMode( old_handle ) );
    return( handle );
}
