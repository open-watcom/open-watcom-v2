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


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "plusplus.h"
#include "errdefns.h"
#include "memmgr.h"
#include "name.h"
#include "carve.h"
#include "initdefs.h"
#include "stats.h"
#include "pcheader.h"
#include "toggle.h"
#ifndef NDEBUG
#include "pragdefn.h"
#endif

#pragma pack(1);
typedef struct name NAME;
struct name {
    NAME                *next;
    uint_16             xhash;
    name_hash_t         hash;
    char                name[1];
};
#pragma pack();

#define NAME_TABLE_HASH NAME_HASH

static NAME *hashTable[ NAME_TABLE_HASH ];

static name_dummy_index_t nameDummyIndex;
static unsigned long nameCount;

static NAME **nameTranslateTable;

static struct {
    unsigned            no_creates_allowed : 1; // name list is frozen
} nameFlags;

ExtraRptCtr( ctr_names );
ExtraRptCtr( ctr_dummy_names );
ExtraRptCtr( ctr_chains );
ExtraRptCtr( ctr_searches );
ExtraRptCtr( ctr_probes );
ExtraRptCtr( ctr_hashes );
ExtraRptCtr( ctr_length );
ExtraRptCtr( ctr_memcmp );
ExtraRptCtr( ctr_memcmp_fail );
ExtraRptCtr( ctr_max_length );

unsigned const NameCmpMask[5] = {
    0x00000000,
    0x000000ff,
    0x0000ffff,
    0x00ffffff,
    0xffffffff,
};

int NameMemCmp( char const *t1, char const *t2, unsigned len )
/************************************************************/
{
    unsigned *s1 = (unsigned*) t1;
    unsigned *s2 = (unsigned*) t2;
    unsigned mask;
    int diff;

    // can only check for one null byte since the common length
    // may access invalid memory with the other string
    DbgAssert( t2[len-1] == 0 );
    if( len > sizeof( unsigned ) ) {
        do {
            diff = *s1 - *s2;
            if( diff != 0 ) {
                return( diff );
            }
            ++s1;
            ++s2;
            len -= sizeof( unsigned );
        } while( len > sizeof( unsigned ) );
    }
    mask = NameCmpMask[ len ];
    return( ( *s1 & mask ) - ( *s2 & mask ) );
}

unsigned NameCalcHashLen( char const *id, size_t len )
/**********************************************/
{
    unsigned *s = (unsigned*) id;
    unsigned mask;
    unsigned c;
    unsigned g;
    unsigned h;

    ExtraRptIncrementCtr( ctr_hashes );
    /* modelled on hashpjw from the Dragon book */
    /* should not be used in mangled names so that it can change in patches */
    h = len;
    c = len;
    if( len > sizeof( unsigned ) ) {
        do {
            c ^= *s;
            h = ( h << 4 ) + c;
            g = h & ~0x0ffffff;
            h ^= g;
            h ^= g >> (4+4+4+4+4);
            ++s;
            len -= sizeof( unsigned );
        } while( len > sizeof( unsigned ) );
    }
    mask = NameCmpMask[ len ];
    c ^= *s & mask;
    h = ( h << 4 ) + c;
    g = h & ~0x0ffffff;
    h ^= g;
    h ^= g >> (4+4+4+4+4);
    g = h & ~0x0fff;
    h ^= g;
    h ^= g >> (4+4+4);
    h ^= h >> (2+4);
    DbgAssert(( h % NAME_HASH ) == h );
    return( h );
}

static char *nameAdd( NAME **head, unsigned bucket, unsigned xhash,
                      char *id, size_t len )
{
    NAME *name;

#ifdef XTRA_RPT
    if( *head == NULL ) {
        ExtraRptIncrementCtr( ctr_chains );
    }
#endif
    ExtraRptIncrementCtr( ctr_names );
    DbgVerify( ! nameFlags.no_creates_allowed
             , "name create occurred during precompiled header processing" );
    name = CPermAlloc( sizeof( NAME ) + len );
    memcpy( name->name, id, len + 1 );
    name->xhash = xhash;
    name->hash = bucket;
    name->next = *head;
    *head = name;
    ++nameCount;
    return( name->name );
}


