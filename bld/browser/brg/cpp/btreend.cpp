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


#include "btreend.h"

#if 1 // only while in CPP file to instantiate
    #include "btree.h"
    #include "btreeit.h"
    #include "mrinfo.h"

    typedef BTreeBucketNode<MergeOffset,MergeDIE>       __grokky_1;
    typedef BTreeBucketNode<MergeNameKey,MergeDIE>      __grokky_2;
    typedef BTreeSearchNode<MergeOffset,MergeDIE>       __grokky_3;
    typedef BTreeSearchNode<MergeNameKey,MergeDIE>      __grokky_4;
#endif

#pragma warning 549 9   // sizeof contains compiler genned info

/*--------------------- BTreeSearchNode -------------------------------*/

template < class Key_T, class Obj_T >
static void BTreeNodeBase<Key_T,Obj_T>::
ragnarok()
//-----------------------------------
{
    BTreeSearchNode<Key_T,Obj_T>::ragnarok();
    BTreeBucketNode<Key_T,Obj_T>::ragnarok();
}

template < class Key_T, class Obj_T >
static void BTreeNodeBase<Key_T,Obj_T>::
setKeyObjOrder( uint keyOrd, uint objOrd )
//------------------------------------------
{
    BTreeSearchNode<Key_T,Obj_T>::setKeyOrder( keyOrd );
    BTreeBucketNode<Key_T,Obj_T>::setObjOrder( objOrd );
}


/*--------------------- BTreeSearchNode -------------------------------*/

template < class Key_T, class Obj_T >
static uint BTreeSearchNode<Key_T,Obj_T>::
_keyOrder( -1 );

const int BTreeSearchNodePoolSize = 1024;
template < class Key_T, class Obj_T >
static MemoryPool BTreeSearchNode<Key_T,Obj_T>::
_pool( sizeof( BTreeSearchNode ), "BTreeSearchNode", BTreeSearchNodePoolSize );

const int BTreeSearchNodeNodePoolSize = 1024;
template < class Key_T, class Obj_T >
static MemoryPool BTreeSearchNode<Key_T,Obj_T>::
_nodePool( "BTreeSearchNodeNode" );

template < class Key_T, class Obj_T >
BTreeSearchNode<Key_T,Obj_T>::
BTreeSearchNode( const Key_T & mid, BTreeNodeBase<Key_T,Obj_T> * lhn,
                BTreeNodeBase<Key_T,Obj_T> * rhn )
        : _degree( 2 )
{
    _nodes = (NodeStore *) _nodePool.alloc();
    _nodes[ 0 ]._separator.operator= ( mid );
    _nodes[ 0 ]._child = lhn;
    _nodes[ 1 ]._child = rhn;
}

template < class Key_T, class Obj_T >
BTreeSearchNode<Key_T,Obj_T>::
BTreeSearchNode()
    : _degree( 0 )
{
    _nodes = (NodeStore *) _nodePool.alloc();
}

template < class Key_T, class Obj_T >
BTreeSearchNode<Key_T,Obj_T>::
~BTreeSearchNode()
{
    // WARNING -- this destructor is not guaranteed to be called
    //            since the elements are pooled and freed using
    //            the pool's ragnarok
}

template < class Key_T, class Obj_T >
static void BTreeSearchNode<Key_T,Obj_T>::
ragnarok()
//----------------------------------------
{
    _pool.ragnarok();
    _nodePool.ragnarok();
}

template < class Key_T, class Obj_T >
static void BTreeSearchNode<Key_T,Obj_T>::
setKeyOrder( uint keyOrder )
//------------------------------------------
{
    _keyOrder = keyOrder;
    _nodePool.setSize( sizeof( NodeStore ) * keyOrder * 2 + sizeof( NodeStore ),
                       BTreeSearchNodeNodePoolSize );
}

