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
 * defines TreeWindow, TreeNode, TreeRoot and TreePtr, base classes used for
 * graphic trees. TreeNode and TreeWindow are abstract base classes.
 */

#include <wmsgdlg.hpp>

#include "assure.h"
#include "symbol.h"
#include "gtbase.h"
#include "gtring.h"
#include "gtwin.h"
#include "gtedges.h"
#include "screendv.h"

const int NodeBuffer = 2;
const int sibSep = 10;
const int edgeStub = 5;
const int childSep = 50;

//////////////////////////// TreePtr ///////////////////////////////

/* store an edge in the tree.  the minimum stored in this base class
 * is a from node (lower in vertical, to right in horizontal graphs),
 * and a to node (higher in vertical, to left in horizontal).  A
 * method is provided to paint the edge by asking the _fromNode for
 * getFromCoord(), and the _toNode for getToCoord().  A virtual
 * function provides the pen for painting.  A virtual method clones
 * pointers---this is used in TreeRing when rings are wrapped or
 * unwrapped.
 */

TreePtr::TreePtr( TreeNode * from, TreeNode * to )
            :_fromNode( from )
            ,_toNode( to )
            ,_cut( FALSE )
//------------------------------------------------
{
}

/* FIXME -- could use r to see if edge needs drawing, maybe even
 * do our own clipping
 */

void TreePtr::paint( OutputDevice *dev, TreeRect * /*r*/ )
//--------------------------------------------------------
{
    PaintInfo * pinf;   /* save the old paint info from dev */

    if( _toNode->getEnable() > TreeNode::Hidden &&
        _fromNode->getEnable() > TreeNode::Hidden ) {

        pinf = dev->getPaintInfo();
        dev->setPaintInfo( getPaintInfo() );

        TreeCoord xOff = _fromNode->getParent()->getXOff();
        TreeCoord yOff = _fromNode->getParent()->getYOff();

        _line.paint( dev, xOff, yOff );

        delete dev->getPaintInfo();
        dev->setPaintInfo( pinf );
    }
}

void TreePtr::addEdges( TreeEdgeBreaker * breaker )
//-------------------------------------------------
{
    TreeCoord x1, x2, y1, y2;

    if( _fromNode->getEnable() && _toNode->getEnable() && !_cut ) {

        _fromNode->getFromCoord( this, x1, y1 );
        _toNode->getToCoord( this, x2, y2 );

        _line.set( x1, y1, x2, y2, TreeLine::Edge );

        if( _toNode->getParent()->getSmartEdges() ) {
            breaker->addLine( &_line );
        }
    }
}

/* compare two edges for sorting purposes; comparison is done based on
 * the center point of the _toNode.
 */

static int TreePtr::comparePtrs( const TreePtr * lhP, const TreePtr * rhP )
//-------------------------------------------------------------------------
{
    return TreeNode::compareNodes( lhP->_toNode, rhP->_toNode );
}

////////////////////////////////// TreeNode ////////////////////////////

/* TreeNode is an abstract base class providing all the functionality
 * needed for graphical nodes.
 */

TreeNode::TreeNode( TreeWindow * prt )
            :_bounding()
            ,_flags( NotPlaced, Visible, 0, 0, 0 )
            ,_sibWidth( 0 )
            ,_parent( prt )
            ,_pRoot( NULL )
{
}

/* set my bounding rectangle and my sibling width.  the bounding rectangle
 * is set to be large enough to hold my name (given by the port), with
 * a buffer of NodeBuffer around the edges.  the _sibWidth is determined
 * by adding the sibling widths of all parents one level above this node,
 * dividing each width by the number of children the node has.
 */

void TreeNode::setBounding( OutputDevice *dev )
//---------------------------------------------
{
    bool sepIncluded = FALSE;

    if( !_flags.boundSet ) {
        #if DEBUG
        _nm = name();
        #endif

        _flags.boundSet = TRUE;

        int     width;
        int     height;
        char *  nodename = name();

        if( nodename != NULL ) {
            width = dev->getTextExtentX( nodename );
            height = dev->getTextExtentY( nodename );
        } else {
            width = 0;
            height = 0;
        }

        _bounding.w( width + 2 * NodeBuffer );
        _bounding.h( height + 2 * NodeBuffer );

        _sibWidth = 0;

        for( int i = getCount( ParentList ); i > 0; i -= 1 ) {
            TreeNode * node = getNode( ParentList, i - 1 );

            node->setBounding( dev );
            _sibWidth += node->getSibContrib( this, sepIncluded );
        }

        bool vert = (_parent->getDirection() == TreeVertical);
        _sibWidth = maxCoord( _sibWidth, (vert) ? _bounding.w()
                                                : _bounding.h() );
    }
}

/* get the sibling-width contribution this node has for node.  This
 * is a virtual function so that TreeRing can do the appropriate thing.
 */

TreeCoord TreeNode::getSibContrib( TreeNode * node, bool & sepInc )
//-----------------------------------------------------------------
{
    TreeCoord ret = 0;

    if( node->getLevel() != getLevel() + 1 ) return 0;

    if( _flags.enabled > Hidden ) {
        int divisor = 0;
        int lvlP1 = getLevel() + 1;

        for( int i = getCount( ChildList ); i > 0; i -= 1 ) {
            TreeNode * chNode = getNode( ChildList, i - 1 );

            if( chNode->_flags.enabled > Hidden &&
                chNode->getLevel() == lvlP1 ) {

                divisor += 1;
            }
        }

        divisor = maxInt( divisor, 1 );

        ret = _sibWidth / divisor;

        if( !sepInc ) {
            sepInc = TRUE;
        } else {
            ret += sibSep / divisor;
        }
    }

    return ret;
}

/* paint a node within its _bounding rectangle
 */