char *NameCreateLen( char *id, unsigned len )
/*******************************************/
{
    unsigned xhash;
    unsigned bucket;
    size_t cmp_len;
    NAME *name;
    NAME **head;
    NAME **prev;

    ExtraRptIncrementCtr( ctr_searches );
    xhash = NameCalcHashLen( id, len );
#if NAME_TABLE_HASH != NAME_HASH
    bucket = xhash % NAME_TABLE_HASH;
#else
    bucket = xhash;
#endif
    // xhash cannot overflow a uint_16 ( 0x0fff + 0x00ff <= 0x1fff )
    xhash += (uint_8) len;
    head = &(hashTable[ bucket ]);
    prev = head;
    cmp_len = len + 1;
    ExtraRptZeroCtr( ctr_length );
    for( name = *prev; ; name = *prev ) {
        ExtraRptIncrementCtr( ctr_probes );
        ExtraRptIncrementCtr( ctr_length );
        if( name == NULL ) break;
        if( name->xhash == xhash ) {
            ExtraRptIncrementCtr( ctr_memcmp );
            if( NameMemCmp( name->name, id, cmp_len ) == 0 ) {
                /* move name to front of list in hash table */
                *prev = name->next;
                name->next = *head;
                *head = name;
                return( name->name );
            }
            ExtraRptIncrementCtr( ctr_memcmp_fail );
        }
        prev = &(name->next);
    }
    ExtraRptMaximum( ctr_length, ctr_max_length );
    return( nameAdd( head, bucket, xhash, id, len ) );
}


char *NameCreateNoLen( char *id )
/*******************************/
{
    size_t len;
    name_dummy_index_t ni;
    unsigned xhash;
    unsigned bucket;
    NAME **head;

    len = strlen( id );
    if( id[0] != NAME_DUMMY_PREFIX_0 ) {
        return( NameCreateLen( id, len ) );
    }
    // everybody uses the same 'name' so the hash doesn't have to be generated
    // from the name contents
    ni = nameDummyIndex++;
    xhash = ni % NAME_TABLE_HASH;
    bucket = xhash;
    head = &(hashTable[ bucket ]);
    return( nameAdd( head, bucket, xhash, id, len ) );
}

name_dummy_index_t NameNextDummyIndex( void )
/*******************************************/
{
    return( nameDummyIndex++ );
}

char *NameDummy( void )
/*********************/
{
    name_dummy_index_t ni;
    unsigned xhash;
    unsigned bucket;
    size_t len;
    NAME **head;
    char buff[ 1 + 1 + sizeof( ni ) * 3 + 1 ];

    ExtraRptIncrementCtr( ctr_dummy_names );
    ni = nameDummyIndex++;
    xhash = ni % NAME_TABLE_HASH;
    bucket = xhash;
    buff[0] = NAME_DUMMY_PREFIX_0;
    buff[1] = NAME_DUMMY_PREFIX_1;
    // the contents of the name don't have to be different just the address
    // but for debugging it is handy to have unique contents
#ifndef NDEBUG
    ultoa( ni, &buff[2], 10 );
    len = strlen( buff );
#else
    buff[2] = '0';
    buff[3] = '\0';
    len = 3;
#endif
    head = &(hashTable[ bucket ]);
    return( nameAdd( head, bucket, xhash, buff, len ) );
}

boolean IsNameDummy( char *name )
/*******************************/
{
    if( name[0] != NAME_DUMMY_PREFIX_0 ) {
        return( FALSE );
    }
    if( name[1] != NAME_DUMMY_PREFIX_1 ) {
        return( FALSE );
    }
    return( TRUE );
}

static int cmpName( const void *lp, const void *rp )
{
    NAME *left = *(NAME **)lp;
    NAME *right = *(NAME **)rp;

    if( left < right ) {
        return( -1 );
    } else if( left > right ) {
        return( 1 );
    }
    return( 0 );
}

