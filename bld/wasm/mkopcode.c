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

static unsigned short inst_table[HASH_TABLE_SIZE] = { 0 };
static unsigned short *index_table;
static unsigned short *pos_table;

int len_compare( const void *pv1, const void *pv2 )
{
    int             len1;
    int             len2;
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
    unsigned short  i;
    unsigned short  *p;
    int             pos;
    int             size = sizeof( AsmOpTable ) / sizeof( AsmOpTable[0] );

    index_table = calloc( count, sizeof( unsigned short ) );
    pos_table = calloc( count, sizeof( unsigned short ) );
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
    unsigned int    i;
    unsigned int    index;
    unsigned int    count;
    unsigned int    len;
    int             idx;
    sword           *Words;
    char            *word;
    char            buf[KEY_MAX_LEN];

    out_name = argv[argc - 1];
    --argc;

    // Count the words in all the input files
    count = 0;
    for( idx = 1; idx < argc; ++idx ) {
        in = fopen( argv[idx], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[idx] );
            exit( 1 );
        }
        for( ; fgets( buf, KEY_MAX_LEN, in ) != NULL; ) {
            count++;
        }
        fclose( in );
    }
    Words = malloc( ( count + 1 ) * sizeof( sword ) );
    if( Words == NULL ) {
        printf( "Unable to allocate Words array\n" );
        exit( 1 );
    }
    Words[count].word = NULL;
    index = 0;
    for( idx = 1; idx < argc; ++idx ) {
        in = fopen( argv[idx], "r" );
        if( in == NULL ) {
            printf( "Unable to open '%s'\n", argv[idx] );
            exit( 1 );
        }
        for( ; fgets( buf, KEY_MAX_LEN, in ) != NULL; ) {
            for( i = 0; buf[i] && !isspace( buf[i] ); i++ )
                ;
            buf[i] = '\0';
            Words[index].word = strdup( buf );
            if( Words[index].word == NULL ) {
                printf( "Out of memory\n" );
                exit( 1 );
            }
            ++index;
        }
        fclose( in );
    }
    qsort( Words, count, sizeof( sword ), len_compare );
    index = 0;
    Chars[0] = '\0';
    for( i = 0; i < count; i++ ) {
        word = strstr( Chars, Words[i].word );
        if( word == NULL ) {
            word = &Chars[index];
            len = strlen( Words[i].word ) - 1;
            if( index < len )
                len = index;
            for( ; ; ) {
                if( len == 0 )
                    break;
                if( memcmp( word - len, Words[i].word, len ) == 0 ) {
                    word -= len;
                    index -= len;
                    break;
                }
                len--;
            }
            strcpy( word, Words[i].word );
            index += strlen( word );
        }
        Words[i].index = word - Chars;
    }
    qsort( Words, count, sizeof( sword ), str_compare );

    make_inst_hash_tables( count, Words );

    out = fopen( out_name, "w" );
    if( out == NULL ) {
        printf( "Unable to open '%s'\n", out_name );
        exit( 1 );
    }
    fprintf( out, "const char AsmChars[] = {\n" );
    for( i = 0; i < index; i++ ) {
        if( i % 10 == 0 )
            fprintf( out, "/*%4d*/ ", i );
        fprintf( out, "'%c',", Chars[i] );
        if( i % 10 == 9 ) {
            fprintf( out, "\n" );
        }
    }
    fprintf( out, "'\\0'\n};\n\n" );
    fprintf( out, "static const unsigned short inst_table[HASH_TABLE_SIZE] = {\n" );
    for( i = 0; i < HASH_TABLE_SIZE; i++ )
        fprintf( out, "\t%d,\n", inst_table[i] );
    fprintf( out, "};\n\n" );
    fprintf( out, "const struct AsmCodeName AsmOpcode[] = {\n" );
    for( i = 0; i < count; i++ ) {
        word = Words[i].word;
        fprintf( out, "\t{\t%d,\t%d,\t%d,\t%d\t},\t/* %s */\n", 
                 pos_table[i], (int)strlen( word ), Words[i].index, index_table[i], get_enum_key( word ) );
    }
    fprintf( out, "\t{\t0,\t0,\t0,\t0\t}\t/* T_NULL */\n" );
    fprintf( out, "};\n\n" );
    fclose( out );
    return( 0 );
}
