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


// System includes --------------------------------------------------------

#if DEBUG
#include <stdio.h>
#endif

// Project includes -------------------------------------------------------

#include "errors.h"
#include "hashtabl.h"

HashTable::HashTable( int size )
/******************************/
    : _size( size )
{
    AnyVector * list;
    int i;

    for( i = 0; i < size; i += 1 ) {
        list = new AnyVector;   // use default size and grow
        list->exceptions( WCExcept::index_range | WCExcept::out_of_memory );
        if( list == NULL ) {
            throw ErrOutOfMemory;
        }
        _table.insert( list );
    }
}

HashTable::~HashTable()
/*********************/
{
    int i;
    int j;

    #if DEBUG_LIST
        int avgload = 0;
        int minload = _table[0]->entries();
        int maxload = _table[0]->entries();
        printf( "HashTable::~HashTable -- _table contains %d lists\n", _table.entries() );
        for( i = 0; i < _table.entries(); i += 1 ) {
            avgload += _table[i]->entries();
            if( _table[i]->entries() < minload ) {
                minload = _table[i]->entries();
            }
            if( _table[i]->entries() > maxload ) {
                maxload = _table[i]->entries();
            }
        }
        avgload /= _table.entries();
        printf( "                      -- min %d, max %d, avg %d\n", minload, maxload, avgload );
    #endif

    for( i = 0; i < _table.entries(); i += 1 ) {
        for( j = 0; j < _table[ i ]->entries(); j += 1 ) {
            delete ( Element * )(*_table[ i ])[ j ];
        }

        delete _table[ i ];
    }
}

void HashTable::add( char * key, void * data )
/********************************************/
{
    int value;
    Element * elem;

    elem = new Element;
    elem->key = key;
    elem->data = data;

    value = hashFunction( key );

    _table[ value ]->insert( elem );

}

void HashTable::find( char * key, AnyVector & list )
/**************************************************/
// Returns a list of void *'s to the data passed in to add()
{
    if( key == NULL ) {
        return;
    }

    int i;
    int value;
    Element * elem;

    value = hashFunction( key );

    for( i = 0; i < _table[ value ]->entries(); i += 1 ) {
        elem = ( Element * )(*(_table[ value ]))[ i ];
        if( strcmp( elem->key, key ) == 0 ) {
            list.insert( elem->data );
        }
    }
}

void HashTable::find( char * key, void * data, AnyVector & list, HCompare fn )
/****************************************************************************/
{
    if( key == NULL ) {
        return;
    }

    int i;
    int value;
    Element * elem;

    value = hashFunction( key );

    for( i = 0; i < _table[ value ]->entries(); i += 1 ) {
        elem = ( Element * )(*(_table[ value ]))[ i ];
        if( strcmp( elem->key, key ) == 0 ) {
            if( fn( elem->data, data )) {
                list.insert( elem->data );
            }
        }
    }
}

int HashTable::hashFunction( char * key )
/***************************************/
{
    int value;
    value = 0;

    for( ;; ) {
        if( *key == '\0' ) break;
        value += *key;
        key++;
    }

    return value % _size;
}
