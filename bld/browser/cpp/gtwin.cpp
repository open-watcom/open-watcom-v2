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


#include <limits.h>

#include "assure.h"
#include "browse.h"
#include "busyntc.h"
#include "dtview.h"
#include "gtrootpk.h"
#include "gtwin.h"
#include "icons.h"
#include "menuids.h"
#include "screendv.h"
#include "viewmgr.h"
#include "util.h"
#include "keysym.h"
#include "querycfg.h"

const int sibSep = 10;


// FIXME --- has too many invalidates due to paranoia

TreeWindow::TreeWindow( char * title, WStyle stl )
                : WBRWindow( title, stl )
                , GlobalViewTree( menuManager() )
                , _rootsLoaded( FALSE )
                , _empty( FALSE )
                , _xOffset( 0 )
                , _yOffset( 0 )
                , _currNode( NULL )
                , _findRoot( -1 )
                , _findNode( -1 )
                , _hScrollFactor( 0 )
                , _vScrollFactor( 0 )
//----------------------------------------------------------
{
    GUIWantPartialRows( handle(), TRUE );

    setIcon( GBTree );

    _direction = optManager()->getTreeDirection();
    _edgeType = optManager()->getEdgeType();
    _autoArrange = optManager()->getTreeAutoArrange();

    _queryConfig = new QueryConfig( this, "Find" );
    _loadFilter = new KeySymbol( optManager()->getQueryFilt() );
    _findFilter = new KeySymbol;

    menuManager()->trackPopup( this, MMTree );
    viewManager()->registerForEvents( this );
}

TreeWindow::~TreeWindow()
//-----------------------
{
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        delete _roots[ i - 1 ];
    }

    delete _queryConfig;
    delete _loadFilter;
    delete _findFilter;
}

bool TreeWindow::reallyClose()
//----------------------------
{
    viewManager()->viewDying( this );

    return TRUE;
}

bool TreeWindow::gettingFocus( WWindow * )
//----------------------------------------
{
    viewManager()->eventOccured( VEGettingFocus, this );
    return FALSE;
}

bool TreeWindow::losingFocus( WWindow * )
//---------------------------------------
{
    viewManager()->eventOccured( VELosingFocus, this );
    return FALSE;
}

void TreeWindow::resized( WOrdinal w, WOrdinal h )
//------------------------------------------------
{
    resetScrollRange();
    WWindow::resized( w, h );
}

void TreeWindow::event( ViewEvent ve, View * )
//--------------------------------------------
{
    switch( ve ) {
    case VEOptionValChange:
        if( _edgeType != optManager()->getEdgeType() ) {
            toggleEdges();
        }

        if( _autoArrange != optManager()->getTreeAutoArrange() ) {
            toggleAutoArrange();
        }

        if( _direction != optManager()->getTreeDirection() ) {
            changeDirection();
        }
        invalidate();
        break;
    case VEQueryFiltChange:
        query();
        break;
    case VEBrowseFileChange:
        reLoad();
        break;
    default:
        NODEFAULT;
    }
}

ViewEvent TreeWindow::wantEvents()
//--------------------------------
{
    return VEQueryFiltChange | VEBrowseFileChange | VEOptionValChange;
}

static MIMenuID TreeMenus[] = {
    MIMenuID( MMLocate, LMFind ),
    MIMenuID( MMLocate, LMFindNext ),
#ifdef DEBUGTREE
    MIMenuID( MMTree, TMDebugInfo ),
    MIMenuID( MMTree, TMSibWidth ),
#endif
};

#define NumTreeMenus (sizeof(TreeMenus) / sizeof(MIMenuID) )

void TreeWindow::setMenus( MenuManager * mgr )
//--------------------------------------------
{
    int i;

    for( i = 0; i < NumTreeMenus; i += 1 ) {
        mgr->registerForMenu( this, TreeMenus[ i ] );
    }

    menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), FALSE );

    GlobalViewTree::setMenus( mgr );

// NYI    changed( NULL );      // disable menus
}

void TreeWindow::unsetMenus( MenuManager * mgr )
//----------------------------------------------
{
    int i;

    for( i = 0; i < NumTreeMenus; i += 1 ) {
        mgr->unRegister( TreeMenus[ i ] );
    }
    GlobalViewTree::unsetMenus( mgr );
}

