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
#include <ctype.h>
#include "orllevel.h"
#include "orlhash.h"

#include "clibext.h"


#define _HashAlloc( a, b )      ORL_PTR_ALLOC( a, b )
#define _HashFree( a, b )       ORL_PTR_FREE( a, b )


static bool number_cmp( orl_hash_key n1, orl_hash_key n2 )
{
    return( n1.u.number == n2.u.number );
}

static bool string_cmp( orl_hash_key p1, orl_hash_key p2 )
{
    return( strcmp( p1.u.string, p2.u.string ) == 0 );
}

static bool string_cmp_ignorecase( orl_hash_key p1, orl_hash_key p2 )
{
    return( stricmp( p1.u.string, p2.u.string ) == 0 );
}

static orl_hash_value hash_encode( orl_hash_value size, const char *ptr, size_t len, bool ignorecase )
{
    unsigned_32     g;
    unsigned_32     h;
    int             c;

    h = 0;
    while( len-- > 0 ) {
        c = *(unsigned char *)ptr;
        if( ignorecase )
            c = toupper( c );
        h = ( h << 4 ) + c;
        if( (g = h & 0xf0000000) != 0 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
        ptr++;
    }
    return( h % size );
}

static orl_hash_value number_hash( orl_hash_value size, orl_hash_key key )
{
    return( hash_encode( size, (const char *)&key.u.number, sizeof( key.u.number ), false ) );
}

static orl_hash_value string_hash( orl_hash_value size, orl_hash_key key )
{
    return( hash_encode( size, key.u.string, strlen( key.u.string ), false ) );
}

static orl_hash_value string_hash_ignorecase( orl_hash_value size, orl_hash_key key )
{
    return( hash_encode( size, key.u.string, strlen( key.u.string ), true ) );
}

orl_return ORLHashTableInsert( orl_hash_table hash_table, orl_hash_key key, orl_hash_data data )
{
    orl_hash_value          hash_value;
    orl_hash_entry          hash_entry;
    orl_hash_entry          new_hash_entry;
    orl_hash_data_entry     last_data_entry;
    orl_hash_data_entry     new_data_entry;

    new_data_entry = (orl_hash_data_entry)_HashAlloc( hash_table, ORL_STRUCT_SIZEOF( orl_hash_data_entry ) );
    if( new_data_entry == NULL )
        return( ORL_OUT_OF_MEMORY );
    new_data_entry->data = data;
    new_data_entry->next = NULL;
    hash_value = hash_table->hash_func( hash_table->size, key );
    for( hash_entry = hash_table->table[hash_value]; hash_entry != NULL; hash_entry = hash_entry->next ) {
        if( hash_table->compare_func( hash_entry->key, key ) ) {
            last_data_entry = hash_entry->data_entry;
            while( last_data_entry->next != NULL ) {
                last_data_entry = last_data_entry->next;
            }
            last_data_entry->next = new_data_entry;
            return( ORL_OKAY );
        }
    }
    new_hash_entry = (orl_hash_entry)_HashAlloc( hash_table, ORL_STRUCT_SIZEOF( orl_hash_entry ) );
    if( new_hash_entry == NULL )
        return( ORL_OUT_OF_MEMORY );
    new_hash_entry->key = key;
    new_hash_entry->data_entry = new_data_entry;
    new_hash_entry->next = hash_table->table[hash_value];
    hash_table->table[hash_value] = new_hash_entry;
    return( ORL_OKAY );
}

orl_hash_data_entry ORLHashTableQuery( orl_hash_table hash_table, orl_hash_key key )
{
    orl_hash_value          hash_value;
    orl_hash_entry          hash_entry;

    hash_value = hash_table->hash_func( hash_table->size, key );
    for( hash_entry = hash_table->table[hash_value]; hash_entry != NULL; hash_entry = hash_entry->next ) {
        if( hash_table->compare_func( hash_entry->key, key ) ) {
            return( hash_entry->data_entry );
        }
    }
    return( NULL );
}

orl_hash_table ORLHashTableCreate( orl_funcs *funcs, orl_hash_value size, orl_hash_table_type type )
{
    orl_hash_table      hash_table;
    orl_hash_value      i;

    hash_table = (orl_hash_table)ORL_CLI_ALLOC( funcs, ORL_STRUCT_SIZEOF( orl_hash_table ) );
    if( hash_table == NULL )
        return( NULL );
    hash_table->table = (orl_hash_entry *)ORL_CLI_ALLOC( funcs, size * sizeof( orl_hash_entry ) );
    if( hash_table->table == NULL ) {
        ORL_CLI_FREE( funcs, hash_table );
        return( NULL );
    }
    hash_table->size = size;
    hash_table->funcs = funcs;
    switch( type ) {
    case ORL_HASH_NUMBER:
        hash_table->hash_func = number_hash;
        hash_table->compare_func = number_cmp;
        break;
    case ORL_HASH_STRING:
        hash_table->hash_func = string_hash;
        hash_table->compare_func = string_cmp;
        break;
    case ORL_HASH_STRING_IGNORECASE:
    default:
        hash_table->hash_func = string_hash_ignorecase;
        hash_table->compare_func = string_cmp_ignorecase;
        break;
    }
    for( i = 0; i < size; i++ ) {
        hash_table->table[i] = NULL;
    }
    return( hash_table );
}

void ORLHashTableFree( orl_hash_table hash_table )
{
    orl_hash_value              i;
    orl_hash_entry              hash_entry;
    orl_hash_entry              next_hash_entry;
    orl_hash_data_entry         data_entry;
    orl_hash_data_entry         next_data_entry;

    for( i = 0; i < hash_table->size; i++ ) {
        for( hash_entry = hash_table->table[i]; hash_entry != NULL; hash_entry = next_hash_entry ) {
            next_hash_entry = hash_entry->next;
            for( data_entry = hash_entry->data_entry; data_entry != NULL; data_entry = next_data_entry ) {
                next_data_entry = data_entry->next;
                _HashFree( hash_table, data_entry );
            }
            _HashFree( hash_table, hash_entry );
        }
    }
    _HashFree( hash_table, hash_table->table );
    _HashFree( hash_table, hash_table );
}