void TreeNode::paint( OutputDevice *dev, TreeRect *r )
//----------------------------------------------------
{
    /* first make sure that this node needs to be drawn:
     * it must be visible, and it must be inside the
     * invalid rectangle
     */

    if( _flags.enabled == Hidden )  return;

    if( ( r->x() + r->w() < _bounding.x() ) ||
         (r->x() > _bounding.x() + _bounding.w() ) )  return;

    if( ( r->y() + r->h() < _bounding.y() ) ||
        ( r->y() > _bounding.y() + _bounding.h() ) )  return;


    WPoint      start;
    WRect       virtBnd;
    TreeCoord   xOff = _parent->getXOff();
    TreeCoord   yOff = _parent->getYOff();
    Color       back;
    Color       fore;
    PaintInfo * pinf;
    PaintInfo * oldPinf;

    //---------- border ---------//

    virtBnd.x( (int) _bounding.x() - xOff );
    virtBnd.y( (int) _bounding.y() - yOff );
    virtBnd.w( (int) _bounding.w() );
    virtBnd.h( (int) _bounding.h() );

    pinf = getPaintInfo();
    oldPinf = dev->getPaintInfo();
    dev->setPaintInfo( pinf );

    dev->rectangle( virtBnd );

    delete pinf;
    dev->setPaintInfo( oldPinf );

    //---------- text -----------//

    if( _flags.selected ) {
        back = CUSTOM_RGB(0,0,128);
        fore = ColorWhite;
    } else {
        back = ColorLiteGray;
        fore = ColorBlack;
    }

    start.x( virtBnd.x() + NodeBuffer );
    start.y( virtBnd.y() + NodeBuffer );

    dev->drawText( start, name(), fore, back );
}

void TreeNode::addEdges( TreeEdgeBreaker * breaker )
//--------------------------------------------------
{
    if( _flags.enabled > Hidden ) {

        _lineTop.set( _bounding.left(), _bounding.top(),
                      _bounding.right() - 1, _bounding.top(),
                      TreeLine::Node );
        _lineRight.set( _bounding.right() - 1, _bounding.top(),
                        _bounding.right() - 1, _bounding.bottom() - 1,
                        TreeLine::Node );
        _lineBottom.set( _bounding.right() - 1, _bounding.bottom() - 1,
                         _bounding.left(), _bounding.bottom() - 1,
                         TreeLine::Node );
        _lineLeft.set( _bounding.left(), _bounding.bottom() - 1,
                       _bounding.left(), _bounding.top(),
                       TreeLine::Node );

        if( _parent->getSmartEdges() ) {
            breaker->addLine( &_lineTop );
            breaker->addLine( &_lineRight );
            breaker->addLine( &_lineBottom );
            breaker->addLine( &_lineLeft );
        }

        for( int i = getCount( ParentList ); i > 0; i -= 1 ) {
            getPtr( ParentList, i - 1 )->addEdges( breaker );
        }
    }
}

/* move the bounding rectangle to a given (x,y) pair.  just a little
 * cleaner than including two lines of code all over the place
 */

void TreeNode::move( TreeCoord x, TreeCoord y )
//---------------------------------------------
{
    _bounding.x( x );
    _bounding.y( y );
}

/* called when the focus comes to this node---ie. when the user clicks on
 * this node with the left or right button.  the return value was meant
 * to be in case the node should not get focus; this is currently not
 * used
 */

bool TreeNode::gettingFocus( WWindow *win )
//-----------------------------------------
{
    TreeRect tr = _bounding;

    tr.x( tr.x() - _parent->getXOff() );
    tr.y( tr.y() - _parent->getYOff() );

    _flags.selected = TRUE;
    win->invalidateRect( tr.makeWRect() );
    return TRUE;
}

/* called when a node loses focus.  the return value was meant to be
 * if the node should not lose focus, but is currently not used
 */

bool TreeNode::losingFocus( WWindow *win )
//----------------------------------------
{
    TreeRect tr = _bounding;

    tr.x( tr.x() - _parent->getXOff() );
    tr.y( tr.y() - _parent->getYOff() );

    _flags.selected = FALSE;
    win->invalidateRect( tr.makeWRect() );
    return TRUE;
}

TreeRoot * TreeNode::getRootPtr( void )
//-------------------------------------
{
    return _pRoot;
}

/*  Arrange all the trees in a list of roots of the trees.
 */

void TreeNode::arrangeAll( TreeDirection dir, TreeRootList& roots,
                           TreeRect& world, OutputDevice *dev )
//----------------------------------------------------------------
{
    TreeCoord   maxX = 0;
    TreeCoord   maxY = 0;
    TreeCoord   sibOff = sibSep;
    int         i;

    for( i = 0; i < roots.count(); i += 1 ) {
        if( roots[ i ]->enabled() ) {
            TreeCoord  sibStart = sibOff;
            TreeRect   r;
            TreeNode * node = roots[ i ]->setNode();

            roots[ i ]->setRingEnable();
            roots[ i ]->setAllBounding( dev );

            node->arrangeTree( sibOff );

            // now mop up any nodes isolated by hiding
            for( int j = node->getCount( FlatList ); j > 0; j -= 1 ) {
                TreeNode * newNode = node->getNode( FlatList, j - 1 );
                if( newNode->_flags.placed != Placed ) {
                    newNode->arrangeTree( sibOff );
                }
            }

            if( dir == TreeVertical ) {
                r.x( sibStart );
                r.y( sibSep );
            } else {
                r.y( sibStart );
                r.x( sibSep );
            }
            if( node->resolveChildWard( r ) ) {
                roots[ i ]->setBound( r );          // set the roots bounding box
                roots[ i ]->sortEdges();

                sibOff = ( dir == TreeVertical) ? r.x() + r.w() + sibSep
                                                : r.y() + r.h() + sibSep;

                maxX = maxCoord( maxX, r.x() + r.w() );
                maxY = maxCoord( maxY, r.y() + r.h() );
            } else {
                roots[ i ]->setEnable( FALSE );
            }
        }
    }

    world.w( maxX );
    world.h( maxY );
}

// ============ WARNING -- WARNING -- WARNING -- WARNING =============== //
// the following code is very complicated, and makes several assumptions //
// about how other functions in this section operate.  be _very_ careful //
// when changing anything in this section                                //
// \/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\\/\/\/\/\ //

