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


#include "plusplus.h"
#include "memmgr.h"
#include "stringl.h"
#include "escchars.h"
#include "scan.h"
#include "initdefs.h"
#include "segment.h"
#include "pcheader.h"
#include "errdefns.h"
#include "preproc.h"
#include "carve.h"

#define DEFINE_UNICODE
#include "unicode.h"

static STRING_CONSTANT uniqueStrings;
static STRING_CONSTANT trashedStrings;

static unsigned stringCount;
static STRING_CONSTANT *stringTranslateTable;

static struct {
    unsigned            unique : 1;
} stringData;

static void walk_strings(       // WALK STRINGS IN A LIST
    STRING_CONSTANT str,        // - first string in a list
    void (*walker)              // - walking routine
        ( STRING_CONSTANT ) )   // - - current string
{
    while( str != NULL ) {
        (*walker)( str );
        str = str->next;
    }
}


void StringWalk(                // WALK DEFINED STRING LITERALS
    void (*walker)              // - walking routine
        ( STRING_CONSTANT ) )   // - - current string
{
    walk_strings( uniqueStrings, walker );
}

static void stringUnique( int yes )
{
    stringData.unique = yes;
}


static void stringInit(         // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    uniqueStrings = NULL;
    trashedStrings = NULL;
    stringCount = 0;
    stringUnique( 0 );
}

INITDEFN( strings, stringInit, InitFiniStub )

static STRING_CONSTANT initLiteral( STRING_CONSTANT literal )
{
    literal->next = NULL;
    literal->cg_handle = NULL;
    literal->segid = SEG_NULL;
    literal->concat = FALSE;
    literal->multi_line = FALSE;
    literal->wide_string = FALSE;
    return( literal );
}

static STRING_CONSTANT findLiteral( unsigned len )
{
    STRING_CONSTANT literal;
    STRING_CONSTANT prev;

    prev = NULL;
    literal = trashedStrings;
    while( literal != NULL ) {
        if( len <= literal->len ) {
            /* try to use more than 1/2 of the trashed string */
            if( len >= literal->len / 2 ) {
                if( prev != NULL ) {
                    prev->next = literal->next;
                } else {
                    trashedStrings = literal->next;
                }
                return( initLiteral( literal ) );
            }
        }
        prev = literal;
        literal = literal->next;
    }
    literal = CPermAlloc( offsetof( STRING_LITERAL, string ) + len + 1 );
    return( initLiteral( literal ) );
}

static unsigned compressLiteral( char *tgt, char *s, unsigned len )
/********************************************************************/
{
    unsigned char   *str = (unsigned char *)s;
    int             chr;               // - current character
    int             chr_1;             // - one char ahead of current char
    int             classification;    // - escape classification
    int             max_digs;          // - max digits remaining
    unsigned        new_len;           // - length after escapes processed
    struct {
        unsigned wide_string : 1;
    } flags;

#define store_chr(tgt,chr)  if(tgt) *tgt++ = (chr)

    flags.wide_string = FALSE;
    if( CurToken == T_LSTRING ) {
        flags.wide_string = TRUE;
    }
    new_len = 0;
    for( ; len > 0; ) {
        chr = *str++;
        --len;
        if( ( len > 0 ) && ( chr == '\\' ) ) {
            chr = *str++;
            -- len;
            classification = classify_escape_char( chr );
            if( classification == ESCAPE_OCTAL ) {
                chr_1 = octal_dig( chr );
                chr = 0;
                max_digs = 3;
                for( ; ; ) {
                    chr = ( chr << 3 ) | chr_1;
                    if( len == 0 ) break;
                    if( --max_digs == 0 ) break;
                    chr_1 = octal_dig( *str );
                    if( chr_1 == 8 ) break;
                    -- len;
                    ++ str;
                }
            } else if( classification == ESCAPE_HEX ) {
                if( ( len > 1 ) && ( 16 != hex_dig( *str ) ) ) {
                    chr = 0;
                    max_digs = 8;
                    for( ; ; ) {
                        if( len == 0 ) break;
                        if( max_digs == 0 ) break;
                        chr_1 = hex_dig( *str );
                        if( chr_1 == 16 ) break;
                        chr = ( chr << 4 ) | chr_1 ;
                        -- max_digs;
                        ++ str;
                        -- len;
                    }
                }
            } else if( classification != ESCAPE_NONE ) {
                chr = classification;
            }
            if( flags.wide_string ) {
                store_chr( tgt, chr );
                ++ new_len;
                chr = chr >> 8;
            }
        } else {
            if( CharSet[chr] & C_DB ) {       /* if double-byte character */
                if( CompFlags.jis_to_unicode && flags.wide_string ) {
                    chr = (chr << 8) + *str;
                    chr = JIS2Unicode( chr );
                    store_chr( tgt, chr );
                    chr = chr >> 8;
                } else {
                    store_chr( tgt, chr );
                    chr = *str;
                }
                ++ new_len;
                ++ str;
                -- len;
            } else if( flags.wide_string ) {
                if( CompFlags.use_unicode ) {           /* 05-jun-91 */
                    chr = UniCode[ chr ];
                } else if( CompFlags.jis_to_unicode ) {
                    chr = JIS2Unicode( chr );
                }
                store_chr( tgt, chr );
                ++ new_len;
                chr = chr >> 8;
//          } else {
//              _ASCIIOUT( chr );
            }
        }
        store_chr( tgt, chr );
        ++new_len;
    }
    --new_len;  /* take one of the '\0' from the end */
    return( new_len );
}


