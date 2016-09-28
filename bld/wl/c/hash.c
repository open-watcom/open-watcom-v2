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


#include "hash.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "debug.h"

/* ----------------------------------------------------------------------- */
pHTable CreateHTable( int size, pHashFunc hashFunc, pHashElemCmp compareFunc,
    pAllocFunc allocFunc, pFreeFunc freeFunc )
{
    pHTable table;

    table = allocFunc( sizeof( *table ) );
    table->tbl = allocFunc( sizeof( table[0] ) * size );
    memset( table->tbl, 0, sizeof( table[0] ) * size );

    table->size = size;
    table->hashFunc = hashFunc;
    table->compareFunc = compareFunc;
    table->allocFunc = allocFunc;
    table->freeFunc = freeFunc;
    table->stats.numElems = 0;
    table->stats.longestChainLen = 0;
    table->allowDoubles = 0;

    return( table );
}

pHTable CreateHTableDouble( int size, pHashFunc hashFunc, pHashElemCmp compareFunc,
         pAllocFunc allocFunc, pFreeFunc freeFunc )
{
    pHTable table = CreateHTable(size, hashFunc, compareFunc, allocFunc, freeFunc);
    if (table != NULL) {
        table->allowDoubles = 1;
    }
    return( table );
}

/* ----------------------------------------------------------------------- */
void *AddHTableElem( pHTable table, void *elem )
{
    unsigned key;
    int chainLen = 0;
    pHTElem tblElem;
    pHashElemCmp cmp = table->compareFunc;

    key = table->hashFunc( elem, table->size );

    if( table->allowDoubles ) {
        for( tblElem = table->tbl[key]; tblElem != NULL; tblElem = tblElem->next ) {
            chainLen++;
        }
    } else {
        for( tblElem = table->tbl[key]; tblElem != NULL; tblElem = tblElem->next ) {
            chainLen++;

            if( cmp( elem, tblElem->userData ) == 0 ) {
                return( tblElem->userData );
            }
        }
    }

    tblElem = table->allocFunc( sizeof( *tblElem ) );
    tblElem->userData = elem;
    tblElem->next = table->tbl[key];
    table->tbl[key] = tblElem;
    chainLen++;

    table->stats.numElems++;

    if( chainLen > table->stats.longestChainLen ) {
        table->stats.longestChainLen = chainLen;
#ifdef _INT_DEBUG
        if( chainLen > 20 ) {
            LPrint( "Hash Warning: max chain len = %d getting long!\n" );
        }
#endif
    }

    return( elem );
}

/* ----------------------------------------------------------------------- */
void *FindHTableElem( pHTable table, void *elem )
{
    int key;
    pHTElem tblElem;
    pHashElemCmp cmp = table->compareFunc;

    key = table->hashFunc( elem, table->size );

    for( tblElem = table->tbl[key]; tblElem != NULL; tblElem = tblElem->next ) {
        if( cmp( elem, tblElem->userData ) == 0 ) {
            return( tblElem->userData );
        }
    }

    return( NULL );
}

/* ----------------------------------------------------------------------- */
int WalkHTableElem( pHTable table, void *elem, void (*action)( void * ) )
{
    pHTElem tblElem;
    int numElem = 0;
    int key = table->hashFunc( elem, table->size );
    pHashElemCmp cmp = table->compareFunc;

    for( tblElem = table->tbl[key]; tblElem != NULL; tblElem = tblElem->next ) {
        if( cmp( elem, tblElem->userData ) == 0 ) {
            action( tblElem->userData );
            numElem++;
        }
    }
    return( numElem );
}

/* ----------------------------------------------------------------------- */
void WalkHTableCookie( pHTable table, void (*action)( void *, void *), void *cookie )
{
    int i;
    pHTElem *tblPtr = table->tbl;
    pHTElem tblElem;

    if( action == NULL ) {
        return;
    }

    for( i = 0; i < table->size; i++ ) {
        for( tblElem = tblPtr[i]; tblElem != NULL; tblElem = tblElem->next ) {
            action( tblElem->userData, cookie );
        }
    }
}

