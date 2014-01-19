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

#include <limits.h>

#include "carve.h"
#include "name.h"
#include "initdefs.h"
#include "stats.h"
#include "pcheader.h"
#include "hashtab.h"

#define BLOCK_HASHTAB           (32) // number of HASTAB struct pre-allocated

#define MIN_CARVE_SIZE          (1024) // minimum size to carve
#define CARVE_TABLE_SIZE        ((MAX_HASHTAB_SIZE - MIN_HASHTAB_SIZE) + 1)

static carve_t carveHASHTAB;
static carve_t carveTable[CARVE_TABLE_SIZE];

static SYMBOL_NAME listSentinel;

ExtraRptCtr( hash_lookups );
ExtraRptCtr( hash_lookup_cost );
ExtraRptCtr( hash_searches );
ExtraRptCtr( hash_lookup_fail );
ExtraRptCtr( hash_inserts );

#define MAX_HASH                0x0fff
#define EXPAND_THRESHOLD        5

struct hash_tab {
    unsigned    avg;            // current average    (num_keys/buckets)
    unsigned    remainder;      // current remainder  (num_keys%buckets)
    unsigned    expand_next;    // next bucket to expand 0<=expand_next<half
    unsigned    half;           // number of buckets in full half of table
    SYMBOL_NAME *table;         // table of size 2*half, with half+expand_next
                                // buckets used
#ifndef NDEBUG
    unsigned    uniques;        // number of unique names inserted
    unsigned    looks;          // number of search requests
    unsigned    max_probes;     // largest number of probes needed to find name
    unsigned    sum_probes;     // total number of probes used
#endif
};

unsigned countBits( unsigned x )
{
    unsigned y;

    y = x & 0x55555555;
    x = y + (( x ^ y ) >> 1 );
    y = x & 0x33333333;
    x = y + (( x ^ y ) >> 2 );
    y = x & 0x0f0f0f0f;
    x = y + (( x ^ y ) >> 4 );
    y = x & 0x00ff00ff;
    x = y + (( x ^ y ) >> 8 );
    y = x & 0x0000ffff;
    x = y + (( x ^ y ) >> 16 );
    return( x );
}

static unsigned base2( unsigned n )
{
    unsigned nbits;

    DbgAssert( ( n & -n ) == n );       // verify number is a power of 2
    // given 010000(base 2) we want 4 ( 1 << 4 == 010000(base 2) )
    // algorithm:
    //   010000 - 1 = 01111
    //   count bits(01111) = 4
    nbits = countBits( n - 1 );
    return nbits;
}

static void hashInit( INITFINI *defn )
{
    unsigned i;
    unsigned size;
    unsigned blocking;

    defn = defn;
    carveHASHTAB = CarveCreate( sizeof( struct hash_tab ), BLOCK_HASHTAB );
    size = (1 << MIN_HASHTAB_SIZE ) * sizeof( SYMBOL_NAME );
    for( i = 0; i < CARVE_TABLE_SIZE; i++ ) {
        blocking = 2;
        if( size < MIN_CARVE_SIZE ) {
            blocking = MIN_CARVE_SIZE / size;
            if( blocking <= 1 ) {
                blocking = 2;
            }
        }
        carveTable[i] = CarveCreate( size, blocking );
        size <<= 1;
    }

    ExtraRptRegisterCtr( &hash_lookups, "hash table lookups" );
    ExtraRptRegisterCtr( &hash_lookup_fail, "hash table lookup failures (includes early exits)" );
    ExtraRptRegisterCtr( &hash_lookup_cost, NULL );
    ExtraRptRegisterAvg( &hash_lookup_cost, &hash_lookups, "average # comparisons per search" );
    ExtraRptRegisterCtr( &hash_inserts, "hash table inserts" );
}

static void hashFini( INITFINI* defn )
{
    unsigned i;

    defn = defn;
    CarveDestroy( carveHASHTAB );
    for( i = 0; i < CARVE_TABLE_SIZE; i++ ) {
        CarveDestroy( carveTable[i] );
    }
}

INITDEFN( hashing, hashInit, hashFini )

void HashPostInit( SCOPE scope )
/******************************/
{
    // the symbol-name needs a scope but the scope needs a sentinel
    // so we have a two-step initialization:
    // HashPostInit( NULL );
    // allocate a scope
    // HashPostInit( first-allocated-scope );
    if( scope == NULL ) {
        listSentinel = AllocSymbolName( NULL, NULL );
    } else {
        ScopeSetContaining( listSentinel, scope );
    }
}