#if INSTRUMENTS
template < class Key_T, class Obj_T >
void BTreeSearchNode<Key_T,Obj_T>::
print( int indent )
//----------------------------------
{
    int i;
    int len = 0;
    int test;
    char * dashes = "-------------------------------------------------------";

    for( i = 0; i < _degree - 1; i += 1 ) {
        if( _nodes[ i ]._separator.getString() ) {
            test = strlen( _nodes[ i ]._separator.getString() );
        } else {
            test = strlen( "NULL" );
        }
        len = (test>len) ? test : len;
    }

    for( i = 0; i < _degree - 1; i += 1 ) {
        _nodes[ i ]._child->print( indent + 15 );
        if( i == 0 ) {
            Log.printf( "%*c/%.*s\\\n", indent, ' ', len, dashes );
        }
        const char * c = _nodes[ i ]._separator.getString();
        if( c ) {
            Log.printf( "%*c|%*.*s|\n", indent, ' ', len, len, c );
        } else {
            Log.printf( "%*c|%*.*s|\n", indent, ' ', len, len, "NULL" );
        }
    }
    Log.printf( "%*c\\%*.*s/\n", indent, ' ', len, len, dashes );
    _nodes[ i ]._child->print( indent + 15 );
}
#endif

template < class Key_T, class Obj_T >
uint BTreeSearchNode<Key_T,Obj_T>::
privSearch( const Key_T & key, uint lower, uint upper )
//-----------------------------------------------------------------
// using a binary search, return the first element greater than key
{
    int middle;

    while( lower != upper ) {
        middle = (lower + upper) / 2;
        if( key.operator<( _nodes[ middle ]._separator ) ) {
            upper = middle;
        } else {
            lower = middle + 1;
        }
    }

    return lower;
}

template < class Key_T, class Obj_T >
Obj_T * BTreeSearchNode<Key_T,Obj_T>::
find( const Key_T & key )
//------------------------------------
{
    int idx;

    idx = privSearch( key, 0, _degree - 1 );
    return _nodes[ idx ]._child->find( key );
}

template < class Key_T, class Obj_T >
void BTreeSearchNode<Key_T,Obj_T>::
remove( const Key_T & key )
//-----------------------------------
// remove an element by finding the appropriate child and
// calling remove for them.
{
    int idx;

    idx = privSearch( key, 0, _degree - 1 );
    _nodes[ idx ]._child->remove( key );
}

template < class Key_T, class Obj_T >
Obj_T * BTreeSearchNode<Key_T,Obj_T>::
insert( Obj_T * obj, bool & needsSplit, Key_T & key,
        BTreeNodeBase *& newNode  )
//--------------------------------------------------------
// insert an element by finding the appropriate child (which may
// be another SearchNode, Bucket, or Leaf), and calling insert for them.
// If they split, they will set splitOccured to TRUE, and key and newNode
// will be set to the parent and right subtree respectively.  If we
// need to split while processing a child's split, set our caller's
// needsSplit, key, and newNode appropriately.
{
    int             idx;
    bool            splitOccurred;
    Obj_T *         res = NULL;

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "inserting in searchnode -- %s\n",
                                    ((const Key_T &)(*obj)).getString() );
    #endif

    needsSplit = FALSE;
    idx = privSearch( (const Key_T&)(*obj), 0, _degree - 1 );
    res = _nodes[ idx ]._child->insert( obj, splitOccurred, key, newNode );

    if( splitOccurred ) {    // our child split while inserting
        if( _degree == 2 * _keyOrder + 1 ) {    // we need to split
            split( key, newNode );
            needsSplit = TRUE;
            return res;
        } else {                                // we can insert without split
            privInsert( key, newNode );
            return res;
        }
    } else {
        return res;
    }
}

template < class Key_T, class Obj_T >
void BTreeSearchNode<Key_T,Obj_T>::
privInsert( const Key_T & key, BTreeNodeBase *& newNode )
//-------------------------------------------------------
// insert a new key / node to this.  This assumes that there
// is room (ie _degree < _keyOrder * 2), and doesn't check!
{
    int i;
    int j;

    for( i = 0; i < _degree - 1; i += 1 ) {
        if( key.operator< ( _nodes[ i ]._separator ) ) break;
    }

    if( i < _degree - 1 ) {
        _nodes[ _degree ]._child = _nodes[ _degree - 1 ]._child;
        for( j = _degree - 1; j > i; j -= 1 ) {
            _nodes[ j ]._separator.operator= ( _nodes[ j - 1 ]._separator );
            _nodes[ j ]._child = _nodes[ j - 1 ]._child;
        }
    }
    _nodes[ i ]._separator.operator= ( key );
    _nodes[ i + 1 ]._child = newNode;

    _degree += 1;
}

