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
#include "hashtabl.h"
#include "memfuncs.h"

int NumberCmp( hash_value n1, hash_value n2 )
{
    return( n1 - n2 );
}

static hash_value stringEncode( const char *string )
{
    const char *        p;
    hash_value          g;
    hash_value          h;

    h = 0;
    for( p = (const char *) string; *p; p++ ) {
        h = ( h << 4 ) + *p;
        if( g = h & 0xf0000000 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
    }
    return( h );
}

static hash_value hashFunc( hash_table hash_tbl, hash_value key )
{
    return( key % hash_tbl->size );
}

return_val HashTableInsert( hash_table hash_tbl, hash_value key, hash_data data )
{
    hash_value                  hash_val;
    hash_struct *               hash_ptr;
    hash_struct *               new_element;

    if( hash_tbl->type == HASH_STRING ) {
        hash_val = hashFunc( hash_tbl, stringEncode( (char *) key ) );
    } else {
        hash_val = hashFunc( hash_tbl, key );
    }
    hash_ptr = hash_tbl->table[hash_val];
    while( hash_ptr != NULL ) {
        if( !( hash_tbl->compare( hash_ptr->key, key ) ) ) {
            hash_ptr->data = data;
            return( OKAY );
        }
        hash_ptr = hash_ptr->next;
    }
    new_element = (hash_struct *) MemAlloc( sizeof( hash_struct ) );
    if( !new_element ) {
        return( OUT_OF_MEMORY );
    }
    new_element->key = key;
    new_element->data = data;
    new_element->next = hash_tbl->table[hash_val];
    hash_tbl->table[hash_val] = new_element;
    return( OKAY );
}

hash_data *HashTableQuery( hash_table hash_tbl, hash_value key )
{
    hash_value          hash_val;
    hash_struct *       hash_ptr;

    if( hash_tbl->type == HASH_STRING ) {
        hash_val = hashFunc( hash_tbl, stringEncode( (char *) key ) );
    } else {
        hash_val = hashFunc( hash_tbl, key );
    }
    hash_ptr = hash_tbl->table[hash_val];

    while( hash_ptr != NULL ) {
        if( !( hash_tbl->compare( hash_ptr->key, key ) ) ) return( &(hash_ptr->data) );
        hash_ptr = hash_ptr->next;
    }
    return( NULL );
}

hash_table HashTableCreate( hash_table_size size, hash_table_type type, hash_table_comparison_func func )
{
    hash_table          hash_tbl;
    int                 loop;

    hash_tbl = (hash_table) MemAlloc( sizeof( hash_table_struct ) );
    if( !hash_tbl) return( NULL );
    hash_tbl->table = (hash_struct **) MemAlloc( size * sizeof( hash_struct * ) );
    if( !(hash_tbl->table) ) return( NULL );
    hash_tbl->size = size;
    hash_tbl->type = type;
    hash_tbl->compare = func;
    for( loop = 0; loop < size; loop ++ ) {
        hash_tbl->table[loop] = NULL;
    }
    return( hash_tbl );
}

void HashTableFree( hash_table hash_tbl )
{
    int                         loop;
    hash_struct *               hash_ptr;
    hash_struct *               last_hash_ptr;

    if( !hash_tbl ) return;

    for( loop = 0; loop < hash_tbl->size; loop ++ )
    {
        hash_ptr = hash_tbl->table[loop];
        while( hash_ptr != NULL ) {
            last_hash_ptr = hash_ptr;
            hash_ptr = hash_ptr->next;
            MemFree( last_hash_ptr );
        }
    }
    MemFree( hash_tbl->table );
    MemFree( hash_tbl );
}
