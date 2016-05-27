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


#ifndef __BTREE_H_INCLUDED__

#include "btreebas.h"

template <class Key_T, class Obj_T> class BTreeNodeBase;

template< class Obj_T >
class BTreeBase {
public:
                        BTreeBase() : _entries( 0 ), _primary( NULL ){};
    virtual             ~BTreeBase();

            uint        entries() const { return _entries; };   // num elements
    virtual bool        unlock( Obj_T * ){ return false; }
    virtual bool        lock( Obj_T * ){ return false; }
            bool        operator== ( const BTreeBase& other ) const {
                            return &other == this;
                        }

            friend class BTree; // so can access protected members thru ptr

protected:
    virtual void        insert( Obj_T * ) = 0;
    virtual void        remove( Obj_T * ) = 0;
    virtual void        update( Obj_T * ) = 0;

            void        incEntries() { _entries += 1; };
            void        decEntries() { _entries -= 1; };
            void        setPrimary( BTreeBase * n );
            BTreeBase * getPrimary() { return _primary; };

            WCPtrSList<BTreeBase> *     getIndicies() { return _indicies; };

private:
    uint                    _entries;
    BTreeBase *             _primary;
    WCPtrSList<BTreeBase> * _indicies;
};

template< class Key_T, class Obj_T >
class BTreeRootedBase : public BTreeBase< Obj_T > {

public:
    virtual BTreeNodeBase< Key_T, Obj_T> * getRoot() = 0;
};

template < class Key_T, class Obj_T >
class BTree : public BTreeRootedBase< Key_T, Obj_T > {

public:
                BTree( uint keyOrder, uint objOrder,
                        BTreeBase<Obj_T> * primary = NULL );
                ~BTree();

            // find an element, return a pointer to a locked object
            Obj_T *     find( const Key_T & key );

            // insert an element, updating all indices
            void        insert( Obj_T * obj );
            void        remove( Obj_T * obj );

            // unlock a found object
            bool        unlock( Obj_T * );
            bool        lock( Obj_T * );

    virtual BTreeNodeBase< Key_T, Obj_T> * getRoot() { return _root; }

            #if INSTRUMENTS
            void    print();
            #endif

protected:
            void    update( Obj_T * obj );

private:
    BTreeNodeBase<Key_T,Obj_T> *    _root;
    uint                            _keyOrder;
    uint                            _objOrder;

    #if INSTRUMENTS
    int                 _locks;
    int                 _unlocks;
    #endif
};

#define __BTREE_H_INCLUDED__
#endif