template < class Key_T, class Obj_T >
void BTreeSearchNode<Key_T,Obj_T>::
split( Key_T & key, BTreeNodeBase *& newNode )
//--------------------------------------------
// split this node into two, each with _keyOrder + 1 children.
// key and newNode are input and output parameters
{
    BTreeSearchNode *   other;
    BTreeNodeBase **    children;
    Key_T *             seps;
    int                 i;

    typedef BTreeNodeBase * BTreeNodeBasePtr;

    children = new BTreeNodeBasePtr [ _keyOrder * 2 + 2 ];
    seps = new Key_T [ _keyOrder * 2 + 1 ];

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "splitting searchnode -- key %s, newNode is\n", key.getString() );
    newNode->print( 0 );
    #endif

    InternalAssert( _degree == _keyOrder * 2 + 1 );

    for( i = 0; i < _degree-1 && _nodes[i]._separator.operator<(key); i += 1 ) {
        children[ i ] = _nodes[ i ]._child;
        seps[ i ].operator= ( _nodes[ i ]._separator );
    }

    seps[ i ].operator= ( key );
    children[ i ] = _nodes[ i ]._child;
    children[ i + 1 ] = newNode;

    for( ; i < _degree - 1; i += 1 ) {
        seps[ i + 1 ].operator= ( _nodes[ i ]._separator );
        children[ i + 2 ] = _nodes[ i + 1 ]._child;
    }

    other = new BTreeSearchNode();
    other->_degree = _keyOrder + 1;
    _degree = _keyOrder + 1;

    for( i = 0; i < _keyOrder; i += 1 ) {
        _nodes[ i ]._separator.operator=( seps[ i ] );
    }

    for( i = 0; i < _keyOrder + 1; i += 1 ) {
        _nodes[ i ]._child = children[ i ];
    }

    for( i = 0; i < _keyOrder; i += 1 ) {
        other->_nodes[ i ]._separator.operator= ( seps[ i + _keyOrder + 1 ] );
    }
    for( i = 0; i < _keyOrder + 1; i += 1 ) {
        other->_nodes[ i ]._child = children[ i + _keyOrder + 1 ];
    }

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "splitting searchnode -- about to assign key %s\n", seps[ _keyOrder ].getString() );
    #endif

    newNode = other;
    key.operator= ( seps[ _keyOrder ] );

    delete [] seps;
    delete [] children;
}


template < class Key_T, class Obj_T >
BTreeNodeBase< Key_T, Obj_T > * BTreeSearchNode<Key_T,Obj_T>::
nextNode( int_16 & idx)
//---------------------------------------------------------------
{
    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "SearchNode %p, idx = %d, _degree = %d, return = %p\n",
                                    this, idx + 1, _degree, (idx + 1 < _degree) ? _nodes[ idx + 1 ]._child : NULL );
    #endif

    idx += 1;
    if( idx >= _degree ) return NULL;

    return _nodes[ idx ]._child;
}

template < class Key_T, class Obj_T >
Obj_T * BTreeSearchNode<Key_T,Obj_T>::
nextObj( int_16 & )
//------------------------------------
{
    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "SearchNode nextObj == NULL\n" );
    #endif

    return NULL;    // no objects in search node
}


/*--------------------- BTreeBucketNode -----------------------------*/

template < class Key_T, class Obj_T >
static uint BTreeBucketNode<Key_T,Obj_T>::
_objOrder( -1 );

const int BTreeBucketNodePoolSize = 1024;
template < class Key_T, class Obj_T >
static MemoryPool BTreeBucketNode<Key_T,Obj_T>::
_pool( sizeof( BTreeBucketNode ), "BTreeBucketNode", BTreeBucketNodePoolSize );

const int BTreeBucketNodeNodePoolSize = 1024;
template < class Key_T, class Obj_T >
static MemoryPool BTreeBucketNode<Key_T,Obj_T>::
_nodePool( "BTreeBucketNodeNode" );

template < class Key_T, class Obj_T >
BTreeBucketNode<Key_T,Obj_T>::
BTreeBucketNode()
    : _degree(0)
//---------------------------------------------------------------
{
    _nodes = (ObjPtr *) _nodePool.alloc();
}

