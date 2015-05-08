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
#include <wos2.h>
#include "rtdata.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "iomode.h"
#include "errorno.h"
#include "seterrno.h"
#include "defwin.h"
#include "close.h"
#include "thread.h"


int __close( int handle )
{
    APIRET      rc;
    int         rv;
#ifdef DEFAULT_WINDOWING
    LPWDATA res;
#endif

    __handle_check( handle, -1 );
    rv = 0;
    rc = DosClose( handle );
    if( rc != 0 ) {
        rv = __set_errno_dos( rc );
    } else {
#ifdef DEFAULT_WINDOWING
        if( _WindowsCloseWindow != 0 ) {
            res = _WindowsIsWindowedHandle( handle );
            if( res != NULL ) {
                _WindowsRemoveWindowedHandle( handle );
                _WindowsCloseWindow( res );
            }
        }
#endif
    }
    __SetIOMode_nogrow( handle, 0 );
    return( rv );
}
