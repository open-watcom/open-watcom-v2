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


// Hash -- hash table implementation
//
// 95/02/03 -- J.W.Welch        -- defined

#include "Carve.h"
#include "Hash.h"


//-------------------------------------------------------------------
// HashEntry Implementation
//-------------------------------------------------------------------


HashEntry::HashEntry            // CONSTRUCTOR
    ( void const *element )     // - element
    : _entry( element )
{
}


//-------------------------------------------------------------------
// HashTable Implementation
//-------------------------------------------------------------------

static Carve hashCarve          // hash carver
                ( sizeof( HashEntry )
                , 500 );


HashTable::HashTable            // CONSTRUCTOR
    ( unsigned (*hfn)           // - hash function
        ( void const * )        // - - element
    , int  (*cfn)               // - comparison function
        ( void const *          // - - element
        , void const * ) )      // - - comparand
    : _hashfun( hfn )
    , _hashcmp( cfn )
{
}


void HashTable::add             // ADD TO HASH TABLE
    ( void const * element      // - new element
    , void const * comparand )  // - comparand
{
    unsigned index = (*_hashfun)( comparand ) % HashModulus;
    HashEntry* entry = new( hashCarve.alloc() ) HashEntry( element );
    _table[ index ].append( entry );
}


void const* HashTable::find     // FIND IN HASH TABLE
    ( void const * comparand )  // - comparand
{
    unsigned index = (*_hashfun)( comparand ) % HashModulus;
    RingIter iter( _table[ index ] );
    void const * retn;
    for( ; ; ) {
        HashEntry* entry = (HashEntry*)iter.next();
        if( 0 == entry ) {
            retn = 0;
            break;
        } else if( (*_hashcmp)( entry->_entry, comparand ) ) {
            retn = entry->_entry;
            break;
        }
    }
    return retn;
}