/* arrange a specific tree.  this operates by first finding a lower
 * bound for this node and all its children by calling getFirstNonContend():
 * this finds the first available position greater than sibOff.  Next,
 * simple children are identified through isSimple(), and arrangeTree()
 * is called for each of these.  the sibOff passed to each child is the
 * lower bound of _descend.  Next, non-simple children are processed with
 * arrangeTree().  Now the node calls place(), then does its parents,
 * again passing the lower bound of _descend.  the _descend width is
 * set by examining the upper bound of all child and parent _descend's.
 */

void TreeNode::arrangeTree( TreeCoord& sibOff )
//---------------------------------------------
{
    TreeCoord  startSib = sibOff;
    TreeCoord  maxDescend = 0;
    TreeCoord  mySib = sibOff;
    bool       vert  = (_parent->getDirection() == TreeVertical);
    int        i;

    if( !_flags.enabled ) {
        _flags.placed = Placed;
        return;
    }

    _flags.placed = Arranging;
    _flags.arranging = 1;

    // partial results used in getFirstNonContend

    TreeCoord offset = sibOff;
    TreeCoord width = _sibWidth;

    getFirstNonContend( offset, width );
    _descend.x( offset );
    _descend.y( offset );
    _descend.w( width );
    _descend.h( width );

    // scram through and check for simple children (one parent, no kids)
    for( i = 0; i < getCount( ChildList ); i += 1 ) {
        TreeNode * node = getNode( ChildList, i );

        if( node->_flags.placed < Arranging && node->isSimple( ChildList ) ) {
            TreeCoord tmpOff = vert ? _descend.x()
                                    : _descend.y();

            node->arrangeTree( tmpOff );
            tmpOff += sibSep;

            if( getLevel() < 0 || node->getLevel() < 0
                || node->getLevel() == getLevel() + 1 ) {

                sibOff = tmpOff;
            }

            if( node->_flags.enabled > Hidden ) {
                maxDescend = maxCoord( maxDescend,
                            vert ? node->_descend.x() + node->_descend.w()
                                 : node->_descend.y() + node->_descend.h()
                            );
            }
        }
    }

    // do non-simple children

    for( i = 0; i < getCount( ChildList ); i += 1 ) {
        TreeNode * node = getNode( ChildList, i );

        if( node->_flags.placed < Arranging ) {
            TreeCoord tmpOff = vert ? _descend.x()
                                    : _descend.y();

            node->arrangeTree( tmpOff );

            tmpOff += sibSep;
            if( getLevel() < 0 || node->getLevel() < 0 ||
                 node->getLevel() == getLevel() + 1 ) {

                sibOff = tmpOff;
            }

            if( node->_flags.enabled > Hidden ) {
                maxDescend = maxCoord( maxDescend,
                            vert ? node->_descend.x() + node->_descend.w()
                                 : node->_descend.y() + node->_descend.h()
                            );
            }
        }
    }

    place( mySib );

    // do simple parents

    for( i = 0; i < getCount( ParentList ); i += 1 ) {
        TreeNode * node = getNode( ParentList, i );

        sibOff = vert ? _descend.x() : _descend.y();

        if( node->isSimple( ParentList ) && node->_flags.placed < Arranging ) {
            node->arrangeTree( sibOff );

            if( node->_flags.enabled > Hidden ) {
                maxDescend = maxCoord( maxDescend,
                            vert ? node->_descend.x() + node->_descend.w()
                                 : node->_descend.y() + node->_descend.h()
                            );
            }
        }
    }

    // do non-simple parents

    for( i = 0; i < getCount( ParentList ); i += 1 ) {
        TreeNode * node = getNode( ParentList, i );

        sibOff = vert ? _descend.x() : _descend.y();

        if( node->_flags.placed < Arranging ) {
            node->arrangeTree( sibOff );

            if( node->_flags.enabled > Hidden ) {
                maxDescend = maxCoord( maxDescend,
                            vert ? node->_descend.x() + node->_descend.w()
                                 : node->_descend.y() + node->_descend.h()
                            );
            }
        }
    }

    if( _flags.placed != Placed ) {
        _flags.placed = PartiallyPlaced;
    } else {
        vert ? _descend.w( maxCoord( _descend.w(), maxDescend - _descend.x() ) )
             : _descend.h( maxCoord( _descend.h(), maxDescend - _descend.y() ) );
    }

    sibOff = maxCoord( sibOff, vert ? _descend.x() + _descend.w()
                                  : _descend.y() + _descend.h() );

    _flags.arranging = 0;
}

/* place a node in the tree.  this first checks that the node is
 * readyToPlace() (ie. all children placed), then calls getMinMax() to
 * find the minimum and maximum children coordinates (based on _bounding)
 * for centering purposes.  Next, getFirstNonContend() is called to
 * find the minimum _bounding and maximum width
 */

void TreeNode::place( TreeCoord& sibOff )
//---------------------------------------
{
    bool vert = (_parent->getDirection() == TreeVertical );
    if( _flags.placed == Placed ) {
        sibOff +=  (vert) ? _descend.w()
                          : _descend.h();
        return;
    }

    if( !readyToPlace() ) {
        return;
    }

    _flags.placed = Placed;

    TreeCoord maxSibW = _sibWidth;
    TreeCoord minSib = (vert) ? _descend.x()
                              : _descend.y();
    TreeCoord maxSib = sibOff;

    sibOff = maxCoord( sibOff, minSib );

    getMinMax( minSib, maxSib );
    maxSibW = maxCoord( maxSibW, maxSib - maxCoord( minSib, sibOff ) );
    TreeCoord oldMaxSibW = maxSibW;
    getFirstNonContend( sibOff, maxSibW );

    (vert) ? _descend.x( maxCoord( _descend.x(), sibOff ) )
           : _descend.y( maxCoord( _descend.y(), sibOff ) );

    TreeCoord x;
    TreeCoord y;
    TreeCoord tryW;
    if( sibOff > minSib || oldMaxSibW > maxSibW ) {
        x = _descend.x() + maxSibW / 2 - _bounding.w() / 2;
        y = _descend.y() + maxSibW / 2 - _bounding.h() / 2;
        tryW = ( vert ? _descend.x() : _descend.y() ) + maxSibW;
    } else {
        x = minSib + maxSibW / 2 - _bounding.w() / 2;
        y = minSib + maxSibW / 2 - _bounding.h() / 2;
        tryW = minSib + maxSibW;
    }

    (vert) ? _descend.w( maxCoord( _descend.w(), tryW - _descend.x() ) )
           : _descend.h( maxCoord( _descend.h(), tryW - _descend.y() ) );

    move( x, y );
    sibOff += maxSibW;

    _descend.w( maxCoord( _descend.w(), _bounding.x() + _bounding.w() - _descend.x() ) );
    _descend.h( maxCoord( _descend.h(), _bounding.y() + _bounding.h() - _descend.y() ) );
}