void WalkHTable( pHTable table, void (*action)( void * ) )
{
    /* For speed, do not use WalkHTableCookie */
    int i;
    pHTElem *tblPtr = table->tbl;
    pHTElem tblElem;

    if( action == NULL ) {
        return;
    }

    for( i = 0; i < table->size; i++ ) {
        for( tblElem = tblPtr[i]; tblElem != NULL; tblElem = tblElem->next ) {
            action( tblElem->userData );
        }
    }
}

/* ----------------------------------------------------------------------- */
void RehashHTable( pHTable table )
{
    int i;
    pHTElem *tbl;
    pHTElem elem, *pelem;
    unsigned hash;

    tbl = table->tbl;
    for( i = 0; i < table->size; i++ ) {
        for( pelem = &tbl[i]; *pelem != NULL; ) {
            elem = *pelem;
            hash = table->hashFunc( elem->userData, table->size );
            if( i != hash ) {
                *pelem = elem->next;
                elem->next = tbl[hash];
                tbl[hash] = elem;
            } else {
                if( *pelem == NULL )
                    break;
                pelem = &((*pelem)->next);
            }
        }
    }
}

/* ----------------------------------------------------------------------- */
void ZapHTable( pHTable table, void (*zapElemAction)( void * ) )
{
    int i;
    pHTElem *tblPtr;
    pHTElem tblElem, temp;
    pFreeFunc free;

    if( table == NULL ) {
        return;
    }

    tblPtr = table->tbl;
    free = table->freeFunc;

    for( i = 0; i < table->size; i++ ) {
        for( tblElem = tblPtr[i]; tblElem != NULL; tblElem = temp ) {
            if( zapElemAction != NULL ) {
                zapElemAction( tblElem->userData );
            }
            temp = tblElem->next;
            free( tblElem );
        }
    }

    free( table->tbl );
    free( table );
}

/* ----------------------------------------------------------------------- */
void GetHTableStats( pHTable table, int *numElems, int *longestChainLen )
{
    *numElems = table->stats.numElems;
    *longestChainLen = table->stats.longestChainLen;
}

long GetHTableNumOfElems(pHTable table)
{
    return( table->stats.numElems );
}

/* ----------------------------------------------------------------------- */
unsigned StringHashFunc( char *s, unsigned size )
{
    enum { b = 101 };
    unsigned long key = 0;
    int i;

    for( i = 0; s[i] != 0; i++ ) {
        key += s[i];
        key *= b;
    }

    key = key & (size - 1);

    return( key );
}

/* ----------------------------------------------------------------------- */
unsigned StringiHashFunc( void *_s, unsigned size )
{
    char *s = _s;
    enum { b = 101 };
    unsigned long key = 0;
    int i;

    for( i = 0; s[i] != 0; i++ ) {
        key += toupper( s[i] );
        key *= b;
    }

    key = key & (size - 1);

    return( key );
}

/* ----------------------------------------------------------------------- */
unsigned DataHashFunc( void *data, unsigned n, unsigned size )
{
    enum { b = 101 };
    unsigned long key = 0;
    int i;

    for( i = 0; i < n; i++ ) {
        key += ((char*)data)[i];
        key *= b;
    }

    key = key & (size - 1);

    return( key );
}

/* ----------------------------------------------------------------------- */
unsigned PtrHashFunc( void *p, unsigned size )
{
    return( DataHashFunc( &p, sizeof( p ), size ) );
}

/* ----------------------------------------------------------------------- */
void CollectHTableDistribution( pHTable table, unsigned *stat )
{
    int i, n;
    pHTElem *tblPtr = table->tbl;
    pHTElem tblElem;

    for( i = 0; i < table->size; i++ ) {
        n = 0;
        for( tblElem = tblPtr[i]; tblElem != NULL; tblElem = tblElem->next ) {
            n++;
        }
        stat[i] = n;
    }
}
