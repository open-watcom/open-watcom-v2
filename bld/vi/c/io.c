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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "posix.h"
#include <fcntl.h>
#include <errno.h>
#include "vi.h"

static int closeAFile( void );

/*
 * FileExists - test if a file exists
 */
int FileExists( char *name )
{
    int i,rc,en;

    while( TRUE ) {
        i = open( name, O_RDWR | O_BINARY, 0 );
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
            if( en != EMFILE ) {
                return( ERR_FILE_OPEN );
            }
            i = closeAFile();
            if( i ) {
                return( i );
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
int FileOpen( char *name, int existflag, int stat, int attr, int *_handle )
{
    int         i,handle,en;

    /*
     * test if file exists
     */
    if( existflag ) {
        i = FileExists( name );
        if( i ) {
            return( i );
        }
    }

    /*
     * try to open, check for problems
     */
    while( TRUE ) {
        handle = open( name, stat, attr );
        en = errno;
        if( en == -1 ) en = ENOENT;     /* CLIB BUG in OS2 libraries */
        if( handle < 0 && en != ENOENT ) {
            if( en != EMFILE ) {
                return( ERR_FILE_OPEN );
            }
            i = closeAFile();
            if( i ) {
                return( i );
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
int FileSeek( int handle, long where )
{
    long        i,relo,lastpos;

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
info *cinfo;

    cinfo = InfoHead;
    while( cinfo != NULL ) {
        if( cinfo->CurrentFile->handle >= 0 ) {
            close( cinfo->CurrentFile->handle );
            cinfo->CurrentFile->handle = -1;
            return( ERR_NO_ERR );
        }
        cinfo = cinfo->next;
    }
    return( ERR_FILE_CLOSE );

} /* closeAFile */

/*
 * GetFromEnvAndOpen - search env and fopen a file
 */
FILE *GetFromEnvAndOpen( char *path )
{
    char        tmppath[_MAX_PATH];

    GetFromEnv( path, tmppath );
    if( tmppath[0] != 0 ) {
        return( fopen( tmppath, "r" ) );
    }
    return( NULL );

} /* GetFromEnvAndOpen */

/*
 * GetFromEnv - get file name from environment
 */
void GetFromEnv( char *what, char *path )
{
    _searchenv(what,"EDPATH",path );
    if( path[0] != 0 ) {
        return;
    }
    _searchenv(what,"PATH",path );

} /* GetFromEnv */

#if defined(__QNX__)
static char altTmpDir[] = "/tmp";
#else
static char altTmpDir[] = "c:";
#endif

/*
 * VerifyTmpDir - make sure TmpDir is valid
 */
void VerifyTmpDir( void )
{
    int i;
    char *env_tmpdir;

    if( TmpDir != NULL ) {
        i = strlen( TmpDir ) - 1;
        if( TmpDir[i] == FILE_SEP && i > 2 ) {
            /* this sucks -- we need the '\' IFF it is [drive]:\ */
            TmpDir[i] = 0;
        }
        if( IsDirectory( TmpDir ) ) {
            /* strip the following file_sep char for [drive]:\ */
            if( TmpDir[i] == FILE_SEP ) {
                TmpDir[i] = 0;
            }
            return;
        }
    }
    env_tmpdir = getenv( "tmpdir" );
    if( env_tmpdir != NULL ) {
        if( env_tmpdir[strlen(env_tmpdir)-1] == '\\' ) {
            char buf[_MAX_PATH];
            strcpy( buf, env_tmpdir );
            buf[strlen(buf)-1] = '\0';
            AddString2( &TmpDir, buf );
        } else {
            AddString2( &TmpDir, env_tmpdir );
        }
    } else {
        // _mkdir( altTmpDir, DIRFLAGS );
        AddString2( &TmpDir, altTmpDir );
    }

} /* VerifyTmpDir */

/*
 * MakeTmpPath - make a path to a file from TmpDir
 */
void MakeTmpPath( char *out, char *in )
{
    out[0] = 0;
    if( TmpDir == NULL ) {
        char *env_tmpdir = getenv( "tmpdir" );
        if( env_tmpdir != NULL ) {
            StrMerge( 3, out, env_tmpdir, FILE_SEP_STR, in );
        } else {
            StrMerge( 3, out, altTmpDir, FILE_SEP_STR, in );
        }
    } else {
        StrMerge( 3, out, TmpDir, FILE_SEP_STR, in );
    }

} /* MakeTmpPath */

/*
 * TmpFileOpen - open a tmp file
 */
int TmpFileOpen( char *inname, int *_handle )
{
    char        file[_MAX_PATH];

    tmpnam( inname );
    MakeTmpPath( file, inname );
    return( FileOpen( file, FALSE, O_TRUNC | O_RDWR | O_BINARY | O_CREAT,
                0, _handle ) );

} /* TmpFileOpen */

/*
 * TmpFileClose - close and delete a tmp file
 */
void TmpFileClose( int handle, char *name )
{
    char        file[_MAX_PATH];

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
#ifndef __QNX__
        strlwr( str );
#else
        str = str;
#endif

} /* FileLower */

/*
 * FileTemplateMatch - check if name falls in subset of template
 */
bool FileTemplateMatch( char *name, char *template )
{
    bool    inExtension = FALSE;

    while( 1 ) {
        if( *template == '*' ) {
            if( inExtension == FALSE ) {
                while( *( name + 1 ) && *( name + 1 ) != '.' ) {
                    name++;
                }
                inExtension = TRUE;
            } else {
                return( TRUE );
            }
        } else if( *template != '?' && *template != *name ) {
            return( FALSE );
        }
        name++;
        template++;
        if( *template == '\0' || *name == '\0' ) {
            break;
        }
    }

    if( *template == *name ) {
        return( TRUE );
    }
    return( FALSE );
}

/*
 * StripPath - return pointer to where actual filename begins
 */
char *StripPath( char *name )
{
    char *ptr;
    if( name == NULL ) return( NULL );
    ptr = name + strlen( name ) - 1;

    while( ptr != name &&( *ptr != '\\' && *ptr != '/')  ) {
        ptr--;
    }
    if( *ptr == '\\' ) {
        ptr++;
    }

    return( ptr );
}