/* get the minimum and maximum coordinates of visible children
 */

void TreeNode::getMinMax( TreeCoord& minSib, TreeCoord& maxSib )
//--------------------------------------------------------------
{
    bool        vert = (_parent->getDirection() == TreeVertical);
    TreeCoord   tmpx, tmpy;
    int         i;

    for( i = getCount( ChildList ); i > 0; i -= 1 ) {
        TreeNode * node = getNode( ChildList, i - 1 );
        if( node->_flags.placed > Arranging && node->_flags.enabled != Hidden ) {

            node->getMinSibCoord( tmpx, tmpy );
            minSib = maxCoord( minSib, vert ? tmpx : tmpy );

            break;
        }
    }

    for( i = getCount( ChildList ); i > 0; i -= 1 ) {
        TreeNode * node = getNode( ChildList,  i - 1 );
        if( node->_flags.enabled > Hidden ) {
            node->getMinCoord( tmpx, tmpy );
            minSib = minCoord( minSib, vert ? tmpx : tmpy );

            node->getMaxCoord( tmpx, tmpy );
            maxSib = maxCoord( maxSib, vert ? tmpx : tmpy );
        }
    }
}

/* this function finds the first available location for a node to be placed.
 * it is called twice for each node -- once before the node is placed to
 * find a left boundary for its children, and once again when it is being
 * placed to see if it has to restrict its width.  To find the first
 * available location, it does the following:  start at the minimum
 * position of your parent (his _descend.{x|y}).  Check all nodes at
 * the same level---if this location lies within another, move the active
 * position (sibOff) to be past the other node.  If a common child node
 * is shared, then restrict the other node to its _sibWidth.  Now, since
 * the nodes aren't ordered, the loop must be restarted (i = -1) to make
 * sure that if we fit in to the left of a node before, we still fit.
 *
 * This procedure could definitely be optimized by sorting the nodes; then
 * it wouldn't be necessary to start all over again.
 */

void TreeNode::getFirstNonContend( TreeCoord& sibOff, TreeCoord& width )
//----------------------------------------------------------------------
{
    if( getLevel() < 0 ) {
        return;
    }

    TreeCoord  origW = width;
    bool vert = _parent->getDirection() == TreeVertical;

    for( int i = 0; i < getCount( FlatList ); i += 1 ) {
        TreeNode * node = getNode( FlatList, i );
        TreeCoord    nodeW = maxCoord( node->_sibWidth,
                                       vert ? node->_descend.w()
                                            : node->_descend.h() );

        if( node != this && node->getLevel() == getLevel() &&
            node->_flags.placed != NotPlaced && node->_flags.enabled > Hidden ) {

            TreeCoord lOff = vert ? node->_descend.x() - sibSep
                            : node->_descend.y() - sibSep;
            TreeCoord mOff = vert ? node->_descend.x() + nodeW + sibSep
                            : node->_descend.y() + nodeW + sibSep;
            if( sibOff + width > lOff && sibOff < mOff ) {
                // contention happening -- try to shrink

                width = maxCoord( _sibWidth,
                                vert ? node->_descend.x() - sibOff - sibSep
                                     : node->_descend.y() - sibOff - sibSep );
            }

            if( sibOff + width > lOff && sibOff < mOff ) {
                // shrinking couldn't fix it -- restore to original size
                // and move past this node.

                width = origW;

                bool done = FALSE;
                for( int childCnt1 = getCount( ChildList );
                     childCnt1 > 0 && !done; childCnt1 -= 1 ) {
                    TreeNode * node1 = getNode( ChildList, childCnt1 - 1 );

                    if( node1->_flags.enabled > Hidden ) {
                        for( int childCnt2 = node->getCount( ChildList );
                             childCnt2 > 0 && !done; childCnt2 -= 1 ) {
                            TreeNode * node2;

                            node2 = node->getNode( ChildList, childCnt2 - 1 );
                            if( node1 == node2 ) {
                                nodeW = node->shrinkToSib();
                                done = TRUE;
                            }
                        }
                    }
                }

                TreeCoord newOff = vert ? node->_descend.x()
                                        : node->_descend.y();
                newOff += nodeW + sibSep;

                sibOff = maxCoord( sibOff, newOff );

                i = -1;     // re-start the loop
            }
        }
    }
}

// \/\/\/\/\/\/\/\/\\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\\/\/\/\/\ //
// the above code is very complicated, and makes several assumptions     //
// about how other functions in this section operate.  be _very_ careful //
// when changing anything in this section                                //
// ============ WARNING -- WARNING -- WARNING -- WARNING =============== //



/* check all of a nodes children; if they are all placed, it is ready
 * to place itself
 */

bool TreeNode::readyToPlace( void )
//---------------------------------
{
    for( int i = getCount( ChildList ); i > 0; i -= 1 ) {
        TreeNode * nd = getNode( ChildList, i - 1 );
        if( (nd->_flags.enabled > Hidden) && (nd->_flags.placed != Placed) ) {
            return FALSE;
        }
    }

    return TRUE;
}

/* a node is simple if it has only one parent and no children.  simple
 * nodes are placed first
 */

bool TreeNode::isSimple( TreeList_T lst )
//---------------------------------------
{
    TreeList_T otherList = (lst == ParentList) ? ChildList : ParentList;

    if( getNumEnabled( otherList ) > 1 ) return FALSE;

    if( getNumEnabled( lst ) > 0 ) return FALSE;


    // this is a special case when rings have been placed

    for( int i = getCount( otherList ); i > 0; i -= 1 ) {
        TreeNode * node = getNode( otherList, i - 1 );

        if( node->_flags.enabled > Hidden && node->getLevel() < 0 &&
            node->getCount( otherList ) > 1 ) return FALSE;
    }

    return TRUE;
}

