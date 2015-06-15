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
* Description:  OS/2 implementation of access().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <io.h>
#include <direct.h>
#define INCL_DOSERRORS
#include <wos2.h>
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif


_WCRTLINK int __F_NAME(access,_waccess)( const CHAR_TYPE *path, int pmode )
{
    APIRET          rc;
    OS_UINT         attr;
#ifndef _M_I86
    FILESTATUS3     fs;
#endif
#ifdef __WIDECHAR__
    char            mbPath[MB_CUR_MAX * _MAX_PATH]; /* single-byte char */

    if( wcstombs( mbPath, path, sizeof( mbPath ) ) == -1 ) {
        mbPath[0] = '\0';
    }
#endif
#ifdef _M_I86
    rc = DosQFileMode( (PSZ)__F_NAME(path,mbPath), &attr, 0 );
#else
    rc = DosQueryPathInfo( (PSZ)__F_NAME(path,mbPath), FIL_STANDARD, &fs, sizeof( fs ) );
    attr = fs.attrFile;
#endif
    if( rc ) {
        return( __set_errno_dos( rc ) );
    }
    if( (pmode & ACCESS_WR) && (attr & _A_RDONLY) ) {
        return( __set_errno_dos( ERROR_ACCESS_DENIED ) );   /* invalid access mode */
    }
    return( 0 );
}