pch_status PCHInitNames( boolean writing )
{
    int i;
    NAME *name;
    NAME **p;

    if( ! writing ) {
        return( PCHCB_OK );
    }
    nameTranslateTable = CMemAlloc( nameCount * sizeof( NAME * ) );
    p = nameTranslateTable;
    for( i = 0; i < NAME_TABLE_HASH; ++i ) {
        for( name = hashTable[ i ]; name != NULL; name = name->next ) {
            *p = name;
            ++p;
        }
    }
    qsort( nameTranslateTable, nameCount, sizeof( NAME * ), cmpName );
#ifndef NDEBUG
    {
        int i;
        for( i = 1; i < nameCount; ++i ) {
            if( nameTranslateTable[i-1] == nameTranslateTable[i] ) {
                CFatal( "two identical names in translation table" );
            }
        }
    }
#endif
    nameFlags.no_creates_allowed = TRUE;
    return( PCHCB_OK );
}

pch_status PCHFiniNames( boolean writing )
{
    writing = writing;
    nameFlags.no_creates_allowed = FALSE;
    CMemFreePtr( &nameTranslateTable );
    return( PCHCB_OK );
}

pch_status PCHReadNames( void )
{
    size_t name_len;
    NAME **p;
    NAME **head;
    NAME *name;

    memset( hashTable, 0, sizeof( hashTable ) );
    PCHRead( &nameCount, sizeof( nameCount ) );
    PCHRead( &nameDummyIndex, sizeof( nameDummyIndex ) );
    nameTranslateTable = CMemAlloc( nameCount * sizeof( NAME * ) );
    p = nameTranslateTable;
    for(;;) {
        name_len = PCHReadUInt();
        if( name_len == 0 ) break;
        name = CPermAlloc( sizeof( NAME ) + name_len );
        PCHRead( &(name->xhash), sizeof(*name) - sizeof(name->next) + name_len );
        head = &hashTable[ name->hash ];
        name->next = *head;
        *head = name;
        *p = name;
        ++p;
    }
    return( PCHCB_OK );
}

pch_status PCHWriteNames( void )
{
    size_t dummy_len;
    int i;
    NAME *name;
    NAME **p;

    PCHWrite( &nameCount, sizeof( nameCount ) );
    PCHWrite( &nameDummyIndex, sizeof( nameDummyIndex ) );
    p = nameTranslateTable;
    for( i = 0; i < nameCount; ++i ) {
        name = p[i];
        dummy_len = strlen( name->name );
        DbgAssert( dummy_len != 0 );
        PCHWriteUInt( dummy_len );
        PCHWrite( &(name->xhash), sizeof(*name) - sizeof(name->next) + dummy_len );
    }
    dummy_len = 0;
    PCHWriteUInt( dummy_len );
    return( PCHCB_OK );
}

char *NameMapIndex( char *index )
/*******************************/
{
    if( index < (char *) PCH_FIRST_INDEX ) {
        return( NULL );
    }
#ifndef NDEBUG
    if( ((unsigned) index) >= nameCount + PCH_FIRST_INDEX ) {
        CFatal( "invalid name index" );
    }
#endif
    return nameTranslateTable[ ((unsigned) index) - PCH_FIRST_INDEX ]->name;
}

static int cmpFindName( const void *kp, const void *tp )
{
    char *key = *(char **)kp;
    char *table = (*((NAME **)tp))->name;

    if( key < table ) {
        return( -1 );
    } else if( key > table ) {
        return( 1 );
    }
    return( 0 );
}

char *NameGetIndex( char *name )
/******************************/
{
    NAME **found;

    if( name == NULL ) {
        return( (char *) PCH_NULL_INDEX );
    }
    found = bsearch( &name, nameTranslateTable, nameCount, sizeof( NAME * ), cmpFindName );
    if( found == NULL ) {
#ifndef NDEBUG
        CFatal( "invalid name passed to NameGetIndex" );
#endif
        return( (char *) PCH_ERROR_INDEX );
    }
    return( (char *) (( found - nameTranslateTable ) + PCH_FIRST_INDEX ) );
}

