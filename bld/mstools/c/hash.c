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


#include "error.h"
#include "hash.h"
#include "memory.h"


/*
 * Define some types.
 */
typedef struct _ListElem {
    const void *        data;
    struct _ListElem *  next;
} ListElem;

typedef struct _Hash {
    unsigned            numbuckets;
    HashFunction        hashfunc;
    HashCompareFunction comparefunc;
    ListElem **         buckets;
} Hash;


/*
 * Return non-zero if the given item is in the given bucket.
 */
static int is_in_bucket( const Hash *table, const void *item,
                         unsigned bucket )
/***********************************************************/
{
    ListElem *          curelem;

    curelem = table->buckets[bucket];
    while( curelem != NULL ) {
        if( (*table->comparefunc)( item, (const void*)curelem->data ) ) {
            return( 1 );
        }
        curelem = curelem->next;
    }

    return( 0 );
}


/*
 * Create a new hash table.  Note that 'hashfunc' will return a hash value
 * given an element, and 'comparefunc' will return non-zero iff the two
 * elements passed to it are equal.
 */
HashTable InitHash( unsigned numbuckets, HashFunction hashfunc,
                    HashCompareFunction comparefunc )
/*************************************************************/
{
    Hash *              table;
    unsigned            count;

    if( numbuckets <= 0 )  Zoinks();
    table = AllocMem( sizeof( Hash ) );
    table->numbuckets = numbuckets;
    table->hashfunc = hashfunc;
    table->comparefunc = comparefunc;
    table->buckets = AllocMem( numbuckets * sizeof( ListElem* ) );
    for( count=0; count<numbuckets; count++ ) {
        table->buckets[count] = NULL;
    }
    return( table );
}


/*
 * Destroy a hash table.  If 'freeitems' is nonzero, FreeMem will be
 * called on each element before it is removed from the hash table.
 */
void FiniHash( HashTable _table, int freeitems )
/**********************************************/
{
    Hash *              table = (Hash*)_table;
    unsigned            count;
    ListElem *          curelem;
    ListElem *          nextelem;

    if( table == NULL )  Zoinks();
    for( count=0; count<table->numbuckets; count++ ) {
        curelem = table->buckets[count];
        while( curelem != NULL ) {
            nextelem = curelem->next;
            if( freeitems )  FreeMem( (void*)curelem->data );
            FreeMem( curelem );
            curelem = nextelem;
        }
    }
    FreeMem( table );
}


/*
 * Insert an item into a hash table.  Duplicates are ignored.
 */
void InsertHash( HashTable _table, const void *item )
/***************************************************/
{
    Hash *              table = (Hash*)_table;
    unsigned            bucket;
    ListElem *          newelem;

    if( table == NULL )  Zoinks();
    bucket = (*table->hashfunc)( item )  %  table->numbuckets;
    if( !is_in_bucket( table, item, bucket ) ) {
        newelem = AllocMem( sizeof( ListElem ) );
        newelem->data = item;
        newelem->next = table->buckets[bucket];
        table->buckets[bucket] = newelem;
    }
}


/*
 * Walk through the bucket to which 'item' hashes.  For each element in
 * the bucket, 'callback' will be called with a pointer to the item and
 * 'cookie', which can be useful for tracking progress or whatever.  The
 * walk will cease early if 'callback' returns zero.
 */
void WalkBucketHash( HashTable *_table, const void *item,
                     HashWalkCallback callback, void *cookie )
/************************************************************/
{
    Hash *              table = (Hash*)_table;
    unsigned            bucket;
    ListElem *          curelem;
    int                 rc;

    if( table == NULL )  Zoinks();
    bucket = (*table->hashfunc)( item )  %  table->numbuckets;
    curelem = table->buckets[bucket];

    while( curelem != NULL ) {
        rc = (*callback)( curelem->data, cookie );
        if( rc == 0 )  break;
        curelem = curelem->next;
    }
}
