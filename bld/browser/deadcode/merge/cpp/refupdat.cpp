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

#include "refupdat.h"

// class ReferenceUpdate methods ------------------------------------------

const int REFUPDATEPOOLSIZE = 1024;
MemoryPool ReferenceUpdate::_pool( sizeof( ReferenceUpdate ), REFUPDATEPOOLSIZE,
                                    "ReferenceUpdate" );

ReferenceUpdate::ReferenceUpdate( int mbrIndex,
                                  uint_32 oldOffset,
                                  uint_32 newOffset )
/***************************************************/
    : _mbrIndex( mbrIndex )
    , _oldOffset( oldOffset )
    , _newOffset( newOffset )
{
}

void * ReferenceUpdate::operator new( size_t )
/*********************************************/
{
    return( _pool.alloc() );
}

void ReferenceUpdate::operator delete( void * mem )
/*************************************************/
{
    _pool.free( mem );
}


// class ReferenceUpdateTable methods -------------------------------------

ReferenceUpdateTable::ReferenceUpdateTable()
/******************************************/
{
}

ReferenceUpdateTable::~ReferenceUpdateTable()
/******************************************/
{
    int i, j;

    #if DEBUG_LIST
        printf( "ReferenceUpdateTable::~ReferenceUpdateTable() -- _table contains %d lists\n", _table.entries() );
        for( i = 0; i < _table.entries(); i += 1 ) {
            printf( "                                              -- _list[%03d] contains %d entries\n", i, _table[i]->entries() );
        }
    #endif

    for( i = 0; i < _table.entries(); i += 1 ) {
        RefUpdateVector * list;

        list = _table[ i ];
        for( j = 0; j < list->entries(); j += 1 ) {
            delete (*list)[ j ];
        }

        delete list;
    }
}

void ReferenceUpdateTable::add( int mbrIndex,
                                uint_32 oldOffset,
                                uint_32 newOffset )
/*************************************************/
// The symbol that was at <mbrIndex,oldOffset> originally
// is now at <newOffset> in the merged file.
{
    int i;
    RefUpdateVector * list;

    /*
     | Make sure there are enough tables...
     */
    if( mbrIndex >= _table.entries() ) {
        for( i = _table.entries(); i <= mbrIndex; i += 1 ) {
            list = new RefUpdateVector( 2048, 1024 );
            list->exceptions( WCExcept::index_range | WCExcept::out_of_memory );
            _table.insert( list );
        }
    }
    list = _table[ mbrIndex ];

    ReferenceUpdate * ref = new ReferenceUpdate( mbrIndex, oldOffset, newOffset );
    list->insert( ref );
}

bool ReferenceUpdateTable::getNewOffset( int mbrIndex,
                                         uint_32 oldOffset,
                                         uint_32 & newOffset )
/************************************************************/
{
    bool rc = FALSE;
    RefUpdateVector * list;
    ReferenceUpdate * ref;
    ReferenceUpdate comp( mbrIndex, oldOffset, 0 );

    if( mbrIndex >= _table.entries() ) {
        return FALSE;
    }

    list = _table[ mbrIndex ];

    ref = list->find( &comp );

    if( ref ) {
        newOffset = ref->_newOffset;
        rc = TRUE;
    } else {
        rc = FALSE;
    }

    return rc;
}

#if DEBUG_DUMP
void ReferenceUpdateTable::dumpData()
/***********************************/
{
    int i, j;
    RefUpdateVector * list;
    ReferenceUpdate * ref;

    printf( "+---- Reference Update Table --------------------------------+\n" );
    printf( "| <mbr,old off> = <new off>\n" );

    for( i = 0; i < _table.entries(); i += 1 ) {
        list = _table[ i ];

        for( j = 0; j < list->entries(); j += 1 ) {
            ref = (*list)[ j ];

            printf( "<%d,%lx> = <%lx>\n", i, ref->_oldOffset, ref->_newOffset );
        }
    }
    printf( "+------------------------------------------------------------+\n" );
}
#endif
