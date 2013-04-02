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
* Description:  Utility to create a self-extracting ZIP archive.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "watcom.h"

#include "zipint.h"

#define BUFFER_SIZE 65536

#include "pushpck1.h"
typedef struct {
    char        signature[4];
    unsigned_16    disk_number;            // not supported by libzip
    unsigned_16    disk_having_cd;         // not supported by libzip
    unsigned_16    num_entries_on_disk;    // not supported by libzip
    unsigned_16    total_num_entries;
    unsigned_32    cd_size;
    unsigned_32    cd_offset;
    unsigned_16    comment_length;
} ZIP_END_OF_CENTRAL_DIRECTORY;

typedef struct {
    char        signature[4];
    unsigned_16    version_made_by;
    unsigned_16    version_needed;
    unsigned_16    flags;
    unsigned_16    method;
    unsigned_16    mod_time;
    unsigned_16    mod_date;
    unsigned_32    crc32;
    unsigned_32    compressed_size;
    unsigned_32    uncompressed_size;
    unsigned_16    file_name_length;
    unsigned_16    extra_field_length;
    unsigned_16    file_comment_length;
    unsigned_16    disk;                   // not supported by libzip
    unsigned_16    int_attrib;
    unsigned_32    ext_attrib;
    unsigned_32    offset;
} ZIP_CENTRAL_DIRECTORY_FILE_HEADER;
#include "poppck.h"

static const char   usage[] = "%s <target exe> <source zip> <source exe>\n";

static long find( char *buffer, size_t buffer_len, char *match, size_t match_len )
{
    char    *ptr = buffer;
    size_t  i;

    while( ptr != NULL ) {
        // find startup character
        ptr = memchr( ptr, match[0], buffer_len - (ptr - buffer) );
        if( ptr == NULL )
            break;

        // make sure there is room for our match
        if( ((ptr - buffer) + match_len) > buffer_len )
            break;

        // check for match
        for( i = 1; i < match_len; i++ ) {
            if( ptr[i] != match[i] ) {
                break;
            }
        }

        // if match found, return its position in buffer
        if( i == match_len ) {
            return( (long)( ptr - buffer ) );
        }

        // skip to next occurance
        ptr++;
    }

    return( -1L );
}


static size_t fix_cd( FILE *f, void *buffer, size_t buffer_len, long offset )
{
    ZIP_END_OF_CENTRAL_DIRECTORY        *eocd;
    ZIP_CENTRAL_DIRECTORY_FILE_HEADER   fileheader;
    long                                header_pos;
    unsigned_32                         i;

    buffer_len = buffer_len;
    eocd = buffer;

    header_pos = offset + eocd->cd_offset;

    // fixup all files in central directory
    for( i = 0; i < eocd->total_num_entries; i++ ) {
        fseek( f, header_pos, SEEK_SET );

        if( fread( &fileheader, sizeof( fileheader ), 1, f ) != 1 ) {
            return( 0 );
        }

        // make sure we are at the correct position in file
        if( find( (char *)&fileheader, 4, CENTRAL_MAGIC, 4 ) != 0) {
            return( 0 );
        }

        fileheader.offset += offset;
        fseek( f, header_pos, SEEK_SET );
        fwrite( &fileheader, sizeof( fileheader ), 1, f );

        // move to next entry
        header_pos += sizeof( fileheader );
        header_pos += fileheader.file_name_length;
        header_pos += fileheader.extra_field_length;
        header_pos += fileheader.file_comment_length;
    }
    return( 1 );
}


