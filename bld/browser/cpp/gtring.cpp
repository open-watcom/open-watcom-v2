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


#include "wbrdefs.h"
#include "assure.h"
#include "gtring.h"
#include "gtedges.h"
#include "gtwin.h"

#define ChildOff 25
#define ChildH   10
#define ChildW   10
const sibSep = 10;

const int POOLSIZE = 64;

#pragma warning 549 5           // sizeof contains compiler genned info.
MemoryPool TreeRing::_pool( sizeof( TreeRing ), "TreeRing", POOLSIZE );
#pragma warning 549 3

#define CONFLICTCOLOUR (ColorWhite)

TreeRing::TreeRing( TreeWindow * parent, TreePtr * ptr )
            : TreeNode( parent )
            , _sorted( FALSE )
            , _cut( FALSE )
            , _hasLineTop( FALSE )
            , _hasLineBottom( FALSE )
            , _hasLineConnector( FALSE )
            , _level( 0 )
//------------------------------------------------------
{
    TreeNode * parnt = ptr->getTo();
    TreeNode * child = ptr->getFrom();
    TreePtr *  pPtr = ptr->makeClone();

    _level = parnt->getLevel();

    pPtr->setFrom( this );
    _parents.add( pPtr );
    parnt->swapNode( ChildList, child, this );

    ptr->setTo( this );
    _children.add( ptr );

    #if DEBUG
    _nm = "TreeRing";
    #endif
}

TreeRing::~TreeRing()
{
    int i;

    for( i = _parents.count(); i > 0; i -= 1 ) {
        delete _parents[ i - 1 ];
    }
}

void * TreeRing::operator new( size_t )
//-------------------------------------
{
    return( _pool.alloc() );
}

void TreeRing::operator delete( void * mem )
//------------------------------------------
{
    _pool.free( mem );
}

void TreeRing::unWrap( void )
//---------------------------
{
    for( int i = _children.count(); i > 0; i -= 1 )  {
        TreePtr * ptr = _children[ i - 1 ]->makeClone();

        for( int j = _parents.count(); j > 0; j -= 1 ) {
            TreeNode * node = _parents[ j - 1 ]->getTo();
            ptr->setTo( node );
            node->addPtr( ChildList, ptr );
        }
        delete ptr;
    }

    for( i = _parents.count(); i > 0; i -= 1 ) {
        TreePtr * ptr = _parents[ i - 1 ]->makeClone();

        for( int j = _children.count(); j > 0; j -= 1 ) {
            TreeNode * node = _children[ j - 1 ]->getFrom();

            ptr->setFrom( node );
            node->addPtr( ParentList, ptr );
            ptr = ptr->makeClone();
        }
        delete ptr;
    }

    TreeNode * rem;
    for( i = _parents.count(); i > 0; i -= 1 ) {
        rem =_parents[ i - 1 ]->getTo()->removeNode( ChildList, this );
        ASSERT( rem == this, "gtring::unwrap parent didn't remove", 0 );
    }
    for( i = _children.count(); i > 0; i -= 1 ) {
        rem = _children[ i - 1 ]->getFrom()->removeNode( ParentList, this );
        ASSERT( rem == this, "gtring::unwrap parent didn't remove", 0 );
    }
}

bool TreeRing::addPtr( TreePtr * ptr )
//------------------------------------
{
    bool found = FALSE;
    TreeNode * parnt = ptr->getTo();
    TreeNode * child = ptr->getFrom();

    _sorted = FALSE;

    if( isParent( parnt ) ) {
        if( canJoin( parnt, child ) ) {
            ptr->setTo( this );
            _children.add( ptr );
            return TRUE;
        } else {
            return FALSE;
        }
    }
    if( isChild( child ) ) {
        if( canJoin( parnt, child ) ) {
            TreePtr * pPtr = ptr->makeClone();
            pPtr->setFrom( this );
            _parents.add( pPtr );
            parnt->swapNode( ChildList, child, this );

            _level = maxInt( _level, parnt->getLevel() );

            return TRUE;
        } else {
            return FALSE;
        }
    }

    return FALSE;
}

