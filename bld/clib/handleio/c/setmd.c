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
#include <fcntl.h>
#include <dos.h>
#include <dosfunc.h>
#if defined(__NT__)
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "fileacc.h"
#include "rtcheck.h"
#include "iomode.h"
#include "errorno.h"
#include "thread.h"

_WCRTLINK int setmode( int handle, int mode )
{
    unsigned        iomode_flags;
    unsigned        old_mode;
    __stream_link   *link;
    FILE            *fp;

    __handle_check( handle, -1 );

    iomode_flags = __GetIOMode( handle );
    if( iomode_flags == 0 ) {
        _RWD_errno = EBADF;
        return( -1 );
    }
    old_mode = (iomode_flags & _BINARY) ? O_BINARY : O_TEXT;
    if( mode != old_mode ) {
        if( mode == O_BINARY  ||  mode == O_TEXT ) {
            iomode_flags &= ~ _BINARY;
            if( mode == O_BINARY ) {
                iomode_flags |= _BINARY;
            }
            __SetIOMode( handle, iomode_flags );
            _AccessFileH( handle );
            for( link = _RWD_ostream; link != NULL; link = link->next ) {
                fp = link->stream;
                if( fp->_flag != 0 ) {      /* if file is open */
                    if( fileno( fp ) == handle ) {
                        fp->_flag &= ~ _BINARY;
                        if( mode == O_BINARY ) {
                            fp->_flag |= _BINARY;
                        }
                        break;
                    }
                }
            }
            _ReleaseFileH( handle );
        } else {
            _RWD_errno = EINVAL;
            old_mode = -1;
        }
    }
    return( old_mode );
}