/* are graphs drawn vertically or horizontally ?
 */

TreeDirection TreeNode::getDirection( void ) const
//------------------------------------------------
{
    return _parent->getDirection();
}

/* This function shrinks a node down to its _sibWidth (the minimum needed
 * to guarantee that parents can place themselves properly.  Nodes are only
 * shrunk when two siblings have a common child; then, the first is shrunk.
 */

TreeCoord TreeNode::shrinkToSib( void )
//-------------------------------------
{
    if( _parent->getDirection() == TreeVertical ) {
        _bounding.x( _descend.x() + (_sibWidth - _bounding.w()) / 2 );
    } else {
        _bounding.y( _descend.y() + (_sibWidth - _bounding.h()) / 2 );
    }

    _descend.w( _sibWidth );
    _descend.h( _sibWidth );
    return _sibWidth;
}

/* this function assigns physical coordinates to match the logical level
 * number.  the return value is true if the root will be enabled
 */

bool TreeNode::resolveChildWard( TreeRect & r )
//---------------------------------------------
{
    bool       vert = _parent->getDirection() == TreeVertical;
    TreeCoord  minSib = vert ? r.x() : r.y();
    TreeCoord  maxSib = 0;
    TreeCoord  childOff = childSep / 2;
    TreeCoord  maxChild = 0;
    bool       minSet = FALSE;
    int        maxLevel;
    int        minLevel;
    int        i;

    for( i = 0; i < getCount( FlatList ); i += 1 ) {
        TreeNode * node = getNode( FlatList, i );

        int tLevel = node->getLevel();
        if( node->_flags.enabled > Hidden && tLevel >= 0 ) {
            if( minSet ) {
                maxLevel = maxInt( maxLevel, tLevel );
                minLevel = minInt( minLevel, tLevel );
            } else {
                maxLevel = tLevel;
                minLevel = tLevel;
                minSet = TRUE;
            }
        }
    }

    if( !minSet ) {
        return FALSE;
    }

    for( int level = minLevel; level <= maxLevel; level += 1 ) {
        maxChild = 0;
        for( i = 0; i < getCount( FlatList ); i += 1 ) {
            TreeNode * node = getNode( FlatList, i );
            if( node->getLevel() == level && node->_flags.enabled > Hidden ) {
                vert ? node->_bounding.y( childOff )
                     : node->_bounding.x( childOff );

                maxChild = maxCoord( maxChild, vert ? node->_bounding.h()
                                                    : node->_bounding.w() );
                maxSib = maxCoord( maxSib, vert ? node->_descend.x() + node->_descend.w()
                                                : node->_descend.y() + node->_descend.h() );
                minSib = minCoord( minSib, vert ? node->_descend.x()
                                                : node->_descend.y() );
            }
        }
        childOff += maxChild + childSep;
    }

    if( vert ) {
        r.x( minSib );
        r.w( maxSib - r.x() );
        r.h( childOff );
        _descend.h( childOff );
    } else {
        r.w( childOff );
        r.y( minSib );
        r.h( maxSib - r.y() );
        _descend.w( childOff );
    }

    return TRUE;
}

/* get the number of parents or children enabled for calculating the
 * edge offset
 */

uint TreeNode::getNumEnabled( TreeList_T lst )
//--------------------------------------------
{
    uint cnt = 0;

    for( uint i = getCount( lst ); i > 0; i -= 1 ) {
        if( getNode( lst, i - 1 )->_flags.enabled > Hidden ) {
            cnt += 1;
        }
    }

    return cnt;
}

/* get the end-point of a line drawn to this node; this is on the
   bottom of vertical trees, the right edge of horizontal trees. by
   sorting the pointers to parent and child nodes, the end points
   can be separaed so that they are not all coincident.
 */

void TreeNode::getToCoord( TreePtr * ptr, TreeCoord& x, TreeCoord& y )
//--------------------------------------------------------------------
{
    int i = 1;
    int divisor;
    bool vert = _parent->getDirection() == TreeVertical;
    TreeNode * child = ptr->getFrom();

    divisor = getNumEnabled( ChildList );

    for( i = divisor; i > 0; i -= 1 ) {
        if( child == getNode( ChildList, i - 1 ) ) break;
    }

    x = vert ? _bounding.x() + i * _bounding.w() / (divisor + 1)
             : _bounding.x() + _bounding.w() - 1;
    y = vert ? _bounding.y() + _bounding.h() - 1
             : _bounding.y() + i * _bounding.h() / (divisor + 1);
}

/* get the endpoint of a line drawn from this node.  this is on the
   top for vertical graphs, on the left for horizontal graphs.
 */

void TreeNode::getFromCoord( TreePtr * ptr, TreeCoord& x, TreeCoord& y )
//----------------------------------------------------------------------
{
    bool vert  = _parent->getDirection() == TreeVertical;
    int  i = 1;
    int  divisor;

    divisor = getNumEnabled( ParentList );
    for( i = divisor; i > 0; i -= 1 ) {
        if( ptr == getPtr( ParentList, i - 1 ) ) break;
    }

    x = (vert) ? _bounding.x() + i * _bounding.w() / (divisor + 1)
               : _bounding.x();

    y = (vert) ? _bounding.y()
               : _bounding.y() + i * _bounding.h() / (divisor + 1);
}

/* get the smallest coordinate of this node's bounding box
 */

void TreeNode::getMinCoord( TreeCoord & x, TreeCoord & y )
//--------------------------------------------------------
{
    x = _bounding.x();
    y = _bounding.y();
}

/* get the smallest coordinate of this node's sibling box
 * (bug: either x or y will be wrong since _sibWidth should only
 * apply to one of them)
 */

void TreeNode::getMinSibCoord( TreeCoord & x, TreeCoord & y )
//-----------------------------------------------------------
{
    x = _bounding.x() + _bounding.w() / 2 - _sibWidth / 2;
    y = _bounding.y() + _bounding.h() / 2 - _sibWidth / 2;
}