template < class Key_T, class Obj_T >
BTreeBucketNode<Key_T,Obj_T>::
BTreeBucketNode( Obj_T * obj )
    : _degree( 1 )
//---------------------------------------------------------------
{
    _nodes = (ObjPtr *) _nodePool.alloc();
    _nodes[ 0 ] = obj;
}

template < class Key_T, class Obj_T >
BTreeBucketNode<Key_T,Obj_T>::
~BTreeBucketNode()
{
    // WARNING -- this destructor is not guaranteed to be called
    //            since the elements are pooled and freed using
    //            the pool's ragnarok
}

template < class Key_T, class Obj_T >
static void BTreeBucketNode<Key_T,Obj_T>::
ragnarok()
//----------------------------------------
{
    _pool.ragnarok();
    _nodePool.ragnarok();
}

template < class Key_T, class Obj_T >
static void BTreeBucketNode<Key_T,Obj_T>::
setObjOrder( uint objOrder )
//-------------------------------------------
{
    _objOrder = objOrder;
    _nodePool.setSize( sizeof( ObjPtr ) * 2 * objOrder + sizeof( ObjPtr ),
                       BTreeBucketNodeNodePoolSize );
}

#if INSTRUMENTS
template < class Key_T, class Obj_T >
void BTreeBucketNode<Key_T,Obj_T>::
print( int indent )
//----------------------------------
{
    int i;

    if( _degree == 0 ) {
        Log.printf( "%*c<empty bucket>\n", indent, ' ' );
    }

    for( i = 0; i < _degree; i += 1 ) {
        Log.printf( "%*c%s\n", indent, ' ',
                                        (const char *)(*_nodes[ i ]) );
    }
}
#endif


template < class Key_T, class Obj_T >
uint BTreeBucketNode<Key_T,Obj_T>::
privSearch( const Key_T & key, uint lower, uint upper )
//-----------------------------------------------------------------
// using a binary search, return the first element greater than key
{
    int middle;

    while( lower != upper ) {
        middle = (lower + upper) / 2;

        if( key.operator== ( (const Key_T&)(*_nodes[ middle ])) ) {
            return middle;
        } else {
            if( key.operator< ( (const Key_T&)(*_nodes[ middle ])) ) {
                upper = middle;
            } else {
                lower = middle + 1;
            }
        }
    }

    return lower;
}


template < class Key_T, class Obj_T >
Obj_T * BTreeBucketNode<Key_T,Obj_T>::
find( const Key_T & key )
//------------------------------------
{
    uint    idx;

    idx = privSearch( key, 0, _degree );

    if( idx < _degree && key.operator== ( (const Key_T&)(*_nodes[idx]) )) {
        return _nodes[ idx ];
    } else {
        return NULL;    // not found
    }
}

template < class Key_T, class Obj_T >
void BTreeBucketNode<Key_T,Obj_T>::
remove( const Key_T & key )
//-----------------------------------
// remove an element from the bucket
{
    int idx;

    idx = privSearch( key, 0, _degree );

    if( idx < _degree &&
         key.operator== ( (const Key_T&)(*_nodes[idx]) ) ) {

        for( int j = idx; j < _degree; j += 1 ) {
            _nodes[ j ] = _nodes[ j + 1 ];
        }

        _degree -= 1;
    } else {
        #if INSTRUMENTS
        Log.printf( "NOT FOUND FOR REMOVE!\n", key.getString() );
        print( 0 );
        #endif

        BTreeExcept a( "Element Not Found for Remove",
                        BTreeExcept::NotFoundForRemove );
        throw( a );
    }
}

template < class Key_T, class Obj_T >
Obj_T * BTreeBucketNode<Key_T,Obj_T>::
insert( Obj_T * obj, bool & needsSplit, Key_T & key,
        BTreeNodeBase *& newNode )
//--------------------------------------------------
// attempt to insert object 'obj' into this bucket.  If the
// bucket is full, split it and return the new key and node
// in 'key' and 'newNode' respectively
{
    if( _degree == 2 * _objOrder + 1 ) {
        needsSplit = TRUE;              // we split
        return split( key, obj, newNode );
    } else {
        needsSplit = FALSE;             // we did not split
        return privInsert( obj );
    }
}

