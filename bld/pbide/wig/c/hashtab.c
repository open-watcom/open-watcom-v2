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
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include "mem.h"
#include "hashtab.h"

typedef struct hash_entry {
        struct hash_entry       *next;
        int                     keylen;
        const char              *key;
        void                    *value;
} hash_entry;

typedef struct hash_table {
        int             size;
        hash_entry      *tab[1];
} hash_table;


hash_tab NewHashTable( int size ) {
/*********************************/

    hash_table          *table;
    long                tabsize;

    assert( size > 0 );

    tabsize = sizeof(hash_table) + ( sizeof(hash_entry) * ( size - 1 ) );
    table = MemMalloc( tabsize );
    memset( table, 0, tabsize );
    table->size = size;
    return( table );
}


void DestroyHashTable( hash_tab tab ) {
/*************************************/

    long        x;
    hash_table  *htable = tab;
    hash_entry  *htemp;

    if( tab != NULL ) {
        for( x = 0; x < htable->size; x++ ) {
            while( htable->tab[x] ) {
                htemp = htable->tab[x];
                htable->tab[x] = htemp->next;
                MemFree( htemp );
            }
        }
        MemFree( tab );
    }
}


void InsertHashValue( hash_tab tab, const char *key, int len, void *value ) {
/***************************************************************************/

    long                x;
    unsigned long       hash = 0;
    hash_table          *htable = tab;
    hash_entry          *htmp;
    hash_entry          *hfinger;

    assert( tab );
    assert( key );
    assert( len > 0 );

    for( x = 0; x < len; x++ ) {
        hash = HashByte( hash, key[x] );
    }
    hash %= htable->size;

    htmp = MemMalloc( sizeof( hash_entry ) );

    if( htable->tab[hash] ) {
        hfinger = htable->tab[hash];
        while( hfinger->next ) {
            hfinger = hfinger->next;
        }
        hfinger->next = htmp;
    } else {
        htable->tab[hash] = htmp;
    }
    htmp->next = NULL;
    htmp->key = key;
    htmp->keylen = len;
    htmp->value = value;
}


long HashByte(long hash, char  byte ) {
/*************************************/

    /* hash byte, and incorporate it into the following hash */

    hash = ( hash << 4 ) + tolower( byte );
    if ( hash & 0xffff0000 ) {
        hash += hash >> 24;
    }
    return( hash );
}


long HashString(char *string, long len ) {
/*****************************************/

    long        hash = 0;

    assert( string );
    assert( len > 0 );

    while( len ) {
        hash = HashByte( hash, *string );
        len--;
        string++;
    }
    return( hash );
}


void *FindHashEntry( hash_tab tab, long hash, const char *key, int len ) {
/************************************************************************/

    hash_table          *htable = tab;
    hash_entry          *htmp;
    unsigned long       cell;

    assert( tab );
    assert( key );
    assert( len > 0 );

    cell = (unsigned long)hash % htable->size;

    htmp = htable->tab[cell];
    while( htmp ) {
        if( ( len == htmp->keylen ) && !strnicmp( htmp->key, key, len ) ) {
            return( htmp->value );
        }
        htmp = htmp->next;
    }
    return( NULL );
}
