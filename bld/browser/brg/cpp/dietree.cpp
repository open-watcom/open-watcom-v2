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


#include <stdio.h>
#include <string.h>

#include "assure.h"
#include "brmerge.h"
#include "dietree.h"
#include "mem.h"
#include "mrdie.h"          // comment this out for DEBUG_DIETREE

const int   SearchPoolSize = 16;   // 16 elements per hunk
const int   BucketPoolSize = 16;   // 16 elements per hunk
const MergeNameKey MidKey;          // initial separator

typedef DIETreeSearch * DIETreeSearchP;
const int StackGuess = 512;
static uint             StackSize = StackGuess;
static DIETreeSearchP * Stack = NULL;


DIETree::DIETree()
        : _searchPool( sizeof( DIETreeSearch ), "DIETree::_searchPool",
                        SearchPoolSize )
        , _bucketPool( sizeof( DIETreeBucket ), "DIETree::_bucketPool",
                        BucketPoolSize )
//---------------------------------------------------------------------
{
    DIETreeBucket * rhs;

    ASSERTION( Stack == NULL );     // ensure only one copy running

    Stack = (DIETreeSearchP *) WBRAlloc( StackSize * sizeof(DIETreeSearchP) );

    _root = allocSearch();
    rhs = allocBucket( NULL );
    _first = allocBucket( rhs );

    _root->_degree = 2;
    _root->_separators[ 0 ] = MidKey;
    _root->_nodes[ 0 ] = _first;
    _root->_nodes[ 1 ] = rhs;

    setToStart();
}

DIETree::~DIETree()
//-----------------
{
    _searchPool.ragnarok();
    _bucketPool.ragnarok();
    WBRFree( Stack );
    Stack = NULL;       // for following executions
}

DIETreeBucket * DIETree::allocBucket( DIETreeBucket * next )
//----------------------------------------------------------
{
    DIETreeBucket * ret;

    ret = (DIETreeBucket *) _bucketPool.alloc();
    new( ret ) DIETreeBucket( next );
    return ret;
}

DIETreeSearch * DIETree::allocSearch( void )
//------------------------------------------
{
    DIETreeSearch * ret;

    ret = (DIETreeSearch *) _searchPool.alloc();
    new( ret ) DIETreeSearch;
    return ret;
}

MergeDIE * DIETree::find( MergeNameKey & name )
//---------------------------------------------
// find first die matching name; return NULL if not found
// update _current so next() returns subsequent elements
{
    DIETreeSearch * search;
    uint            middle;
    uint            lower;
    uint            upper;

    search = _root;
    while( !search->_leaf ) {
        lower = 0;
        upper = search->_degree - 1;
        while( lower != upper ) {
            middle = (lower + upper) / 2;
            if( name < search->_separators[ middle ] ) {
                upper = middle;
            } else {
                lower = middle + 1;
            }
        }

        // now lower == upper == first separator > name
        search = (DIETreeSearch *) search->_nodes[ lower ];
    }

    _current = (DIETreeBucket *) search;
    lower = 0;
    upper = _current->_degree;
    while( lower != upper ) {
        middle = (lower + upper) / 2;

        if( name == _current->_nodes[ middle ]->name() ) {
            // we've got our match
            _index = middle;
            return _current->_nodes[ middle ];
        }

        if( name < _current->_nodes[ middle ]->name() ) {
            upper = middle;
        } else {
            lower = middle + 1;
        }
    }

    _current = _first;
    _index = -1;
    return NULL;
}