int main( int argc, char *argv[] )
{
    FILE        *ftarget;
    FILE        *fsrc_zip;
    FILE        *fsrc_exe;
    char        *buffer = NULL;
    char        *prg;
    char        *target;
    char        *source_zip;
    char        *source_exe;
    long        offset;
    long        length;
    size_t      n;
    long        pos;

    prg = argv[0];

    if( argc < 4 ) {
        fprintf( stderr, usage, prg );
        exit( 2 );
    }

    target     = argv[1];
    source_zip = argv[2];
    source_exe = argv[3];

    ftarget = fopen( target, "w+b" );
    if( ftarget == NULL ) {
        fprintf( stderr, "can't open '%s' for writing\n", target );
        return( 1 );
    }

    fsrc_zip = fopen( source_zip, "rb" );
    if( fsrc_zip == NULL ) {
        fprintf( stderr, "can't open '%s' for reading\n", source_zip );
        fclose( ftarget );
        return( 1 );
    }

    fsrc_exe = fopen( source_exe, "rb" );
    if( fsrc_exe == NULL ) {
        fprintf( stderr, "can't open '%s' for reading\n", source_exe );
        fclose( ftarget );
        fclose( fsrc_zip );
        return( 1 );
    }

    buffer = malloc( BUFFER_SIZE );
    if( buffer == NULL ) {
        fprintf( stderr, "not enough memory for buffer\n" );
        fclose( ftarget );
        fclose( fsrc_zip );
        fclose( fsrc_exe );
        return( 1 );
    }

    // --- copy source executable ---
    while( (n = fread( buffer, 1, BUFFER_SIZE, fsrc_exe )) != 0 ) {
        if( fwrite( buffer, 1, n, ftarget ) != n ) {
            fprintf( stderr, "can't write to '%s'\n", target );
            fclose( ftarget );
            fclose( fsrc_zip );
            fclose( fsrc_exe );
            free( buffer );
            return( 1 );
        }
    }

    // source executable is now copied
    fclose( fsrc_exe );

    // save length of executable for later usage
    offset = ftell( ftarget );

    // --- copy zip ---
    while( (n = fread( buffer, 1, BUFFER_SIZE, fsrc_zip )) != 0 ) {
        if( fwrite( buffer, 1, n, ftarget ) != n ) {
            fprintf( stderr, "can't write to '%s'\n", target );
            fclose( ftarget );
            fclose( fsrc_zip );
            free( buffer );
            return( 1 );
        }
    }

    // source zip is now copied
    fclose( fsrc_zip );

    // save length of target executable for later usage
    length = ftell( ftarget );

    // --- fixup offsets ---

    // find end of central directory
    fseek( ftarget, -((length < BUFFER_SIZE) ? length : BUFFER_SIZE), SEEK_END );
    n = fread( buffer, 1, BUFFER_SIZE, ftarget );

    if( n == 0 ) {
        fprintf( stderr, "error: failed to read data from end of file '%s'\n", target );
        fclose( ftarget );
        free( buffer );
        return( 1 );
    }

    pos = 0;
    while( pos != -1L ) {
        pos = find( buffer + pos, n - pos, EOCD_MAGIC, strlen( EOCD_MAGIC ) );
        if( pos == -1L ) {
            fprintf( stderr, "error: no ZIP magic found in '%s'\n", target );
            fclose( ftarget );
            free( buffer );
            return( 1 );
        }

        if( fix_cd( ftarget, buffer + pos, n - pos, offset ) ) {
            // fixup also eocd
            ZIP_END_OF_CENTRAL_DIRECTORY    eocd;
            long                            off;

            off  = -((length < BUFFER_SIZE) ? length : BUFFER_SIZE);
            off += pos;

            fseek( ftarget, off, SEEK_END );

            if( fread( &eocd, sizeof( eocd ), 1, ftarget ) != 1 ) {
                fprintf( stderr, "file read error: '%s'\n", target );
                fclose( ftarget );
                free( buffer );
                return( 1 );
            }

            if( find( (char *)&eocd, 4, EOCD_MAGIC, 4 ) != 0 ) {
                fprintf( stderr, "file read error while checking eocd magic: '%s'\n", target );
                fclose( ftarget );
                free( buffer );
                return( 1 );
            }

            eocd.cd_offset += offset;

            fseek( ftarget, off, SEEK_END );
            fwrite( &eocd, sizeof( eocd ), 1, ftarget );
            break;
        } else {
            pos++;
        }
    }

    // --- cleanup  ---
    fclose( ftarget );
    free( buffer );
    return( 0 );
}
