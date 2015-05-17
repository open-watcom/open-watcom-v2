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


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <dos.h>
#include <direct.h>
#include <windows.h>
#include "rterrno.h"
#include "seterrno.h"
#include "libwin32.h"
#include "liballoc.h"
#include "thread.h"

_WCRTLINK CHAR_TYPE *__F_NAME(getcwd,_wgetcwd)( CHAR_TYPE *buf, size_t size )
{
    CHAR_TYPE           path[ _MAX_PATH ];
    DWORD               realsize;

    /*** Get the current directory ***/
    realsize = __lib_GetCurrentDirectory( _MAX_PATH, path );

    if( realsize == 0 ) {
        __set_errno_nt();
        return( NULL );
    }
    if( buf == NULL ) {
        buf = lib_malloc( max( size, realsize + 1 ) * CHARSIZE );
        if( buf == NULL ) {
            _RWD_errno = ENOMEM;
            return( NULL );
        }
    } else {
        if( realsize + 1 > size ) {
            _RWD_errno = ERANGE;
            return( NULL );
        }
    }
    return( memcpy( buf, path, ( realsize + 1 ) * CHARSIZE ) );
}