static void init(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    if( CompFlags.dll_subsequent ) {
        memset( hashTable, 0, sizeof( hashTable ) );
        nameDummyIndex = 0;
        nameCount = 0;
    }
    ExtraRptRegisterCtr( &ctr_hashes, "# names hashed" );
    ExtraRptRegisterCtr( &ctr_searches, "# names searched" );
    ExtraRptRegisterCtr( &ctr_probes, NULL );
    ExtraRptRegisterAvg( &ctr_probes
                       , &ctr_searches
                       , "average length of name search" );
    ExtraRptRegisterCtr( &ctr_names, "# names defined (includes dummy names)" );
    ExtraRptRegisterCtr( &ctr_dummy_names, "# dummy names defined" );
    ExtraRptRegisterCtr( &ctr_chains, "# name chains defined" );
    ExtraRptRegisterAvg( &ctr_names
                       , &ctr_chains
                       , "average length of name chain" );
    ExtraRptRegisterCtr( &ctr_length, NULL );
    ExtraRptRegisterCtr( &ctr_memcmp, "# of memcmps" );
    ExtraRptRegisterCtr( &ctr_memcmp_fail, "# of memcmps that failed" );
    ExtraRptRegisterMax( &ctr_max_length, "maximum name chain" );
}

#ifndef NDEBUG
void dumpNames( void )
{
    unsigned long sum;
    unsigned long length;
    unsigned long max;
    unsigned long min;
    double uniform_stat;
    double perfect_stat;
    int i;
    NAME *name;
    FILE *fp;
    static unsigned freq[100];

    sum = 0;
    max = 0;
    min = nameCount;
    fp = fopen( "name.lst", "w" );
    if( fp == NULL ) {
        return;
    }
    uniform_stat = 0;   // see p.436 of dragon book
    fprintf( fp, "histogram chart of chain contents:\n" );
    for( i = 0; i < NAME_TABLE_HASH; ++i ) {
        fprintf( fp, "%4u:", i );
        name = hashTable[ i ];
        length = 0;
        for( ; name != NULL; name = name->next ) {
            fprintf( fp, " %s", name->name );
            ++length;
        }
        uniform_stat += length * ( length + 1 ) / 2;
        if( length > max ) {
            max = length;
        }
        if( length < min ) {
            min = length;
        }
        sum += length;
        putc( '\n', fp );
        if( length >= (sizeof(freq)/sizeof(freq[0])) ) {
            length = (sizeof(freq)/sizeof(freq[0]))-1;
        }
        freq[ length ]++;
    }
    perfect_stat = sum * ( sum + 2.0 * NAME_TABLE_HASH - 1 ) / (2*NAME_TABLE_HASH);
    fprintf( fp, "histogram chart of chain lengths:\n" );
    for( i = 0; i < NAME_TABLE_HASH; ++i ) {
        name = hashTable[ i ];
        length = 0;
        for( ; name != NULL; name = name->next ) {
            ++length;
        }
        if( length < min ) {
            min = length;
        }
        fprintf( fp, "%4u: length %4u: ", i, length );
        while( length != 0 ) {
            putc( '*', fp );
            --length;
        }
        putc( '\n', fp );
    }
    fprintf( fp, "frequency chart of chain lengths:\n" );
    for( i = 0; i < (sizeof(freq)/sizeof(freq[0])); ++i ) {
        if( freq[i] ) {
            fprintf( fp, "%4u[%4u]: ", i, freq[i] );
            while( freq[i] ) {
                putc( '*', fp );
                --freq[i];
            }
            putc( '\n', fp );
        }
    }
    fprintf( fp, "total names:    %6u\n", sum );
    fprintf( fp, "longest chain:  %6u\n", max );
    fprintf( fp, "shortest chain: %6u\n", min );
    fprintf( fp, "average chain:  %6u\n", sum / NAME_TABLE_HASH );
    fprintf( fp, "quality ratio:  %f (should be close to 1.0)\n", uniform_stat / perfect_stat );
    fclose( fp );
}
#endif

static void fini(               // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
#ifndef NDEBUG
    if( PragDbgToggle.dump_names ) {
        dumpNames();
        puts( "created name.lst" );
    }
#endif
}


INITDEFN( names, init, fini );