void TreeWindow::menuSelected( const MIMenuID & id )
//--------------------------------------------------
{
    switch( id.mainID() ) {
    case MMLocate:
        switch( id.subID() ) {
        case LMFind:
            findFirst();
            break;
        case LMFindNext:
            findNext();
            break;
        case LMFindSelected:
            treeViewEvent( ScrollToSelected );
            break;
        default:
            NODEFAULT;
        }
        break;
#ifdef DEBUGTREE
    case MMTree:
        switch( id.subID() ) {
        case TMDebugInfo:
            showDebug();
            break;
        case TMSibWidth:
            showSib();
            break;
        default:
            GlobalViewTree::menuSelected( id );
        }
        break;
#endif
    default:
        GlobalViewTree::menuSelected( id );
    }
}

void TreeWindow::treeViewEvent( TreeViewEvent tve )
//-------------------------------------------------
{
    WRect rect;
    int i;

    switch( tve ) {
    case ShowDetail:
        showDetail();
        break;
    case ShowDefinition:
        showDefinition();
        break;
    case ShowReferences:
        showReferences();
        break;
    case ShowUsers:
        showUsers();
        break;
    case ScrollToSelected:
        if( _currNode ) {
            getClientRect( rect );
            scrollToNode( _currNode, rect );
            invalidate();
        }
        break;
    case Arrange:
        arrangeAllSelectRoots();
        break;
    case SelectRoots:
        selectRoots();
        break;
    case ExpandOne:
        if( _currNode ) {
            _currNode->setEnable( TreeNode::Visible );
            for( i = _currNode->getCount( TreeNode::ChildList ); i > 0; i -= 1 ) {
                TreeNode * node;
                node = _currNode->getNode( TreeNode::ChildList, i - 1 );
                node->setEnable( TreeNode::Visible );
            }

            arrangeAll();
        }
        break;
    case ExpandBranch:
        if( _currNode ) {
            _currNode->enableKids( TRUE );
            arrangeAll();
        }
        break;
    case ExpandAll:
        enableAll();
        arrangeAll();
        break;
    case CollapseBranch:
        if( _currNode ) {
            _currNode->enableKids( FALSE );
            // enableKids affects node itself so enable the current node
//          _currNode->setEnable( TreeNode::Visible );

            if( _autoArrange ) {
                arrangeAll();
            } else {
                for( int i = _roots.count(); i > 0; i -= 1 ) {
                    _roots[ i - 1 ]->sortEdges();
                    _roots[ i - 1 ]->needsUpdating();
                }
                invalidate();
            }
        }
        break;
    case CollapseAll:
        enableAll( FALSE );

        scrollTo( 0, 0 );
        arrangeAll();
        break;
    }
}

TreeWindow::ExpandState TreeWindow::state()
//-----------------------------------------
{
    if( _currNode ) {
        int count = _currNode->getCount( TreeNode::ChildList );

        if( count == 0 ) {
            return Leaf;
        } else {
            for( int i = 0; i < count; i += 1 ) {
                if( _currNode->getNode( TreeNode::ChildList, i )->getEnable() == TreeNode::Hidden ) {
                    return Collapsed;
                }
            }
            return Expanded;
        }
    } else {
        return NoSymbol;
    }
}

void TreeWindow::selectRoots()
//----------------------------
// select which trees to display, which to hide
{
    TreeRootSelect rootSel( this, &_roots );

    if( rootSel.process() ) { // ie OK selected
        scrollTo( 0, 0 );
        arrangeAllSelectRoots();
    }
}

void TreeWindow::showDetail()
//---------------------------
{
    if( _currNode ) {
        Symbol * sym = _currNode->makeSymbol();
        popDetail( sym );
        delete sym;
    }
}

void TreeWindow::showDefinition()
//-------------------------------
{
    if( _currNode ) {
        Symbol * sym = _currNode->makeSymbol();
        browseTop->showSource( sym );
        delete sym;
    }
}

void TreeWindow::showReferences()
//-------------------------------
{
    if( _currNode ) {
        Symbol * sym = _currNode->makeSymbol();
        viewManager()->showDetailView( sym, DetailView::ViewSourceRefs );
        delete sym;
    }
}