HASHTAB HashCreate( unsigned init_table_size )
/********************************************/
{
    HASHTAB hash;
    SYMBOL_NAME init_name;
    SYMBOL_NAME *table;
    unsigned i;
    unsigned buckets;
    unsigned half;
    unsigned expand_next;       // same value as 'half' but may not always be

    DbgAssert( init_table_size >= MIN_HASHTAB_SIZE );
    DbgAssert( init_table_size <= MAX_HASHTAB_SIZE );
    hash = CarveAlloc( carveHASHTAB );
    hash->avg = 0;
    hash->remainder = 0;
    half = 1 << (init_table_size - 1);
    expand_next = half;
    hash->half = half;
    hash->expand_next = expand_next;
    DbgAssert( half != 0 );
    table = CarveAlloc( carveTable[init_table_size - MIN_HASHTAB_SIZE] );
    hash->table = table;
    buckets = half + expand_next;
    init_name = listSentinel;
    for( i = 0; i < buckets; ++i ) {
        table[i] = init_name;
    }
#ifndef NDEBUG
    hash->uniques = 0;
    hash->looks = 0;
    hash->max_probes = 0;
    hash->sum_probes = 0;
#endif
    return( hash );
}

void HashDestroy( HASHTAB hash )
/*******************************/
{
    CarveFree( carveTable[base2( hash->half ) + 1 - MIN_HASHTAB_SIZE], hash->table );
    CarveFree( carveHASHTAB, hash );
}

bool HashEmpty( HASHTAB hash )
/****************************/
{
    if( hash->avg == 0 && hash->remainder == 0 ) {
        return( TRUE );
    }
    return( FALSE );
}

HASHTAB HashMakeMax( HASHTAB hash )
/*********************************/
{
    DbgAssert( HashEmpty( hash ) );
    HashDestroy( hash );
    return( HashCreate( MAX_HASHTAB_SIZE ) );
}

SYMBOL_NAME HashLookup( HASHTAB hash, NAME name )
/***********************************************/
{
    unsigned x;
    unsigned half;
    unsigned mask;
    unsigned xmask;
    unsigned expand_next;
    SYMBOL_NAME *head;
    SYMBOL_NAME *prev;
    SYMBOL_NAME s;
    DbgStmt( unsigned probes );

    x = NameHash( name );
    ExtraRptIncrementCtr( hash_lookups );
    DbgStmt( hash->looks++ );
    half = hash->half;
    mask = half - 1;
    xmask = x & mask;
    expand_next = hash->expand_next;
    if( xmask < expand_next ) { // name hashes to already expanded bucket
        xmask |= x & half;
    }
    ExtraRptIncrementCtr( hash_searches );
    listSentinel->name = name;
    head = &(hash->table[xmask]);
    prev = head;
    s = *prev;
    DbgStmt( probes = 1 );
    ExtraRptIncrementCtr( hash_lookup_cost );
    for( ; s->name != name; ) {
        prev = &(s->next);
        s = *prev;
        DbgStmt( ++probes );
        ExtraRptIncrementCtr( hash_lookup_cost );
    }
#ifndef NDEBUG
    hash->sum_probes += probes;
    if( probes > hash->max_probes ) {
        hash->max_probes = probes;
    }
#endif
    if( s != listSentinel ) {
        /* move name to front of list */
        *prev = s->next;
        s->next = *head;
        *head = s;
        return( s );
    }
    /* found sentinel so we didn't find the name */
    ExtraRptIncrementCtr( hash_lookup_fail );
    return( NULL );
}


#ifndef NDEBUG
double dragonStat( HASHTAB hash )
/*******************************/
{
    unsigned i;
    unsigned buckets;
    double dragon_stat;
    double random_stat;

    // see p.436 of dragon book
    buckets = hash->half + hash->expand_next;
    dragon_stat = 0;
    for( i = 0; i < buckets; ++i ) {
        SYMBOL_NAME s;
        SYMBOL_NAME stop = listSentinel;
        unsigned count = 0; // count number of names in bucket i

        for( s = hash->table[i]; s != stop; s = s->next ) {
            ++count;
        }
        // 1+2+...+i = i(i+1)/2 =
        // compares used to look up each name in bucket i
        dragon_stat += count * ( count + 1 ) / 2;
    }
    random_stat = hash->uniques * ( hash->uniques + 2.0 * buckets - 1 ) / ( 2 * buckets );
    if( random_stat == 0 ) {
        ++random_stat;
    }
    return( dragon_stat / random_stat );
}