void TreeRing::paint( OutputDevice *dev, TreeRect * r )
//-----------------------------------------------------
{
    PaintInfo * pinf;
    TreeCoord xOff = _parent->getXOff();
    TreeCoord yOff = _parent->getYOff();
    int i;

    pinf = dev->getPaintInfo();
    dev->setPaintInfo( getPaintInfo() );

    /*
     * _hasLineTop, _hasLineBottom, _hasLineConnector are set
     * in addEdges
     */
    if( _hasLineTop ) {
        _lineTop.paint( dev, xOff, yOff );
    }

    if( _hasLineBottom ) {
        _lineBottom.paint( dev, xOff, yOff );
    }

    if( _hasLineConnector ) {
        _lineConnector.paint( dev, xOff, yOff );
    }

    for( i = 0; i < _parents.count(); i += 1 ) {
        _parents[ i ]->paint( dev, r );
    }

    delete dev->getPaintInfo();
    dev->setPaintInfo( pinf );
}

PaintInfo * TreeRing::getPaintInfo( void )
//----------------------------------------
// go through all parents and children -- if they are all the
// same colour and style, then use that.  Otherwise, use a
// conflict colour and style
{
    PaintInfo * pinf;
    Color       clr = ColorBlack;
    WLineStyle  stl = LS_PEN_SOLID;
    unsigned    thk = 1;
    int         i;

    if( _parents.count() > 0 ) {
        pinf = _parents[ 0 ]->getPaintInfo();
        clr = pinf->getColour();
        stl = pinf->getStyle();
        thk = pinf->getThickness();
        delete pinf;
    }

    for( i = 1; i < _parents.count(); i += 1 ) {
        pinf = _parents[ i ]->getPaintInfo();
        if( clr != pinf->getColour() ||
            stl != pinf->getStyle()  ||
            thk != pinf->getThickness() ) {

            clr = CONFLICTCOLOUR;
            stl = LS_PEN_SOLID;
            thk = 1;
            delete pinf;
            break;
        }

        delete pinf;
    }

    for( i = 0; i < _children.count(); i += 1 ) {
        pinf = _children[ i ]->getPaintInfo();
        if( clr != pinf->getColour() ||
            stl != pinf->getStyle()  ||
            thk != pinf->getThickness() ) {

            clr = CONFLICTCOLOUR;
            stl = LS_PEN_SOLID;
            thk = 1;
            delete pinf;
            break;
        }

        delete pinf;
    }

    return new PaintInfo( clr, thk, stl );
}