static STRING_CONSTANT makeLiteral( char *s, unsigned len )
/*********************************************************/
{
    STRING_CONSTANT literal;
    unsigned        new_len;

    new_len = len;
    if( CurToken == T_LSTRING ) {
        new_len = compressLiteral( NULL, s, len + 1 );
    }
    literal = findLiteral( new_len );
    literal->len = compressLiteral( literal->string, s, len + 1 );
    if( CurToken == T_LSTRING ) {
        literal->wide_string = TRUE;
    }
    return( literal );
}

void StringTrash( STRING_CONSTANT string )
/****************************************/
{
    STRING_CONSTANT *head;
    STRING_CONSTANT search;

    head = &uniqueStrings;
    for( search = *head; search != NULL; search = *head ) {
        if( string == search ) {
            --stringCount;
            *head = search->next;
            string->next = trashedStrings;
            trashedStrings = string;
            return;
        }
        head = &(search->next);
    }
}


static STRING_CONSTANT stringAdd(// ADD LITERAL TO STRING
    STRING_CONSTANT literal,    // - literal to be added
    STRING_CONSTANT *list )     // - addr( list )
{
    literal->next = *list;
    *list = literal;
    return( literal );
}


STRING_CONSTANT StringCreate( char *s, unsigned len )
/***************************************************/
{
    ++stringCount;
    return( stringAdd( makeLiteral( s, len ), &uniqueStrings ) );
}

void StringConcatDifferentLines( STRING_CONSTANT v )
/**************************************************/
{
    v->multi_line = TRUE;
}

STRING_CONSTANT StringConcat( STRING_CONSTANT v1, STRING_CONSTANT v2 )
/********************************************************************/
{
    STRING_CONSTANT literal;
    unsigned        len;        // - length

    if( v1->wide_string != v2->wide_string ) {
        // an error has already been diagnosed
        StringTrash( v2 );
        return v1;
    }
    literal = findLiteral( v1->len + v2->len );
    literal->concat = TRUE;
    memcpy( literal->string, v1->string, v1->len );
    len = v1->len;
    if( v1->wide_string ) {
        --len;
        literal->wide_string = TRUE;
    }
    if( v1->multi_line ) {
        literal->multi_line = TRUE;
    }
    literal->len = len + v2->len;
    memcpy( &(literal->string[ len ]), v2->string, v2->len + 1 );
    StringTrash( v1 );
    StringTrash( v2 );
    ++stringCount;
    return( stringAdd( literal, &uniqueStrings ) );
}

boolean StringSame( STRING_CONSTANT v1, STRING_CONSTANT v2 )
/**********************************************************/
{
    if( v1->len != v2->len ) {
        return( FALSE );
    }
    return( memcmp( v1->string, v2->string, v1->len + 1 ) == 0 );
}

size_t StringByteLength( STRING_CONSTANT s )
/******************************************/
{
    // byte length should include '\0' character
    return s->len + TARGET_CHAR;
}

size_t StringAWStrLen( STRING_CONSTANT s )
/****************************************/
{
    size_t len;

    // string length should include '\0' character
    if( s->wide_string ) {
        DbgAssert( s->len & 1 );
        len = ( s->len + 1 ) / TARGET_WIDE_CHAR;
    } else {
        len = ( s->len + 1 ) / TARGET_CHAR;
    }
    return( len );
}