void TreeWindow::showUsers()
//--------------------------
{
    if( _currNode ) {
        Symbol * sym = _currNode->makeSymbol();
        viewManager()->showDetailView( sym, DetailView::ViewSymbolRefs );
        delete sym;
    }
}

#if 0 // dead expand / collapse stuff

/*
 * show active node (called from main menu only)
 */

void TreeWindow::showNode()
//-------------------------
// DEAD
{
    if( _currNode ) {

        _currNode->setEnable( TreeNode::Visible );
        arrangeAll();
    }
}

/*
 * hide this node
 */

void TreeWindow::hideNode()
//-------------------------
// DEAD
{
    if( _currNode ) {
        _currNode->setEnable( TreeNode::Hidden );
        _currNode = NULL;

        if( _autoArrange ) {
            arrangeAll();
        } else {
            for( int i = _roots.count(); i > 0; i -= 1 ) {
                _roots[ i - 1 ]->sortEdges();
                _roots[ i - 1 ]->needsUpdating();
            }
            invalidate();
        }
    }
}


/*
 * show all of a nodes parents
 */

void TreeWindow::showParents()
//----------------------------
// DEAD
{
    if( _currNode ) {
        _currNode->enableParents( TRUE );
        arrangeAll();
    }
}

/*
 * hide all of a nodes parents
 */

void TreeWindow::hideParents()
//----------------------------
// DEAD
{
    if( _currNode ) {
        _currNode->enableParents( FALSE );
        _currNode->setEnable( TreeNode::Visible );
        if( _autoArrange ) {
            arrangeAll();
        } else {
            for( int i = _roots.count(); i > 0; i -= 1 ) {
                _roots[ i - 1 ]->sortEdges();
                _roots[ i - 1 ]->needsUpdating();
            }
            invalidate();
        }
    }
}

/* show the immediate parents and kids of the active node
 */

void TreeWindow::showOnlyPrtKids()
//--------------------------------
// DEAD
{
    int i;

    if( _currNode ) {
        for( i = _currNode->getCount( TreeNode::FlatList ); i > 0; i -= 1 ) {
            TreeNode * node;
            node = _currNode->getNode( TreeNode::FlatList, i - 1 );
            node->setEnable( TreeNode::Hidden );
        }

        _currNode->setEnable( TreeNode::Visible );

        for( i = _currNode->getCount( TreeNode::ParentList ); i > 0; i -= 1 ) {
            TreeNode * node;
            node = _currNode->getNode( TreeNode::ParentList, i - 1 );
            node->setEnable( TreeNode::Visible );
        }
        for( i = _currNode->getCount( TreeNode::ChildList ); i > 0; i -= 1 ) {
            TreeNode * node;
            node = _currNode->getNode( TreeNode::ChildList, i - 1 );
            node->setEnable( TreeNode::Visible );
        }

        arrangeAll();
    }
}
#endif


/*
 * switch from straight edges to square or vice versa
 */

void TreeWindow::toggleEdges()
//----------------------------
{
    int i;

    ASSERTION( _edgeType != optManager()->getEdgeType() );

    _edgeType = optManager()->getEdgeType();

    if( _edgeType == EdgesSquare ) {
        for( i = _roots.count(); i > 0; i -= 1 ) {
            _roots[ i - 1 ]->wrapRings();
        }
    } else {
        for( i = _roots.count(); i > 0; i -= 1 ) {
            _roots[ i - 1 ]->unWrapRings();
        }
    }

    for( i = _roots.count(); i > 0; i -= 1 ) {
        _roots[ i - 1 ]->sortEdges();
        _roots[ i - 1 ]->needsUpdating();
    }
    TreeRoot::sortRoots( _roots );
    invalidate();
}

void TreeWindow::toggleAutoArrange()
//----------------------------------
{
    ASSERTION( _autoArrange != optManager()->getTreeAutoArrange() );

    _autoArrange = optManager()->getTreeAutoArrange();
    arrangeAll();
}

/* change tree direction
 */