void TreeRing::addEdges( TreeEdgeBreaker * breaker )
//--------------------------------------------------
{
    bool    vert = (_parent->getDirection() == TreeVertical);

    TreeCoord cfx, cfy, clx, cly;   //child first x, ..., child lasty
    TreeCoord pfx, pfy, plx, ply;   //parent first x, ..., parent lasty
    TreeCoord mx, my;               //middle x, middle y
    TreeCoord tmpx, tmpy;
    TreeCoord xOff = _parent->getXOff();
    TreeCoord yOff = _parent->getYOff();
    bool      cSet = FALSE;
    int       i;

    setChildWard();

    if( !_sorted ) {
        sortPrtKids();
    }

    for( i = 0; i < _children.count(); i += 1 ) {
        if( _children[ i ]->getFrom()->getEnable() > Hidden ) {
            _children[i]->getFrom()->getFromCoord( _children[i], tmpx, tmpy );

            if( !cSet ) {
                cfx = clx = tmpx;
                cfy = cly = tmpy;
                cSet = TRUE;
            } else {
                cfx = minCoord( cfx, tmpx );
                cfy = minCoord( cfy, tmpy );
                clx = maxCoord( clx, tmpx );
                cly = maxCoord( cly, tmpy );
            }
        }
    }

    /*
     * If there aren't any children, then we don't need to draw any parts
     * of the tree ring
     */
    if( !cSet ) {
        return;
    }

    cSet = FALSE;
    for( i = 0; i < _parents.count(); i += 1 ) {
        if( _parents[i]->getTo()->getEnable() > Hidden ) {
            _parents[i]->getTo()->getToCoord( _parents[i], tmpx, tmpy );

            if( !cSet ) {
                pfx = plx = tmpx;
                pfy = ply = tmpy;
                cSet = TRUE;
            } else {
                pfx = minCoord( pfx, tmpx );
                pfy = minCoord( pfy, tmpy );
                plx = maxCoord( plx, tmpx );
                ply = maxCoord( ply, tmpy );
            }
        }
    }

    /*
     * If there aren't any parents, then we don't need to draw any parts
     * of the tree ring
     */
    if( !cSet ) {
        return;
    }

    mx = minInt(cfx,pfx) + (maxInt(clx,plx) - minInt(cfx,pfx)) / 2;
    my = minInt(cfy,pfy) + (maxInt(cly,ply) - minInt(cfy,pfy)) / 2;

    _hasLineBottom = FALSE;
    _hasLineTop = FALSE;
    _hasLineConnector = FALSE;
    if( vert ) {
        /*
         * If there's more then one enabled child, we need a horizontal
         * bar to join the children's edges
         */
        if( getNumEnabled( ChildList ) > 1 ) {

            _lineBottom.set( minInt(mx,cfx),
                             _bounding.y() + _bounding.h() + 1,
                             maxInt(mx,clx)+1,
                             _bounding.y() + _bounding.h() + 1,
                             TreeLine::RingBottom );
            breaker->addLine( &_lineBottom );
            _hasLineBottom = TRUE;
        } else {
            // numEnabled == 1, so cfx == clx
            _lineConnector.set( cfx, _bounding.y(), cfx,
                               _bounding.y() + _bounding.h() + 2,
                               TreeLine::RingConnector );
            breaker->addLine( &_lineConnector );
            _hasLineConnector = TRUE;
        }

        /*
         * If there's more than one parent or the parents aren't aligned
         * over the children, then we need a horizontal line to join
         * the parents
         */
        if( getNumEnabled( ParentList ) > 1 || clx < pfx || cfx > plx ) {

            _lineTop.set( minInt(minInt(mx,pfx),clx), _bounding.y(),
                          maxInt(maxInt(mx,plx),cfx) + 1,
                          _bounding.y(),
                          TreeLine::RingTop );
            breaker->addLine( &_lineTop );
            _hasLineTop = TRUE;
        } else {
            if( _hasLineBottom ) {          // child hasn't already drawn
                _lineConnector.set( pfx, _bounding.y(), pfx,
                                    _bounding.y() + _bounding.h() + 1,
                                    TreeLine::RingConnector );
            breaker->addLine( &_lineConnector );
            _hasLineConnector = TRUE;
            }
        }

        if( _hasLineBottom && _hasLineTop ) {
            _lineConnector.set( mx, _bounding.y(), mx,
                                _bounding.y() + _bounding.h() + 1,
                                TreeLine::RingConnector );
            breaker->addLine( &_lineConnector );
            _hasLineConnector = TRUE;
        }
    } else {
        if( getNumEnabled( ChildList ) > 1 ) {
            _lineBottom.set( _bounding.x() + _bounding.w() + 1, minInt( my, cfy ),
                             _bounding.x() + _bounding.w() + 1, maxInt( my, cly ),
                             TreeLine::TreeLine::RingBottom );
            breaker->addLine( &_lineBottom );
            _hasLineBottom = TRUE;
        } else {
            _lineConnector.set( _bounding.x(), cfy,
                                _bounding.x() + _bounding.w() + 2, cfy,
                                TreeLine::RingConnector );
            breaker->addLine( &_lineConnector );
            _hasLineConnector = TRUE;
        }

        if( getNumEnabled( ParentList ) > 1 || cly < pfy || cfy > ply ) {
            _lineTop.set( _bounding.x(), minInt( minInt( my, pfy ), cly ),
                                _bounding.x(), maxInt( maxInt( my, ply ), cfy ) + 1,
                                TreeLine::RingTop );
            breaker->addLine( &_lineTop );
            _hasLineTop = TRUE;
        } else {
            if( _hasLineBottom ) {
                _lineConnector.set( _bounding.x(), pfy,
                                    _bounding.x() + _bounding.w() + 1, pfy,
                                    TreeLine::RingConnector );
                breaker->addLine( &_lineConnector );
                _hasLineConnector = TRUE;
            }
        }

        if( _hasLineBottom && _hasLineTop ) {
            _lineConnector.set( _bounding.x(), my,
                                _bounding.x() + _bounding.w() + 1, my,
                                TreeLine::RingConnector );
            breaker->addLine( &_lineConnector );
            _hasLineConnector = TRUE;
        }
    }

    for( i = 0; i < _parents.count(); i += 1 ) {
        _parents[ i ]->addEdges( breaker );
    }
}