template < class Key_T, class Obj_T >
Obj_T * BTreeBucketNode<Key_T,Obj_T>::
privInsert( Obj_T * obj )
//------------------------------------
// insert the element -- there must be room for it!
{
    int idx;

    InternalAssert( _degree < _objOrder * 2 + 1 );

    idx = privSearch( (const Key_T&)(*obj), 0, _degree );

    if( idx < _degree &&
         ((const Key_T&)(*obj)).operator== ( (const Key_T&)(*_nodes[idx]) ) ) {

        if( obj == _nodes[ idx ] ) {
            return obj;
        }

        #if INSTRUMENTS
        Log.printf( "DUPLICATE! %s = \n", (const char *) (*obj) );
        Log.printf( "           %s\n", (const char *) (*_nodes[ idx ]) );
        print( 0 );
        #endif

        BTreeExcept a( "Duplicate", BTreeExcept::Duplicate );
        throw( a );
    }

    if( idx < _degree ) {
        for( int j = _degree; j > idx; j -= 1 ) {
            _nodes[ j ] = _nodes[ j - 1 ];
        }
    }
    _nodes[ idx ] = obj;

    _degree += 1;

    return _nodes[ idx ];
}

template < class Key_T, class Obj_T >
Obj_T * BTreeBucketNode<Key_T,Obj_T>::
split( Key_T & key, Obj_T * obj, BTreeNodeBase *& newNode )
//---------------------------------------------------------
{
    BTreeBucketNode *   right;
    Obj_T *             ret;

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "splitting bucketnode -- obj %s\n", (const char *)(*obj) );
    #endif

    right = new BTreeBucketNode();

    try {
        if( ((const Key_T &)(*obj)).operator< ( (const Key_T &)(*_nodes[ _objOrder ]) ) ) {

            for( int i = 0; i < _objOrder + 1; i += 1 ) {
                right->_nodes[ i ] = _nodes[ _objOrder + i ];
            }

            right->_degree = _objOrder + 1;
            _degree = _objOrder;
            ret = privInsert( obj );
        } else {
            if( ((const Key_T &)(*obj)).operator== (
                                (const Key_T &)(*_nodes[ _objOrder ]) ) ) {

                #if INSTRUMENTS
                Log.printf( "DUPLICATE! %s =\n", (const char *) (*obj) );
                Log.printf( "           %s\n", (const char *) (*_nodes[ _objOrder ] ) );
                print( 0 );
                #endif

                BTreeExcept a( "Duplicate", BTreeExcept::Duplicate );
                throw( a );
            } else {
                for( int i = 0; i < _objOrder; i += 1 ) {
                    right->_nodes[ i ] = _nodes[ _objOrder + i + 1 ];
                }
                right->_degree = _objOrder;
                _degree = _objOrder + 1;
                ret = right->privInsert( obj );
            }
        }
    } catch( BTreeExcept( oops ) ) {
        // privInsert may throw a duplicate (or other) exception -- try
        // to clean up as best as possible

        delete right;
        right = NULL;
        newNode = NULL;
        _degree = _objOrder * 2 + 1;
        throw;  // re-throw oops
    }

    if( right ) {
        key.operator= ( (const Key_T &) (*right->_nodes[ 0 ]) );
    }
    newNode = right;

    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "split bucketnode -- key, %s, oldnode, newnode\n", key.getString() );
    print(0);
    newNode->print(0);
    #endif

    return ret;
}

template < class Key_T, class Obj_T >
BTreeNodeBase< Key_T, Obj_T > *
BTreeBucketNode<Key_T,Obj_T>::
nextNode( int_16 & )
//-----------------------------------
{
    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "BucketNode nextNode == NULL\n" );
    #endif

    return NULL;    // no nodes in buckets
}

template < class Key_T, class Obj_T >
Obj_T * BTreeBucketNode<Key_T,Obj_T>::
nextObj( int_16 & idx)
//------------------------------------
{
    #if ( INSTRUMENTS == INSTRUMENTS_FULL_LOGGING )
    Log.printf( "BucketNode %p, idx = %d, _degree = %d, return = %p\n",
                                    this, idx + 1, _degree, (idx +1 < _degree) ? _nodes[ idx + 1 ] : NULL );
    #endif

    idx += 1;
    if( idx >= _degree ) return NULL;

    return _nodes[ idx ];
}
