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


#ifndef _DOUBLY_LINKED_LIST_H
#define _DOUBLY_LINKED_LIST_H

// System includes --------------------------------------------------------

// Project includes -------------------------------------------------------

#include "errors.h"

/*
 | Doubly Linked List Element ---------------------------------------------
 */

template<class T> class DLLElement {
friend class DLList;
                        DLLElement( DLLElement * l, DLLElement * r, T data );

    DLLElement *        _left;
    DLLElement *        _right;
    T                   _data;
};

template<class T> DLLElement<T>::DLLElement( DLLElement<T> * l,
                                             DLLElement<T> * r,
                                             T data )
/**********************************************************/
    : _left( l )
    , _right( r )
    , _data( data )
{
}

/*
 | Doubly Linked List -----------------------------------------------------
 */

template<class T>class DLList {
public:
                        DLList();
                        ~DLList();

    void                addLeft( T );
    void                addRight( T );
    T                   getLeft();
    T                   getRight();
    T                   deleteLeft();
    T                   deleteRight();
    T                   operator[]( int );
    int                 count();

protected:
    DLLElement<T> *     _leftMost;
    DLLElement<T> *     _rightMost;

    /*
     | currIndex is the zero-based index of the element which _currElem
     | points to, where we index from left to right; this is used by
     | operator[]
     */
    int                 _currIndex;
    DLLElement<T> *     _currElem;
    int                 _count;
};

template<class T> DLList<T>::DLList()
/********************************/
    : _leftMost( NULL )
    , _rightMost( NULL )
    , _currIndex( -1 )
    , _count( 0 )
{
}

template<class T> DLList<T>::~DLList()
/*********************************/
{
    DLLElement<T> * next;

    while( _leftMost != NULL ) {
        next = _leftMost->_right;
        delete _leftMost;
        _leftMost = next;
    }
}

template<class T> void DLList<T>::addLeft( T obj )
/*********************************************/
{
    DLLElement<T> * elem;

    if( _leftMost == NULL ) {
        _leftMost = new DLLElement<T>( NULL, NULL, obj );
        if( _leftMost == NULL ) {
            throw ErrOutOfMemory;
        }
        _rightMost = _leftMost;
    } else {
        elem = new DLLElement<T>( NULL, _leftMost, obj );
        if( elem == NULL ) {
            throw ErrOutOfMemory;
        }
        _leftMost->_left = elem;
        _leftMost = elem;
        /*
         | Adding an element on the left side increases the index of the
         | current element.
         */
        if( _currIndex >= 0 ) {
            _currIndex++;
        }
    }
    _count++;
}

template<class T> void DLList<T>::addRight( T obj )
/*************************************************/
{
    DLLElement<T> * elem;

    if( _rightMost == NULL ) {
        _rightMost = new DLLElement<T>( NULL, NULL, obj );
        if( _rightMost == NULL ) {
            throw ErrOutOfMemory;
        }
        _leftMost = _rightMost;
    } else {
        elem = new DLLElement<T>( _rightMost, NULL, obj );
        if( elem == NULL ) {
            throw ErrOutOfMemory;
        }
        _rightMost->_right = elem;
        _rightMost = elem;
    }
    _count++;
}

template<class T> T DLList<T>::getLeft()
/**************************************/
{
    return( _leftMost->_data );
}

template<class T> T DLList<T>::getRight()
/***************************************/
{
    return( _rightMost->_data );
}

template<class T> T DLList<T>::deleteLeft()
/*****************************************/
{
    T tmp;
    DLLElement<T> * next;

    if( _leftMost != NULL ) {
        tmp = _leftMost->_data;
        next = _leftMost->_right;
        delete _leftMost;
        _leftMost = next;

        if( _leftMost != NULL ) {
            _leftMost->_left = NULL;
        } else {
            _rightMost = NULL;
        }

        if( _currIndex <= 0 ) {
            _currIndex = -1;
            _currElem = NULL;
        }

        _count--;

        return tmp;
    }


    return NULL;

}

template<class T> T DLList<T>::deleteRight()
/***************************************/
{
    T tmp;
    DLLElement<T> * next;

    if( _rightMost != NULL ) {
        tmp = _rightMost->_data;
        next = _rightMost->_left;

        if( _rightMost == _currElem ) {
            _currElem = next;
            _currIndex--;
        }

        delete _rightMost;
        _rightMost = next;
        if( _rightMost != NULL ) {
            _rightMost->_right = NULL;
        } else {
            _leftMost = NULL;
        }
        _count--;
        return tmp;
    }

    return NULL;
}

template<class T> T DLList<T>::operator[]( int index )
/*************************************************/
{
    DLLElement<T> * elem;
    int i;

    if( _leftMost == NULL ) {
        return NULL;
    }

    elem = _leftMost;
    for( i = 0; i < index; i += 1 ) {
        if( elem == NULL ) {
            return NULL;
        }

        elem = elem->_right;
    }

    return elem->_data;

#if 0
    DLLElement<T> * elem;
    int i;

    if( _leftMost == NULL ) {
        return NULL;                            // <-- early return ***
    }

    if( _currIndex == -1 ) {
        elem = _leftMost;
        for( i = 0; i < index; i += 1 ) {
            elem = elem->_right;

            /*
             | Check if index out of range
             */
            if( elem == NULL ) {
                return NULL;                    // <-- early return ***
            }
        }

        _currElem = elem;
        _currIndex = index;
    } else {
        if( index < _currIndex ) {
            for( ; _currIndex > index; _currIndex-- ) {
                _currElem = elem->_left;
            }
        } else {
            elem = _currElem;
            for( i = _currIndex; i < index; i += 1 ) {
                elem = elem->_right;

                if( elem == NULL ) {
                    return NULL;                // <-- early return ***
                }
            }

            _currElem = elem;
            _currIndex = index;
        }
    }

    return( _currElem->_data );
#endif
}

template<class T> int DLList<T>::count()
/**************************************/
{
    return( _count );
}

#endif
