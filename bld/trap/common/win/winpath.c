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


#include <string.h>
#include <windows.h>
#include "digtypes.h"
#include "winpath.h"


char *StrCopyDst( const char *src, char *dst )
{
    while( (*dst = *src++) != '\0' ) {
        ++dst;
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

const char *DOSEnvFind( const char *src )
{
    const char  *p;
    const char  *env;

    env = GetDOSEnvironment();
    do {
        p = src;
        do {
            if( *p == '\0' && *env == '=' ) {
                return( env + 1 );
            }
        } while( *env++ == *p++ );
        while( *env++ != '\0' ) {
            {}
        }
    } while( *env != '\0' );
    return( NULL );
}

static bool tryPath( const char *name, char *end, const char *ext_list )
{
    tiny_ret_t  rc;
    int         mode;

    mode = 0;
    do {
        ext_list = StrCopySrc( ext_list, end ) + 1;
        rc = TinyOpen( name, mode );
        if( TINY_OK( rc ) ) {
            TinyClose( TINY_INFO( rc ) );
            return( true );
        }
    } while( *ext_list != '\0' );
    return( false );
}

size_t FindFilePath( dig_filetype file_type, const char *pgm, char *buffer )
{
    const char  __far *path;
    char        *p2;
    const char  *p3;
    bool        has_ext;
    bool        has_path;
    const char  *ext_list;

    has_ext = false;
    has_path = false;
    for( p3 = pgm, p2 = buffer; (*p2 = *p3) != '\0'; ++p3, ++p2 ) {
        switch( *p3 ) {
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
    if( tryPath( buffer, p2, ext_list ) )
        return( strlen( buffer ) );
    if( !has_path ) {
        path = DOSEnvFind( "PATH" );
        if( path != NULL ) {
            for( ; *path != '\0'; path++ ) {
                p2 = buffer;
                while( *path != '\0' && *path != ';' ) {
                    *p2++ = *path++;
                }
                if( p2 != buffer && p2[-1] != '\\' && p2[-1] != '/' ) {
                    *p2++ = '\\';
                }
                p2 = StrCopyDst( pgm, p2 );
                if( tryPath( buffer, p2, ext_list ) )
                    return( strlen( buffer ) );
                if( *path == '\0' ) {
                    break;
                }
            }
        }
    }
    *buffer = '\0';
    return( 0 );
}
