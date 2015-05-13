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
#ifdef __WIDECHAR__
    #include <mbstring.h>
#endif
#include <wos2.h>
#include "rtdata.h"
#include "errorno.h"
#include "thread.h"
#include "seterrno.h"


#pragma on(stack_check);
_WCRTLINK CHAR_TYPE *__F_NAME(getcwd,_wgetcwd)( CHAR_TYPE *buf, size_t size )
{
    APIRET          error;
    ULONG           drive_map;
#ifndef __WIDECHAR__
    char            path[_MAX_PATH];            /* single-byte chars */
    OS_UINT         pathlen = _MAX_PATH - 3;
#else
    char            path[MB_CUR_MAX*_MAX_PATH]; /* multi-byte chars */
    OS_UINT         pathlen = MB_CUR_MAX * _MAX_PATH - 3;
#endif
    OS_UINT         drive;

    error = DosQCurDir( 0, &path[3], &pathlen );
    if( error ) {
        __set_errno_dos( error );
        return( NULL );
    }
    DosQCurDisk( &drive, &drive_map );
    path[ 0 ] = drive + 'A' - 1;
    path[ 1 ] = ':';
    path[ 2 ] = '\\';
    if( buf == NULL ) {
        if( (buf = malloc( max(size,pathlen+4)*CHARSIZE )) == NULL ) {
            _RWD_errno = ENOMEM;
            return( NULL );
        }
        size = pathlen + 3;
    }

    /*** Copy the pathname into a buffer and quit ***/
    #ifndef __WIDECHAR__
        return( strncpy( buf, path, size ) );
    #else
        if( mbstowcs( buf, path, size ) != (size_t)-1 )
            return( buf );
        else
            return( NULL );
    #endif
}