void TreeRing::joinRings( TreeRingList & list )
//---------------------------------------------
//
// Try to join as many rings as possible, using a "greedy" algorithm
{
    bool joinMore;

    /*
     * In each pass of this loop, we try to join any of the rings in the
     * ring list.  We stop when we can't join any more rings.
     */
    do {
        joinMore = FALSE;

        /*
         * Go through the list of rings, trying to join pairs of rings
         */
        for( int i = list.count(); i > 0; i -= 1 ) {
            TreeRing * tryRing = list[ i - 1 ];

            if( tryRing != NULL ) {
                for( int j = list.count(); j > 0; j -= 1 ) {
                    if( list[ j - 1 ] != NULL && j != i ) {
                        if( tryRing->sameRing( list[ j - 1 ] ) ) {
                            joinMore = TRUE;

                            tryRing->joinTo( list[ j - 1 ] );
                            delete list[ j - 1 ];
                            list.replaceAt( j - 1, NULL );
                        }
                    }
                }
            }
        }

        for( i = list.count(); i > 0; i -= 1 ) {
            if( list[ i - 1 ] == NULL ) {
                list.removeAt( i - 1 );
            }
        }
    } while( joinMore );
}

void TreeRing::joinTo( TreeRing * other )
//---------------------------------------
{
    int i;

    for( i = other->_children.count(); i > 0; i -= 1 ) {
        other->_children[ i - 1 ]->setTo( this );

        TreeNode * node = other->_children[ i - 1 ]->getFrom();
        if( !isChild( node ) ) {
            _children.add( other->_children[ i - 1 ] );
            other->_children[ i - 1 ]->setTo( this );
        } else {
            node->removeNode( ParentList, other->_children[ i - 1 ]->getTo() );
        }
    }

    for( i = other->_parents.count(); i > 0; i -= 1 ) {
        TreeNode * node = other->_parents[ i - 1 ]->getTo();
        if( !isParent( node ) ) {
            TreePtr * ptr = other->_parents[ i - 1 ]->makeClone();
            ptr->setFrom( this );
            _parents.add( ptr );
            node->swapNode( ChildList, other, this );

            _level = maxInt( _level, node->getLevel() );
        } else {
            node->removeNode( ChildList, other->_parents[ i - 1 ]->getFrom() );
        }
    }
}

bool TreeRing::sameRing( TreeRing * other )
//-----------------------------------------
// Check if the other ring can be joined to this ring.
//
// Rings can be joined only if all the parents are the same, or
// if all the children are the same.
//
{
    int i;
    bool allMatch = TRUE;

    if( _parents.count() == other->_parents.count() ) {
        for( i = _parents.count(); i > 0 && allMatch; i -= 1 ) {
            if( !other->isParent( _parents[ i - 1 ]->getTo() ) ) {
                allMatch = FALSE;
            }
        }
        if( allMatch ) return TRUE;
    }

    if( _children.count() == other->_children.count() ) {
        allMatch = TRUE;

        for( i = _children.count(); i > 0 && allMatch; i -= 1 ) {
            if( !other->isChild( _children[ i - 1 ]->getFrom() ) ) {
                allMatch = FALSE;
            }
        }
        if( allMatch ) return TRUE;
    }

    return FALSE;
}

bool TreeRing::canJoin( TreeNode * parnt, TreeNode * child )
//----------------------------------------------------------
// Checks to see if, given a parent and child node, they can be linked
// using this ring
//
// Returns :
//  TRUE if there is a common parent between the child and the ring AND
//          there is a common child between the parent and the ring
//  FALSE if there are no common parents OR no common children
//
{
    int  i;
    bool found;

    /*
     * Check if any of the child's parents are in this ring's parent list
     */
    for( i = _parents.count(); i > 0; i -= 1 ) {
        found = FALSE;
        for( int j = child->getCount( ParentList ); j > 0 && !found; j -= 1 ) {
            if( child->getNode( ParentList, j - 1 ) == _parents[ i - 1 ]->getTo() ) {
                found = TRUE;
            }
        }
        if( !found ) return FALSE;      // no common parents
    }


    for( i = _children.count(); i > 0; i -= 1 ) {
        found = FALSE;
        for( int j = parnt->getCount( ChildList ); j > 0 && !found; j -= 1 ) {
            if( parnt->getNode( ChildList, j - 1 ) == _children[ i - 1 ]->getFrom() ) {
                found = TRUE;
            }
        }
        if( !found ) return FALSE;      // no common children
    }

    return TRUE;
}

bool TreeRing::isRelated( TreeNode * node )
//-----------------------------------------
{
    return isParent( node ) || isChild( node );
}

bool TreeRing::isParent( TreeNode * node )
//----------------------------------------
{
    for( int i = _parents.count(); i > 0; i -= 1 ) {
        if( _parents[ i - 1 ]->getTo() == node ) {
            return TRUE;
        }
    }
    return FALSE;
}

