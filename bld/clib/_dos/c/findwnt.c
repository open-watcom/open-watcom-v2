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
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <direct.h>
#include <dos.h>
#include <windows.h>
#include "rtdata.h"
#include "ntex.h"
#include "seterrno.h"


_WCRTLINK unsigned _dos_findfirst( const char *path, unsigned attr, struct find_t *buf )
{
    HANDLE              h;
    int                 error;
    WIN32_FIND_DATA     ffb;

    h = FindFirstFile( (LPTSTR)path, &ffb );

    if( h == (HANDLE)-1 ) {
        HANDLE_OF( buf ) = BAD_HANDLE;
        error = GetLastError();
        __set_errno_dos( error );
        return( error );
    }
//  if( attr == _A_NORMAL ) {
//      attr = ~(_A_SUBDIR|_A_VOLID);
//  }
    if( !__NTFindNextFileWithAttr( h, attr, &ffb ) ) {
        error = GetLastError();
        __set_errno_dos( error );
        HANDLE_OF( buf ) = BAD_HANDLE;
        FindClose( h );
        return( error );
    }
    HANDLE_OF( buf ) = h;
    ATTR_OF( buf ) = attr;
    __GetNTDirInfo( (struct dirent *) buf, &ffb );

    return( 0 );
}

_WCRTLINK unsigned _dos_findnext( struct find_t *buf )
{
    int                 error;
    WIN32_FIND_DATA     ffd;

    if( !FindNextFile( HANDLE_OF( buf ), &ffd ) ) {
        error = GetLastError();
        __set_errno_dos( error );
        return( error );
    }
    if( !__NTFindNextFileWithAttr( HANDLE_OF( buf ), ATTR_OF( buf ), &ffd ) ) {
        error = GetLastError();
        __set_errno_dos( error );
        return( error );
    }
    __GetNTDirInfo( (struct dirent *) buf, &ffd );

    return( 0 );
}

_WCRTLINK unsigned _dos_findclose( struct find_t *buf )
{
    int                 error;

    if( HANDLE_OF( buf ) != BAD_HANDLE ) {
        if( !FindClose( HANDLE_OF( buf ) ) ) {
            error = GetLastError();
            __set_errno_dos( error );
            return( error );
        }
    }
    return( 0 );
}
