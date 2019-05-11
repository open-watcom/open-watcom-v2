/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#if defined( _M_I86 )
    #undef  USE_ZIP     /* ziplib and zlib code don't support 16-bit architectures */
#else
    #define USE_ZIP
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "setup.h"
#include "setupio.h"
#ifdef USE_ZIP
    #include "zip.h"
#endif


typedef enum ds_type {
    DS_INVALID,
    DS_FILE,
    DS_ZIP
} ds_type;

typedef struct file_handle_t {
    ds_type                 type;
    union {
        FILE                *fp;
#ifdef USE_ZIP
        struct zip_file     *zf;
#endif
    } u;
} *file_handle;

static ds_type          srcType;

#ifdef USE_ZIP
static struct zip       *srcZip;

/* At the moment the incoming path may have either forward or backward
 * slashes as path separators. However, ziplib only likes forward slashes,
 * so we must manually flip them.
 */
static void flipBackSlashes( const VBUF *old_path, VBUF *new_path )
{
    char        *s;

    VbufSetVbuf( new_path, old_path );
    /* Need to flip slashes - at the moment they may be
     * forward or backward, and ziplib requires forward
     * slashes only.
     */
    s = (char *)VbufString( new_path );
    while( *s != '\0' ) {
        if( *s == '\\' )
            *s = '/';
        ++s;
    }
}
#endif

int FileInit( const VBUF *archive )
{
#ifdef USE_ZIP
    int             zerr;

    /* Attempt to open a ZIP archive */
    srcZip = zip_open_vbuf( archive, 0, &zerr );
    if( srcZip != NULL ) {
        srcType = DS_ZIP;
    } else {
        srcType = DS_FILE;
    }
#else
    srcType = DS_FILE;
#endif
    return( 0 );
}


int FileFini( void )
{
#ifdef USE_ZIP
    if( srcType == DS_ZIP ) {
        zip_close( srcZip );
    }
#endif
    return( 0 );
}


int FileIsPlainFS( void )
{
    return( srcType == DS_FILE );
}


int FileIsArchive( void )
{
    return( srcType == DS_ZIP );
}


int FileStat( const VBUF *path, struct stat *buf )
{
    int                 rc;
#ifdef USE_ZIP
    struct zip_stat     zs;

    rc = -1;
    if( srcType == DS_ZIP ) {
        VBUF    alt_path;

        VbufInit( &alt_path );

        /* First try a file inside a ZIP archive */
        flipBackSlashes( path, &alt_path );
        if( VbufLen( &alt_path ) > 0 ) {
            rc = zip_stat( srcZip, VbufString( &alt_path ), 0, &zs );
            if( rc == 0 ) {
                memset( buf, 0, sizeof( *buf ) );
                buf->st_ino   = zs.index;
                buf->st_size  = zs.size;
                buf->st_mtime = zs.mtime;
            }
        }
        VbufFree( &alt_path );
    }
    if( rc != 0 ) {
#endif
        /* If that fails, try local file */
        rc = stat_vbuf( path, buf );
#ifdef USE_ZIP
    }
#endif
    return( rc );
}


file_handle FileOpen( const VBUF *path, const char *flags )
{
    file_handle     fh;

    fh = malloc( sizeof( *fh ) );
    if( fh == NULL )
        return( NULL );

#ifdef USE_ZIP
    fh->u.zf = NULL;
    if( srcType == DS_ZIP ) {
        VBUF    alt_path;

        VbufInit( &alt_path );

        /* First try opening the file inside a ZIP archive */
        flipBackSlashes( path, &alt_path );
        if( VbufLen( &alt_path ) > 0 ) {
            fh->u.zf = zip_fopen( srcZip, VbufString( &alt_path ), 0 );
            fh->type = DS_ZIP;
        }
        VbufFree( &alt_path );
    }
    if( fh->u.zf == NULL ) {
#endif
        /* If that fails, try opening the file directly */
        fh->u.fp = fopen_vbuf( path, flags );
        fh->type = DS_FILE;
#ifdef USE_ZIP
    }
#endif
    if( fh->type == DS_FILE && fh->u.fp == NULL ) {
        free( fh );
        fh = NULL;
    }
    return( fh );
}


int FileClose( file_handle fh )
{
    int             rc;

    switch( fh->type ) {
    case DS_FILE:
        rc = fclose( fh->u.fp );
        break;
#ifdef USE_ZIP
    case DS_ZIP:
        rc = zip_fclose( fh->u.zf );
        break;
#endif
    default:
        rc = -1;
    }

    free( fh );
    return( rc );
}


long FileSeek( file_handle fh, long offset, int origin )
{
    long            pos;

    switch( fh->type ) {
    case DS_FILE:
        if( fseek( fh->u.fp, offset, origin ) ) {
            pos = -1;
        } else {
            pos = ftell( fh->u.fp );
        }
        break;
#ifdef USE_ZIP
    case DS_ZIP:
        /* I really want to be able to seek! */
#endif
    default:
        pos = -1;
    }

    return( pos );
}


size_t FileRead( file_handle fh, void *buffer, size_t length )
{
    size_t          amt;

    switch( fh->type ) {
    case DS_FILE:
        amt = fread( buffer, 1, length, fh->u.fp );
        break;
#ifdef USE_ZIP
    case DS_ZIP:
        amt = zip_fread( fh->u.zf, buffer, length );
        break;
#endif
    default:
        amt = 0;
    }

    return( amt );
}