void DIETree::insert( MergeDIE * die )
//------------------------------------
// insert die into the tree, not allowing duplicates.
{
    // store visited search nodes
    MergeDIE *      nodes[ BUCKETORDER * 2 + 2 ];
    uint            depth = 0;  // stack depth;
    DIETreeSearch * search;     // search node
    DIETreeBucket * lhn;        // bucket to add node to
    DIETreeBucket * rhn;        // new bucket for split
    DIETreeNode *   add;        // node to add to search node
    MergeNameKey    key;
    uint            middle;
    uint            lower;
    uint            upper;
    int             i;
    bool            split = FALSE;  // split needed ?


    search = _root;
    while( !search->_leaf ) {
        Stack[ depth++ ] = search;
        if( depth >= StackSize ) {
            StackSize += StackGuess;
            Stack = (DIETreeSearchP *) WBRRealloc( Stack,
                                    StackSize * sizeof(DIETreeSearchP) );
        }
        lower = 0;
        upper = search->_degree - 1;
        while( lower != upper ) {
            middle = (lower + upper) / 2;
            if( die->name() < search->_separators[ middle ] ) {
                upper = middle;
            } else {
                lower = middle + 1;
            }
        }

        // now lower == upper == first separator > name
        search = (DIETreeSearch *) search->_nodes[ lower ];
    }

    lhn = (DIETreeBucket *) search;
    lower = 0;
    upper = lhn->_degree;
    while( lower != upper ) {
        middle = (lower + upper) / 2;

        if( die->name() == lhn->_nodes[ middle ]->name() ) {
            // we've got a match
            #if INSTRUMENTS
                Log.printf( "!DUPLICATE! -- %s already in table\n",
                            lhn->_nodes[ middle ]->name().getString() );
                print( NULL, 0 );
            #endif
            InternalAssert( 0 /* duplicate */ );
            break;
        }

        if( die->name() < lhn->_nodes[ middle ]->name() ) {
            upper = middle;
        } else {
            lower = middle + 1;
        }
    }

    for( i = 0; i < lower; i += 1 ) {
        nodes[ i ] = lhn->_nodes[ i ];
    }
    nodes[ i ] = die;
    for( ; i < lhn->_degree; i += 1 ) {
        nodes[ i + 1 ] = lhn->_nodes[ i ];
    }

    // now nodes contains all the MergeDIE pointers, but a split may
    // be necessary

    if( lhn->_degree < (BUCKETORDER * 2 + 1) ) {
        lhn->_degree++;
        memcpy( lhn->_nodes, nodes, sizeof( lhn->_nodes ) );
        return;
    } else {
        rhn = allocBucket( lhn->_next );
        lhn->_next = rhn;
        lhn->_degree = BUCKETORDER + 1;
        rhn->_degree = BUCKETORDER + 1;

        memcpy( lhn->_nodes, nodes,
                sizeof( lhn->_nodes[ 0 ] ) * (BUCKETORDER + 1) );

        memcpy( rhn->_nodes, nodes + BUCKETORDER + 1,
                sizeof( lhn->_nodes[ 0 ] ) * (BUCKETORDER + 1) );

        split = TRUE;
        key = rhn->_nodes[ 0 ]->name();
        add = rhn;
    }

    while( split ) {
        depth--;
        split = doSplit( Stack[ depth ], add, key );
        if( depth == 0 ) {
            if( split ) {
                DIETreeSearch * nRoot = allocSearch();
                nRoot->_degree = 2;
                nRoot->_nodes[ 0 ] = _root;
                nRoot->_nodes[ 1 ] = add;
                nRoot->_separators[ 0 ] = key;
                _root = nRoot;
            }
            break;
        }
    }
}

bool DIETree::doSplit( DIETreeSearch * search, DIETreeNode *& add,
                        MergeNameKey & key )