void StatsHASHTAB( HASHTAB hash )
/******************************/
{
    unsigned avg_i;
    unsigned avg_d;

    printf( "HASHTAB: %p\n", hash );
    printf( "  current average       %u\n", hash->avg );
    printf( "  current remainder     %u\n", hash->remainder );
    printf( "  # of buckets          %u\n", hash->half + hash->expand_next );
    printf( "  next bucket to expand %u\n", hash->expand_next );
    printf( "  buckets in full half of table %u\n", hash->half );

    printf( "looks: %u\n", hash->looks );
    printf( "uniques: %u\n", hash->uniques );
    printf( "sum probes: %u\n", hash->sum_probes );
    printf( "max probes: %u\n", hash->max_probes );
    avg_i = 0;
    avg_d = 0;
    if( hash->looks ) {
        avg_i = hash->sum_probes / hash->looks;
        avg_d = ( hash->sum_probes * 100 / hash->looks ) % 100;
    }
    printf( "avg probes: %u.%02u\n", avg_i, avg_d );
    printf( "uniform ratio: %f\n", dragonStat( hash ) );
    DbgAssert( avg_i <= hash->max_probes );
}
#endif

void expandHASHTAB( HASHTAB hash )
/********************************/
{
    unsigned expand_next;
    unsigned half;
    unsigned buckets;

    half = hash->half;
    expand_next = hash->expand_next;
    buckets = half + expand_next;
    if( buckets < MAX_HASH + 1 ) {
        unsigned num_keys;
        unsigned avg;
        unsigned old_size;
        unsigned x;
        SYMBOL_NAME next;
        SYMBOL_NAME s;
        SYMBOL_NAME stop;
        SYMBOL_NAME *source; // bucket being split
        SYMBOL_NAME *target; // other bucket half of source being split to
        SYMBOL_NAME source_list;
        SYMBOL_NAME *table;
        SYMBOL_NAME *old_table;

        if( expand_next == half  ) {
            expand_next = 0;
            hash->expand_next = expand_next;
            half <<= 1;
            old_size = base2( half );
            DbgAssert( old_size + 1 <= MAX_HASHTAB_SIZE );
            hash->half = half;
            old_table = hash->table;
            table = CarveAlloc( carveTable[old_size + 1 - MIN_HASHTAB_SIZE] );
            hash->table = table;
            memcpy( table, old_table, half * sizeof( SYMBOL_NAME ) );
            CarveFree( carveTable[old_size - MIN_HASHTAB_SIZE], old_table );
        }
        buckets = expand_next + half;
        num_keys = hash->avg * buckets + hash->remainder;
        hash->expand_next++;
        ++buckets;
        avg = num_keys / buckets;
        hash->avg = avg;
        hash->remainder = num_keys - avg * buckets;
        source = &(hash->table[expand_next]);
        target = source + half;
        source_list = *source;
        *source = NULL;
        *target = NULL;
        stop = listSentinel;
        for( s = source_list; s != stop; s = next ) {
            next = s->next;
            s->next = NULL;
            x = NameHash( s->name );
            if( x & half ) {
                *target = s;
                target = &(s->next);
            } else {
                *source = s;
                source = &(s->next);
            }
        }
        *source = stop;
        *target = stop;
    }
}

void HashInsert( HASHTAB hash, SYMBOL_NAME sym_name, NAME name )
/**************************************************************/
{
    unsigned mask;
    unsigned buckets;
    unsigned half;
    unsigned expand_next;
    unsigned x;
    unsigned xmask;
    SYMBOL_NAME *head;

    /* assumption: HashLookup just returned NULL for this name */
    DbgAssert( CompFlags.extra_stats_wanted || HashLookup( hash, name ) == NULL );
    ExtraRptIncrementCtr( hash_inserts );
    DbgStmt( hash->uniques++ );

    x = NameHash( name );
    half = hash->half;
    mask = half - 1;
    xmask = x & mask;
    expand_next = hash->expand_next;
    if( xmask < expand_next ) { // name hashes to already expanded bucket
        xmask |= x & half;
    }
    head = &(hash->table[xmask]);
    sym_name->next = *head;
    *head = sym_name;
    buckets = half + expand_next;
    if( ++(hash->remainder) == buckets ) {
        hash->remainder = 0;
        hash->avg++;
        if( hash->avg > EXPAND_THRESHOLD ) {
            expandHASHTAB( hash );
        }
    }
}

