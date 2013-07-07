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
* Description:  WASM keyword structures definition program.
*
****************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "hash.h"
#include "mkopcode.h"

#include "asmops.h"
#define MKOPCODE
#include "asminsd.h"

char Chars[32000];

static unsigned inst_table[HASH_TABLE_SIZE] = { 0 };
static unsigned *index_table;
static unsigned *pos_table;

int len_compare( const void *pv1, const void *pv2 )
{
    size_t          len1;
    size_t          len2;
    const sword     *p1 = pv1;
    const sword     *p2 = pv2;

    len1 = strlen( p1->word );
    len2 = strlen( p2->word );
    if( len1 < len2 )
        return( 1 );
    if( len1 > len2 )
        return( -1 );
    return( strcmp( p1->word, p2->word ) );
}

void make_inst_hash_tables( unsigned int count, sword *Words )
/*******************************************************************/
{
    char            *name;
    unsigned        i;
    unsigned        *p;
    unsigned        pos;
    unsigned        size = sizeof( AsmOpTable ) / sizeof( AsmOpTable[0] );

    index_table = calloc( count, sizeof( *index_table ) );
    pos_table = calloc( count, sizeof( *pos_table ) );
    for( pos = 0, i = 0; i < count; i++ ) {
        // create indexes for hash item lists
        name = Words[i].word;
        for( p = &inst_table[hashpjw( name )]; *p; p = &index_table[*p - 1] ) {
            if( stricmp( name, Words[*p - 1].word ) == 0 ) {
                break;
            }
        }
        if( *p == 0 ) {
            index_table[i] = 0;
            *p = i + 1;
        }
        // create index for position in AsmOpTable
        while ( AsmOpTable[pos] < i && pos < size )
            pos++;
        if( AsmOpTable[pos] != i || pos >= size ) {
            printf( "Wrong data in asminsd.h. position=%d, index=%d\n", pos, i );
            exit( 1 );
        }
        pos_table[i] = pos;
    }
}


int main( int argc, char *argv[] )
{
    FILE            *in;
    FILE            *out;
    char            *out_name;
    int             i1;
    int             i2;
    unsigned        words_count;
    unsigned        chars_count;
    unsigned        len;
    unsigned        idx;
    sword           *Words;
    char            *word;
    char            buf[KEY_MAX_LEN];

    out_name = argv[argc - 1];
    --argc;

    // Count the words in all the input files
    words_count = 0;
    for( i1 = 1; i1 < argc; ++i1 ) {
        in = fopen( argv[i1], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[i1] );
            exit( 1 );
        }
        for( ; fgets( buf, KEY_MAX_LEN, in ) != NULL; ) {
            words_count++;
        }
        fclose( in );
    }
    Words = malloc( ( words_count + 1 ) * sizeof( sword ) );
    if( Words == NULL ) {
        printf( "Unable to allocate Words array\n" );
        exit( 1 );
    }
    Words[words_count].word = NULL;
    words_count = 0;
    for( i1 = 1; i1 < argc; ++i1 ) {
        in = fopen( argv[i1], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[i1] );
            exit( 1 );
        }
        for( ; fgets( buf, KEY_MAX_LEN, in ) != NULL; ) {
            for( i2 = 0; buf[i2] && !isspace( buf[i2] ); i2++ )
                ;
            buf[i2] = '\0';
            Words[words_count].word = strdup( buf );
            if( Words[words_count].word == NULL ) {
                printf( "Out of memory\n" );
                exit( 1 );
            }
            ++words_count;
        }
        fclose( in );
    }
    qsort( Words, words_count, sizeof( sword ), len_compare );
    chars_count = 0;
    Chars[0] = '\0';
    for( idx = 0; idx < words_count; idx++ ) {
        word = strstr( Chars, Words[idx].word );
        if( word == NULL ) {
            word = &Chars[chars_count];
            len = strlen( Words[idx].word ) - 1;
            if( chars_count < len )
                len = chars_count;
            for( ; ; ) {
                if( len == 0 )
                    break;
                if( memcmp( word - len, Words[idx].word, len ) == 0 ) {
                    word -= len;
                    chars_count -= len;
                    break;
                }
                len--;
            }
            strcpy( word, Words[idx].word );
            chars_count += (unsigned)strlen( word );
        }
        Words[idx].index = word - Chars;
    }
    qsort( Words, words_count, sizeof( sword ), str_compare );

    make_inst_hash_tables( words_count, Words );

    out = fopen( out_name, "w" );
    if( out == NULL ) {
        printf( "Unable to open '%s'\n", out_name );
        exit( 1 );
    }
    fprintf( out, "const char AsmChars[] = {\n" );
    for( idx = 0; idx < chars_count; idx++ ) {
        if( idx % 10 == 0 )
            fprintf( out, "/*%4u*/ ", idx );
        fprintf( out, "'%c',", Chars[idx] );
        if( idx % 10 == 9 ) {
            fprintf( out, "\n" );
        }
    }
    fprintf( out, "'\\0'\n};\n\n" );
    fprintf( out, "static const unsigned short inst_table[HASH_TABLE_SIZE] = {\n" );
    for( idx = 0; idx < HASH_TABLE_SIZE; idx++ )
        fprintf( out, "\t%u,\n", inst_table[idx] );
    fprintf( out, "};\n\n" );
    fprintf( out, "const struct AsmCodeName AsmOpcode[] = {\n" );
    for( idx = 0; idx < words_count; idx++ ) {
        word = Words[idx].word;
        fprintf( out, "\t{\t%u,\t%u,\t%u,\t%u\t},\t/* %s */\n", 
                 pos_table[idx], (unsigned)strlen( word ), Words[idx].index,
                 index_table[idx], get_enum_key( word ) );
    }
    fprintf( out, "\t{\t0,\t0,\t0,\t0\t}\t/* T_NULL */\n" );
    fprintf( out, "};\n\n" );
    fclose( out );
    return( 0 );
}