bool TreeRing::isChild( TreeNode * node )
//----------------------------------------
{
    for( int i = _children.count(); i > 0; i -= 1 ) {
        if( _children[ i - 1 ]->getFrom() == node ) {
            return TRUE;
        }
    }

    return FALSE;
}

static int CompareChildren( const TreePtr ** lhs, const TreePtr ** rhs )
//----------------------------------------------------------------------
{
    const TreeNode * lhN = (*lhs)->getFrom();
    const TreeNode * rhN = (*rhs)->getFrom();

    return TreeNode::compareNodes( lhN, rhN );
}

static int CompareParents( const TreePtr ** lhs, const TreePtr ** rhs )
//---------------------------------------------------------------------
{
    const TreeNode * lhN = (*lhs)->getTo();
    const TreeNode * rhN = (*rhs)->getTo();

    return TreeNode::compareNodes( lhN, rhN );
}

void TreeRing::sortPrtKids( void )
//--------------------------------
{
    _parents.sort( (TComp) CompareParents );
    _children.sort( (TComp) CompareChildren );
    _sorted = TRUE;
}

void TreeRing::getToCoord( TreePtr * ptr, TreeCoord& x, TreeCoord& y )
//----------------------------------------------------------------
{
    TreeCoord  tmpX, tmpY;
    bool vert = (_parent->getDirection() == TreeVertical);

    setChildWard();

    ptr->getFrom()->getFromCoord( ptr, tmpX, tmpY );

    x = (vert) ? tmpX : _bounding.x() + _bounding.w() + 1;
    y = (vert) ? _bounding.y() + _bounding.h() + 1: tmpY;
}

void TreeRing::getFromCoord( TreePtr * ptr, TreeCoord& x, TreeCoord& y )
//------------------------------------------------------------------
{
    TreeCoord  tmpX, tmpY;
    bool vert = (_parent->getDirection() == TreeVertical);

    setChildWard();

    ptr->getTo()->getToCoord( ptr, tmpX, tmpY );

    x = (vert) ? tmpX          : _bounding.x();
    y = (vert) ? _bounding.y() : tmpY;
}

void TreeRing::getMinCoord( TreeCoord & x, TreeCoord & y )
//--------------------------------------------------------
{
    TreeRect r;
    bool     set;

    if( !_sorted ) {
        sortPrtKids();
    }

    for( int i = _children.count(); i > 0; i -= 1 ) {
        TreeNode * child = _children[ i - 1 ]->getFrom();

        if( child->getEnable() > Hidden ) {
            TreeCoord tryX;
            TreeCoord tryY;

            child->getMinCoord( tryX, tryY );
            if( !set ) {
                set = TRUE;

                x = tryX;
                y = tryY;
            } else {
                x = minCoord( x, tryX );
                y = minCoord( y, tryY );
            }
        }
    }
}

void TreeRing::getMinSibCoord( TreeCoord & x, TreeCoord & y )
//-----------------------------------------------------------
{
    TreeRect  r;
    bool     set = FALSE;

    if( !_sorted ) {
        sortPrtKids();
    }

    for( int i = _children.count(); i > 0; i -= 1 ) {
        TreeNode * child = _children[ i - 1 ]->getFrom();

        if( child->getEnable() > Hidden ) {
            TreeCoord tryX;
            TreeCoord tryY;

            child->getMinSibCoord( tryX, tryY );
            if( !set ) {
                set = TRUE;

                x = tryX;
                y = tryY;
            } else {
                x = minCoord( x, tryX );
                y = minCoord( y, tryY );
            }
        }
    }
}

void TreeRing::getMaxCoord( TreeCoord & x, TreeCoord & y )
//--------------------------------------------------------
{
    TreeRect r;
    bool     set = FALSE;

    if( !_sorted ) {
        sortPrtKids();
    }

    for( int i = _children.count(); i > 0; i -= 1 ) {
        TreeNode * child = _children[ i - 1 ]->getFrom();

        if( child->getEnable() > Hidden ) {
            TreeCoord tryX;
            TreeCoord tryY;

            child->getMaxCoord( tryX, tryY );
            if( !set ) {
                set = TRUE;

                x = tryX;
                y = tryY;
            } else {
                x = maxCoord( x, tryX );
                y = maxCoord( y, tryY );
            }
        }
    }
}

TreeNode * TreeRing::getNode( TreeList_T lst, int index )
//-------------------------------------------------------
{
    switch( lst ) {
    case ParentList:
        return _parents[ index ]->getTo();
    case ChildList:
        return _children[ index ]->getFrom();
    default:
        REQUIRE( 0, "TreeRing::getNode -- bad list" );
    }
    return NULL;
}