void HashWalk( HASHTAB hash, void (*do_it)( SYMBOL_NAME ) )
/*********************************************************/
{
    unsigned i;
    unsigned buckets;

    buckets = hash->half + hash->expand_next;
    for( i = 0; i < buckets; ++i ) {
        SYMBOL_NAME s;
        SYMBOL_NAME stop;
        SYMBOL_NAME next;

        stop = listSentinel;
        for( s = hash->table[i]; s != stop; s = next ) {
            next = s->next;
            (*do_it)( s );
        }
    }
}

void HashWalkData( HASHTAB hash, void (*do_it)( SYMBOL_NAME, void * ), void *data )
/*********************************************************************************/
{
    unsigned i;
    unsigned buckets;

    buckets = hash->half + hash->expand_next;
    for( i = 0; i < buckets; ++i ) {
        SYMBOL_NAME s;
        SYMBOL_NAME next;
        SYMBOL_NAME stop = listSentinel;
        for( s = hash->table[i]; s != stop; s = next ) {
            next = s->next;
            (*do_it)( s, data );
        }
    }
}

static void markFreeHashTable( void *p )
{
    HASHTAB hash = p;

    hash->half = 0;
}

static void saveHashTable( void *e, carve_walk_base *d )
{
    HASHTAB hash = e;
    unsigned i;
    SYMBOL_NAME *src_links;
    unsigned buckets;

    if( hash->half == 0 ) {
        return;
    }
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *hash );
    src_links = hash->table;
    buckets = hash->half + hash->expand_next;
    for( i = 0; i < buckets; ++i ) {
        SymbolNamePCHWrite( src_links[i] );
    }
}

pch_status PCHWriteHashTables( void )
/***********************************/
{
    auto carve_walk_base data;

    SymbolNamePCHWrite( listSentinel );
    CarveWalkAllFree( carveHASHTAB, markFreeHashTable );
    CarveWalkAll( carveHASHTAB, saveHashTable, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHReadHashTables( void )
/**********************************/
{
    int i;
    HASHTAB hash;
    SYMBOL_NAME *links;
    unsigned half;
    unsigned size;
    unsigned buckets;
    auto cvinit_t data;

    listSentinel = SymbolNamePCHRead();
    CarveInitStart( carveHASHTAB, &data );
    for( ; (hash = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *hash );
        half = hash->half;
        size = base2( half ) + 1;
        links = CarveAlloc( carveTable[size - MIN_HASHTAB_SIZE] );
        hash->table = links;
        buckets = half + hash->expand_next;
        for( i = 0; i < buckets; ++i ) {
            links[i] = SymbolNamePCHRead();
        }
    }
    return( PCHCB_OK );
}

HASHTAB HashGetIndex( HASHTAB e )
/*******************************/
{
    return( CarveGetIndex( carveHASHTAB, e ) );
}

HASHTAB HashMapIndex( HASHTAB i )
/*******************************/
{
    return( CarveMapIndex( carveHASHTAB, i ) );
}

pch_status PCHInitHashTables( bool writing )
/******************************************/
{
    unsigned i;

    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveHASHTAB ) );
        for( i = 0; i < CARVE_TABLE_SIZE; i++ ) {
            PCHWriteCVIndex( CarveLastValidIndex( carveTable[i] ) );
        }
    } else {
        carveHASHTAB = CarveRestart( carveHASHTAB );
        CarveMapOptimize( carveHASHTAB, PCHReadCVIndex() );
        for( i = 0; i < CARVE_TABLE_SIZE; i++ ) {
            carveTable[i] = CarveRestart( carveTable[i] );
            CarveMapOptimize( carveTable[i], PCHReadCVIndex() );
        }
    }
    return( PCHCB_OK );
}

pch_status PCHFiniHashTables( bool writing )
/******************************************/
{
    unsigned i;
    if( ! writing ) {
        CarveMapUnoptimize( carveHASHTAB );
        for( i = 0; i < CARVE_TABLE_SIZE; i++ ) {
            CarveMapUnoptimize( carveTable[i] );
        }
    }
    return( PCHCB_OK );
}
