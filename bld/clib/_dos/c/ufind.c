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
#include <dos.h>
#include <mbstring.h>
#include <stdlib.h>
#include <string.h>
#include "seterrno.h"

// define code as in Win32 or OS/2
#define ERROR_FILE_NOT_FOUND 2

_WCRTLINK unsigned _wdos_findfirst( const wchar_t *path, unsigned attr, struct _wfind_t *buf )
{
    int                 rc;
    struct find_t       mbBuf;
    char                mbPath[MB_CUR_MAX * _MAX_PATH];

    /*** Find using MBCS buffer ***/
    if( wcstombs( mbPath, path, sizeof( mbPath ) ) == -1 ) {
        mbPath[0] = '\0';
    }
    rc = _dos_findfirst( mbPath, attr, &mbBuf );
    if( rc == 0 ) {
        /*** Transfer returned info to _wfind_t buffer ***/
        memcpy( buf, &mbBuf, sizeof( struct find_t ) );
        if( mbstowcs( buf->name, mbBuf.name, sizeof( buf->name ) / sizeof( wchar_t ) ) == -1 ) {
            return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );
        }
    }
    return( rc );
}


_WCRTLINK unsigned _wdos_findnext( struct _wfind_t *buf )
{
    int                 rc;
    struct find_t       mbBuf;

    /*** Find using MBCS buffer ***/
    memcpy( &mbBuf, buf, sizeof( struct find_t ) );
    if( wcstombs( mbBuf.name, buf->name, sizeof( mbBuf.name ) ) == -1 )
        return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );

    rc = _dos_findnext( &mbBuf );
    if( rc == 0 ) {
        /*** Transfer returned info to _wfind_t buffer ***/
        memcpy( buf, &mbBuf, sizeof( struct find_t ) );
        if( mbstowcs( buf->name, mbBuf.name, sizeof( buf->name ) / sizeof( wchar_t ) ) == -1 ) {
            return( __set_errno_dos( ERROR_FILE_NOT_FOUND ) );
        }
    }
    return( rc );
}


_WCRTLINK unsigned _wdos_findclose( struct _wfind_t *buf )
{
    return( _dos_findclose( (struct find_t *)buf ) );
}
