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


// STR.CPP -- strings for MSGBUILD
//
// Strings are allocated and accessed only.
//
// 95/01/30 -- J.W.Welch        -- defined

#include "Str.h"

//-----------------------------------------------------------------------
// Str::StoreList implementation
//-----------------------------------------------------------------------


Str::StoreList::StoreList       // DEFAULT CONSTRUCTOR
    ( void )
    : _list( 0 )
    , _left( 0 )
{
}


Str::StoreList::~StoreList      // DESTRUCTOR
    ( void )
{
    while( _list != 0 ) {
        Store* dlt = _list;
        _list = _list->_next;
        delete dlt;
    }
}


char* Str::StoreList::alloc     // ALLOCATOR( string )
    ( char const *input )       // - string to be allocated
{
    return input == 0 ? 0 : alloc( input, ::strlen( input ) );
}


char* Str::StoreList::alloc     // ALLOCATOR( vector, size )
    ( char const *input         // - vector to be allocated
    , unsigned size )           // - vector size
{
    if( size + 1 > _left ) {
        Store* alloced = new Store;
        _left = _BlkSize;
        alloced->_next = _list;
        _list = alloced;
        _left = _BlkSize;
    }
    char* tgt = &_list->_data[ _BlkSize - _left ];
    tgt[ size ] = '\0';
    memcpy( tgt, input, size );
    _left -= size + 1;
    return tgt;
}


//-----------------------------------------------------------------------
// Str implementation
//-----------------------------------------------------------------------


static Str::StoreList storageList; // string storage


Str::Str                        // CONSTRUCTOR( string )
    ( char const *input )       // - input string
    : _str( storageList.alloc( input ) )
{
}


Str::Str                        // CONSTRUCTOR( vector, size )
    ( char const *vector        // - input vector
    , unsigned size )           // - size of vector
    : _str( storageList.alloc( vector, size ) )
{
}


Str::Str                        // copy CONSTRUCTOR
    ( Str const &input )        // - input Str
    : _str( storageList.alloc( input._str ) )
{
}


int Str::same                   // TEST IF SAME STRING
    ( char const *chrs          // - characters
    , unsigned size )           // - # characters
{
    int retn;                   // - return: TRUE ==> same
    char const *cp;             // - points into string chars
    cp = _str;
    for( ; ; --size, ++chrs, ++cp ) {
        if( 0 == size ) {
            if( *cp == '\0' ) {
                retn = 1;
            } else {
                retn = 0;
            }
            break;
        }
        if( *cp != *chrs ) {
            retn = 0;
            break;
        }
    }
    return retn;
}


unsigned Str::size              // GET SIZE
    ( void ) const
{
    return ::strlen( _str );
}


Str& Str::operator =            // ASSIGN STRING
    ( char const * str )        // - string
{
    _str = storageList.alloc( str );
    return *this;
}