TreePtr * TreeRing::getPtr( TreeList_T lst, int index )
//-----------------------------------------------------
{
    switch( lst ) {
    case ParentList:
        return _parents[ index ];
    case ChildList:
        return _children[ index ];
    default:
        REQUIRE( 0, "TreeRing::getPtr -- bad list" );
    }
    return NULL;
}

void TreeRing::addPtr( TreeList_T, TreePtr * )
//--------------------------------------------
{
}

TreeNode* TreeRing::removeNode( TreeList_T, TreeNode * )
//------------------------------------------------------
{
    return NULL;
}

int TreeRing::getCount( TreeList_T lst )
//--------------------------------------
{
    switch( lst ) {
    case ParentList:
        return _parents.count();
    case ChildList:
        return _children.count();
    case FlatList:
        return 0;
    default:
        REQUIRE( 0, "TreeRing::getCount -- bad list" );
    }
    return -1;
}

int TreeRing::getLevel( void ) const
//----------------------------------
{
    return _level;
}

void TreeRing::setBounding( OutputDevice *dev )
//---------------------------------------------
{
    TreeNode::setBounding( dev );

    setChildWard();
}

void TreeRing::setChildWard()
//---------------------------
{
    TreeRect   r;
    int     idx = 0;
    int     maxLevel = 0;

    for( int i = _parents.count(); i > 0; i -= 1 ) {
        TreeNode * prNode = _parents[ i - 1 ]->getTo();

        if( prNode->getEnable() > Hidden ) {
            int tstLvl = prNode->getLevel();
            if( tstLvl > maxLevel ) {
                maxLevel = tstLvl;
                idx = i - 1;
            }
        }
    }

    _parents[ idx ]->getTo()->myRect( r );

    if( _parent->getDirection() == TreeVertical ) {
        _bounding.y( r.y() + r.h() + ChildOff );
        _bounding.h( ChildH );
        _bounding.w( 0 );
    } else {
        _bounding.x( r.x() + r.w() + ChildOff );
        _bounding.w( ChildW );
        _bounding.h( 0 );
    }
}

TreeCoord TreeRing::getSibContrib( TreeNode * child, bool & sepInc )
//------------------------------------------------------------------
{
    TreeCoord ret = 0;
    int       divisor = 0;
    int       i;

    for( i = _parents.count(); i > 0; i -= 1 ) {
        TreeNode * prNode = _parents[ i - 1 ]->getTo();

        if( prNode->getEnable() > Hidden &&
            prNode->getLevel() == child->getLevel() - 1 ) {

            // find the number of immediate children of prNode, looking in
            // all of its child rings

            divisor = 0;
            for( int j = prNode->getCount( ChildList ); j > 0; j -= 1 ) {
                TreeNode * ring = prNode->getNode( ChildList, j - 1 );

                for( int k = ring->getCount( ChildList ); k > 0; k -= 1 ) {
                    TreeNode * chNode = ring->getNode( ChildList, k - 1 );

                    if( chNode->getEnable() > Hidden &&
                        chNode->getLevel() == child->getLevel() ) {

                        divisor += 1;
                    }
                }
            }

            divisor = maxInt( 1, divisor );             // prevent div by 0
            ret += prNode->getSibWidth() / divisor;

            if( !sepInc ) {
                sepInc = TRUE;
            } else {
                ret += sibSep / divisor;
            }
        }
    }

    return ret;
}

void TreeRing::setEnableStatus( void )
//------------------------------------
{
    bool enabled = ( getNumEnabled( ChildList ) > 0 ) &&
                   ( getNumEnabled( ParentList ) > 0 );

    setEnable( enabled ? Visible : Hidden );
}

bool TreeRing::childrenEnabled( void )
//------------------------------------
{
    int i;
    bool rc;

    rc = TRUE;
    for( i = 0; i < _children.count(); i += 1 ) {
        TreeNode * pNode;

        pNode = _children[ i ]->getFrom();
        if( !pNode->getEnable() ) {
            rc = FALSE;
            break;
        }
    }

    return rc;
}

bool TreeRing::parentsEnabled( void )
//-----------------------------------
{
    int i;
    bool rc;

    rc = TRUE;
    for( i = 0; i < _parents.count(); i += 1 ) {
        TreeNode * pNode;

        pNode = _parents[ i ]->getTo();
        if( !pNode->getEnable() ) {
            rc = FALSE;
            break;
        }
    }

    return rc;
}
