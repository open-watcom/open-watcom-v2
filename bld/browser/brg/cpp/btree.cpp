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


#include <stdlib.h>
#include <wclist.h>
#include <wclistit.h>
#include "btree.h"
#include "btreend.h"
#include "btreeit.h"
#include "mrinfo.h"
#include "mrfile.h"

/*-------------------------- BTreeBase------------------------------*/

template <class Obj_T>
void BTreeBase<Obj_T>::setPrimary( BTreeBase<Obj_T> * n )
//-------------------------------------------------------
{
    while( n && n->_primary ) {
        n = n->_primary;
    }
    _primary = n;

    if( n != NULL ) {
        _primary->_indicies->insert( this );
    } else {
        _indicies = new WCPtrSList<BTreeBase>;
        _indicies->append( this );  // we are first index that's updated
    }
};

template <class Obj_T>
BTreeBase<Obj_T>::~BTreeBase()
//-----------------------------
{
    if( _primary ) {
        //NYI -- remove from primary's indicies
    } else {
        _indicies->get();   // remove this from list
        _indicies->clear();
        delete _indicies;
    }
}

/*-------------------------- BTree -------------------------------*/

template < class Key_T, class Obj_T >
BTree<Key_T, Obj_T>::BTree( uint keyOrder, uint objOrder,
                            BTreeBase<Obj_T> * primary )
    :_root( NULL )
    ,_keyOrder( keyOrder )
    ,_objOrder( objOrder )
//------------------------------------------------------
{
    setPrimary( primary );      // also puts us in indicies
    BTreeNodeBase<Key_T,Obj_T>::setKeyObjOrder( keyOrder, objOrder );

    #if INSTRUMENTS
    _locks = 0;
    _unlocks = 0;
    #endif
}

template < class Key_T, class Obj_T >
BTree<Key_T, Obj_T>::~BTree()
//-----------------------------------
{
    _root->ragnarok();

    #if INSTRUMENTS
    Log.printf( "BTree - %d locks, %d unlocks\n", _locks, _unlocks );
    #endif
}

#if INSTRUMENTS
template < class Key_T, class Obj_T >
void BTree<Key_T, Obj_T>::print()
//-----------------------------------
{
    Log.printf( "\n-------------------------------------------\n\n" );
    if( _root != NULL ) {
        _root->print( 0 );
    } else {
        Log.printf( "[empty tree]\n" );
    }
}
#endif

template < class Key_T, class Obj_T >
Obj_T * BTree<Key_T, Obj_T>::find( const Key_T & key )
//----------------------------------------------------
{
    Obj_T * res;

    if( _root != NULL ) {
        res = _root->find( key );
    } else {
        res = NULL;
    }

    #if INSTRUMENTS
    if( res ) {
        lock( res );
    }
    #endif

    return res;
}

template < class Key_T, class Obj_T >
bool BTree<Key_T, Obj_T>::unlock( Obj_T * obj )
//---------------------------------------------
// unlock a locked node.
{
    if( obj != NULL ) {
        if( _primary ) {
            return _primary->unlock( obj );
        } else {
            #if INSTRUMENTS
            _unlocks += 1;
            #endif

            return TRUE;
        }
    }

    return FALSE;
}

template < class Key_T, class Obj_T >
bool BTree<Key_T, Obj_T>::lock( Obj_T * obj )
//-------------------------------------------
// lock a node.
{
    if( obj != NULL ) {
        if( _primary ) {
            return _primary->lock( obj );
        } else {
            #if INSTRUMENTS
            _locks += 1;
            #endif

            return TRUE;
        }
    } else {
        #if INSTRUMENTS
        Log.printf( "lock NULL!!\n" );
        #endif
    }

    return FALSE;
}

template < class Key_T, class Obj_T >
void BTree<Key_T, Obj_T>::update( Obj_T * obj )
// add the object to this b-tree, disregarding
// other indicies
//---------------------------------------------------
{
    BTreeNodeBase<Key_T,Obj_T> * newNode;
    Key_T           newKey;
    Obj_T *         result;
    bool            needsSplit;

    if( _root == NULL ) {
        BTreeBucketNode<Key_T,Obj_T> * lhn;
        BTreeBucketNode<Key_T,Obj_T> * rhn;

        lhn = new BTreeBucketNode<Key_T,Obj_T>();
        rhn = new BTreeBucketNode<Key_T,Obj_T>( obj );

        _root = new BTreeSearchNode<Key_T,Obj_T>( (const Key_T&)(*obj),
                                                    lhn, rhn );
    } else {
        result =_root->insert( obj, needsSplit, newKey, newNode );
        if( needsSplit ) {
            _root = new BTreeSearchNode<Key_T,Obj_T>( newKey, _root,
                                                        newNode );
        }
    }

    incEntries();
}

template < class Key_T, class Obj_T >
void BTree<Key_T, Obj_T>::insert( Obj_T * obj )
//---------------------------------------------
{
    WCPtrSListIter< BTreeBase<Obj_T> >   indicies( *getIndicies() );

    if( getPrimary() != NULL ) {        // this is secondary tree
        getPrimary()->insert( obj );
    } else {                    // this is primary tree

        // update all the indexes on this b-file.  Note that the
        // primary tree is the first index

        for(;;) {
            if( !(indicies += 1) ) break;
            indicies.current()->update( obj );
        }
    }

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
        print();
    #endif
}


template < class Key_T, class Obj_T >
void BTree<Key_T, Obj_T>::remove( Obj_T * obj )
//---------------------------------------------
{
    _root->remove( (const Key_T &)*obj );
    decEntries();
}

//typedef BTreeIterator<MergeOffset,MergeDIE> __grokky_1;
//typedef BTreeIterator<MergeNameKey,MergeDIE>__grokky_2;
