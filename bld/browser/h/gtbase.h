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


#ifndef __GTBASE_H__
#define __GTBASE_H__

// disable "integral value may be truncated" warning
#pragma warning 389 9

#include <wtptlist.hpp>
#include "gtcoord.h"
#include "gtedges.h"
#include "outdev.h"
#include "optmgr.h"

class Symbol;

class TreeRing;
class TreeWindow;
class TreeNode;
class TreePtr;
class TreeRoot;

typedef TemplateList<TreePtr *>         TreePtrList;
typedef TemplateList<TreeNode *>        TreeNodeList;
typedef TemplateList<TreeRoot *>        TreeRootList;
typedef TemplateList<TreeRing *>        TreeRingList;

inline TreeCoord maxCoord( TreeCoord a, TreeCoord b ) { return (a > b) ? a : b; }
inline TreeCoord minCoord( TreeCoord a, TreeCoord b ) { return (a < b) ? a : b; }

/*--------------------------- TreePtr --------------------------------*/

class TreePtr
{
public:
            TreePtr(){}
            TreePtr( TreeNode * from, TreeNode * to );
    virtual ~TreePtr(){}

            void        paint( OutputDevice *, TreeRect * );
            void        addEdges( TreeEdgeBreaker * );

            void        setTo( TreeNode * to ) { _toNode = to; }
            void        setFrom( TreeNode * from ) { _fromNode = from; }
            void        cut() { _cut = TRUE; }
            void        join() { _cut = FALSE; }
            bool        isCut() { return _cut; }

            TreeNode *  getTo( void ) const { return _toNode; }
            TreeNode *  getFrom( void ) const { return _fromNode; }

    virtual PaintInfo * getPaintInfo( void ) = 0;
    virtual TreePtr *   makeClone( void ) = 0;
    static  int         comparePtrs( const TreePtr * lhs, const TreePtr * rhs );
private:
            TreeLine    _line;
            TreeNode *  _fromNode;
            TreeNode *  _toNode;
            bool        _cut : 1;
};

/*----------------------------- TreeNode -------------------------------*/

class TreeNode
{
public:
            TreeNode( TreeWindow * prt );
            TreeNode( TreeNode & node );
    virtual ~TreeNode() {}

    /*------------- pure virtual functions ----------------------------*/

    virtual int         getLevel( void ) const = 0;
    virtual dr_handle   getHandle( void ) const = 0;

    enum    TreeList_T { ParentList, ChildList, FlatList };

    virtual TreePtr*    getPtr( TreeList_T list, int index ) = 0;
    virtual TreeNode*   getNode( TreeList_T list, int index ) = 0;
    virtual int         getCount( TreeList_T list ) = 0;
    virtual TreeNode *  removeNode( TreeList_T list, TreeNode * node ) = 0;
    virtual void        addPtr( TreeList_T list, TreePtr * ) = 0;
    virtual void        swapNode( TreeList_T list, TreeNode * from, TreeNode * to ) = 0;
    virtual void        sortPrtKids() = 0;

    virtual char *      name() = 0;
    virtual bool        isRelated( TreeNode * ) = 0;
    virtual void        deleteTree( void ) = 0;
    virtual Symbol *    makeSymbol( void ) = 0;

    /*---------------- end of pure virtual functions ---------------------*/

#ifdef DEBUG
    virtual const char *className() const { return "TreeNode"; }
#endif

    virtual bool        isReference( void ) const { return FALSE; }
    virtual bool        hasReference( void ) const { return FALSE; }

    virtual bool        gettingFocus( WWindow * );
    virtual bool        losingFocus( WWindow * );

    virtual void        getFromCoord( TreePtr * ptr, TreeCoord & x, TreeCoord & y );
    virtual void        getToCoord( TreePtr * ptr, TreeCoord & x, TreeCoord & y );

    virtual void        getMinCoord( TreeCoord & x, TreeCoord & y );
    virtual void        getMinSibCoord( TreeCoord & x, TreeCoord & y );
    virtual void        getMaxCoord( TreeCoord & x, TreeCoord & y );

    virtual void        setBounding( OutputDevice *dev );
    virtual TreeCoord   getSibContrib( TreeNode *, bool& );

    virtual PaintInfo * getPaintInfo( void ){ return NULL; }
    virtual void        paint( OutputDevice *, TreeRect *r );
    virtual void        addEdges( TreeEdgeBreaker * );

            enum EnableLevel { Hidden = 0x0, Visible = 0x01 };
            enum PlaceLevel { NotPlaced = 0x00, PartiallyPlaced = 0x01,
                              Arranging = 0x02, Placed = 0x03 };

