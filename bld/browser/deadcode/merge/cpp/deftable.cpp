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

#include <stdio.h>

// Project includes -------------------------------------------------------

#include "deftable.h"

DefinedSymbolTable::DefinedSymbolTable( int size )
/************************************************/
    : HashTable( size )
{
}

DefinedSymbolTable::~DefinedSymbolTable()
/****************************************/
{
    int i;
    int j;
    AnyVector * list;
    Element * elem;
    SymbolInfo * sym;

    for( i = 0; i < _table.entries(); i += 1 ) {
        list = _table[ i ];
        for( j = 0; j < list->entries(); j += 1 ) {
            elem = ( Element * )(*list)[ j ];
            sym = ( SymbolInfo * ) elem->data;
            delete sym;
        }
    }
}

bool DefinedSymbolTable::hasDefinition( char * str, uint_32 tag )
/***************************************************************/
{
    int i;
    int value;
    Element * elem;
    SymbolInfo * sym;

    value = hashFunction( str );

    for( i = 0; i < _table[ value ]->entries(); i += 1 ) {
        elem = ( Element * )(*(_table[ value ]))[ i ];
        if( strcmp( elem->key, str ) == 0 ) {
            sym = (SymbolInfo *) elem->data;
            if( sym->tag() == tag ) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

SymbolInfo * DefinedSymbolTable::isDefinition( SymbolInfo * info )
/****************************************************************/
{
    int i;
    int value;
    Element * elem;
    SymbolInfo * other;

    value = hashFunction( info->name() );

    for( i = 0; i < _table[ value ]->entries(); i += 1 ) {
        elem = ( Element * )(*(_table[ value ]))[ i ];
        if( strcmp( elem->key, info->name() ) == 0 ) {
            other = ( SymbolInfo * ) elem->data;
            if( other->mbrIndex() == info->mbrIndex() &&
                other->tag() == info->tag() ) {
                return other;
            }
        }
    }

    return NULL;
}

#if DEBUG_DUMP
void DefinedSymbolTable::dumpData()
/*********************************/
{
    int i, j;
    Element * elem;

    printf( "+--- Defined Symbols -----------------------------------+\n" );
    for( i = 0; i < _table.entries(); i += 1 ) {

        for( j = 0; j < _table[ i ]->entries(); j += 1 ) {
            elem = ( Element * )(*(_table[ i ]))[ j ];
            printf( "  %s\n", elem->key );
        }
    }
    printf( "+-------------------------------------------------------+\n" );
}
#endif

