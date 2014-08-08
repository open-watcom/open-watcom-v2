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
* Description:  Installer data source file I/O routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "setupio.h"
#include "zip.h"


enum ds_type {
    DS_INVALID,
    DS_FILE,
    DS_ZIP
};

typedef struct data_source_t {
    enum ds_type            type;
    union {
        int                 fhandle;
        struct zip_file     *zf;
    };
} data_source;

static enum ds_type     srcType;
static struct zip       *srcZip;


/* At the moment the incoming path may have either forward or backward
 * slashes as path separators. However, ziplib only likes forward slashes,
 * so we must manually flip them.
 */
static char *flipBackSlashes( const char *old_path )
{
    char    *new_path;

    new_path = strdup( old_path );
    if( new_path != NULL ) {
        char        *s;

        /* Need to flip slashes - at the moment they may be
         * forward or backward, and ziplib requires forward
         * slashes only.
         */
        s = new_path;
        while( *s ) {
            if( *s == '\\' )
                *s = '/';
            ++s;
        }
    }
    return( new_path );
}

extern int FileInit( const char *archive )
{
    int             zerr;

    /* Attempt to open a ZIP archive */
    srcZip = zip_open( archive, 0, &zerr );
    if( srcZip != NULL ) {
        srcType = DS_ZIP;
    } else {
        srcType = DS_FILE;
    }
    return( 0 );
}


extern int FileFini( void )
{
    if( srcType == DS_ZIP ) {
        zip_close( srcZip );
    }
    return( 0 );
}


extern int FileIsPlainFS( void )
{
    return( srcType == DS_FILE );
}


extern int FileIsArchive( void )
{
    return( srcType == DS_ZIP );
}


extern int FileStat( const char *path, struct stat *buf )
{
    int                 rc;
    struct zip_stat     zs;

    rc = -1;
    if( srcType == DS_ZIP ) {
        char        *alt_path;

        /* First try a file inside a ZIP archive */
        alt_path = flipBackSlashes( path );
        if( alt_path != NULL ) {
            rc = zip_stat( srcZip, alt_path, 0, &zs );
            if( rc == 0 ) {
                memset( buf, 0, sizeof( *buf ) );
                buf->st_ino   = zs.index;
                buf->st_size  = zs.size;
                buf->st_mtime = zs.mtime;
            }
            free( alt_path );
        }
    }
    if( rc != 0 ) {
        /* If that fails, try local file */
        rc = stat( path, buf );
    }
    return( rc );
}


extern void *FileOpen( const char *path, int flags )
{
    data_source     *ds;
    char            *alt_path;

    ds = malloc( sizeof( *ds ) );
    if( ds == NULL )
        return( NULL );

    ds->zf = NULL;
    if( srcType == DS_ZIP ) {
        /* First try opening the file inside a ZIP archive */
        alt_path = flipBackSlashes( path );
        if( alt_path != NULL ) {
            ds->zf = zip_fopen( srcZip, alt_path, 0 );
            ds->type = DS_ZIP;
            free( alt_path );
        }
    }
    if( ds->zf == NULL ) {
        /* If that fails, try opening the file directly */
        ds->fhandle = open( path, flags );
        ds->type = DS_FILE;
    }
    if( ds->type == DS_FILE && ds->fhandle == -1 ) {
        free( ds );
        ds = NULL;
    }
    return( ds );
}


extern int FileClose( void *handle )
{
    data_source     *ds = handle;
    int             rc;

    switch( ds->type ) {
    case DS_FILE:
        rc = close( ds->fhandle );
        break;
    case DS_ZIP:
        rc = zip_fclose( ds->zf );
        break;
    default:
        rc = -1;
    }

    free( ds );
    return( rc );
}


extern long FileSeek( void *handle, long offset, int origin )
{
    data_source     *ds = handle;
    long            pos;

    switch( ds->type ) {
    case DS_FILE:
        pos = lseek( ds->fhandle, offset, origin );
        break;
    case DS_ZIP:
        /* I really want to be able to seek! */
    default:
        pos = -1;
    }

    return( pos );
}


extern size_t FileRead( void *handle, void *buffer, size_t length )
{
    data_source     *ds = handle;
    size_t          amt;

    switch( ds->type ) {
    case DS_FILE:
        amt = read( ds->fhandle, buffer, length );
        break;
    case DS_ZIP:
        amt = zip_fread( ds->zf, buffer, length );
        break;
    default:
        amt = 0;
    }

    return( amt );
}
