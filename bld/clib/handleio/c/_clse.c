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
* Description:  low-level component of close() for DOS/Win16
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include "rtdata.h"
#include "rterrno.h"
#include "tinyio.h"
#include "rtcheck.h"
#include "iomode.h"
#include "defwin.h"
#include "close.h"
#include "thread.h"

int __close( int handle )
{
    tiny_ret_t rc;
#ifdef DEFAULT_WINDOWING
    LPWDATA res;
#endif
    int     rv;

    __handle_check( handle, -1 );
    rv = 0;
    rc = TinyClose( handle );
    if( TINY_OK(rc) ) {
#ifdef DEFAULT_WINDOWING
        if( _WindowsCloseWindow != 0 ) {
            res = _WindowsIsWindowedHandle( handle );
            if( res != NULL ) {
                _WindowsRemoveWindowedHandle( handle );
                _WindowsCloseWindow( res );
            }
        }
#endif
    } else {
        _RWD_errno = EBADF;
        rv = -1;
    }
    __SetIOMode_nogrow( handle, 0 );
    return( rv );
}
