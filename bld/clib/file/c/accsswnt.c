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
#include "widechar.h"
#include <io.h>
#include <direct.h>
#include <windows.h>
#include "libwin32.h"
#include "seterrno.h"


_WCRTLINK int __F_NAME(access,_waccess)( const CHAR_TYPE *path, int pmode )
{
    LONG                attr;

    #ifdef __WIDECHAR__
        attr = __lib_GetFileAttributesW( path );
    #else
        /*
         * If we actually want the attributes for the file, we should call
         * __lib_GetFileAttributesA, which works around a bug in NT's
         * GetFileAttributesA.  However, we're only concerned about the
         * existence of the file, and what NT provides is sufficient, so
         * to minimize executable size, don't call the clib internal smarter
         * version.
         */
        attr = GetFileAttributesA( path );
    #endif
    if( attr == -1 ) {
        return( __set_errno_nt() );
    }

    if( ( pmode & ACCESS_WR ) && ( attr & FILE_ATTRIBUTE_READONLY ) ) {
        return( __set_errno_dos( ERROR_ACCESS_DENIED ) );
    }
    return( 0 );
}
