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


#ifndef __DIETREE_H__
#define __DIETREE_H__

#include "mrnmkey.h"

class MergeDIE;

#define SEARCHORDER  11
#define BUCKETORDER  11

struct DIETreeNode {
            DIETreeNode( bool leaf )
                    : _leaf( leaf ), _degree( 0 ) {}

    bool    _leaf    : 1;               // true if this is a bucket node
    uint    _degree  : 15;              // how many elements in this node
};

struct DIETreeSearch : public DIETreeNode {
                    DIETreeSearch() : DIETreeNode( FALSE ) {}

    MergeNameKey    _separators[ SEARCHORDER * 2 ];
    DIETreeNode *   _nodes[ SEARCHORDER * 2 + 1 ];
};

struct DIETreeBucket : public DIETreeNode {
                    DIETreeBucket() : DIETreeNode( TRUE ), _next( NULL ) {}
                    DIETreeBucket( DIETreeBucket * n )
                        : DIETreeNode( TRUE ), _next( n ) {}

    MergeDIE *      _nodes[ BUCKETORDER * 2 + 1 ];
    DIETreeBucket * _next;
};

class DIETree {
public:
                    DIETree();
                    ~DIETree();

    MergeDIE *      find( MergeNameKey & );     // find die by name
    void            insert( MergeDIE * die );   // insert

    void            setToStart( void );         // move pointer to first
    MergeDIE *      next( void );               // die after last find / ins

    void            freeDirectory( void );      // release search nodes

    DIETreeBucket * allocBucket( DIETreeBucket * next );
    DIETreeSearch * allocSearch( void );

    #if INSTRUMENTS
    void            print( DIETreeNode * node, uint indent );
    #endif

private:

    bool            doSplit( DIETreeSearch * search,
                             DIETreeNode *& add, MergeNameKey & key );

    DIETreeBucket *     _first;     // first bucket in linked-list
    DIETreeSearch *     _root;      // root of directory

    DIETreeBucket *     _current;   // current bucket
    int                 _index;     // index within current bucket

    MemoryPool          _searchPool;    // pool for search nodes
    MemoryPool          _bucketPool;    // pool for bucket nodes
};

#endif // __DIETREE_H__
