/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#define USE_ZIP
//#define USE_LZMA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "setup.h"
#include "setupio.h"
#if defined( USE_ZIP )
    #include "zip.h"
#elif defined( USE_LZMA )
#endif


typedef enum ds_type {
    DS_INVALID,
    DS_FILE,
    DS_ZIP,
    DS_LZMA
} ds_type;

typedef struct file_handle_t {
    ds_type             type;
    union {
        FILE            *fp;
#if defined( USE_ZIP )
        struct zip_file *zf;
#elif defined( USE_LZMA )
#endif
    } u;
    char                *textbuf;
    char                *textpos;
    char                *textend;
} file_handle_t;

static ds_type          srcType;

#if defined( USE_ZIP )
static struct zip       *srcZip;

/*
 * At the moment the incoming path may have either forward or backward
 * slashes as path separators. However, ziplib only likes forward slashes,
 * so we must manually flip them.
 */
static void flipBackSlashes( const VBUF *old_path, VBUF *new_path )
{
    char        *s;

    VbufSetVbuf( new_path, old_path );
    /*
     * Need to flip slashes - at the moment they may be
     * forward or backward, and ziplib requires forward
     * slashes only.
     */
    for( s = (char *)VbufString( new_path ); *s != '\0'; s++ ) {
        if( *s == '\\' ) {
            *s = '/';
        }
    }
}
#elif defined( USE_LZMA )
#endif

int FileInit( const VBUF *archive )
{
#if defined( USE_ZIP )
    int             zerr;

    /* Attempt to open a ZIP archive */
    srcZip = zip_open_vbuf( archive, 0, &zerr );
    if( srcZip != NULL ) {
        srcType = DS_ZIP;
    } else {
        srcType = DS_FILE;
    }
#elif defined( USE_LZMA )
    /* unused parameters */ (void)archive;

    srcType = DS_LZMA;
#else
    /* unused parameters */ (void)archive;

    srcType = DS_FILE;
#endif
    return( 0 );
}


int FileFini( void )
{
#if defined( USE_ZIP )
    if( srcType == DS_ZIP ) {
        zip_close( srcZip );
    }
#elif defined( USE_LZMA )
#endif
    return( 0 );
}


int FileIsPlainFS( void )
{
    return( srcType == DS_FILE );
}


int FileIsArchive( void )
{
#if defined( USE_ZIP )
    return( srcType == DS_ZIP );
#elif defined( USE_LZMA )
    return( srcType == DS_LZMA );
#else
    return( 0 );
#endif
}


int FileStat( const VBUF *path, struct stat *statbuf )
{
    int                 rc;
#if defined( USE_ZIP )
    struct zip_stat     zs;

    rc = -1;
    if( srcType == DS_ZIP ) {
        VBUF    alt_path;

        VbufInit( &alt_path );
        /*
         * First try a file inside a ZIP archive
         */
        flipBackSlashes( path, &alt_path );
        if( VbufLen( &alt_path ) > 0 ) {
            rc = zip_stat( srcZip, VbufString( &alt_path ), 0, &zs );
            if( rc == 0 ) {
                memset( statbuf, 0, sizeof( *statbuf ) );
                statbuf->st_ino   = zs.index;
                statbuf->st_size  = zs.size;
                statbuf->st_mtime = zs.mtime;
            }
        }
        VbufFree( &alt_path );
    }
    if( rc != 0 ) {
#elif defined( USE_LZMA )
#endif
        /*
         * If that fails, try local file
         */
        rc = stat_vbuf( path, statbuf );
#if defined( USE_ZIP )
    }
#elif defined( USE_LZMA )
#endif
    return( rc );
}


