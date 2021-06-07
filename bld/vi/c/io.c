/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "vi.h"
#include "posix.h"
#include <fcntl.h>
#include <errno.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
#include <fnmatch.h>
#endif

#include "clibext.h"


#ifdef __UNIX__
    #define FNMATCH_FLAGS   (FNM_NOESCAPE)
#else
    #define FNMATCH_FLAGS   (FNM_NOESCAPE | FNM_IGNORECASE)
#endif

static int closeAFile( void );

/*
 * FileExists - test if a file exists
 */
vi_rc FileExists( const char *name )
{
    int     i, en;
    vi_rc   rc;

    for( ;; ) {
        i = open( name, O_RDWR | O_BINARY );
        if( i == - 1 ) {
            en = errno;
            if( en == -1 ) {
                en = ENOENT;    /* CLIB BUG in OS2 libraries */
            }
            if( en == ENOENT ) {
                return( ERR_NO_ERR );
            }
            if( en == EACCES )  {
                return( ERR_READ_ONLY_FILE );
            }
            if( en == EIO ) {
                /*
                 * Trying to open file as writable in read only network share will cause EIO, so
                 * try to open it as read only to determine that share is read only.
                 */
                i = open( name, O_RDONLY | O_BINARY );
                if( i == - 1 ) {
                    en = errno;
                    if( en == -1 ) {
                        en = ENOENT;    /* CLIB BUG in OS2 libraries */
                    }
                    if( en == EMFILE ) {
                        closeAFile();
                    }
                    return( ERR_FILE_OPEN );
                } else {
                    /* If got owe did success now open file, report it as a read only */
                    close( i );
                    return( ERR_READ_ONLY_FILE );
                }
            }
            if( en != EMFILE ) {
                return( ERR_FILE_OPEN );
            }
            rc = closeAFile();
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
        } else {
            if( isatty( i ) ) {
                rc = ERR_TTY_FILE;
            } else {
                rc = ERR_FILE_EXISTS;
            }
            close( i );
            return( rc );
        }
    }

} /* FileExists */

/*
 * FileOpen - open a file, conditional on exist flag
 */
vi_rc FileOpen( const char *name, bool existflag, int stat, int attr, int *_handle )
{
    int         handle, en;
    vi_rc       rc;

    /*
     * test if file exists
     */
    if( existflag ) {
        rc = FileExists( name );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
    }

    /*
     * try to open, check for problems
     */
    for( ;; ) {
        handle = open( name, stat, attr );
        en = errno;
        if( en == -1 )
            en = ENOENT;     /* CLIB BUG in OS2 libraries */
        if( handle < 0 && en != ENOENT ) {
            if( en != EMFILE ) {
                return( ERR_FILE_OPEN );
            }
            rc = closeAFile();
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
        } else {
            if( handle >= 0 && isatty( handle ) ) {
                close( handle );
                return( ERR_TTY_FILE );
            }
            break;
        }
    }

    *_handle = handle;
    return( ERR_NO_ERR );

} /* FileOpen */

/*
 * FileSeek - seek location in the file
 */
vi_rc FileSeek( int handle, long where )
{
    long        i, relo, lastpos;

    lastpos = lseek( handle, 0, SEEK_CUR );
    if( lastpos == -1L ) {
        return( ERR_FILE_SEEK );
    }
    relo = where - lastpos;
    if( where > labs( relo ) ) {
        i = lseek( handle, relo, SEEK_CUR );
    } else {
        i = lseek( handle, where, SEEK_SET );
    }

    if( i == -1L ) {
        return( ERR_FILE_SEEK );
    }
    return( ERR_NO_ERR );

} /* FileSeek */

/*
 * closeAFile - close an open file to free up file entries
 */
static int closeAFile( void )
{
    info    *cinfo;

    for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
        if( cinfo->CurrentFile->handle >= 0 ) {
            close( cinfo->CurrentFile->handle );
            cinfo->CurrentFile->handle = -1;
            return( ERR_NO_ERR );
        }
    }
    return( ERR_FILE_CLOSE );

} /* closeAFile */

/*
 * GetFromEnvAndOpen - search env and fopen a file
 */
FILE *GetFromEnvAndOpen( const char *path )
{
    char        tmppath[_MAX_PATH];

    GetFromEnv( path, tmppath );
    if( tmppath[0] != '\0' ) {
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * GetFromEnv - get file name from environment
 */
void GetFromEnv( const char *what, char *path )
{
    _searchenv( what, "EDPATH", path );
    if( path[0] != '\0' ) {
        return;
    }
    _searchenv( what, "PATH", path );

} /* GetFromEnv */

/*
 * FileLower - change case of the file name
 */
void FileLower( char *str )
{
#ifndef __UNIX__
    strlwr( str );
#else
    /* unused parameters */ (void)str;
#endif

} /* FileLower */

/*
 * FileTemplateMatch - check if name falls in subset of template
 */
bool FileTemplateMatch( const char *name, const char *template )
{
    return( fnmatch( template, name, FNMATCH_FLAGS ) == 0 );

} /* FileTemplateMatch */

/*
 * StripPath - return pointer to where actual filename begins
 */
char *StripPath( const char *name )
{
    const char  *ptr;

    if( name == NULL ) {
        return( NULL );
    }
    ptr = name + strlen( name ) - 1;

    while( ptr != name && (*ptr != '\\' && *ptr != '/') ) {
        ptr--;
    }
    if( *ptr == '\\' ) {
        ptr++;
    }

    return( (char *)ptr );

} /* StripPath */
