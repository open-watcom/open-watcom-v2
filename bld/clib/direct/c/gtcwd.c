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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "dos.h"
#include "dosfunc.h"
#include "tinyio.h"
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#include "variety.h"
#ifdef __WIDECHAR__
    #include <direct.h>
    #include <mbstring.h>
#endif


_WCRTLINK CHAR_TYPE *__F_NAME(getcwd,_wgetcwd)( CHAR_TYPE *buf, size_t size )
{
    int len;
    tiny_ret_t rc;
#ifndef __WIDECHAR__
    char cwd[_MAX_PATH];                    /* single-byte chars */
#else
    char cwd[MB_CUR_MAX*_MAX_PATH];         /* multi-byte chars */
#endif

    __null_check( buf, 1 );
    rc = TinyGetCWDir( &cwd[3], 0 );        /* leave space for 'x:\' */
    if( TINY_ERROR( rc ) ) {
        __set_errno( ENOENT );       /* noent? */
        return( NULL );
    }

    /* get current drive and insert into cwd[0] */
    cwd[0] = TinyGetCurrDrive() + 'A';
    cwd[1] = ':';
    cwd[2] = '\\';
    len = __F_NAME(strlen,_mbslen)( cwd ) + 1;
    if( buf == NULL ) {
        if( (buf = lib_malloc( max(size,len)*CHARSIZE )) == NULL ) {
            __set_errno( ENOMEM );
            return( NULL );
        }
        size = len;
    }

    /*** Copy the pathname into a buffer and quit ***/
    #ifndef __WIDECHAR__
        return( strncpy( buf, cwd, size ) );
    #else
        if( mbstowcs( buf, cwd, size ) != (size_t)-1 )
            return( buf );
        else
            return( NULL );
    #endif
}
