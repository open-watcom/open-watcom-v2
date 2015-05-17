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
* Description:  Implementation of (_w)getcwd for DOS, WINDOWS
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <direct.h>
#include <string.h>
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif
#include "rtdata.h"
#include "rterrno.h"
#include "liballoc.h"
#include "_direct.h"
#include "_doslfn.h"


_WCRTLINK CHAR_TYPE *__F_NAME(getcwd,_wgetcwd)( CHAR_TYPE *buf, size_t size )
/***************************************************************************/
{
    int         len;
#ifdef __WIDECHAR__
    char        cwd[MB_CUR_MAX * _MAX_PATH];       /* multi-byte chars */
#else
    char        cwd[_MAX_PATH];                    /* single-byte chars */
#endif

    __null_check( buf, 1 );
    if( __getdcwd( &cwd[3], 0 ) ) {
        _RWD_errno = ENOENT;      /* noent? */
        return( NULL );
    }

    /* get current drive and insert into cwd[0] */
    cwd[0] = TinyGetCurrDrive() + 'A';
    cwd[1] = ':';
    cwd[2] = '\\';
    len = __F_NAME(strlen,_mbslen)( cwd ) + 1;
    if( buf == NULL ) {
        if( (buf = lib_malloc( max( size, len ) * CHARSIZE )) == NULL ) {
            _RWD_errno = ENOMEM;
            return( NULL );
        }
        size = len;
    }

    /*** Copy the pathname into a buffer and quit ***/
#ifdef __WIDECHAR__
    if( mbstowcs( buf, cwd, size ) == -1 ) {
        return( NULL );
    }
    return( buf );
#else
    return( strncpy( buf, cwd, size ) );
#endif
}
