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


#ifndef __BTREEND_H__
#define __BTREEND_H__

#include <wstd.h>

#include "mempool.h"

template < class Key_T, class Obj_T > class BTreeBucketNode;

template < class Key_T, class Obj_T >
class BTreeNodeBase {
public:
                                BTreeNodeBase(){};
    virtual                     ~BTreeNodeBase(){};

    virtual Obj_T *             find( const Key_T & key ) = 0;
    virtual Obj_T *             insert( Obj_T * obj, bool & nSplit,
                                        Key_T & key,
                                        BTreeNodeBase *& newNode ) = 0;
    virtual void                remove( const Key_T & key ) = 0;

    virtual BTreeNodeBase *     nextNode( int_16 & idx ) = 0;
    virtual Obj_T *             nextObj( int_16 & idx ) = 0;
    static  void                ragnarok();
    static  void                setKeyObjOrder( uint keyOrd, uint objOrd );

    #if INSTRUMENTS
    virtual void                print( int indent ) = 0;
    #endif

};

template< class Key_T, class Obj_T >
class BTreeSearchNode : public BTreeNodeBase< Key_T, Obj_T > {
public:
                    BTreeSearchNode( const Key_T & mid, BTreeNodeBase * lhn,
                                     BTreeNodeBase * rhn );
    virtual         ~BTreeSearchNode();

    virtual Obj_T * find( const Key_T & key );
    virtual Obj_T * insert( Obj_T * obj, bool & nSplit, Key_T & key,
                            BTreeNodeBase *& newNode );
    virtual void    remove( const Key_T & key );

    virtual BTreeNodeBase<Key_T,Obj_T> *    nextNode( int_16 & idx );
    virtual Obj_T *                         nextObj( int_16 & idx );

            void *  operator new( size_t ) { return _pool.alloc(); };
            void    operator delete( void * p ) { _pool.free( p ); };
    static  void    ragnarok();
    static  void    setKeyOrder( uint KeyOrder );


    #if INSTRUMENTS
    virtual void        print( int indent );
    #endif

protected:
    struct NodeStore {
        Key_T           _separator;
        BTreeNodeBase * _child;
    };
                        BTreeSearchNode();

    void                privInsert( const Key_T & key,
                                BTreeNodeBase *& newNode );
    void                split( Key_T & key, BTreeNodeBase *& newNode );
    uint                privSearch( const Key_T & key, uint lower, uint upper );

private:
            uint        _degree;                    // number of children
            NodeStore * _nodes;                     // dynamic array

    static uint         _keyOrder;
    static MemoryPool   _pool;
    static MemoryPool   _nodePool;
};

template < class Key_T, class Obj_T >
class BTreeBucketNode : public BTreeNodeBase< Key_T, Obj_T > {
public:
            BTreeBucketNode();
            BTreeBucketNode( Obj_T * obj );
    virtual ~BTreeBucketNode();

    virtual Obj_T *     find( const Key_T & key );
    virtual Obj_T *     insert( Obj_T * obj, bool & nSplit, Key_T & key,
                            BTreeNodeBase *& newNode );
    virtual void        remove( const Key_T & key );

    virtual BTreeNodeBase<Key_T,Obj_T> *        nextNode( int_16 & idx );
    virtual Obj_T *                             nextObj( int_16 & idx );

            void *      operator new( size_t ) { return _pool.alloc(); };
            void        operator delete( void * p ) { _pool.free( p ); };
    static  void        ragnarok();
    static  void        setObjOrder( uint objOrder );

    #if INSTRUMENTS
    virtual void        print( int indent );
    #endif

protected:
    Obj_T *             privInsert( Obj_T * obj );
    Obj_T *             split( Key_T & key, Obj_T * obj,
                            BTreeNodeBase *& newNode );
    uint                privSearch( const Key_T & key, uint lower, uint upper );

private:
    typedef Obj_T * ObjPtr;

            uint            _degree;                    // number of objects
            ObjPtr *        _nodes;                     // dynamic array of ptrs

    static  uint            _objOrder;
    static  MemoryPool      _pool;
    static  MemoryPool      _nodePool;
};


#endif // __BTREEND_H__