//----------------------------------------------------------------
{
    MergeNameKey    seps[ SEARCHORDER * 2 + 1 ];
    DIETreeNode *   nodes[ SEARCHORDER * 2 + 2 ];
    DIETreeSearch * rhn;
    int             i;
    bool            split;

    for( i = 0; i < search->_degree - 1
                && (search->_separators[ i ] < key); i += 1 ) {

        nodes[ i ] = search->_nodes[ i ];
        seps[ i ] = search->_separators[ i ];
    }

    seps[ i ] = key;
    nodes[ i ] = search->_nodes[ i ];
    nodes[ i + 1 ] = add;

    for( ; i < search->_degree - 1; i += 1 ) {
        seps[ i + 1 ] = search->_separators[ i ];
        nodes[ i + 2 ] = search->_nodes[ i + 1 ];
    }

    if( search->_degree < (2 * SEARCHORDER + 1) ) {
        split = FALSE;  // don't need to split this node
        search->_degree++;
        memcpy( search->_separators, seps, sizeof( search->_separators ) );
        memcpy( search->_nodes, nodes, sizeof( search->_nodes ) );
    } else {
        split = TRUE;   // split propagates
        rhn = allocSearch();
        add = rhn;
        search->_degree = SEARCHORDER + 1;
        rhn->_degree = SEARCHORDER + 1;

        memcpy( search->_separators, seps,
                sizeof( seps[ 0 ] ) * SEARCHORDER );
        memcpy( search->_nodes, nodes,
                sizeof( nodes[ 0 ] ) * (SEARCHORDER + 1) );

        memcpy( rhn->_separators, seps + SEARCHORDER + 1,
                sizeof( seps[ 0 ] ) * SEARCHORDER );
        memcpy( rhn->_nodes, nodes + SEARCHORDER + 1,
                sizeof( nodes[ 0 ] ) * (SEARCHORDER + 1) );
    }

    key = seps[ SEARCHORDER ];

    return split;
}

void DIETree::setToStart( void )
//------------------------------
// set _current to be before the first element
// so that next() will return the first element in the list.
{
    _current = _first;
    _index = -1;
}

MergeDIE * DIETree::next( void )
//------------------------------
// advance the _current pointer and return
// the next die
{
    while( _current ) {
        if( (_index + 1) < _current->_degree ) {
            _index++;
            return _current->_nodes[ _index ];
        } else {
            _current = _current->_next;
            _index = -1;
        }
    }

    return NULL;
}

void DIETree::freeDirectory( void )
//---------------------------------
// release all memory for search nodes
{
    _root = NULL;
    _searchPool.ragnarok();
}

#if INSTRUMENTS

void DIETree::print( DIETreeNode * node, uint indent )
//----------------------------------------------------
{
    DIETreeSearch * search;
    DIETreeBucket * bucket;
    int             i;

    node = node ? node : _root;

    if( node->_leaf ) {
        bucket = (DIETreeBucket *) node;
        if( bucket->_degree == 0 ) {
            Log.printf( "%*c<empty>\n", indent, ' ' );
        } else {
            for( i = 0; i < bucket->_degree; i += 1 ) {
                Log.printf( "%*c%s\n", indent, ' ',
                            bucket->_nodes[ i ]->name().getString() );
            }
        }
    } else {
        search = (DIETreeSearch *) node;
        print( search->_nodes[ 0 ], indent + 8 );
        for( i = 0; i < (search->_degree - 1); i += 1 ) {
            Log.printf( "%*c%s\n", indent, ' ',
                        search->_separators[ i ].getString() );
            print( search->_nodes[ i + 1 ], indent + 8 );
        }
    }
}
#endif

#if defined( DEBUG_DIETREE )

void main() {
    char        buf[ 512 ];
    DIETree     tree;
    MergeDIE *  die;

    printf( "Ready\n" );

    while( fgets( buf, 512, stdin ) ) {
        buf[ strlen( buf ) - 1 ] = '\0';
        die = tree.find( buf );
        printf( "%s -- %s:%s\n", buf, (die) ? "found" : "not found",
                (die) ? die->name() : "" );

        while( (die = tree.next()) ) {
            printf( "    next = %s\n", die->name() );
        }

        printf( "Inserting...\n" );
        tree.insert( new MergeDIE( buf ) );

        printf( "Whole Tree:\n" );
        tree.setToStart();
        while( (die = tree.next()) ) {
            printf( "    next = %s\n", die->name() );
        }

        tree.print( NULL, 0 );

        printf( "Ready\n" );
    }
}
#endif

