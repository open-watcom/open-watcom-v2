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


#include "vi.h"
#include "posix.h"
#include <fcntl.h>
#include <errno.h>
#include "clibext.h"

static int closeAFile( void );

/*
 * FileExists - test if a file exists
 */
vi_rc FileExists( char *name )
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
            if ( en == EIO ) {
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
vi_rc FileOpen( char *name, int existflag, int stat, int attr, int *_handle )
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
        if( en == -1 ) en = ENOENT;     /* CLIB BUG in OS2 libraries */
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
 * FileSeek - seek location in swap file
 */
vi_rc FileSeek( int handle, long where )
{
    long        i, relo, lastpos;

    lastpos = tell( handle );
    if( lastpos < 0 ) {
        return( ERR_FILE_SEEK );
    }
    relo = where - lastpos;
    if( where > labs( relo ) ) {
        i = lseek( handle, relo, SEEK_CUR );
    } else {
        i = lseek( handle, where, SEEK_SET );
    }

    if( i == -1 ) {
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
    char        tmppath[FILENAME_MAX];

    GetFromEnv( path, tmppath );
    if( tmppath[0] != 0 ) {
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
    if( path[0] != 0 ) {
        return;
    }
    _searchenv( what, "PATH", path );

} /* GetFromEnv */

#if defined(__UNIX__)
static char altTmpDir[] = "/tmp";
#else
static char altTmpDir[] = "c:";
#endif

/*
 * VerifyTmpDir - make sure TmpDir is valid
 */
void VerifyTmpDir( void )
{
    int     i;
    char    *env_tmpdir;

    if( EditVars.TmpDir != NULL ) {
        i = strlen( EditVars.TmpDir ) - 1;
        if( EditVars.TmpDir[i] == FILE_SEP && i > 2 ) {
            /* this sucks -- we need the '\' IFF it is [drive]:\ */
            EditVars.TmpDir[i] = 0;
        }
        if( IsDirectory( EditVars.TmpDir ) ) {
            /* strip the following file_sep char for [drive]:\ */
            if( EditVars.TmpDir[i] == FILE_SEP ) {
                EditVars.TmpDir[i] = 0;
            }
            return;
        }
    }
    env_tmpdir = getenv( "tmp" );
    if( env_tmpdir != NULL ) {
        if( env_tmpdir[strlen( env_tmpdir ) - 1] == '\\' ) {
            char buf[FILENAME_MAX];
            strcpy( buf, env_tmpdir );
            buf[strlen( buf ) - 1] = '\0';
            AddString2( &EditVars.TmpDir, buf );
        } else {
            AddString2( &EditVars.TmpDir, env_tmpdir );
        }
    } else {
        // _mkdir( altTmpDir, DIRFLAGS );
        AddString2( &EditVars.TmpDir, altTmpDir );
    }

} /* VerifyTmpDir */

/*
 * MakeTmpPath - make a path to a file from TmpDir
 */
void MakeTmpPath( char *out, char *in )
{
    out[0] = 0;
    if( EditVars.TmpDir == NULL ) {
        char *env_tmpdir = getenv( "tmp" );
        if( env_tmpdir != NULL ) {
            StrMerge( 3, out, env_tmpdir, FILE_SEP_STR, in );
        } else {
            StrMerge( 3, out, altTmpDir, FILE_SEP_STR, in );
        }
    } else {
        StrMerge( 3, out, EditVars.TmpDir, FILE_SEP_STR, in );
    }

} /* MakeTmpPath */

/*
 * TmpFileOpen - open a tmp file
 */
vi_rc TmpFileOpen( char *inname, int *_handle )
{
    char        file[FILENAME_MAX];

    tmpnam( inname );
    MakeTmpPath( file, inname );
    return( FileOpen( file, FALSE, O_TRUNC | O_RDWR | O_BINARY | O_CREAT, PMODE_RW, _handle ) );

} /* TmpFileOpen */

/*
 * TmpFileClose - close and delete a tmp file
 */
void TmpFileClose( int handle, char *name )
{
    char        file[FILENAME_MAX];

    if( handle < 0 ) {
        return;
    }
    close( handle );
    MakeTmpPath( file, name );
    remove( file );

} /* TmpFileClose */

/*
 * FileLower - change case of the file name
 */
void FileLower( char *str )
{
#ifndef __UNIX__
        strlwr( str );
#else
        str = str;
#endif

} /* FileLower */

/*
 * FileTemplateMatch - check if name falls in subset of template
 */
bool FileTemplateMatch( const char *name, const char *template )
{
    bool    inExtension = FALSE;

    for( ;; ) {
        if( *template == '*' ) {
            if( inExtension == FALSE ) {
                while( *(name + 1) && *(name + 1) != '.' ) {
                    name++;
                }
                inExtension = TRUE;
            } else {
                return( TRUE );
            }
#ifndef __UNIX__
        } else if( *template != '?' && tolower( *template ) != tolower( *name ) ) {
#else
        } else if( *template != '?' && *template != *name ) {
#endif
            return( FALSE );
        }
        name++;
        template++;
        if( *template == '\0' || *name == '\0' ) {
            break;
        }
    }

#ifndef __UNIX__
    if( tolower( *template ) == tolower( *name ) ) {
#else
    if( *template == *name ) {
#endif
        return( TRUE );
    }
    return( FALSE );

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
