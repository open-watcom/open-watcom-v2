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


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void main( int argc, char **argv ) {

    FILE        *input;
    int         ch;
    int         index;
    int         rec_len;
    char        line[257];
    int         pos;

    if( argc > 2 ) {
        fprintf( stderr, "usage: watdump [segfile]\n" );
        exit( EXIT_FAILURE );
    }
    if( argc == 2 ) {
        input = fopen( argv[1], "rb" );
        if( input == NULL ) {
            fprintf( stderr, "unable to open %s for reading: %s\n",
                argv[1], strerror( errno ) );
        }
    } else {
        input = stdin;
    }
    index = 1;
    for(;;) {
        rec_len = fgetc( input );
        if( rec_len == EOF ) break;
        ch = fgetc( input );    /* get record type */
        if( ch == EOF ) {
            printf( "error: unexpected EOF\n" );
            exit( EXIT_FAILURE );
        }
        if( ( ( ch & 0xf0 ) == 0x60 || ( ch & 0xf0 ) == 0x50 ) &&
                ( ch & 0x0f ) != 0 ) {
            printf( "       " );
        } else {
            printf( "0x%04x:", index );
            ++index;
        }
        if( rec_len == 0 ) {
            printf( "\nerror: zero length record\n" );
            exit( EXIT_FAILURE );
        }
        --rec_len;
        pos = 0;
        if( rec_len > 0 ) {
            for(;;) {
                printf( " %02x", ch );
                line[pos] = isprint(ch) ? ch : '.';
                ++pos;
                --rec_len;
                if( rec_len == 0 ) break;
                ch = fgetc( input );
                if( ch == EOF ) {
                    printf( "\nerror: unexpected EOF\n" );
                    exit( EXIT_FAILURE );
                }
            }
        }
        line[pos] = 0;
        printf( "\t<%s>\n", line );
    }
    exit( EXIT_SUCCESS );
}
