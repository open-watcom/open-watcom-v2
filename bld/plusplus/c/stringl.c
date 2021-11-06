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
#include "preproc.h"
#include "carve.h"
#include "unicode.h"


#define STRING_CHARSIZE(s)  ((s->flags & STRLIT_WIDE) ? TARGET_WIDE_CHAR : TARGET_CHAR)

static STRING_CONSTANT  uniqueStrings;
static STRING_CONSTANT  trashedStrings;

static unsigned         stringCount;
static STRING_CONSTANT  *stringTranslateTable;

static struct {
    unsigned            unique : 1;
} stringData;

static void walk_strings(       // WALK STRINGS IN A LIST
    STRING_CONSTANT str,        // - first string in a list
    void (*walker)              // - walking routine
        ( STRING_CONSTANT ) )   // - - current string
{
    for( ; str != NULL; str = str->next ) {
        (*walker)( str );
    }
}


void StringWalk(                // WALK DEFINED STRING LITERALS
    void (*walker)              // - walking routine
        ( STRING_CONSTANT ) )   // - - current string
{
    walk_strings( uniqueStrings, walker );
}

static void stringUnique( bool yes )
{
    stringData.unique = yes;
}


static void stringInit(         // INITIALIZATION
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    uniqueStrings = NULL;
    trashedStrings = NULL;
    stringCount = 0;
    stringUnique( false );
}

INITDEFN( strings, stringInit, InitFiniStub )

static STRING_CONSTANT initLiteral( STRING_CONSTANT literal, target_size_t len, string_literal_flags flags )
{
    literal->next = NULL;
    literal->cg_handle = NULL;
    literal->segid = SEG_NULL;
    literal->len = len;
    literal->flags = flags;
    return( literal );
}

static STRING_CONSTANT allocLiteral( target_size_t len, string_literal_flags flags )
{
    STRING_CONSTANT literal;
    STRING_CONSTANT prev;

    prev = NULL;
    for( literal = trashedStrings; literal != NULL; literal = literal->next ) {
        if( len <= literal->alloc_len ) {
            /* try to use more than 1/2 of the trashed string */
            if( len >= literal->alloc_len / 2 ) {
                if( prev != NULL ) {
                    prev->next = literal->next;
                } else {
                    trashedStrings = literal->next;
                }
                return( initLiteral( literal, len, flags ) );
            }
        }
        prev = literal;
    }
    literal = CPermAlloc( offsetof( STRING_LITERAL, string ) + len );
    literal->alloc_len = len;
    return( initLiteral( literal, len, flags ) );
}

static char *store_wchar( char *tgt, int c )
/******************************************/
{
    int i;

    i = TARGET_WIDE_CHAR;
    while( i-- > 0 ) {
        *tgt++ = c;
        c >>= 8;
    }
    return( tgt );
}

static size_t compressLiteral( char *tgt, const char *s, size_t len, bool wide )
/******************************************************************************/
{
    unsigned char   *str = (unsigned char *)s;
    int             chr;               // - current character
    int             chr_1;             // - one char ahead of current char
    int             classification;    // - escape classification
    int             max_digs;          // - max digits remaining
    size_t          new_len;           // - length after escapes processed

#define STORE_CHAR(tgt,c,tlen)  if(tgt) *tgt++ = (c); tlen += TARGET_CHAR
#define STORE_WCHAR(tgt,c,tlen) if(tgt) tgt = store_wchar(tgt, (c)); tlen += TARGET_WIDE_CHAR

    new_len = 0;
    for( ; len > 0; ) {
        chr = *str++;
        --len;
        if( ( len > 0 ) && ( chr == '\\' ) ) {
            chr = *str++;
            --len;
            classification = classify_escape_char( chr );
            if( classification == ESCAPE_OCTAL ) {
                chr_1 = octal_dig( chr );
                chr = 0;
                max_digs = 3;
                for( ;; ) {
                    chr = ( chr << 3 ) | chr_1;
                    if( len == 0 )
                        break;
                    if( --max_digs == 0 )
                        break;
                    chr_1 = octal_dig( *str );
                    if( chr_1 == 8 )
                        break;
                    --len;
                    ++str;
                }
            } else if( classification == ESCAPE_HEX ) {
                chr = 0;
                if( ( len > 1 ) && ( 16 != hex_dig( *str ) ) ) {
                    max_digs = 8;
                    for( ;; ) {
                        if( len == 0 )
                            break;
                        if( max_digs == 0 )
                            break;
                        chr_1 = hex_dig( *str );
                        if( chr_1 == 16 )
                            break;
                        chr = ( chr << 4 ) | chr_1 ;
                        --max_digs;
                        ++str;
                        --len;
                    }
                }
            } else if( classification != ESCAPE_NONE ) {
                chr = classification;
            }
            if( wide ) {
                STORE_WCHAR( tgt, chr, new_len );
            } else {
                STORE_CHAR( tgt, chr, new_len );
            }
        } else {
            if( ( len > 0 ) && (CharSet[chr] & C_DB) ) {    /* if double-byte character */
                if( CompFlags.jis_to_unicode && wide ) {
                    chr = (chr << 8) + *str++;
                    chr = JIS2Unicode( chr );
                    STORE_WCHAR( tgt, chr, new_len );
                } else {
                    STORE_CHAR( tgt, chr, new_len );
                    chr = *str++;
                    STORE_CHAR( tgt, chr, new_len );
                }
                --len;
            } else if( wide ) {
                if( CompFlags.use_unicode ) {
                    chr = UniCode[chr];
                } else if( CompFlags.jis_to_unicode ) {
                    chr = JIS2Unicode( chr );
                }
                STORE_WCHAR( tgt, chr, new_len );
            } else {
//              _ASCIIOUT( chr );
                STORE_CHAR( tgt, chr, new_len );
            }
        }
    }

#undef STORE_CHAR
#undef STORE_WCHAR

    return( new_len );
}