void TreeWindow::changeDirection()
//--------------------------------
/*
 * This is pretty much the same as arrangeAll() and
 * arrangeAllSelectRoots(), except some
 * extra work has to be done because of the change in direction.
 * Integrating the three functions would result in some code
 * size savings, but would make it pretty messy.
 */
{
    TreeDirection oldDirection;
    oldDirection  =  _direction;

    ASSERTION( _direction != optManager()->getTreeDirection() );

    /*
     * We want to keep the window positioned at roughly the same
     * place, even after changing the orientation of the tree.
     * So we save the old position and which tree was being looked at.
     * Note : use sibSep + 1 as width and height because each root's
     *        bounding rectangle is separated from other roots by sibSep
     */
    TreeRect oldPos( getXOff(), getYOff(), sibSep + 1, sibSep + 1 );
    TreeRect oldRootPos;
    TreeRoot ** ppRoot;

    ppRoot = _roots.search( &oldPos, (TComp) TreeRoot::rootRectComp );
    if( ppRoot != NULL ) {
        oldRootPos = (*ppRoot)->getBound();
    }

    _direction = optManager()->getTreeDirection();

    rePlaceAll();

    ScreenDev   dev;

    BusyNotice busy( "Arranging Graph..." );
    dev.open( this );
    TreeNode::arrangeAll( getDirection(), _roots, _world, &dev );
    dev.close();

    if( ppRoot != NULL ) {
        setCurrentPosition( *ppRoot, &oldPos, &oldRootPos, TRUE );
    } else {
        scrollTo( 0, 0 );
    }
    TreeRoot::sortRoots( _roots );

    for( int i = _roots.count(); i > 0; i -= 1 ) {
        _roots[ i - 1 ]->needsUpdating();
    }
    invalidate();
    resetScrollRange();
}

#ifdef DEBUGTREE
void TreeWindow::showDebug()
//--------------------------
{
    TreeRoot * root = _roots[ 0 ];

    for( int i = _roots.count(); i > 0; i -= 1 ) {
        if( _roots[ i - 1 ]->isRelated( _currNode ) ) {
            root = _roots[ i - 1 ];
        }
    }

    _currNode->debugInfo( root );
}

void TreeWindow::showSib()
//------------------------
{
    _currNode->sibWidth();
}
#endif

/* clear the window, re-arrange all nodes and re-display
 */

void TreeWindow::arrangeAll()
//---------------------------
{
    ScreenDev   dev;
    BusyNotice busy( "Arranging Graph..." );

    rePlaceAll();
    dev.open( this );
    TreeNode::arrangeAll( getDirection(), _roots, _world, &dev );
    dev.close();

    resetScrollRange();

    TreeRoot::sortRoots( _roots );
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        _roots[ i - 1 ]->needsUpdating();
    }
    invalidate();
}

void TreeWindow::arrangeAllSelectRoots()
//--------------------------------------
// Arranges everything, plus resets the scroll bars
{
    BusyNotice busy( "Arranging Graph..." );
    /*
     * We want to keep the window positioned at roughly the same
     * place, even after changing the orientation of the tree.
     * So we save the old position and which tree was being looked at.
     */
    TreeRect oldPos( getXOff(), getYOff(), sibSep + 1, sibSep + 1 );
    TreeRect oldRootPos;
    TreeRoot ** ppRoot;

    ppRoot = _roots.search( &oldPos, (TComp) TreeRoot::rootRectComp );
    if( ppRoot != NULL ) {
        oldRootPos = (*ppRoot)->getBound();
    }

    rePlaceAll();

    ScreenDev   dev;

    dev.open( this );
    TreeNode::arrangeAll( getDirection(), _roots, _world, &dev );
    dev.close();

    resetScrollRange();

    if( ppRoot != NULL ) {
        setCurrentPosition( *ppRoot, &oldPos, &oldRootPos, FALSE );
    } else {
        scrollTo( 0, 0 );
    }

    TreeRoot::sortRoots( _roots );
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        _roots[ i - 1 ]->needsUpdating();
    }
    invalidate();
}

void TreeWindow::setCurrentPosition( TreeRoot * pRoot, TreeRect *oldPos,
                                     TreeRect *oldRootPos,
                                     bool changeDirection )
