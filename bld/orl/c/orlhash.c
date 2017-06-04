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
#include "orllevel.h"
#include "orlhash.h"

#define _HashAlloc( a, b )      ORL_PTR_ALLOC( a, b )
#define _HashFree( a, b )       ORL_PTR_FREE( a, b )
#define _HashCompare( a, b, c ) (a->compare( b, c ) == 0)

#define orlNumberEncode( s, k ) ((orl_hash_value)((pointer_int)(k) % (s) ))

int ORLNumberCmp( orl_hash_key n1, orl_hash_key n2 )
{
    if( (pointer_int)n1 == (pointer_int)n2 )
        return( 0 );
    if( (pointer_int)n1 < (pointer_int)n2 )
        return( -1 );
    return( 1 );
}

static orl_hash_value orlStringEncode( orl_hash_value size, const char *string )
{
    const char      *p;
    orl_hash_value  g;
    orl_hash_value  h;

    h = 0;
    for( p = string; *p != '\0'; p++ ) {
        h = ( h << 4 ) + *p;
        if( (g = h & 0xf0000000) != 0 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
    }
    return( h % size );
}

orl_return ORLHashTableInsert( orl_hash_table hash_table, orl_hash_key key, orl_hash_data data )
{
    orl_hash_value          hash_value = 0;
    orl_hash_key_struct     *key_entry;
    orl_hash_key_struct     *new_key_entry;
    orl_hash_data_struct    *last_data_entry;
    orl_hash_data_struct    *new_data_entry;

    new_data_entry = (orl_hash_data_struct *)_HashAlloc( hash_table, sizeof( orl_hash_data_struct ) );
    if( new_data_entry == NULL )
        return( ORL_OUT_OF_MEMORY );
    new_data_entry->data = data;
    new_data_entry->next = NULL;
    switch( hash_table->type ) {
    case ORL_HASH_STRING:
        hash_value = orlStringEncode( hash_table->size, key );
        break;
    case ORL_HASH_NUMBER:
        hash_value = orlNumberEncode( hash_table->size, key );
        break;
    }
    for( key_entry = hash_table->table[hash_value]; key_entry != NULL; key_entry = key_entry->next ) {
        if( _HashCompare( hash_table, key_entry->key, key ) ) {
            last_data_entry = key_entry->data_entry;
            while( last_data_entry->next ) {
                last_data_entry = last_data_entry->next;
            }
            last_data_entry->next = new_data_entry;
            return( ORL_OKAY );
        }
    }
    new_key_entry = (orl_hash_key_struct *)_HashAlloc( hash_table, sizeof( orl_hash_key_struct ) );
    if( new_key_entry == NULL )
        return( ORL_OUT_OF_MEMORY );
    new_key_entry->key = key;
    new_key_entry->data_entry = new_data_entry;
    new_key_entry->next = hash_table->table[hash_value];
    hash_table->table[hash_value] = new_key_entry;
    return( ORL_OKAY );
}

orl_hash_data_struct *ORLHashTableQuery( orl_hash_table hash_table, orl_hash_key key )
{
    orl_hash_value          hash_value = 0;
    orl_hash_key_struct     *key_entry;

    switch( hash_table->type ) {
    case ORL_HASH_STRING:
        hash_value = orlStringEncode( hash_table->size, key );
        break;
    case ORL_HASH_NUMBER:
        hash_value = orlNumberEncode( hash_table->size, key );
        break;
    }
    for( key_entry = hash_table->table[hash_value]; key_entry != NULL; key_entry = key_entry->next ) {
        if( _HashCompare( hash_table, key_entry->key, key ) ) {
            return( key_entry->data_entry );
        }
    }
    return( NULL );
}

orl_hash_table ORLHashTableCreate( orl_funcs *funcs, orl_hash_table_size size, orl_hash_table_type type, orl_hash_comparison_func compare )
{
    orl_hash_table      hash_table;
    unsigned            loop;

    hash_table = (orl_hash_table)ORL_CLI_ALLOC( funcs, sizeof( orl_hash_table_struct ) );
    if( hash_table == NULL )
        return( NULL );
    hash_table->table = (orl_hash_key_struct **)ORL_CLI_ALLOC( funcs, size * sizeof( orl_hash_key_struct * ) );
    if( hash_table->table == NULL ) {
        ORL_CLI_FREE( funcs, hash_table );
        return( NULL );
    }
    hash_table->size = size;
    hash_table->type = type;
    hash_table->funcs = funcs;
    hash_table->compare = compare;
    for( loop = 0; loop < size; loop ++ ) {
        hash_table->table[loop] = NULL;
    }
    return( hash_table );
}

void ORLHashTableFree( orl_hash_table hash_table )
{
    unsigned                    loop;
    orl_hash_key_struct         *key_entry;
    orl_hash_key_struct         *next_key_entry;
    orl_hash_data_struct        *data_entry;
    orl_hash_data_struct        *next_data_entry;

    for( loop = 0; loop < hash_table->size; loop++ ) {
        for( key_entry = hash_table->table[loop]; key_entry != NULL; key_entry = next_key_entry ) {
            next_key_entry = key_entry->next;
            for( data_entry = key_entry->data_entry; data_entry != NULL; data_entry = next_data_entry ) {
                next_data_entry = data_entry->next;
                _HashFree( hash_table, data_entry );
            }
            _HashFree( hash_table, key_entry );
        }
    }
    _HashFree( hash_table, hash_table->table );
    _HashFree( hash_table, hash_table );
}
