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


#ifndef __BTREEIT_H__
#define __BTREEIT_H__

#include "btreebas.h"

template <class Type> class WCValOrderedVector;

template <class Key_T, class Obj_T> class BTreeNodeBase;

template <class Key_T, class Obj_T>
class SearchEntry {
public:
    bool operator==( const SearchEntry& ) const { return FALSE; }

    BTreeNodeBase<Key_T,Obj_T> *    nextNode();
    Obj_T *                         nextObj();

    BTreeNodeBase<Key_T,Obj_T> *        _node;
    int_16                              _entry;
};

template < class Key_T, class Obj_T >
class BTreeIterator {
public:
                BTreeIterator( BTreeRootedBase< Key_T, Obj_T > * tree );
                ~BTreeIterator();

        Obj_T&  current();
        bool    operator++ ();

private:

    Obj_T *                                             _currObj;
    BTreeRootedBase< Key_T, Obj_T > *                   _tree;
    WCValOrderedVector< SearchEntry<Key_T, Obj_T> > *   _stack;
};

#endif