/* get the maximum coordinates of this node's bounding box
 */

void TreeNode::getMaxCoord( TreeCoord & x, TreeCoord & y )
//--------------------------------------------------------
{
    x = _bounding.x() + _bounding.w();
    y = _bounding.y() + _bounding.h();
}

/* paint the tree containing this node.  any node in the tree
 * can paint the entire tree
 */

void TreeNode::paintTree( OutputDevice *dev, TreeRect * r )
//---------------------------------------------------------
{
    int     i;

    TreeCoord xOff = _parent->getXOff();
    TreeCoord yOff = _parent->getYOff();

    /*
     * Draw nodes before edges so that when using straight edges,
     * we won't have any edges overlapped by nodes.
     */

    for( i = getCount( FlatList ); i > 0; i -= 1 ) {
        TreeNode * node = getNode( FlatList, i - 1 );
        node->paint( dev, r );
    }

    for( i = getCount( FlatList ); i > 0; i -= 1 ) {
        TreeNode * node = getNode( FlatList, i - 1 );
        for( int j = node->getCount( ParentList ); j > 0; j -= 1 ) {
            node->getPtr( ParentList, j - 1 )->paint( dev, r );
        }
    }
}

/* check the entire tree to find a node containing a given
 * coordinate.  this is used for mouse hit-testing right now.
 */

TreeNode* TreeNode::hitTest( TreeCoord x, TreeCoord y )
//-----------------------------------------------------
{
    TreeNode * node = NULL;
    for( int i = getCount( FlatList ); i > 0; i -= 1 ) {
        node = getNode( FlatList, i - 1 );
        if( node->_flags.enabled > Hidden && node->_bounding.contains(x,y) ) {
            return node;
        }
    }

    return NULL;
}

/* enable all children, and their children's children, etc.
 */

void TreeNode::enableKids( bool enable )
//--------------------------------------
{
    if( enable ) {
        _flags.enabled = Visible;
    } else {
        _flags.enabled = Hidden;
    }

    for( int i = getCount( ChildList ); i > 0; i -= 1 ) {
        getNode( ChildList, i - 1 )->enableKids( enable );
    }
}

/* reset all data in the object for re-arranging this node
 */
void TreeNode::rePlace( void )
//----------------------------
{
    _flags.placed = NotPlaced;
    _flags.arranging = 0;
    _flags.boundSet = FALSE;
    _descend = TreeRect();
    _bounding = TreeRect();
    _sibWidth = 0L;
}

/* apply lvl to an entire tree
 */

void TreeNode::enableTree( uint lvl )
//-----------------------------------
{
    for( int i = getCount( FlatList ); i > 0; i -= 1 ) {
        getNode( FlatList, i - 1 )->_flags.enabled = lvl;
    }
}

/* enable all parents, and their parents, etc.
 */

void TreeNode::enableParents( bool enable )
//-----------------------------------------
{
    if( enable ) {
        _flags.enabled = Visible;
    } else {
        _flags.enabled = Hidden;
    }

    for( int i = getCount( ParentList ); i > 0; i -= 1 ) {
        getNode( ParentList, i - 1 )->enableParents( enable );
    }
}

/* compare two nodes for the purposes of sorting.  A node is less than
 * another if the center of its bounding rectangle is to the left of
 * the other
 */

static int TreeNode::compareNodes( const TreeNode * lhN, const TreeNode * rhN )
//-----------------------------------------------------------------------------
{
    if( lhN->_flags.enabled == Hidden ) return +1;
    if( rhN->_flags.enabled == Hidden ) return -1;

    TreeCoord    lhC = (lhN->getDirection() == TreeVertical)
                            ? lhN->_bounding.x() + lhN->_bounding.w() / 2
                            : lhN->_bounding.y() + lhN->_bounding.h() / 2;
    TreeCoord    rhC = (rhN->getDirection() == TreeVertical)
                            ? rhN->_bounding.x() + rhN->_bounding.w() / 2
                            : rhN->_bounding.y() + lhN->_bounding.h() / 2;

    if( lhC < rhC ) {
        return -1;
    } else {
        if( lhC > rhC ) {
            return +1;
        } else {
            return 0;
        }
    }
}

bool TreeNode::isVisible( void )
//------------------------------
// Returns TRUE if this node is enabled
//
// Note : should also check to see if root for this node's tree is
//        enabled!
//

