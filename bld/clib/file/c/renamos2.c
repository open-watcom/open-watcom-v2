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
#include <stdio.h>
#include <wos2.h>
#include "seterrno.h"


_WCRTLINK int __F_NAME(rename,_wrename)( const CHAR_TYPE *old, const CHAR_TYPE *new )
{
    APIRET      rc;
#ifdef __WIDECHAR__
    char        mbOld[MB_CUR_MAX * _MAX_PATH];  /* single-byte char */
    char        mbNew[MB_CUR_MAX * _MAX_PATH];  /* single-byte char */

    if( wcstombs( mbOld, old, sizeof( mbOld ) ) == -1 ) {
        mbOld[0] = '\0';
    }
    if( wcstombs( mbNew, new, sizeof( mbNew ) ) == -1 ) {
        mbNew[0] = '\0';
    }
#endif
#ifdef _M_I86
    rc = DosMove( (PSZ)__F_NAME(old,mbOld), (PSZ)__F_NAME(new,mbNew), 0 );
#else
    rc = DosMove( (PSZ)__F_NAME(old,mbOld), (PSZ)__F_NAME(new,mbNew) );
#endif
    if( rc != 0  ) {
        return( __set_errno_dos( rc ) );
    }
    return( 0 );            /* indicate no error */
}
