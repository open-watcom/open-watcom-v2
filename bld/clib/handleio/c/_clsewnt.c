/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
#include <stddef.h>
#include <stdbool.h>
#include <windows.h>
#include "seterrno.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "iomode.h"
#include "defwin.h"
#include "close.h"
#include "thread.h"


int _WCNEAR __close( int handle )
{
    bool        is_closed;
    int         rc;
    HANDLE      osfh;
#ifdef DEFAULT_WINDOWING
    LPWDATA     res;
#endif

    __handle_check( handle, -1 );

    is_closed = false;
    rc = 0;
    osfh = __getOSHandle( handle );

#ifdef DEFAULT_WINDOWING
    if( _WindowsCloseWindow != NULL
      && (res = _WindowsIsWindowedHandle( handle )) != NULL ) {
        _WindowsRemoveWindowedHandle( handle );
        _WindowsCloseWindow( res );
        is_closed = true;
    }
#endif
    if( !is_closed
      && CloseHandle( osfh ) == 0 ) {
        rc = __set_errno_nt();
    }
    __freePOSIXHandle( handle );
    __SetIOMode( handle, 0 );
    return( rc );
}