//---------------------------------------------------------------------------
// pRoot -- pointer of root to scroll to
// oldPos -- old position before arranging tree
// oldRootPos -- old bounding rectangle of root to scroll to
// changeDirection -- TRUE if direction of trees has changed horiz.<->vert.
{
    TreeCoord nx;
    TreeCoord ny;

    if( pRoot->enabled() ) {
        TreeRect newRootPos;
        TreeCoord oldDeltaX;
        TreeCoord oldDeltaY;

        newRootPos = pRoot->getBound();

        oldDeltaX = oldPos->x() - oldRootPos->x();
        oldDeltaY = oldPos->y() - oldRootPos->y();

        if( changeDirection ) {
            nx = ( newRootPos.x() +
                       (oldDeltaY * newRootPos.w()) / oldRootPos->h() );

            ny = ( newRootPos.y() +
                       (oldDeltaX * newRootPos.h()) / oldRootPos->w() );
        } else {
            if( newRootPos.x() != oldRootPos->x() ) {
                nx = ( newRootPos.x() + (oldDeltaX * newRootPos.w())
                            / oldRootPos->w());
            } else {
                nx = newRootPos.x();
            }
            if( newRootPos.y() != oldRootPos->y() ) {
                ny = ( newRootPos.y() + (oldDeltaY * newRootPos.h())
                            / oldRootPos->h() );
            } else {
                ny = newRootPos.y();
            }
        }
    } else {
        nx = 0;
        ny = 0;
    }

    scrollTo( nx, ny );
}

bool TreeWindow::keyDown( WKeyCode code, WKeyState st )
//-----------------------------------------------------
{
    if( code == WKeyEnter ) {
        showDetail();
        return( TRUE );
    } else {
        return key( code, st );
    }
}

bool TreeWindow::paint()
//----------------------
{
    ScreenDev   dev;
    WRect       rect;
    int         i;
    bool        setRange = FALSE;       // should scroll range be set?

    dev.open( this );

    if( !_rootsLoaded ) {
        BusyNotice busy( "Loading..." );
        _rootsLoaded = TRUE;
        fillRoots();

        _empty = (_roots.count() == 0 );

        if( !_empty ) {
            BusyNotice busy( "Arranging Graph..." );
            TreeNode::arrangeAll( getDirection(), _roots, _world, &dev );
            TreeRoot::sortRoots( _roots );
            TreeRoot::addRootPtrToNodes( _roots );

            if( getSmartEdges() ) {
                for( i = _roots.count(); i > 0; i -= 1 ) {
                    _roots[ i - 1 ]->wrapRings();
                }
            }
        }

        setRange = TRUE;
    }

    _empty = TRUE;
    for( i = 0; i < _roots.count(); i += 1 ) {
        if( _roots[ i ]->enabled() ) {
            _empty = FALSE;
            break;
        }
    }

    if( _empty ) {
        dev.drawText( WPoint( 0, 0 ), emptyText() );
    } else {
        getPaintRect( rect );

        TreeRect r( getXOff() + rect.x(),
                    getYOff() + rect.y(),
                    (TreeCoord) rect.w(), (TreeCoord) rect.h() );

        TreeRoot::paint( _roots, &dev, &r );
    }

    dev.close();

    if( setRange ) {
        resetScrollRange();
    }

    return FALSE;
}

#if 0
TreeCoord TreeWindow::getMaxXOff()
//--------------------------------
// Gets the maximum x-offset on the scroll bar
{
    TreeCoord maxX;

    maxX = _world.w() / _hScrollFactor;
    if( maxX < 0 ){
        maxX = 0;
    }
    return maxX;
}

TreeCoord TreeWindow::getMaxYOff()
//--------------------------------
// Gets the maximum y-offset on the scroll bar
{
    TreeCoord maxY;

    maxY = _world.h() / _vScrollFactor;

    if( maxY < 0 ) {
        maxY = 0;
    }
    return maxY;
}

