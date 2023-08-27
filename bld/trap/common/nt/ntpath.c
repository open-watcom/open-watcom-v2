/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <windows.h>
#include "digtypes.h"
#include "ntpath.h"


char *StrCopyDst( const char *src, char *dst )
{
    while( (*dst = *src++) != '\0' ) {
        dst++;
    }
    return( dst );
}

const char *StrCopySrc( const char *src, char *dst )
{
    while( (*dst++ = *src) != '\0' ) {
        src++;
    }
    return( src );
}

int tryPath( const char *name, char *end, const char *ext_list )
{
    HANDLE  h;

    do {
        ext_list = StrCopySrc( ext_list, end ) + 1;
        h = CreateFile( name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if( h != INVALID_HANDLE_VALUE ) {
            CloseHandle( h );
            return( 0 );
        }
    } while( *ext_list != '\0' );
    return( -1 );
}

unsigned long FindFilePath( dig_filetype file_type, const char *pgm, char *buffer )
{
    const char      *p;
    char            *p2;
    bool            has_ext;
    bool            has_path;
    char            *envbuf;
    DWORD           envlen;
    unsigned long   rc;
    const char      *ext_list;

    has_ext = false;
    has_path = false;
    for( p = pgm, p2 = buffer; (*p2 = *p) != 0; ++p, ++p2 ) {
        switch( *p ) {
        case '\\':
        case '/':
        case ':':
            has_path = true;
            has_ext = false;
            break;
        case '.':
            has_ext = true;
            break;
        }
    }
    ext_list = "\0";
    if( !has_ext && file_type == DIG_FILETYPE_EXE ) {
        ext_list = ".com\0.exe\0";
    }
    if( !tryPath( buffer, p2, ext_list ) ) {
        return( 0 );
    }
    if( has_path ) {
        return( ERROR_FILE_NOT_FOUND );
    }
    envlen = GetEnvironmentVariable( "PATH", NULL, 0 );
    if( envlen == 0 )
        return( GetLastError() );
    envbuf = LocalAlloc( LMEM_FIXED, envlen );
    GetEnvironmentVariable( "PATH", envbuf, envlen );
    rc = ERROR_FILE_NOT_FOUND;
    for( p = envbuf; *p != '\0'; ++p ) {
        p2 = buffer;
        while( *p != '\0' && *p != ';' ) {
            *p2++ = *p++;
        }
        if( p2 != buffer && p2[-1] != '\\' && p2[-1] != '/' ) {
            *p2++ = '\\';
        }
        p2 = StrCopyDst( pgm, p2 );
        if( !tryPath( buffer, p2, ext_list ) ) {
            rc = 0;
            break;
        }
        if( *p == '\0' ) {
            break;
        }
    }
    LocalFree( envbuf );
    return( rc );
}
