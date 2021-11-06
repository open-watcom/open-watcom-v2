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
* Description:  hash table routines
*
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dis.h"
#include "global.h"
#include "hashtabl.h"
#include "memfuncs.h"

#include "clibext.h"


static bool handle_cmp( hash_key n1, hash_key n2 )
{
    return( n1.u.sec_handle == n2.u.sec_handle );
}

static bool string_cmp( hash_key p1, hash_key p2 )
{
    return( strcmp( p1.u.string, p2.u.string ) == 0 );
}

static bool string_cmp_ignorecase( hash_key p1, hash_key p2 )
{
    return( stricmp( p1.u.string, p2.u.string ) == 0 );
}

static hash_value hash_encode( hash_value size, const char *ptr, size_t len, bool ignorecase )
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

static hash_value handle_hash( hash_value size, hash_key key )
{
    return( hash_encode( size, (const char *)&key.u.sec_handle, sizeof( key.u.sec_handle ), false ) );
}

static hash_value string_hash( hash_value size, hash_key key )
{
    return( hash_encode( size, key.u.string, strlen( key.u.string ), false ) );
}

static hash_value string_hash_ignorecase( hash_value size, hash_key key )
{
    return( hash_encode( size, key.u.string, strlen( key.u.string ), true ) );
}

return_val HashTableInsert( hash_table hash_tbl, hash_entry_data *key_data )
{
    hash_value              hash_val;
    hash_entry_struct       *hash_entry;
    hash_entry_struct       *new_hash_entry;

    hash_val = hash_tbl->hash_func( hash_tbl->size, key_data->key );
    for( hash_entry = hash_tbl->table[hash_val]; hash_entry != NULL; hash_entry = hash_entry->next ) {
        if( hash_tbl->compare_func( hash_entry->entry.key, key_data->key ) ) {
            hash_entry->entry.data = key_data->data;
            return( RC_OKAY );
        }
    }
    new_hash_entry = (hash_entry_struct *)MemAlloc( sizeof( hash_entry_struct ) );
    if( new_hash_entry == NULL ) {
        return( RC_OUT_OF_MEMORY );
    }
    new_hash_entry->entry.key = key_data->key;
    new_hash_entry->entry.data = key_data->data;
    new_hash_entry->next = hash_tbl->table[hash_val];
    hash_tbl->table[hash_val] = new_hash_entry;
    return( RC_OKAY );
}

hash_data *HashTableQuery( hash_table hash_tbl, hash_key key )
{
    hash_value          hash_val;
    hash_entry_struct   *hash_entry;

    hash_val = hash_tbl->hash_func( hash_tbl->size, key );
    for( hash_entry = hash_tbl->table[hash_val]; hash_entry != NULL; hash_entry = hash_entry->next ) {
        if( hash_tbl->compare_func( hash_entry->entry.key, key ) ) {
            return( &(hash_entry->entry.data) );
        }
    }
    return( NULL );
}

hash_table HashTableCreate( hash_value size, hash_table_type type )
{
    hash_table          hash_tbl;
    hash_value          i;

    hash_tbl = (hash_table)MemAlloc( sizeof( hash_table_struct ) );
    if( hash_tbl == NULL )
        return( NULL );
    hash_tbl->table = (hash_entry_struct **)MemAlloc( size * sizeof( hash_entry_struct * ) );
    if( hash_tbl->table == NULL ) {
        MemFree( hash_tbl );
        return( NULL );
    }
    hash_tbl->size = size;
    switch( type ) {
    case HASH_HANDLE:
        hash_tbl->hash_func = handle_hash;
        hash_tbl->compare_func = handle_cmp;
        break;
    case HASH_STRING:
        hash_tbl->hash_func = string_hash;
        hash_tbl->compare_func = string_cmp;
        break;
    case HASH_STRING_IGNORECASE:
    default:
        hash_tbl->hash_func = string_hash_ignorecase;
        hash_tbl->compare_func = string_cmp_ignorecase;
        break;
    }
    for( i = 0; i < size; i++ ) {
        hash_tbl->table[i] = NULL;
    }
    return( hash_tbl );
}

void HashTableFree( hash_table hash_tbl )
{
    hash_value          i;
    hash_entry_struct   *hash_entry;
    hash_entry_struct   *next_hash_entry;

    if( hash_tbl == NULL )
        return;

    for( i = 0; i < hash_tbl->size; i++ ) {
        for( hash_entry = hash_tbl->table[i]; hash_entry != NULL; hash_entry = next_hash_entry ) {
            next_hash_entry = hash_entry->next;
            MemFree( hash_entry );
        }
    }
    MemFree( hash_tbl->table );
    MemFree( hash_tbl );
}