{
    if( _flags.enabled != Hidden ) {
        if(( _pRoot != NULL ) && ( _pRoot->enabled() )) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

void TreeNode::setRootPtr( TreeRoot * pRoot )
//-------------------------------------------
{
    _pRoot = pRoot;
}
/* a constructor for the flags
 */

TreeNode::TreeNodeFlags::TreeNodeFlags( PlaceLevel p, EnableLevel e,
                                        bool s, bool b, bool a )
            : placed( p )
            , enabled( e )
            , selected( s )
            , boundSet( b )
            , arranging( a )
{
}

#ifdef DEBUGTREE

/* draw the _sibWidth line, and a rectangle representing the
 * _descend rectangle
 */

void TreeNode::sibWidth()
//-----------------------
{
    WString     msg;
    TreeNode *  node;
    int         i;
    bool        sepInc = TRUE;

    msg.concatf( "%s: _sibWidth = %ld; parents: ",
                        _nm ? _nm : "<>", _sibWidth );

    for( i = 0; i < getCount( ParentList ); i += 1 ) {
        node = getNode( ParentList, i );
        msg.concatf( "%s: sibContrib() = %ld; ",
                     node->_nm ? node->_nm : "<>",
                     node->getSibContrib( this, sepInc ) );
    }

    msg.concat( "children: " );
    for( i = 0; i < getCount( ChildList ); i += 1 ) {
        node = getNode( ChildList, i );
        msg.concatf( "%s: _sibWidth = %ld; ",
                     node->_nm ? node->_nm : "<>", node->_sibWidth );
    }

    WMessageDialog::message( _parent, MsgPlain, MsgOk, msg.gets(),
                            "Source Browser" );
}

/* display most of the information stored in a TreeNode.
 */

void TreeNode::debugInfo( TreeRoot * root )
//-----------------------------------------
{
    WString boundLine;
    WString descendLine;
    char *  enabled;
    char *  placed;
    WString statLine;
    WString refLine;
    WString rootLine;

    TreeCoord   ox, oy;
    TreeRect rootRect;

    ox = _parent->getXOff();
    oy = _parent->getYOff();

    root->getBound( rootRect );

    rootLine.printf( "_rootNode = %s, bounding(%ld,%ld,%ld,%ld) ",
                     root->node()->name(), rootRect.x(), rootRect.y(),
                     rootRect.w(), rootRect.h() );
    boundLine.printf( "_bounding [%s] = (%ld,%ld,%ld,%ld), offset = (%ld,%ld) ",
                        (_flags.boundSet) ? "set" : "unset",
                        _bounding.x(), _bounding.y(),
                        _bounding.w(), _bounding.h(),
                        ox, oy );
    descendLine.printf( "_sibWidth = %ld, _descend = (%ld,%ld,%ld,%ld) ",
                        _sibWidth, _descend.x(), _descend.y(),
                        _descend.w(), _descend.h() );

    switch( _flags.enabled ) {
    case Hidden:
        enabled = "Hidden";
        break;
    default:
        enabled = "Visible";
        break;
    }

    switch( _flags.placed ) {
    case NotPlaced:
        placed = "NotPlaced";
        break;
    case PartiallyPlaced:
        placed = "PartiallyPlaced";
        break;
    case Arranging:
        placed = "Arranging";
        break;
    default:
        placed = "Placed";
    }
    statLine.printf( " enabled = %s, placed = %s, selected = %s, _level = %d\n",
                        enabled, placed, (_flags.selected) ? "TRUE" : "FALSE",
                        getLevel() );

    refLine.printf( "hasReference() = %s, isReference() = %s",
                    hasReference() ? "TRUE" : "FALSE",
                    isReference() ? "TRUE" : "FALSE" );

    WMessageDialog::messagef( _parent, MsgPlain, MsgOk, name(), "%s%s%s%s%s",
                            (const char *)rootLine, (const char *)boundLine,
                            (const char *)descendLine, (const char *)statLine,
                            (const char *)refLine );
}
#endif // DEBUGTREE

////////////////////////////// TreeRoot /////////////////////////////////

/* TreeRoot is an class used to represent a single, unrelated tree.
 * TreeWindow stores a sorted list of TreeRoots, and TreeRoot has
 * methods to selectively paint or hit-test a sorted list of roots.
 * a list of all of the TreeRings used in the current tree is
 * stored, as well as the enabled state (False if the whole tree is hidden
 * from TreeRootSelect), the bounding rectangle for the tree, and a "root"
 * node of the tree (actually an arbitrary node in the tree with level 0).
 */

TreeRoot::TreeRoot( TreeWindow * prt, TreeNode * rt )
            : _rootNode( rt )
            , _enabled( TRUE )
            , _parent( prt )
            , _needsUpdating( TRUE )
{
    if( prt != NULL && prt->getSmartEdges() ) {
        _rings = new TreeRingList;
    } else {
        _rings = NULL;
    }
}

TreeRoot::~TreeRoot()
{
    if( _rings != NULL ) {
        for( int i = _rings->count(); i > 0; i -= 1 ) {
            delete (*_rings)[ i - 1 ];
        }
        delete _rings;
    }

    _rootNode->deleteTree();
}

void TreeRoot::paintTree( OutputDevice *dev, TreeRect *r )
//--------------------------------------------------------
{
    TreeCoord xOff = _rootNode->getParent()->getXOff();
    TreeCoord yOff = _rootNode->getParent()->getYOff();

    if( _needsUpdating ) {
        breakEdges();
    }

    if( _enabled ) {
        _rootNode->paintTree( dev, r );

        if( _rings != NULL ) {
            for( int i = _rings->count(); i > 0; i -= 1 ) {
                if( (*_rings)[ i - 1 ]->getEnable() > TreeNode::Hidden ) {
                    (*_rings)[ i - 1 ]->paint( dev, r );
                }
            }
        }
    }

}

static int TreeRoot::rootRectComp( const TreeRect * r,
                                   const TreeRoot ** root )
//---------------------------------------------------------
{
    if( !( *root )->enabled() ) {
        return -1;
    }

    TreeRect rRect = (*root)->getBound();

    if( (*root)->_rootNode->getDirection() == TreeVertical ) {
        if( r->x() + r->w() < rRect.x() ) return -1;
        if( r->x() > rRect.x() + rRect.w() ) return +1;

        return 0;
    } else {
        if( r->y() + r->h() < rRect.y() ) return -1;
        if( r->y() > rRect.y() + rRect.h() ) return +1;

        return 0;
    }
}

static int TreeRoot::rootComp( const TreeRoot ** lhs, const TreeRoot** rhs )
//--------------------------------------------------------------------------
{
    if( (*lhs)->enabled() && !(*rhs)->enabled() ) {
        return -1;
    }

    if( !(*lhs)->enabled() && (*rhs)->enabled() ) {
        return +1;
    }

    if( !(*lhs)->enabled() && !(*rhs)->enabled() ) {
        return 0;
    }

    TreeRect r = (*lhs)->getBound();
    return rootRectComp( &r, rhs );
}

void TreeRoot::sortRoots( TreeRootList & roots )
//----------------------------------------------
{
    roots.sort( (TComp) rootComp );
}

void TreeRoot::paint( TreeRootList& roots, OutputDevice *dev, TreeRect *r )
//-------------------------------------------------------------------------
{
    int        index;
    TreeRoot ** tst = roots.search( r, (TComp) rootRectComp );

    if( tst != NULL ) {
        TreeRoot    *R;
        int         i;

        index = roots.indexOfSame( *tst );
        for( i = index; i < roots.count(); i += 1 ) {
            R = roots[ i ];

            if( R->_enabled && rootRectComp( r, (TreeRoot const **) &R ) )
                break;

            R->paintTree( dev, r );
        }

        for( i = index - 1; i >= 0; i -= 1 ) {
            R = roots[ i ];

            if( R->_enabled && rootRectComp( r, (TreeRoot const **) &R ) )
                break;

            R->paintTree( dev, r );
        }
    }
}

TreeNode * TreeRoot::hitTest( TreeRootList & roots, TreeCoord & x,
                                     TreeCoord & y )
//----------------------------------------------------------------
{
    TreeRect r( x, y, 0, 0 );
    TreeRoot ** tst = roots.search( &r, (TComp) rootRectComp );

    if( tst != NULL && (*tst)->_enabled ) {
        return (*tst)->_rootNode->hitTest( x, y );
    }

    return NULL;
}

void TreeRoot::rePlaceAll( void )
//-------------------------------
{
    for( int j = node()->getCount( TreeNode::FlatList ); j > 0; j -= 1 ) {
        _rootNode->getNode( TreeNode::FlatList, j - 1 )->rePlace();
    }

    if( _rings != NULL ) {
        for( int i = _rings->count(); i > 0; i -= 1 ) {
            (*_rings)[ i - 1 ]->rePlace();
        }
    }
}

void TreeRoot::wrapRings( void )
//------------------------------
{
    if( _rings == NULL ) {
        _rings = new TreeRingList;
    }

    for( int i = _rootNode->getCount( TreeNode::FlatList ); i > 0; i -= 1 ) {
        TreeNode * node = _rootNode->getNode( TreeNode::FlatList, i - 1 );

        for( int j = node->getCount( TreeNode::ParentList ); j > 0; j -= 1 ) {
            TreePtr * ptr = node->getPtr( TreeNode::ParentList, j - 1 );

            bool done = FALSE;
            for( int k = _rings->count(); k > 0 && !done; k -= 1 ) {
                if( (*_rings)[ k - 1 ]->addPtr( ptr ) ) {
                    done = TRUE;
                }
            }

            if( !done ) {
                _rings->add( new TreeRing( _rootNode->getParent(), ptr ) );
            }
        }
    }

    TreeRing::joinRings( *_rings );
    setRingEnable();
   // breakEdges();     // NYI not fully working yet

}

void TreeRoot::unWrapRings( void )
//--------------------------------
{
    if( _rings != NULL ) {

        breaker.purgeContents();

        for( int i = _rings->count(); i > 0; i -= 1 ) {
            (*_rings)[ i - 1 ]->unWrap();
            delete (*_rings)[ i - 1 ];
        }

        delete _rings;
        _rings = NULL;
    }
}

void TreeRoot::breakEdges( void )
//-------------------------------
{
    int             i;

    breaker.purgeContents();

    for( i = _rootNode->getCount( TreeNode::FlatList ); i > 0; i -= 1 ) {
        TreeNode * node = _rootNode->getNode( TreeNode::FlatList, i - 1 );
        node->addEdges( &breaker );
    }

    if( _rings != NULL ) {
        for( i = _rings->count(); i > 0; i -= 1 ) {
            TreeRing * ring = (*_rings)[ i - 1 ];
            ring->addEdges( &breaker );
        }
        if( _rootNode->getDirection() == TreeVertical ) {
            breaker.breakLinesV();
        } else {
            breaker.breakLinesH();
        }
    }

    _needsUpdating = FALSE;
}

void TreeRoot::setAllBounding( OutputDevice *dev )
//------------------------------------------------
{
    int i;

    if( _rings != NULL ) {
        for( i = _rings->count(); i > 0; i -= 1 ) {
            (*_rings)[ i - 1 ]->setBounding( dev );
        }
    }

    for( i = _rootNode->getCount( TreeNode::FlatList ); i > 0; i -= 1 ) {
        TreeNode * node = _rootNode->getNode( TreeNode::FlatList, i - 1 );
        node->setBounding( dev );
    }
}

void TreeRoot::sortEdges( void )
//------------------------------
{
    for( int i = _rootNode->getCount( TreeNode::FlatList ); i > 0; i -= 1 ){
        _rootNode->getNode( TreeNode::FlatList, i - 1 )->sortPrtKids();
    }

    if( _rings != NULL ) {
        for( int i = _rings->count(); i > 0; i -= 1 ) {
            (*_rings)[ i - 1 ]->sortPrtKids();
        }
    }
}

TreeNode * TreeRoot::setNode( void )
//----------------------------------
{
    TreeNode *  nodeAtMin;
    int         minLevel;
    bool        minSet = FALSE;

    for( int i = _rootNode->getCount( TreeNode::FlatList ); i > 0; i -= 1 ) {
        TreeNode * node = _rootNode->getNode( TreeNode::FlatList, i - 1 );
        int        nodeLvl = node->getLevel();
        if( node->getEnable() > TreeNode::Hidden ) {
            if( nodeLvl >= 0 ) {
                if( !minSet ) {
                    minSet = TRUE;
                    minLevel = nodeLvl;
                    nodeAtMin = node;
                } else {
                    if( node->getLevel() < minLevel ) {
                        nodeAtMin = node;
                        minLevel =  nodeLvl;
                    }
                }
            }
        }
    }

    if( !minSet || _rootNode->getLevel() == minLevel ) {
        return _rootNode;
    } else {
        _rootNode = nodeAtMin;
        return nodeAtMin;
    }
}

void TreeRoot::setRingEnable( void )
//----------------------------------
{
    if( _rings != NULL ) {
        for( int i = _rings->count(); i > 0; i -= 1 ) {
            (*_rings)[ i - 1 ]->setEnableStatus();
        }
    }
}

void TreeRoot::needsUpdating( void )
//----------------------------------
// Whenever we need to recalculate any of the lines in this tree,
// this function should be called.
{
    _needsUpdating = TRUE;
}

static void TreeRoot::addRootPtrToNodes( TreeRootList & roots )
//-------------------------------------------------------------
// Each node has a pointer to its TreeRoot
{
    int i, j;

    for( i = 0; i < roots.count(); i += 1 ) {
        TreeRoot * pTreeRoot;

        pTreeRoot = roots[ i ];

        for( j = 0; j < pTreeRoot->node()->getCount( TreeNode::FlatList ); j += 1 ) {
            pTreeRoot->node()->getNode( TreeNode::FlatList, j )->setRootPtr( pTreeRoot );
        }
    }
}