bool TreeWindow::fixOffsets()
//---------------------------
// Adjust the _xOffset and _yOffset so that we don't go off the world
{
    TreeCoord maxX;
    TreeCoord maxY;
    bool changed;

    changed = FALSE;

    maxX = _world.w() / _hScrollFactor /* getMaxXOff() */;
    maxY = _world.h() / _vScrollFactor /* getMaxYOff() */;

    if( _xOffset > maxX ) {
        _xOffset = maxX;
        changed = TRUE;
    } else if( _xOffset < 0 ) {
        _xOffset = 0;
        changed = TRUE;
    }

    if( _yOffset > maxY ) {
        _yOffset = maxY;
        changed = TRUE;
    } else if( _yOffset < 0 ) {
        _yOffset = 0;
        changed = TRUE;
    }
    return changed;
}
#endif

TreeDirection TreeWindow::getDirection() const
//--------------------------------------------
{
    return _direction;
}

bool TreeWindow::getSmartEdges() const
//------------------------------------
{
    return( _edgeType == EdgesSquare );
}

void TreeWindow::enableAll( bool enable, bool rePlace )
//-----------------------------------------------------
{
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        if( _roots[ i - 1 ]->enabled() ) {
            for( int j = _roots[ i - 1 ]->node()->getCount( TreeNode::FlatList ); j > 0; j -= 1 ) {
                TreeNode * node;
                node = _roots[ i - 1 ]->node()->getNode( TreeNode::FlatList, j - 1 );
                if( enable ) {
                    node->setEnable( TreeNode::Visible );
                } else {
                    if( node != _roots[ i - 1 ]->node() ) {
                        node->setEnable( TreeNode::Hidden );
                    }
                }
                if( rePlace ) {
                    node->rePlace();
                }
            }
        }
    }
}

void TreeWindow::rePlaceAll()
//---------------------------
{
    for( int i = _roots.count(); i > 0; i -= 1 ) {
        _roots[ i - 1 ]->rePlaceAll();
    }
}

bool TreeWindow::scrollNotify( WScrollNotification sn, int diff )
//---------------------------------------------------------------
{
    TreeCoord   xDiff = 0;
    TreeCoord   yDiff = 0;
    TreeCoord   maxY;
    TreeCoord   maxX;
    WRect       r;

    getClientRect( r );
    maxY = _world.h() - r.h();
    maxX = _world.w() - r.w();

    switch( sn ) {
    case WScrollUp:
        yDiff =  -_vScrollFactor;
        break;
    case WScrollPageUp:
        yDiff =  -r.h();
        break;
    case WScrollDown:
        yDiff = _vScrollFactor;
        break;
    case WScrollPageDown:
        yDiff =  r.h();
        break;
    case WScrollBottom:
        yDiff = _world.h() - r.h();
        break;
    case WScrollTop:
        yDiff = -getYOff();
        break;
    case WScrollVertical:
        yDiff = diff * _vScrollFactor;
        break;

    case WScrollLeft:
        xDiff =  -_hScrollFactor;
        break;
    case WScrollPageLeft:
        xDiff =  -r.w();
        break;
    case WScrollRight:
        xDiff = _hScrollFactor;
        break;
    case WScrollPageRight:
        xDiff =  r.w();
        break;
    case WScrollFullRight:
        xDiff = _world.w() - r.w();
        break;
    case WScrollFullLeft:
        xDiff = -getXOff();
        break;
    case WScrollHorizontal:
        xDiff = diff * _hScrollFactor;
        break;
    default:
        return FALSE;
    }

    scrollTo( getXOff() + xDiff, getYOff() + yDiff );

    return TRUE;
}

TreeCoord TreeWindow::getXOff() const
//-----------------------------------
{
    return _xOffset * _hScrollFactor;
}

TreeCoord TreeWindow::getYOff() const
//-----------------------------------
{
    return _yOffset * _vScrollFactor;
}

