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

#include <string.h>
#include <stdio.h>

// Project includes -------------------------------------------------------

#include "mstrtble.h"
#include "errors.h"

MergeStringTable::MergeStringTable()
/**********************************/
{
}

MergeStringTable::~MergeStringTable()
/***********************************/
{
    int i;

    for( i = 0; i < _table.entries(); i += 1 ) {
        delete _table[ i ];
    }
}

void MergeStringTable::add( char * key, void * data, int index )
/**************************************************************/
{
    Element * elem;
    void * other;
    int otherIndex;

    find( key, &other, otherIndex );
    if( other == NULL ) {
        elem = new Element( key, data, index );
        if( elem == NULL ) {
            throw ErrOutOfMemory;
        }
        _table.insert( elem );
    } else {
        printf( "%s : already exists.\n" );
    }
}

void MergeStringTable::find( char * key, void ** data, int & index )
/******************************************************************/
// Search for the element with the same key
//
// Returns :
//      NULL if not found
//      data associated with key if found
{
    int i;

    *data = NULL;
    index = -1;

    for( i = 0; i < _table.entries(); i += 1 ) {
        if( strcmp( _table[ i ]->key, key ) == 0 ) {
            *data = _table[ i ]->data;
            index = _table[ i ]->index;
            break;
        }
    }
}

MergeStringTable::Element::Element( char * k, void * d, int i )
    : key( NULL )
    , data( d )
    , index( i )
/*************************************************************/
{
    key = new char[ strlen( k ) + 1 ];
    if( key == NULL ) {
        throw ErrOutOfMemory;
    }
    strcpy( key, k );
}

MergeStringTable::Element::~Element()
/***********************************/
{
    delete [] key;
}
