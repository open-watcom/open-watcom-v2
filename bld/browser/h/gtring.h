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


#ifndef __GTRING_H__
#define __GTRING_H__

#include <wrect.hpp>
#include "gtbase.h"
#include "mempool.h"

class TreeRing : public TreeNode
{
public:
            TreeRing( TreeWindow * parent, TreePtr * ptr );
            ~TreeRing();
    void *              operator new( size_t );
    void                operator delete( void * );

            bool        addPtr( TreePtr * ptr );
            void        unWrap( void );
    virtual void        addEdges( TreeEdgeBreaker * );
            void        cut() { _cut = TRUE; }
            void        join() { _cut = FALSE; }
            bool        isCut() { return _cut; }

    virtual int         getLevel( void ) const;
    virtual uint_32     getHandle( void ) const { return 0; }
    virtual TreePtr*    getPtr( TreeList_T, int );
    virtual TreeNode*   getNode( TreeList_T, int );
    virtual int         getCount( TreeList_T );
    virtual TreeNode*   removeNode( TreeList_T, TreeNode * );
    virtual void        addPtr( TreeList_T list, TreePtr * );
    virtual void        swapNode( TreeList_T, TreeNode *, TreeNode * ){};
    virtual void        sortPrtKids();

    virtual char *      name() { return NULL; }
    virtual bool        isRelated( TreeNode * );
    virtual void        deleteTree( void ){}
    virtual Symbol *    makeSymbol( void ) {return NULL; }

    virtual void        getFromCoord( TreePtr * ptr, TreeCoord& x, TreeCoord& y );
    virtual void        getToCoord( TreePtr * ptr, TreeCoord& x, TreeCoord& y );

    virtual void        getMinCoord( TreeCoord & x, TreeCoord & y );
    virtual void        getMinSibCoord( TreeCoord & x, TreeCoord & y );
    virtual void        getMaxCoord( TreeCoord & x, TreeCoord & y );

    virtual void        setBounding( OutputDevice * );
            void        setChildWard( void );

            bool        isChild( TreeNode * );
            bool        isParent( TreeNode * );
            void        setEnableStatus( void );
    static  void        joinRings( TreeRingList & );
    virtual void        paint( OutputDevice *dev, TreeRect * r );
    virtual PaintInfo * getPaintInfo( void );
            bool        childrenEnabled( void );
            bool        parentsEnabled( void );

protected:
    virtual TreeCoord   getSibContrib( TreeNode *, bool& );

private:
            bool            canJoin( TreeNode * parnt, TreeNode * child );

                            // true if other ring can be joined to this one
            bool            sameRing( TreeRing * other );
            void            joinTo( TreeRing * other );

            TreePtrList     _parents;
            TreePtrList     _children;

            TreeLine        _lineTop;
            TreeLine        _lineBottom;
            TreeLine        _lineConnector;

            bool            _hasLineTop;
            bool            _hasLineBottom;
            bool            _hasLineConnector;

            int             _level;

            #if 0   // ITB -- think this should come out
            WRect           _bounding;
            #endif

            bool            _sorted;
            bool            _cut;
    static MemoryPool       _pool;
};


#endif // __GTRING_H__