void TreeWindow::resetScrollRange()
//---------------------------------
// set the scroll range
{
    WPoint  avgChar;
    WPoint  maxChar;
    WRect   r;
    int     range;
    TreeCoord newX;
    TreeCoord newY;

    getClientRect( r );

    textMetrics( avgChar, maxChar );

    _hScrollColMult = (int) (_world.w() / SHRT_MAX + 1);
    _vScrollRowMult = (int) (_world.h() / SHRT_MAX + 1);

    _vScrollFactor = _vScrollRowMult * maxChar.y();
    _hScrollFactor = _hScrollColMult * avgChar.x();

    if( _empty ) {
        setScrollRange( WScrollBarVertical, 0 );
        setScrollRange( WScrollBarHorizontal, 0 );
        scrollTo( 0, 0 );
    } else {
        range = (int) (_world.h() / _vScrollFactor + 3);
        range += (int) ((_vScrollRowMult - 1) * r.h() / _vScrollFactor);
        if( range > r.h() / _vScrollFactor ) {
            setScrollTextRange( WScrollBarVertical, range );
            newY = minCoord( _yOffset, range );
            newY = maxCoord( newY, 0 );
        } else {
            setScrollTextRange( WScrollBarVertical, 0 );
            newY = 0;
        }

        range = (int) (_world.w() / _hScrollFactor + 3);
        range += (int) ((_hScrollColMult - 1) * r.w() / _hScrollFactor);
        if( range > r.w() / _hScrollFactor ) {
            setScrollTextRange( WScrollBarHorizontal, range );
            newX = minCoord( _xOffset, range );
            newX = maxCoord( newX, 0 );
        } else {
            setScrollTextRange( WScrollBarHorizontal, 0 );
            newX = 0;
        }

        if( _xOffset != newX || _yOffset != newY ) {
            _xOffset = newX;
            _yOffset = newY;
            GUIInitHScrollCol( handle(), (int)_xOffset );
            GUIInitVScrollRow( handle(), (int)_yOffset );
            invalidate();
        }
    }
}

void TreeWindow::scrollTo( TreeCoord x, TreeCoord y )
//---------------------------------------------------
// scroll the physical screen so the top left corner
// is roughly at x,y on the virtual screen
{
    TreeCoord   nx;
    TreeCoord   ny;
    int         diff;
    WRect       r;
    WPoint      avgChar;
    WPoint      maxChar;

    textMetrics( avgChar, maxChar );

    getClientRect( r );

    nx = x / _hScrollFactor;
    nx = minCoord( nx, getScrollTextRange( WScrollBarHorizontal ) );
    nx = maxCoord( nx, 0 );
    diff = (int) (nx - _xOffset);

    GUIInitHScrollCol( handle(), nx );
    if( nx != _xOffset ) {
        _xOffset = nx;
        if( maxCoord( diff*_hScrollFactor, -diff*_hScrollFactor ) < r.w() ) {
            scrollWindow( WScrollBarHorizontal, diff * _hScrollColMult );
        } else {
            invalidate();
        }
    }

    ny = y / _vScrollFactor;
    ny = minCoord( ny, getScrollTextRange( WScrollBarVertical ) );
    ny = maxCoord( ny, 0 );
    diff = (int) (ny - _yOffset);

    GUIInitVScrollRow( handle(), ny );
    if( ny != _yOffset ) {
        _yOffset = ny;
        if( maxCoord( diff*_vScrollFactor, -diff*_vScrollFactor ) < r.h() ) {
            scrollWindow( WScrollBarVertical, diff * _vScrollRowMult );
        } else {
            invalidate();
        }
    }

    invalidateRow( getRows() );
}

void TreeWindow::scrollToNode( TreeNode * node, WRect & rect )
//------------------------------------------------------------
// Scroll the window so that node is visible and roughly centred.
//
// node -- ptr to the node we want to scroll to
// rect -- the dimensions of the window; we're interested in width and
//         height only
{
    if( node == NULL ) return;

    TreeRect rootRect;
    TreeRect nodeRect;
    TreeCoord nx;
    TreeCoord ny;

    node->myRect( nodeRect );
    node->getRootPtr()->getBound( rootRect );

    if( getDirection() == TreeVertical ) {

        if( rootRect.h() < nodeRect.y() + rect.h() / 2 ) {
            ny = ( rootRect.h() - rect.h() );
        } else {
            ny = ( nodeRect.y() - rect.h() / 2 );
        }
        nx = ( nodeRect.x() - rect.w() / 2 );

    } else {

        ny = ( nodeRect.y() - rect.h() / 2 );

        if( rootRect.w() < nodeRect.x() + rect.w() / 2 ) {
            nx = ( rootRect.w() - rect.w() );
        } else {
            nx = ( nodeRect.x() - rect.w() / 2 );
        }
    }

    scrollTo( nx, ny );
}

