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

#define _HashAlloc( a, b )  ORL_CLI_ALLOC( a->funcs, b )
#define _HashFree( a, b )   ORL_CLI_FREE( a->funcs, b )
#define _HashCompare( a, b, c ) a->compare( b, c )

int ORLNumberCmp( orl_hash_data n1, orl_hash_data n2 )
{
    return( (orl_hash_value)(pointer_int)n1 - (orl_hash_value)(pointer_int)n2 );
}

static orl_hash_value orlStringEncode( const char *string )
{
    const char      *p;
    orl_hash_value  g;
    orl_hash_value  h;

    h = 0;
    for( p = (const char *) string; *p; p++ ) {
        h = ( h << 4 ) + *p;
        if( (g = h & 0xf0000000) != 0 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
    }
    return( h );
}

static orl_hash_value orlHash( orl_hash_table hash_table, orl_hash_value key )
{
    return( key % hash_table->size );
}

orl_return ORLHashTableInsert( orl_hash_table hash_table, orl_hash_data key, orl_hash_data data )
{
    orl_hash_value              hash_value = 0;
    orl_hash_struct *           hash_ptr;
    orl_hash_struct *           new_element;
    orl_hash_data_struct *      last;
    orl_hash_data_struct *      new_data_element;

    new_data_element = (orl_hash_data_struct *) _HashAlloc( hash_table, sizeof( orl_hash_data_struct ) );
    if( !new_data_element )
        return( ORL_OUT_OF_MEMORY );
    new_data_element->data = data;
    new_data_element->next = NULL;
    switch( hash_table->type ) {
        case ORL_HASH_STRING:
            hash_value = orlHash( hash_table, orlStringEncode( key ) );
            break;
        case ORL_HASH_NUMBER:
            hash_value = orlHash( hash_table, (orl_hash_value)(pointer_int)key );
            break;
    }
    hash_ptr = hash_table->table[hash_value];
    while( hash_ptr != NULL ) {
        if( !( _HashCompare( hash_table, hash_ptr->key, key ) ) ) {
            last = hash_ptr->data_struct;
            while( last->next ) {
                last = last->next;
            }
            last->next = new_data_element;
            return( ORL_OKAY );
        }
        hash_ptr = hash_ptr->next;
    }
    new_element = (orl_hash_struct *) _HashAlloc( hash_table, sizeof( orl_hash_struct ) );
    if( !new_element )
        return( ORL_OUT_OF_MEMORY );
    new_element->key = key;
    new_element->data_struct = new_data_element;
    new_element->next = hash_table->table[hash_value];
    hash_table->table[hash_value] = new_element;
    return( ORL_OKAY );
}

orl_hash_data_struct *ORLHashTableQuery( orl_hash_table hash_table, orl_hash_data key )
{
    orl_hash_value      hash_value = 0;
    orl_hash_struct     *hash_ptr;

    switch( hash_table->type ) {
        case ORL_HASH_STRING:
            hash_value = orlHash( hash_table, orlStringEncode( key ) );
            break;
        case ORL_HASH_NUMBER:
            hash_value = orlHash( hash_table, (orl_hash_value)(pointer_int)key );
            break;
    }
    hash_ptr = hash_table->table[hash_value];
    while( hash_ptr != NULL ) {
        if( !( _HashCompare( hash_table, hash_ptr->key, key ) ) )
            return( hash_ptr->data_struct );
        hash_ptr = hash_ptr->next;
    }
    return( NULL );
}

orl_hash_table ORLHashTableCreate( orl_funcs * funcs, orl_hash_table_size size, orl_hash_table_type type, orl_hash_comparison_func compare )
{
    orl_hash_table      hash_table;
    int                 loop;

    hash_table = (orl_hash_table)ORL_CLI_ALLOC( funcs, sizeof( orl_hash_table_struct ) );
    if( !hash_table)
        return( NULL );
    hash_table->table = (orl_hash_struct **)ORL_CLI_ALLOC( funcs, size * sizeof( orl_hash_struct * ) );
    if( !(hash_table->table) ) {
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
    int                         loop;
    orl_hash_struct *           hash_ptr;
    orl_hash_struct *           last_hash_ptr;
    orl_hash_data_struct *      data_ptr;
    orl_hash_data_struct *      last_data_ptr;

    for( loop = 0; loop < hash_table->size; loop++ ) {
        hash_ptr = hash_table->table[loop];
        while( hash_ptr != NULL ) {
            data_ptr = hash_ptr->data_struct;
            while( data_ptr != NULL ) {
                last_data_ptr = data_ptr;
                data_ptr = data_ptr->next;
                _HashFree( hash_table, last_data_ptr );
            }
            last_hash_ptr = hash_ptr;
            hash_ptr = hash_ptr->next;
            _HashFree( hash_table, last_hash_ptr );
        }
    }
    _HashFree( hash_table, hash_table->table );
    _HashFree( hash_table, hash_table );
}
