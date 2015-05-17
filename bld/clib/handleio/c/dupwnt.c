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
#include "rterrno.h"
#include "iomode.h"
#include "rtcheck.h"
#include "seterrno.h"
#include "thread.h"

_WCRTLINK int dup( int old_hid )
{
    HANDLE      new_handle;
    int         hid;
    HANDLE      cprocess;

    __handle_check( old_hid, -1 );

    // First try to get the required slot.
    // No point in creating a new handle only to not use it.  JBS 99/11/01
    hid = __allocPOSIXHandle( DUMMY_HANDLE );
    if( hid == -1 ) {
        return( -1 );
    }

    cprocess = GetCurrentProcess();

    if( !DuplicateHandle( cprocess,  __getOSHandle( old_hid ), cprocess,
                        &new_handle, 0, TRUE, DUPLICATE_SAME_ACCESS ) ) {
        // Give back the slot we got
        __freePOSIXHandle( hid );
        return( __set_errno_nt() );
    }
    // Now use the slot we got
    __setOSHandle( hid, new_handle );   // JBS 99/11/01
    __SetIOMode( hid, __GetIOMode( old_hid ) );
    return( hid );
}
