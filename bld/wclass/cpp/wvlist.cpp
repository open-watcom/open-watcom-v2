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


#include "wvlist.hpp"
#include "wobjfile.hpp"

extern "C" {
    #include <stdlib.h>
};

Define( WVList )

WEXPORT WVList::WVList()
    : _set( NULL )
    , _count( 0 )
    , _free( 0 )
{
}

WEXPORT WVList::WVList( WVList& x )
    : _set( NULL )
    , _count( x._count )
    , _free( x._free )
{
    if( x._set != NULL ) {
        _set = new WObject*[ _count ];
        for( int i=0; i<_count; i++ ) {
            _set[i] = x._set[i];
        }
    }
}

WEXPORT WVList::~WVList()
{
    delete [] _set;
}

#ifndef NOPERSIST
WVList* WEXPORT WVList::createSelf( WObjectFile& )
{
    return new WVList();
}

void WEXPORT WVList::readSelf( WObjectFile& p )
{
    int count;
    p.readObject( &count );
    for( int i=0; i<count; i++ ) {
        add( p.readObject() );
    }
}

void WEXPORT WVList::writeSelf( WObjectFile& p )
{
    p.writeObject( count() );
    for( int i=0; i<count(); i++ ) {
        p.writeObject( _set[i] );
    }
}
#endif

void WEXPORT WVList::deleteContents()
{
    if( _set != NULL ) {
        for( int i=_free; i>0; i-- ) {
            if( _set[ i-1 ] != NULL ) {
                delete _set[ i-1 ];
            }
        }
        _free = 0;
        delete [] _set;
        _set = NULL;
    }
}

void WEXPORT WVList::reset()
{
    _free = 0;
    delete [] _set;
    _set = NULL;
}

WObject* WEXPORT WVList::find( WObject* obj )
{
    for( int i=0; i<_free; i++ ) {
        if( _set[ i ] != NULL ) {
            if( _set[i]->isEqual( obj ) ) {
                return _set[ i ];
            }
        }
    }
    return NULL;
}

void WVList::growBlock()
{
    if( _set == NULL ) {
        static int _countInit = 10;
        _set = new WObject*[ _countInit ];
        if( _set != NULL ) {
            _count = _countInit;
            _free = 0;
        }
    }
    if( _set != NULL ) {
        if( _free >= _count ) {
            static int _countIncr = 5;
            WObject** nset = new WObject*[ _count + _countIncr ];
            if( nset != NULL ) {
                for( int i=0; i<_count; i++ ) {
                    nset[i] = _set[i];
                }
                delete [] _set;
                _set = nset;
                _count += _countIncr;
            }
        }
    }
}

WObject* WEXPORT WVList::add( WObject* obj )
{
    growBlock();
    if( _set != NULL ) {
        _set[ _free ] = obj;
        _free += 1;
        return obj;
    }
    return NULL;
}

WObject* WEXPORT WVList::insertAt( int i, WObject* obj )
/*
 * NOTE: this doesn't insert anything after the end of the string.
*/
{
    if( i < 0 ) return add( obj );
    growBlock();
    if( _set != NULL && i <= _free ) {
        if( i < _free ) {
            memmove( _set + i + 1, _set + i,
                    ( _free - i ) * sizeof( WObject * ) );
        }
        _set[ i ] = obj;
        _free += 1;
        return obj;
    }
    return NULL;
}

int WEXPORT WVList::indexOfSame( WObject* obj )
{
    for( int i=0; i<_free; i++ ) {
        if( _set[ i ] != NULL ) {
            if( obj == _set[ i ] ) {
                return i;
            }
        }
    }
    return -1;
}

WObject* WEXPORT WVList::replaceAt( int i, WObject* obj )
{
    if( i >= 0 && i < _free ) {
        WObject* o = _set[ i ];
        _set[ i ] = obj;
        return o;
    }
    return NULL;
}

WObject* WEXPORT WVList::removeAt( int i )
{
    if( i >= 0 && i < _free ) {
        WObject* obj = _set[ i ];
        _free -= 1;
        if( _free > 0 ) {
            for( ; i< _count-1; i++ ) {
                _set[ i ] = _set[ i+1 ];
            }
        } else {
            delete [] _set;
            _set = NULL;
        }
        return obj;
    }
    return NULL;
}

WObject* WEXPORT WVList::removeSame( WObject* obj )
{
    return removeAt( indexOfSame( obj ) );
}

extern "C" int ccompare( const void* lhs, const void* rhs )
{
    return (*(WObject**)lhs)->compare( *(WObject**)rhs );
}

void WEXPORT WVList::sort()
{
    if( _free > 1 ) {
        qsort( _set, _free, sizeof( WObject* ), ccompare );
    }
}
