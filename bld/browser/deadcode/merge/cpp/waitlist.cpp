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

#include "waitlist.h"

// WaitingInfo methods ----------------------------------------------------

WaitingInfo::WaitingInfo( int mbrIndex, uint_32 offset, RefVector * refs )
/************************************************************************/
    : _mbrIndex( mbrIndex )
    , _offset( offset )
    , _refs( refs )
{
}

WaitingInfo::~WaitingInfo()
/*************************/
{
    #if DEBUG_LIST
        if( _refs ) {
            printf( "WaitingInfo::~WaitingInfo -- _refs contains %d items\n", _refs->entries() );
        }
    #endif
}

// WaitingList methods ----------------------------------------------------

WaitingList::WaitingList()
/************************/
{
}

WaitingList::~WaitingList()
/*************************/
{
    int i;

    #if DEBUG_LIST
        printf( "WaitingList::~WaitingList() -- _table contains %d items\n", _table.entries() );
    #endif

    for( i = 0; i < _table.entries(); i += 1 ) {
        delete _table[ i ];
    }
}

RefVector * WaitingList::findList( int mbrIndex, uint_32 offset )
/***************************************************************/
{
    int i;
    WaitingInfo * info;

    for( i = 0; i < _table.entries(); i += 1 ) {
        info = _table[ i ];
        if( info->_mbrIndex == mbrIndex && info->_offset == offset ) {
            return info->_refs;
        }
    }
    return NULL;
}

void WaitingList::add( int mbrIndex, uint_32 offset, RefVector * refs )
/*********************************************************************/
{
    WaitingInfo * info;

    #if DEBUG_LIST
        if( _table.entries() % 1000 == 0 ) {
            printf( "WaitingList::add() -- %d elements\n", _table.entries() );
        }
    #endif

    info = new WaitingInfo( mbrIndex, offset, refs );
    _table.insert( info );
}

void WaitingList::remove( RefVector * refs )
/******************************************/
{
    int i;

    for( i = 0; i < _table.entries(); i += 1 ) {
        if( _table[ i ]->_refs == refs ) {
            delete _table[ i ];
            _table[ i ] = NULL;
        }
    }

    for( i = _table.entries() - 1; i >= 0; i -= 1 ) {
        if( _table[ i ] == NULL ) {
            _table.removeAt( i );
        }
    }
}
