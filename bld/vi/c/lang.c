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


#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "vi.h"
#include "sstyle.h"
#include "lang.h"
#include "misc.h"

static lang_info    langInfo[ LANG_MAX ] = {
    //table,  entries , ref_count ,read_buf
    { NULL,          0,          0,    NULL },  // C
    { NULL,          0,          0,    NULL },  // C++
    { NULL,          0,          0,    NULL },  // Fortran
    { NULL,          0,          0,    NULL },  // Java
    { NULL,          0,          0,    NULL },  // SQL
    { NULL,          0,          0,    NULL },  // BAT
    { NULL,          0,          0,    NULL },  // Basic
    { NULL,          0,          0,    NULL }   // Perl
};

/*
 * hashpjw - taken from red dragon book, pg 436
 */
int hashpjw( char *s )
{
    unsigned h = 0, g;
    while( *s != '\0' ) {
        h = ( h << 4 ) + (*s);
        if( g = h & 0xf0000000 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
        s++;
    }
    return( h % langInfo[ CurrentInfo->Language ].table_entries );
}

bool IsKeyword( char *keyword )
{
    hash_entry  *entry;

    assert( langInfo[ CurrentInfo->Language ].ref_count > 0 );

    entry = langInfo[ CurrentInfo->Language ].keyword_table +
            hashpjw( keyword );
    if( entry->real == FALSE ) {
        return( FALSE );
    }
    while( entry != NULL && strcmp( entry->keyword, keyword ) != 0 ) {
        entry = entry->next;
        if( entry ) {
            assert( entry->real == FALSE );
        }
    }
    return( entry != NULL );
}

hash_entry *createTable( int entries )
{
    hash_entry  *table;

    table = MemAlloc( entries * sizeof( hash_entry ) );
    memset( table, 0, entries * sizeof( hash_entry ) );

    return( table );
}

char *nextKeyword( char *keyword )
{
    while( *keyword ) {
        keyword++;
    }
    return( keyword + 1 );
}

void addTable( hash_entry *table, char *Keyword, int NumKeyword )
{
    int         i;
    hash_entry  *entry, *empty;
    char        *keyword;
    typedef struct tagTmpValue {
        int     hashValue;
        char    *keyword;
    } TmpValue;
    TmpValue *tmpValue, *tmpIndex;

    tmpValue = tmpIndex = MemAlloc( NumKeyword * sizeof( TmpValue ) );
    keyword = Keyword;
    for( i = 0; i < NumKeyword; i++ ) {
        tmpIndex->hashValue = hashpjw( keyword );
        tmpIndex->keyword = keyword;
        table[ tmpIndex->hashValue ].real = TRUE;
        keyword = nextKeyword( keyword );
        tmpIndex++;
    }

    empty = table;
    tmpIndex = tmpValue;
    for( i = 0; i < NumKeyword; i++ ) {
        assert( table[ tmpIndex->hashValue ].real == TRUE );

        entry = table + tmpIndex->hashValue;
        if( entry->keyword != NULL ) {
            while( entry->next != NULL ) {
                entry = entry->next;
            }
            while( empty->real == TRUE ) {
                empty++;
            }
            entry->next = empty;
            entry = empty;
            empty++;
        }
        entry->keyword = tmpIndex->keyword;
        entry->next = NULL;
        tmpIndex++;
    }

    MemFree( tmpValue );
}

/*
 * LangInit - build hash table based on current language
 */
void LangInit( int newLanguage )
{
    int         *dummy;
    int         rc, nkeywords;
    char        *buff;
    char        *fname[] = { NULL, "c.dat", "cpp.dat", "fortran.dat", "java.dat", "sql.dat",
                                "bat.dat", "basic.dat", "perl.dat" };

    assert( CurrentInfo != NULL );
    CurrentInfo->Language = newLanguage;

    if( newLanguage == LANG_NONE ) {
        return;
    }

    if( langInfo[ newLanguage ].ref_count == 0 ) {
        rc = ReadDataFile( fname[ newLanguage ], &nkeywords,
                           &buff, &dummy, FALSE );
        if( rc ) {
            Error( GetErrorMsg( rc ) );
            CurrentInfo->Language = LANG_NONE;
            return;
        }
        // build new langInfo entry
        langInfo[ newLanguage ].table_entries = nkeywords;
        langInfo[ newLanguage ].keyword_table =
                createTable( NextBiggestPrime( nkeywords ) );
        addTable( langInfo[ newLanguage ].keyword_table, buff, nkeywords );
        langInfo[ newLanguage ].read_buf = buff;
        MemFree( dummy );
    }
    langInfo[ newLanguage ].ref_count++;

    return;
}

/*
 * LangFini
 */
void LangFini( int language )
{
    if( language == LANG_NONE || langInfo[ language ].ref_count == 0 ) {
        return;
    }
    langInfo[ language ].ref_count--;
    if( langInfo[ language ].ref_count == 0 ) {
        MemFree( langInfo[ language ].keyword_table );
        MemFree( langInfo[ language ].read_buf );
        langInfo[ language ].keyword_table = NULL;
        langInfo[ language ].table_entries = 0;
    }
}

/*
 * LangFiniAll
 */
void LangFiniAll( void )
{
    int i;
    for( i = LANG_NONE; i < LANG_MAX; i++ ) {
        while( langInfo[ i ].ref_count ) {
            LangFini( i );
        }
    }
}
