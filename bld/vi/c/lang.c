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
* Description:  Processing of .dat files used for syntax highlighting.
*
****************************************************************************/


#include "vi.h"
#include "sstyle.h"
#include <assert.h>

#include "clibext.h"


static lang_info    langInfo[] = {
    #define pick_lang(enum,enumrc,name,namej,fname) { NULL, 0, 0, NULL },
    #include "langdef.h"
    #undef pick_lang
};

static hash_entry   *pragma_table           = NULL;
static int          pragma_table_entries    = 0;
static char         *pragma_read_buf        = NULL;

static hash_entry   *declspec_table         = NULL;
static int          declspec_table_entries  = 0;
static char         *declspec_read_buf      = NULL;

#define PRAGMA_DATFILE      "pragma.dat"
#define DECLSPEC_DATFILE    "declspec.dat"

/*
 * hashpjw - taken from red dragon book, pg 436
 */
static int hashpjw( char *s, int entries )
{
    unsigned long   h = 0, g;
    
    while( *s != '\0' ) {
        h = (h << 4) + toupper( *s );
        if( (g = h & 0xf0000000) != 0 ) {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
        s++;
    }
    return( h % entries );
}

bool IsKeyword( char *keyword, bool case_ignore )
{
    hash_entry  *entry;

    assert( langInfo[CurrentInfo->fsi.Language].ref_count > 0 );

    if( langInfo[CurrentInfo->fsi.Language].keyword_table == NULL ) {
        return( false );
    }

    entry = langInfo[CurrentInfo->fsi.Language].keyword_table +
        hashpjw( keyword, langInfo[CurrentInfo->fsi.Language].table_entries );
    if( !entry->real ) {
        return( false );
    }
    if( case_ignore ) {
        while( entry != NULL && stricmp( entry->keyword, keyword ) != 0 ) {
            entry = entry->next;
            if( entry ) {
                assert( !entry->real );
            }
        }
    } else {
        while( entry != NULL && strcmp( entry->keyword, keyword ) != 0 ) {
            entry = entry->next;
            if( entry ) {
                assert( !entry->real );
            }
        }
    }
    return( entry != NULL );
}

bool IsPragma( char *pragma )
{
    hash_entry  *entry;

    if( pragma_table == NULL ) {
        return( false );
    }

    entry = pragma_table + hashpjw( pragma, pragma_table_entries );
    if( !entry->real ) {
        return( false );
    }
    while( entry != NULL && strcmp( entry->keyword, pragma ) != 0 ) {
        entry = entry->next;
        if( entry ) {
            assert( !entry->real );
        }
    }
    return( entry != NULL );
}

bool IsDeclspec( char *keyword )
{
    hash_entry  *entry;

    if( declspec_table == NULL ) {
        return( false );
    }

    entry = declspec_table + hashpjw( keyword, declspec_table_entries );
    if( !entry->real ) {
        return( false );
    }
    while( entry != NULL && strcmp( entry->keyword, keyword ) != 0 ) {
        entry = entry->next;
        if( entry ) {
            assert( !entry->real );
        }
    }
    return( entry != NULL );
}

static hash_entry *createTable( int entries )
{
    hash_entry  *table;

    table = MemAlloc( entries * sizeof( hash_entry ) );
    memset( table, 0, entries * sizeof( hash_entry ) );

    return( table );
}

static char *nextKeyword( char *keyword )
{
    while( *keyword ) {
        keyword++;
    }
    return( keyword + 1 );
}

static void addTable( hash_entry *table, char *Keyword, int NumKeyword, int entries )
{
    int         i;
    hash_entry  *entry, *empty;
    char        *keyword;
    typedef struct tagTmpValue {
        int     hashValue;
        char    *keyword;
    } TmpValue;
    TmpValue    *tmpValue, *tmpIndex;

    tmpValue = tmpIndex = MemAlloc( NumKeyword * sizeof( TmpValue ) );
    keyword = Keyword;
    for( i = 0; i < NumKeyword; i++ ) {
        tmpIndex->hashValue = hashpjw( keyword, entries );
        tmpIndex->keyword = keyword;
        table[tmpIndex->hashValue].real = true;
        keyword = nextKeyword( keyword );
        tmpIndex++;
    }

    empty = table;
    tmpIndex = tmpValue;
    for( i = 0; i < NumKeyword; i++ ) {
        assert( table[tmpIndex->hashValue].real );

        entry = table + tmpIndex->hashValue;
        if( entry->keyword != NULL ) {
            while( entry->next != NULL ) {
                entry = entry->next;
            }
            while( empty->real ) {
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

static int nkeywords = 0;

static bool lang_alloc( int cnt )
{
    nkeywords = cnt;
    return( false );
}

static bool lang_save( int i, char *buff )
{
    i = i; buff = buff;
    return( true );
}

/*
 * LangInit - build hash table based on current language
 */
void LangInit( lang_t newLanguage )
{
    vi_rc       rc;
    char        *buff;
    char        *fname[] = {
        #define pick_lang(enum,enumrc,name,namej,fname) fname,
        #include "langdef.h"
        #undef pick_lang
    };

    assert( CurrentInfo != NULL );
    CurrentInfo->fsi.Language = newLanguage;

    if( newLanguage == LANG_NONE ) {
        return;
    }

    if( langInfo[newLanguage].ref_count == 0 ) {
        rc = ReadDataFile( fname[newLanguage], &buff, lang_alloc, lang_save, EditFlags.BoundData );
        if( rc != ERR_NO_ERR ) {
            if( rc == ERR_FILE_NOT_FOUND ) {
                ErrorBox( GetErrorMsg( ERR_SPECIFIC_FILE_NOT_FOUND ),
                          fname[newLanguage] );
            } else {
                ErrorBox( GetErrorMsg( rc ) );
            }
            CurrentInfo->fsi.Language = LANG_NONE;
            return;
        }
        // build new langInfo entry
        langInfo[newLanguage].table_entries = nkeywords;
        langInfo[newLanguage].keyword_table =
            createTable( NextBiggestPrime( nkeywords ) );
        addTable( langInfo[newLanguage].keyword_table, buff, nkeywords,
                  langInfo[newLanguage].table_entries );
        langInfo[newLanguage].read_buf = buff;
    }
    langInfo[newLanguage].ref_count++;

    if( (newLanguage == LANG_C || newLanguage == LANG_CPP) && pragma_table == NULL ) {
        rc = ReadDataFile( PRAGMA_DATFILE, &pragma_read_buf, lang_alloc, lang_save, EditFlags.BoundData );
        if( rc == ERR_FILE_NOT_FOUND ) {
            ErrorBox( GetErrorMsg( ERR_SPECIFIC_FILE_NOT_FOUND ), PRAGMA_DATFILE );
            return;
        } else if( rc != ERR_NO_ERR ) {
            ErrorBox( GetErrorMsg( rc ) );
            return;
        }
        pragma_table_entries = nkeywords;
        pragma_table = createTable( NextBiggestPrime( nkeywords ) );
        addTable( pragma_table, pragma_read_buf, nkeywords, pragma_table_entries );

        rc = ReadDataFile( DECLSPEC_DATFILE, &declspec_read_buf, lang_alloc, lang_save, EditFlags.BoundData );
        if( rc == ERR_FILE_NOT_FOUND ) {
            ErrorBox( GetErrorMsg( ERR_SPECIFIC_FILE_NOT_FOUND ), DECLSPEC_DATFILE );
            return;
        } else if( rc != ERR_NO_ERR ) {
            ErrorBox( GetErrorMsg( rc ) );
            return;
        }
        declspec_table_entries = nkeywords;
        declspec_table = createTable( NextBiggestPrime( nkeywords ) );
        addTable( declspec_table, declspec_read_buf, nkeywords, declspec_table_entries );
    }

} /* LangInit */

/*
 * LangFini
 */
void LangFini( lang_t language )
{
    if( language == LANG_NONE || langInfo[language].ref_count == 0 ) {
        return;
    }
    langInfo[language].ref_count--;
    if( langInfo[language].ref_count == 0 ) {
        MemFree( langInfo[language].keyword_table );
        MemFree( langInfo[language].read_buf );
        langInfo[language].keyword_table = NULL;
        langInfo[language].table_entries = 0;
        langInfo[language].read_buf = NULL;
    }
    if( language == LANG_C || language == LANG_CPP ) {
        if( langInfo[LANG_C].ref_count == 0 && langInfo[LANG_CPP].ref_count == 0 ) {
            MemFree( pragma_table );
            MemFree( pragma_read_buf );
            pragma_table = NULL;
            pragma_table_entries = 0;
            pragma_read_buf = NULL;
        }
    }

} /* LangFini */

/*
 * LangFiniAll
 */
void LangFiniAll( void )
{
    lang_t  i;

    for( i = LANG_NONE; i < LANG_MAX; i++ ) {
        while( langInfo[i].ref_count ) {
            LangFini( i );
        }
    }

} /* LangFiniAll */
