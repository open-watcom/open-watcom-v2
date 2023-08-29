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


#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <sys/stat.h>
#include "digtypes.h"
#include "lnxpath.h"
#include "servio.h"


char *StrCopyDst( const char *src, char *dst )
{
    while( (*dst = *src++) != '\0' ) {
        dst++;
    }
    return( dst );
}

static size_t tryOnePath( const char *path, struct stat *tmp, const char *name, char *result )
{
    char        *ptr;

    if( path != NULL && *path != '\0' ) {
        ptr = result;
        for( ; *path != '\0'; path++ ) {
            switch( *path ) {
            case ':':
                if( path[1] == ':' )
                    continue;
                if( ptr == result )
                    continue;
                if( ptr[-1] != '/' )
                    *ptr++ = '/';
                ptr = StrCopyDst( name, ptr );
                if( stat( result, tmp ) == 0 )
                    return( ptr - result );
                ptr = result;
                break;
            case ' ':
            case '\t':
                break;
            default:
                *ptr++ = *path;
                break;
            }
        }
        if( ptr != result ) {
            if( ptr[-1] != '/' )
                *ptr++ = '/';
            ptr = StrCopyDst( name, ptr );
            if( stat( result, tmp ) == 0 ) {
                return( ptr - result );
            }
        }
    }
    *result = '\0';
    return( 0 );
}

size_t FindFilePath( dig_filetype file_type, const char *name, char *result )
{
    struct stat tmp;
    size_t      len;
#if defined( SERVER )
    char        *end;
    char        cmd[256];
#endif

    if( stat( name, &tmp ) == 0 ) {
        return( StrCopyDst( name, result ) - result );
    }
#ifdef BLDVER
    len = tryOnePath( getenv( "WD_PATH" QSTR( BLDVER ) ), &tmp, name, result );
    if( len != 0 )
        return( len );
#endif
    if( file_type == DIG_FILETYPE_EXE ) {
        return( tryOnePath( getenv( "PATH" ), &tmp, name, result ) );
    } else {
        len = tryOnePath( getenv( "WD_PATH" ), &tmp, name, result );
        if( len != 0 )
            return( len );
        len = tryOnePath( getenv( "HOME" ), &tmp, name, result );
        if( len != 0 )
            return( len );
#if defined( SERVER )
        if( _cmdname( cmd ) != NULL ) {
            end = strrchr( cmd, '/' );
            if( end != NULL ) {
                *end = '\0';
                /* look in the executable's directory */
                len = tryOnePath( cmd, &tmp, name, result );
                if( len != 0 )
                    return( len );
                end = strrchr( cmd, '/' );
                if( end != NULL ) {
                    /* look in the wd sibling directory of where the command
                       came from */
                    StrCopyDst( "wd", end + 1 );
                    len = tryOnePath( cmd, &tmp, name, result );
                    if( len != 0 ) {
                        return( len );
                    }
                }
            }
        }
#endif
        return( tryOnePath( "/opt/watcom/wd", &tmp, name, result ) );
    }
}
