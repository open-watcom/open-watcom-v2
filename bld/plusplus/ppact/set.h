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


#ifndef __SET__H
#define __SET__H

// SET -- templates for sets implementation

#include "setimpl.h"

template< class T >class Set;
template< class T >class SetIter;


template< class T >
class Set {                     // Set -- definition
                                // *****************

public :

    Set                         // constructor
        ( void )
        {
        }

    ~Set                        // destructor
        ( void )
        {
        }

    int empty                   // test if empty
        ( void )
        const
        {
            return _set.empty();
        }

    T* first                    // get first
        ( void
        ) const
        {
            return (T*)_set.first();
        }

    T* last                     // get last
        ( void
        ) const
        {
            return (T*)_set.last();
        }

    T* insert                   // insert
        ( T* obj )              // - item to be inserted
        {
            return (T*)_set.insert( obj );
        }

    T* insert                   // insert
        ( T* obj                // - item to be inserted
        , T* pred )             // - preceding element
        {
            return (T*)_set.insert( obj, pred );
        }

    void erase                  // remove from set
        ( int index )           // - index of item to be erased
        {
            _set.erase( index );
        }

    void erase                  // remove from set
        ( T* obj )              // - object to be removed
        {
            _set.erase( obj );
        }

    _SetImpl const& implementation // get implementation
        ( void )
        const
        {
            return _set;
        }

protected:
    _SetImpl _set;              // set implementation
};


template< class T >
class SetIter {                 // SetIter -- iterator
                                // *******************

public:

    SetIter                     // constructor
        ( Set<T> const & set )  // - header
        : _iter( set.implementation() )
        {
        }

    SetIter& operator++         // moves ahead
        ( void )
        {
            ++_iter;
            return *this;
        }

    T* operator *               // extracts member
        ( void
        ) const
        {
            return (T*)*_iter;
        }

    void erase                  // erase current element, move ahead
        ( void )
        {
            _iter.erase();
        }

    int index                   // get current index
        ( void )
        const
        {
            return _iter.index();
        }

private:
    _SetIterImpl _iter;         // implementation

};


template< class T >
class SetAlloc : public Set<T>  // SetAlloc -- allocated set
{                               // *************************

public:

    SetAlloc                    // constructor
        ( void )
        {
        }

    ~SetAlloc                   // destructor
        ( void );
};


template< class T >
SetAlloc<T>::~SetAlloc          // destructor
    ( void )
{
    SetIter<T> iter( *this );
    for( ; ; ++iter ) {
        T* curr = *iter;
        if( curr == 0 ) break;
        delete curr;
    }
}

// type T must have operator >
//
template< class T >
class SetOrdered                // SetOrdered -- ordered, allocated set
    : public SetAlloc<T>        // ************************************
{
public:

    T* insert                   // insert
        ( T* obj );             // - item to be inserted
};


template< class T >
T* SetOrdered<T>::insert        // INSERT INTO ORDERED SET
    ( T* obj )                  // - object to be inserted
{
    SetIter<T> iter( *this );
    T* curr;
    T* pred;
    for( pred = 0; ; pred = curr, ++iter ) {
        curr = *iter;
        if( curr == 0
         || *curr > *obj ) {
            obj = (T*)_set.insert( obj, pred );
            break;
        }
    }
    return obj;
}

#endif