file_handle FileOpen( const VBUF *path, data_mode mode )
{
    file_handle     afh;

    (void)mode; /* unused parameters */

    afh = malloc( sizeof( *afh ) );
    if( afh == NULL )
        return( NULL );

    afh->textbuf = NULL;
    afh->textpos = NULL;
    afh->textend = NULL;
    if( mode == DATA_TEXT )
        afh->textbuf = malloc( TEXTBUF_SIZE );
#if defined( USE_ZIP )
    afh->u.zf = NULL;
    if( srcType == DS_ZIP ) {
        VBUF    alt_path;

        VbufInit( &alt_path );
        /*
         * First try opening the file inside a ZIP archive
         */
        flipBackSlashes( path, &alt_path );
        if( VbufLen( &alt_path ) > 0 ) {
            afh->u.zf = zip_fopen( srcZip, VbufString( &alt_path ), 0 );
            afh->type = DS_ZIP;
        }
        VbufFree( &alt_path );
    }
    if( afh->u.zf == NULL ) {
#elif defined( USE_LZMA )
#endif
        /*
         * If that fails, try opening the file directly
         */
        afh->u.fp = fopen_vbuf( path, "rb" );
        afh->type = DS_FILE;
#if defined( USE_ZIP )
    }
#elif defined( USE_LZMA )
#endif
    if( afh->type == DS_FILE
      && afh->u.fp == NULL ) {
        if( afh->textbuf != NULL )
            free( afh->textbuf );
        free( afh );
        afh = NULL;
    }
    return( afh );
}


int FileClose( file_handle afh )
{
    int             rc;

    switch( afh->type ) {
    case DS_FILE:
        rc = fclose( afh->u.fp );
        break;
#if defined( USE_ZIP )
    case DS_ZIP:
        rc = zip_fclose( afh->u.zf );
        break;
#elif defined( USE_LZMA )
#endif
    default:
        rc = -1;
    }

    if( afh->textbuf != NULL )
        free( afh->textbuf );
    free( afh );
    return( rc );
}


static size_t file_read( file_handle afh, void *buffer, size_t length )
/**********************************************************************
 * binary data mode processing
 */
{
    size_t          amt;

    switch( afh->type ) {
    case DS_FILE:
        amt = fread( buffer, 1, length, afh->u.fp );
        if( amt == 0
          && ferror( afh->u.fp ) )
            amt = (size_t)-1;
        break;
#if defined( USE_ZIP )
    case DS_ZIP:
        amt = zip_fread( afh->u.zf, buffer, length );
        if( (int)amt < 0 )
            amt = (size_t)-1;
        break;
#elif defined( USE_LZMA )
#endif
    default:
        amt = 0;
    }
    return( amt );
}

static size_t read_line( file_handle afh, char *buffer, size_t length )
/**********************************************************************
 * text data mode processing
 */
{
    char            *start;
    size_t          read_bytes;
    size_t          len;
    bool            done;

    len = 0;
    done = false;
    do {
        /*
         * Read data into text buffer if it's empty
         */
        if( afh->textpos == NULL ) {
            read_bytes = file_read( afh, afh->textbuf, TEXTBUF_SIZE );
            if( (int)read_bytes <= 0 ) {
                return( read_bytes );
            }
            afh->textpos = afh->textbuf;
            afh->textend = afh->textbuf + read_bytes;
        }
        /*
         * Look for a newline
         * check for end of source buffer and size of target buffer
         */
        start = afh->textpos;
        while( (afh->textpos[0] != '\n')
          && (afh->textpos < afh->textend)
          && ((size_t)( afh->textpos - start ) < length) ) {
            afh->textpos++;
        }
        if( afh->textpos[0] == '\n' ) {
            /*
             * Found a newline; increment past it
             */
            afh->textpos++;
            done = true;
        } else if( afh->textpos == afh->textend ) {
            /*
             * We're at the end of the buffer
             * copy what we have to output buffer
             */
            read_bytes = afh->textpos - start;
            memcpy( buffer, start, read_bytes );
            length -= read_bytes;
            buffer += read_bytes;
            len += read_bytes;
            /*
             * Force read of more data into buffer
             */
            afh->textpos = NULL;
        } else {
            /*
             * No more space in output buffer
             */
            done = true;
        }
    } while( !done );

    read_bytes = afh->textpos - start;
    memcpy( buffer, start, read_bytes );
    buffer[read_bytes] = '\0';
    len += read_bytes;
    return( len );
}

size_t FileRead( file_handle afh, void *buffer, size_t length )
{
    size_t          amt;

    if( afh->textbuf != NULL ) {
        amt = read_line( afh, buffer, length );
    } else {
        amt = file_read( afh, buffer, length );
    }
    return( amt );
}
