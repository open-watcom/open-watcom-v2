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


#include <wcvector.h>

#include "btreeit.h"
#include "btree.h"
#include "mrdeath.h"

#if 1   // only while in C file to instantiate
    #include "btreend.h"
    #include "mrinfo.h"
    typedef BTreeIterator<MergeNameKey,MergeDIE>__grokky_7;
    typedef BTreeIterator<MergeOffset,MergeDIE> __grokky_8;
#endif

template <class Key_T, class Obj_T>
BTreeNodeBase<Key_T, Obj_T> *
SearchEntry<Key_T,Obj_T>::nextNode()
//----------------------------------
{
    return _node->nextNode( _entry );
}

template <class Key_T, class Obj_T>
Obj_T * SearchEntry<Key_T, Obj_T>::nextObj()
//------------------------------------------
{
    return _node->nextObj( _entry );
}

template < class Key_T, class Obj_T >
BTreeIterator<Key_T,Obj_T>::
BTreeIterator( BTreeRootedBase< Key_T, Obj_T > * tree )
                : _tree( tree )
                , _currObj( NULL )
                , _stack( NULL )
//----------------------------------------------------
{
}

template < class Key_T, class Obj_T >
BTreeIterator<Key_T,Obj_T>::
~BTreeIterator()
//-----------------------------------
{
    _tree->unlock( _currObj );
}

template < class Key_T, class Obj_T >
Obj_T & BTreeIterator<Key_T,Obj_T>::
current()
//-----------------------------------
{
    InternalAssert( _currObj != NULL );
    return *_currObj;       // might be NULL, but c'est la vie
}

template < class Key_T, class Obj_T >
bool BTreeIterator<Key_T,Obj_T>::
operator++ ()
//-----------------------------------
{
    SearchEntry<Key_T,Obj_T>        entry;
    BTreeNodeBase<Key_T,Obj_T> *    next;

    if( _stack == NULL ) {
        _stack = new WCValOrderedVector< SearchEntry<Key_T,Obj_T> >( 200, 100 );
        next = _tree->getRoot();

        if( next == NULL ) return FALSE;    //<----- early return for empty tree

        do {
            entry._entry = -1;
            entry._node = next;

            next = entry.nextNode();

            _stack->append( entry );
        } while( next != NULL );
    }

    InternalAssert( _stack->entries() != 0 );

    _tree->unlock( _currObj );      // might be NULL, but that's ok to unlock

    while( 1 ) {
        entry = (*_stack)[ _stack->entries() - 1 ];
        _stack->removeLast();
        _currObj = entry.nextObj();
        _stack->append( entry );

        if( _currObj != NULL ) {
            _tree->lock( _currObj );
            return TRUE;                // <------- next object found
        }

        do {
            _stack->removeLast();       // discard empty node
            if( _stack->entries() == 0 ) {
                _currObj = NULL;
                _stack->clear();        // redundant, but what the hey
                delete _stack;
                _stack = NULL;
                return FALSE;           // <------- end of objects
            }
            next = (*_stack)[ _stack->entries() - 1 ].nextNode();
        } while( next == NULL );

        do {
            entry._node = next;
            entry._entry = -1;
            next = entry.nextNode();
            _stack->append( entry );
        } while( next != NULL );
    }
}

#if 0
template < class Key_T, class Obj_T >
bool BTreeIterator<Key_T,Obj_T>::
operator++ ()
//-----------------------------------
{
    SearchEntry<Key_T,Obj_T> entry;
    BTreeNodeBase<Key_T,Obj_T> * next;

    _tree->unlock( _currObj );

    if( _currObj == NULL ) {
        _stack = new WCStack< SearchEntry<Key_T,Obj_T>,
                                WCValSList< SearchEntry<Key_T,Obj_T> > >;

        next = _tree->getRoot();

        if( next == NULL ) return FALSE;    //<----- early return for empty tree

        do {
            entry._entry = -1;
            entry._node = next;

            next = entry._node->nextNode( entry._entry );

            _stack->push( entry );
        } while( next != NULL );
    }

    do {
        entry = _stack->pop();

        _currObj = entry._node->nextObj( entry._entry );

        if( _currObj != NULL ) {
            _stack->push( entry );
            return TRUE;
        }

        do {
            if( _stack->isEmpty() ) {
                _currObj = NULL;
                _stack->clear();
                delete _stack;
                _stack = NULL;
                return FALSE;
            }

            entry = _stack->pop();

            next = entry._node->nextNode( entry._entry );
        } while( next == NULL );
        _stack->push( entry );  // put the entry back on

        do {
            entry._entry = -1;
            entry._node = next;

            next = entry._node->nextNode( entry._entry );

            _stack->push( entry );
        } while( next != NULL );

    } while( next == NULL );

    _tree->lock( _currObj );
    return TRUE;
}
#endif