            void            myRect( TreeRect & r ) const { r = _bounding; }
            void            myDescend( TreeRect & r ) const { r = _descend; }
            unsigned long   getSibWidth( void ) const { return _sibWidth; }
            TreeCoord       shrinkToSib( void );
            void            getMinMax( TreeCoord&, TreeCoord& );

            int             getEnable( void ) const { return _flags.enabled; }
            void            setEnable( uint lvl ) { _flags.enabled = lvl; }
            void            enableKids( bool enable = TRUE );
            void            enableParents( bool enable = TRUE );
            void            enableTree( uint lvl );
            void            rePlace( void );

            void            move( TreeCoord x, TreeCoord y );
            void            place( TreeCoord& sibOff );
            void            arrangeTree( TreeCoord& sibOff );
            bool            isSimple( TreeList_T );
            TreeDirection   getDirection( void ) const;
            TreeWindow *    getParent( void ) const { return _parent; }

            void            paintTree( OutputDevice *, TreeRect *r );
            TreeNode*       hitTest( TreeCoord x, TreeCoord y );
            bool            isVisible( void );

            #ifdef DEBUGTREE
            void        debugInfo( TreeRoot * );
            void        sibWidth();
            #endif

    static  void            arrangeAll( TreeDirection dir, TreeRootList& roots,
                                        TreeRect& world, OutputDevice *dev );
    static  int             compareNodes( const TreeNode * lhs,
                                          const TreeNode * rhs );
            void            setRootPtr( TreeRoot * pRoot );
            TreeRoot *      getRootPtr( void );

protected:
            bool            readyToPlace( void );
            uint            getNumEnabled( TreeList_T );

            TreeWindow*     _parent;
            TreeRect        _bounding;

            #if DEBUG
            char*           _nm;
            #endif

private:
            void            getFirstNonContend( TreeCoord& sibOff, TreeCoord& width );
            bool            resolveChildWard( TreeRect & r );

            TreeRoot *      _pRoot;
            TreeRect        _descend;
            TreeCoord       _sibWidth;

            struct TreeNodeFlags {
                TreeNodeFlags( PlaceLevel, EnableLevel, bool, bool, bool );

                uint    placed   : 2;
                uint    enabled  : 1;
                uint    selected : 1;
                uint    boundSet : 1;
                uint    arranging: 1;
            } _flags;

            /*
             * The four sides of the node
             */
            TreeLine        _lineTop;
            TreeLine        _lineLeft;
            TreeLine        _lineRight;
            TreeLine        _lineBottom;
};

/*--------------------------- TreeRoot -------------------------------*/

class TreeRoot
{
public:
            TreeRoot( TreeWindow * prt, TreeNode * node );
            ~TreeRoot();

            void        getBound( TreeRect & r ) const { r = _rootBound; }
            TreeRect    getBound( void ) const { return _rootBound; }
            void        setBound( TreeRect & r ) { _rootBound = r; }
            void        setAllBounding( OutputDevice * );
            void        sortEdges( void );

            TreeNode *  node( void ) const { return _rootNode; }
            TreeNode *  setNode( void );
            bool        isRelated( TreeNode * node ) { return _rootNode->isRelated( node ); }

            void        deleteTree( void ) { _rootNode->deleteTree(); }

            bool        enabled( void ) const { return _enabled; }
            void        setEnable( bool state ) { _enabled = state; }
            void        setRingEnable( void );

            void        paintTree( OutputDevice *, TreeRect *r );

            void        rePlaceAll( void );

            void        wrapRings( void );
            void        unWrapRings( void );

            void        breakEdges( void );

    static  void        addRootPtrToNodes( TreeRootList & roots );
    static  void        paint( TreeRootList&, OutputDevice *, TreeRect * );
    static  TreeNode*   hitTest( TreeRootList&, TreeCoord& x, TreeCoord& y );
    static  void        sortRoots( TreeRootList& roots );
    static  int         rootRectComp( const TreeRect * r,
                                      const TreeRoot ** root );
    static  int         rootComp( const TreeRoot ** rhs,
                                  const TreeRoot** lhs );
            void        needsUpdating( void );

private:
            TreeEdgeBreaker  breaker;
            bool            _needsUpdating;     // true if need to break lines
            TreeNode *      _rootNode;

            TreeRect        _rootBound;
            TreeRingList *  _rings;
            bool            _enabled;
            TreeWindow *    _parent;
};

#endif // __GTBASE_H__