char *StringBytes( STRING_CONSTANT s )
/************************************/
{
    return s->string;
}

static int cmpString( const void *lp, const void *rp )
{
    STRING_CONSTANT left = *(STRING_CONSTANT *)lp;
    STRING_CONSTANT right = *(STRING_CONSTANT *)rp;

    if( left < right ) {
        return( -1 );
    } else if( left > right ) {
        return( 1 );
    }
    return( 0 );
}

pch_status PCHInitStringPool( boolean writing )
{
    STRING_CONSTANT curr;
    STRING_CONSTANT *p;

    if( ! writing ) {
        return( PCHCB_OK );
    }
    stringTranslateTable = CMemAlloc( stringCount * sizeof( STRING_CONSTANT ) );
    p = stringTranslateTable;
    for( curr = uniqueStrings; curr != NULL; curr = curr->next ) {
        *p = curr;
        ++p;
    }
    qsort( stringTranslateTable, stringCount, sizeof( STRING_CONSTANT ), cmpString );
#ifndef NDEBUG
    {
        int i;
        for( i = 1; i < stringCount; ++i ) {
            if( stringTranslateTable[i-1] == stringTranslateTable[i] ) {
                CFatal( "two identical strings in translation table" );
            }
        }
    }
#endif
    return( PCHCB_OK );
}

pch_status PCHFiniStringPool( boolean writing )
{
    writing = writing;
    CMemFreePtr( &stringTranslateTable );
    return( PCHCB_OK );
}

pch_status PCHReadStringPool( void )
{
    STRING_CONSTANT *p;
    STRING_CONSTANT str;
    size_t str_len;

    while( uniqueStrings != NULL ) {
        StringTrash( uniqueStrings );
    }
    stringCount = PCHReadUInt();
    stringTranslateTable = CMemAlloc( stringCount * sizeof( STRING_CONSTANT ) );
    p = stringTranslateTable;
    for( ; (str_len = PCHReadUInt()) != 0; ) {
        str = findLiteral( str_len );
        str->len = str_len - 1;
        PCHRead( str->string, str_len );
        stringAdd( str, &uniqueStrings );
        *p = str;
        ++p;
    }
    return( PCHCB_OK );
}

pch_status PCHWriteStringPool( void )
{
    unsigned len;
    int i;
    STRING_CONSTANT str;
    STRING_CONSTANT *p;

    PCHWriteUInt( stringCount );
    p = stringTranslateTable;
    for( i = 0; i < stringCount; ++i ) {
        str = p[i];
        len = StringByteLength( str );
        PCHWriteUInt( len );
        PCHWrite( StringBytes( str ), len );
    }
    PCHWriteUInt( 0 );
    return( PCHCB_OK );
}

STRING_CONSTANT StringMapIndex( STRING_CONSTANT index )
/*****************************************************/
{
    if( PCHGetUInt( index ) < PCH_FIRST_INDEX ) {
        return( NULL );
    }
#ifndef NDEBUG
    if( PCHGetUInt( index ) >= stringCount + PCH_FIRST_INDEX ) {
        CFatal( "invalid string index" );
    }
#endif
    return( stringTranslateTable[ PCHGetUInt( index ) - PCH_FIRST_INDEX ] );
}

static int cmpFindString( const void *kp, const void *tp )
{
    STRING_CONSTANT key = *(STRING_CONSTANT *)kp;
    STRING_CONSTANT table = *((STRING_CONSTANT *)tp);

    if( key < table ) {
        return( -1 );
    } else if( key > table ) {
        return( 1 );
    }
    return( 0 );
}

STRING_CONSTANT StringGetIndex( STRING_CONSTANT str )
/**************************************************/
{
    STRING_CONSTANT *found;

    if( str == NULL ) {
        return( PCHSetUInt( PCH_NULL_INDEX ) );
    }
    found = bsearch( &str, stringTranslateTable, stringCount, sizeof( STRING_CONSTANT ), cmpFindString );
    if( found == NULL ) {
#ifndef NDEBUG
        CFatal( "invalid string passed to StringGetIndex" );
#endif
        return( PCHSetUInt( PCH_ERROR_INDEX ) );
    }
    return( PCHSetUInt( ( found - stringTranslateTable ) + PCH_FIRST_INDEX ) );
}