TreeNode* TreeWindow::hitTest( int x, int y )
//-------------------------------------------
{
    TreeCoord   newx = x + getXOff();
    TreeCoord   newy = y + getYOff();

    return TreeRoot::hitTest( _roots, newx, newy );
}

bool TreeWindow::leftBttnDn( int x, int y, WMouseKeyFlags )
//---------------------------------------------------------
{
    TreeNode * node = hitTest( x, y );

    if( node != NULL ) {
        giveFocusToNode( node );
        selChange();
    }

    return (node != NULL);
}

bool TreeWindow::leftBttnDbl( int x, int y, WMouseKeyFlags )
//----------------------------------------------------------
{
    TreeNode * node = hitTest( x, y );

    if( node != NULL ) {
        if( _currNode != node ) {
            giveFocusToNode( node );
        } else {
            showDetail();
        }
        return TRUE;
    }
    return FALSE;
}

bool TreeWindow::rightBttnDn( int x, int y, WMouseKeyFlags )
//----------------------------------------------------------
{
    TreeNode * node = hitTest( x, y );

    if( node != NULL ) {
        giveFocusToNode( node );
        selChange();
    }

    return FALSE;
}

void TreeWindow::giveFocusToNode( TreeNode * pNode )
//--------------------------------------------------
// Takes the focus away from the current node and gives it
// to the new node.
//
// pNode -- pointer to the tree node which is getting the focus
//
{
    if( pNode == NULL ) {
        return;
    }
    if( _currNode != NULL ) {
        _currNode->losingFocus( this );
    }
    _currNode = pNode;
    _currNode->gettingFocus( this );

    selChange();
}

void TreeWindow::query()
//----------------------
{
    delete _loadFilter;
    _loadFilter = new KeySymbol( optManager()->getQueryFilt() );
    reLoad();
}

void TreeWindow::findFirst()
//--------------------------
{
    if( _queryConfig->editFilter( _findFilter ) ) {
        _findRoot = 0;
        _findNode = -1;
        findNext();
    }
}

void TreeWindow::findNext()
//-------------------------
{
    ASSERTION( _findRoot >= 0 );
    TreeNode * node;
    int        i = _findNode + 1;
    bool       found = FALSE;
    WRect      rect;
    BusyNotice busy( "Searching..." );

    for( ; _findRoot < _roots.count(); ++_findRoot ) {
        TreeRoot * root = _roots[ _findRoot ];
        for( ; i < root->node()->getCount( TreeNode::FlatList ); ++i ) {
            if( matches( _findRoot, i, _findFilter ) ) {
                _findNode = i;
                found = TRUE;
                break;
            }
        }

        if( found ) {
            break;
        } else {
            i = 0;      // start at the top of the next tree
        }
    }

    if( !found ) {
        if( _findNode == -1 ) {
            errMessage( "No matching symbols" );     // FIXME-- not an error
        } else {
            errMessage( "End of matching symbols" ); // FIXME-- not an error
        }
        _findRoot = -1;
        _findNode = -1;
        menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), FALSE );

    } else {

        node = _roots[_findRoot]->node()->getNode( TreeNode::FlatList, _findNode );

        node->enableParents( TRUE );
        arrangeAll();
        giveFocusToNode( node );
        getClientRect( rect );
        scrollToNode( node, rect );

        menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), TRUE );
    }
}

bool TreeWindow::matches( int r, int n, KeySymbol * filt )
//--------------------------------------------------------
{
    TreeRoot * root = _roots[ r ];
    TreeNode * node;

    node = root->node()->getNode( TreeNode::FlatList, n );

    return filt->matches( node->getHandle(), node->name() );
}

void TreeWindow::reLoad()
//-----------------------
{
    int i;

    for( i = 0; i < _roots.count(); i += 1 ) {
        delete _roots[ i ];
    }
    _roots.reset();

    _currNode = NULL;
    _findRoot = -1;
    _findNode = -1;

    _rootsLoaded = FALSE;
    _xOffset = 0;
    _yOffset = 0;
    invalidate();               // causes a paint, re-loads using _loadFilter
    resetScrollRange();
}
