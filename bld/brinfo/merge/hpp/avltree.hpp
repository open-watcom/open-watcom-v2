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


/*
    AVLTREE.HPP
    -----------
    A specialized container class for the browse/merge dll.

    AvlTree<T> : Implementation of AVL-trees, a balanced tree algorithm.
    Faster than a hash table but uses way more memory.
*/

#ifndef _AVLTREE_HPP
#define _AVLTREE_HPP

#include "pool.hpp"

/*  Turn off memory tracking to redefine "new" and "delete"
*/
#undef new
#undef delete

// I'm not terribly worried about over-flowing the stacks...it would
// require over 12 billion entries to create an AvlTree of height 65...
#define AVL_STACK_SIZE  64


/*  AvlTreeBase:
      Guts of the AvlTree template class.
*/

class AvlTreeBase {
    protected:
        AvlTreeBase();

    public:
        ~AvlTreeBase();

        // Clear the tree without deleting the pointed-to entries.
        void    Clear();

        // Access functions.
        WBool   Empty() { return _head == NULL; }
        int     Count() { return _numEntries; }
        uint_32 CurrentKey() { return _current?_current->key:0; }

    protected:
        // Functions overriden in the derived template classes.
        WBool   Insert( uint_32 key, void *data, WBool replace );
        void    *Find( uint_32 key );
        void    *First();
        void    *Next();

        // Search for entries with keys just before or after the given key.
        // Used by UsageList::NextAtLeft() and UsageList::NextAtRight().
        void    *FindLeft( uint_32 key );
        void    *FindRight( uint_32 key );

        // Warning!  Do not use Remove if you plan to Insert more stuff
        // later!  Remove does not preserve the balanced tree properties.
        void    *Remove( uint_32 key );

    protected:

        struct AvlNode {
            AvlNode     *left;
            AvlNode     *right;
            void        *data;
            uint_32     key;
            unsigned    equalSubtrees : 1;
            unsigned    leftHeavy : 1;

            static Pool avlPool;

            void        *operator new(size_t) { return avlPool.Get(); }
            void        *operator new(size_t, const WChar *, const WChar *,
                                      WULong) { return avlPool.Get(); }
            void        operator delete(void *p) { avlPool.Release(p); }
        };

        struct AvlStack {
            AvlNode     *array[AVL_STACK_SIZE];
        };
        static Pool     _stackPool;     // stacks are shared to save memory

        AvlStack        *_stack;

        AvlNode         *_head;
        int             _sp;
        AvlNode         *_current;
        int             _numEntries;
};


/*  AvlTree<T>:
      A template container class using balanced trees.
*/

template<class T>
class AvlTree : public AvlTreeBase {
    public:
        AvlTree() : AvlTreeBase() {}

        T *     Find( uint_32 key ) { return (T *) AvlTreeBase::Find( key ); }
        T *     FindLeft( uint_32 key ) { return (T *) AvlTreeBase::FindLeft( key ); }
        T *     FindRight( uint_32 key ) { return (T *) AvlTreeBase::FindRight( key ); }
        void    Insert( uint_32 key, T *data, WBool replace=FALSE ) { AvlTreeBase::Insert( key, data, replace ); }
        T *     First() { return (T *) AvlTreeBase::First(); }
        T *     Next() { return (T *) AvlTreeBase::Next(); }

        // See warning for AvlTreeBase::Remove, above.
        T *     Remove( uint_32 key ) { return (T *) AvlTreeBase::Remove( key ); }
};


/*  Restart memory tracking
*/
#include "wnew.hpp"

#endif  // _AVLTREE_HPP