static STRING_CONSTANT makeLiteral( const char *s, target_size_t len, bool wide )
/*******************************************************************************/
{
    STRING_CONSTANT literal;
    target_size_t   new_len;

    new_len = compressLiteral( NULL, s, len + 1, wide );
    literal = allocLiteral( new_len, ( wide ) ? STRLIT_WIDE : STRLIT_NONE );
    compressLiteral( literal->string, s, len + 1, wide );
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


STRING_CONSTANT StringCreate( const char *s, size_t len, bool wide )
/******************************************************************/
{
    ++stringCount;
    return( stringAdd( makeLiteral( s, len, wide ), &uniqueStrings ) );
}

void StringConcatDifferentLines( STRING_CONSTANT v )
/**************************************************/
{
    v->flags |= STRLIT_MLINE;
}

STRING_CONSTANT StringConcat( STRING_CONSTANT v1, STRING_CONSTANT v2 )
/********************************************************************/
{
    STRING_CONSTANT literal;
    target_size_t   v1_len;

    if( (v1->flags & STRLIT_WIDE) != (v2->flags & STRLIT_WIDE) ) {
        // an error has already been diagnosed
        StringTrash( v2 );
        return( v1 );
    }
    v1_len = StringLength( v1 );
    literal = allocLiteral( v1_len + v2->len, v1->flags | STRLIT_CONCAT );
    memcpy( literal->string, v1->string, v1_len );
    memcpy( &(literal->string[v1_len]), v2->string, v2->len );
    StringTrash( v1 );
    StringTrash( v2 );
    ++stringCount;
    return( stringAdd( literal, &uniqueStrings ) );
}

bool StringSame( STRING_CONSTANT v1, STRING_CONSTANT v2 )
/*******************************************************/
{
    if( v1->len != v2->len ) {
        return( false );
    }
    return( memcmp( v1->string, v2->string, v1->len ) == 0 );
}

target_size_t StringLength( STRING_CONSTANT s )
/**********************************************
 * length doesn't include '\0' character
 */
{
    return( s->len - STRING_CHARSIZE( s ) );
}

target_size_t StringAWStrLen( STRING_CONSTANT s )
/***********************************************/
{
    return( s->len / STRING_CHARSIZE( s ) );
}

target_size_t StringAlign( STRING_CONSTANT s )
/********************************************/
{
    target_size_t len;

#if _CPU == _AXP
    (void)s;

    len = TARGET_INT;
#else
    if( s->flags & STRLIT_WIDE ) {
        len = TARGET_WIDE_CHAR;
    } else {
        len = TARGET_CHAR;
    }
#endif
    return( len );
}

target_size_t StringCharSize( STRING_CONSTANT s )
/***********************************************/
{
    return( STRING_CHARSIZE( s ) );
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

pch_status PCHInitStringPool( bool writing )
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
        unsigned i;
        for( i = 1; i < stringCount; ++i ) {
            if( stringTranslateTable[i -1 ] == stringTranslateTable[i] ) {
                CFatal( "two identical strings in translation table" );
            }
        }
    }
#endif
    return( PCHCB_OK );
}

pch_status PCHFiniStringPool( bool writing )
{
    /* unused parameters */ (void)writing;

    CMemFreePtr( &stringTranslateTable );
    return( PCHCB_OK );
}

pch_status PCHReadStringPool( void )
{
    STRING_CONSTANT *p;
    STRING_CONSTANT str;
    target_size_t str_len;

    while( uniqueStrings != NULL ) {
        StringTrash( uniqueStrings );
    }
    stringCount = PCHReadUInt();
    stringTranslateTable = CMemAlloc( stringCount * sizeof( STRING_CONSTANT ) );
    p = stringTranslateTable;
    for( ; (str_len = PCHReadUInt()) != 0; ) {
        str = allocLiteral( str_len, PCHReadUInt() );
        PCHRead( str->string, str_len );
        stringAdd( str, &uniqueStrings );
        *p++ = str;
    }
    return( PCHCB_OK );
}

pch_status PCHWriteStringPool( void )
{
    unsigned i;
    STRING_CONSTANT str;
    STRING_CONSTANT *p;

    PCHWriteUInt( stringCount );
    p = stringTranslateTable;
    for( i = 0; i < stringCount; ++i ) {
        str = p[i];
        PCHWriteUInt( str->len );
        PCHWriteUInt( str->flags );
        PCHWrite( str->string, str->len );
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
    return( stringTranslateTable[PCHGetUInt( index ) - PCH_FIRST_INDEX] );
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
        DbgStmt( CFatal( "invalid string passed to StringGetIndex" ) );
        return( PCHSetUInt( PCH_ERROR_INDEX ) );
    }
    return( PCHSetUInt( ( found - stringTranslateTable ) + PCH_FIRST_INDEX ) );
}
