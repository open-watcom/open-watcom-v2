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

_WCRTLINK int __F_NAME(chmod,_wchmod)( const CHAR_TYPE *pathname, int pmode )
{
    BOOL        rc;
    DWORD       attr;

#ifdef __WIDECHAR__
    attr = __lib_GetFileAttributesW( pathname );
#else
    attr = __lib_GetFileAttributesA( pathname );
#endif
    if( attr == INVALID_FILE_ATTRIBUTES ) {
        return( __set_errno_nt() );
    }
    attr &= ~FILE_ATTRIBUTE_READONLY;
    if( !( pmode & S_IWRITE ) ) {
        attr |= FILE_ATTRIBUTE_READONLY;
    }
#ifdef __WIDECHAR__
    rc = __lib_SetFileAttributesW( pathname, attr );
#else
    rc = SetFileAttributesA( pathname, attr );
#endif
    if( rc == FALSE ) {
        return( __set_errno_nt() );
    }
    return( 0 );
}
