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


// Carve -- carving support
//
// 95/02/02 -- J.W.Welch        -- defined

#include "Carve.h"


Carve::Carve                    // CONSTRUCTOR
    ( unsigned entry_size       // - size of an entry
    , unsigned block_size )     // - # entries / block
    : _size_entry( sizeof( CarveBlock ) > entry_size
                 ? sizeof( CarveBlock )
                 : entry_size )
    , _size_block( block_size )
{
}


Carve::~Carve                   // DESTRUCTOR
    ( void )
{
    RingIter iter( _blks );
    for( ; ; ) {
        CarveBlock* curr = (CarveBlock*)iter.next();
        if( 0 == curr ) break;
        delete[] curr->_data;
        delete curr;
    }
}


void* Carve::alloc              // ALLOCATOR
    ( void )
{
    void* retn = _freed.pop();
    if( 0 == retn ) {
        char* data = new char[ _size_block * _size_entry ];
        CarveBlock* blk = new CarveBlock;
        blk->_data = data;
        _blks.append( blk );
        for( int ctr = _size_block; ctr > 0; --ctr, data += _size_entry ) {
            _freed.append( (FreeElement*)data );
        }
        retn = _freed.pop();
    }
    return retn;
}


void Carve::free                // FREE AN ENTRY
    ( void* entry )             // - the entry
{
    _freed.push( (FreeElement*)entry );
}
