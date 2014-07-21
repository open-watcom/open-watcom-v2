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
* Description:  WASM keyword enumaration definition program.
*
****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"

#include "mkopcode.h"

int main( int argc, char *argv[] )
{
    FILE            *in;
    FILE            *out;
    char            *out_name;
    unsigned int    i;
    unsigned int    index;
    unsigned int    count;
    int             idx;
    sword           *Words;
    char            buf[ KEY_MAX_LEN ];
    size_t          max_len;

    out_name = argv[ argc - 1 ];
    --argc;

    // Count the words in all the input files
    count = 0;
    for( idx = 1; idx < argc; ++idx ) {
        in = fopen( argv[ idx ], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[ idx ] );
            exit( 1 );
        }
        for( ; fgets( buf, KEY_MAX_LEN, in ) != NULL; ) {
            count++;
        }
        fclose( in );
    }
    Words = malloc( (count+1) * sizeof( sword ) );
    if( Words == NULL ) {
        printf( "Unable to allocate Words array\n" );
        exit( 1 );
    }
    Words[ count ].word = NULL;
    index = 0;
    for( idx = 1; idx < argc; ++idx ) {
        in = fopen( argv[ idx ], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[ idx ] );
            exit( 1 );
        }
        for( ; fgets( buf, KEY_MAX_LEN, in ) != NULL; ) {
            for( i = 0; buf[ i ] && !isspace( buf[ i ] ); i++ )
                ;
            buf[ i ] = '\0';
            Words[ index ].word = strdup( buf );
            if( Words[ index ].word == NULL ) {
                printf( "Out of memory\n" );
                exit( 1 );
            }
            ++index;
        }
        fclose( in );
    }
    qsort( Words, count, sizeof( sword ), str_compare );

    out = fopen( out_name, "w" );
    if( out == NULL ) {
        printf( "Unable to open '%s'\n", out_name );
        exit( 1 );
    }
    fprintf( out, "typedef enum asm_token {\n" );
    max_len = 0;
    for( i = 0; i < count; i++ ) {
        size_t len = strlen( Words[i].word );
        if( len > max_len )
            max_len = len;
        fprintf( out, "    %s,\n", get_enum_key( Words[i].word ) );
    }
    // following are not real tokens
    // empty token
    fprintf( out, "    T_NULL,\n" );
    // enable to include '*' in arithmetic operators class
    fprintf( out, "    T_OP_TIMES,\n" );
    // enable to include '/' in arithmetic operators class
    fprintf( out, "    T_OP_DIVIDE,\n" );
    fprintf( out, "} asm_token;\n" );
    fprintf( out, "\n" );
    fprintf( out, "#define MAX_KEYWORD_LEN %u\n", (unsigned)max_len );
    fclose( out );
    return( 0 );
}
