/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Program to split a file into pieces so that
*               it will fit onto a diskette.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "wio.h"
#include "pathgrp2.h"

#include "clibext.h"


#define BUFFER_SIZE     512

#if !defined( SPLITZIP )
void SplitFile( long size, long max_size, char * file );

int main( int argc, char *argv[] )
//=================================

{
    if( argc != 4 ) {
        printf( "Usage: SPLITFIL filename size max-size\n" );
        exit( 1 );
    }
    SplitFile( atol( argv[2] ), atol( argv[3] ), argv[1] );
    return( 0 );
}
#endif

void SplitFile( long size, long max_size, char * input_file )
/***********************************************************/
{
    char                fullname[_MAX_PATH];
    pgroup2             pg;
    FILE                *bigfile, *smallfile;
    char                *buffer;
    int                 len_read;
    unsigned            idx;
    char                ext_idx[5];

    bigfile = fopen( input_file, "rb" );
    if( bigfile == NULL ) {
        printf( "Unable to open '%s'\n", input_file );
        exit( 2 );
    }
    buffer = malloc( BUFFER_SIZE );
    if( buffer == NULL ) {
        printf( "Unable to allocate buffer\n" );
        exit( 3 );
    }

    idx = 0;
    _splitpath2( input_file, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
    for( ;; ) {
        sprintf( ext_idx, "%u", ++idx );
        _makepath( fullname, pg.drive, pg.dir, pg.fname, ext_idx );
        smallfile = fopen( fullname, "wb" );
        if( smallfile == NULL ) {
            free( buffer );
            printf( "Unable to create '%s'\n", fullname );
            exit( 4 );
        }
        len_read = 0;
        while( size > 0 ) {
            len_read = read( fileno( bigfile ), buffer, BUFFER_SIZE );
            if( len_read == 0 )
                break;
            if( write( fileno( smallfile ), buffer, len_read ) != len_read ) {
                free( buffer );
                printf( "Error writing '%s'\n", fullname );
                exit( 5 );
            }
            size -= len_read;
        }
        fclose( smallfile );
        if( len_read == 0 )
            break;
        size = max_size;
    }
    free( buffer );
    fclose( bigfile );
}
