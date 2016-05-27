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


#include <whotspot.hpp>
#include <wkeydefs.hpp>
#include <wcvector.h>

#include "assure.h"
#include "browse.h"
#include "busyntc.h"
#include "dtview.h"
#include "gtbase.h"
#include "gtrootpk.h"
#include "hotspots.h"
#include "keysym.h"
#include "menuids.h"
#include "outline.h"
#include "symbol.h"
#include "screendv.h"
#include "viewmgr.h"
#include "querycfg.h"
#include "wbrwin.h"

// FIXME -- out of general paranoia, too many resets are being done

//-------------------------- OutlineElement ----------------------------------

static MemoryPool OutlineElement::_pool( sizeof( OutlineElement ),
                                         "OutlineElement", 100 );

OutlineElement::OutlineElement()
            : _sibling( NULL )
            , _child( NULL )
            , _state( ESLeaf )
            , _symbol( NULL )
            , _lastSib( false )
            , _name( NULL )
//------------------------------
// create a sentinel node
{
}

OutlineElement::OutlineElement( int level, TreeNode * node, OutlineElement * next )
            : _sibling( next )
            , _child( NULL )
            , _symbol( node->makeSymbol() )
            , _lastSib( false )
            , _level( (uint) level )
            , _name( NULL )
//---------------------------------------------------------------------------------
{
    int                 i;
    int                 numChildren;
    OutlineElement *    prevChild = next;
    OutlineElement *    lastChild = NULL;

    _name = new char[ strlen( node->name() ) + 1 ];
    strcpy( _name, node->name() );

    numChildren = node->getCount( TreeNode::ChildList );

    for( i = numChildren; i > 0; i -= 1 ) {
        _child = new OutlineElement( level + 1, node->getNode( TreeNode::ChildList, i - 1 ),
                                    prevChild );
        if( i == numChildren ) {
            lastChild = _child;
        }

        prevChild = _child;
    }

    if( lastChild ) {
        lastChild->_lastSib = true;
        lastChild->_sibling = next;
    }

    if( numChildren > 0 ) {
        _state = ESCollapsed;
    } else {
        _state = ESLeaf;
    }
}

OutlineElement::~OutlineElement()
//-------------------------------
{
    delete _symbol;
    delete _child;
    delete [] _name;

    if( !_lastSib ) {
        delete _sibling;
    }
}

void OutlineElement::toggleExpand()
//---------------------------------
{
    if( _state == ESExpanded ) {
        collapse();
    } else {
        expand();
    }
}

void OutlineElement::expand( bool all )
//-------------------------------------
{
    OutlineElement * elm;

    if( _state != ESLeaf ) {
        _state = ESExpanded;

        if( all ) {
            for( elm = _child; elm != NULL; elm = elm->_sibling ) {
                elm->expand( all );
                if( elm->_lastSib ) {
                    break;
                }
            }
        }
    }
}

void OutlineElement::collapse( bool all )
//---------------------------------------
{
    OutlineElement * elm;

    if( _state != ESLeaf ) {
        _state = ESCollapsed;
        if( all ) {
            for( elm = _child; elm != NULL; elm = elm->_sibling ) {
                elm->collapse( all );
                if( elm->_lastSib ) {
                    break;
                }
            }
        }
    }
}

void OutlineElement::setEnable( bool en )
//---------------------------------------
{
    if( en ) {
        _state &= ~ESHidden;
    } else {
        _state |= ESHidden;
    }
}


void * OutlineElement::operator new( size_t )
//-------------------------------------------
{
    return _pool.alloc();
}

void OutlineElement::operator delete( void * ptr )
//------------------------------------------------
{
    _pool.free( ptr );
}

OutlineElement * OutlineElement::next() const
//-------------------------------------------
{
    OutlineElement * next = (state() == ESExpanded) ? _child : _sibling;

    while( next && !next->enabled() ) {
        if( next->state() == ESExpanded ) {
            next = next->_child;
        } else {
            next = next->_sibling;
        }
    }

    return next;
}

OutlineElement * OutlineElement::visibleSib()
//-------------------------------------------
{
    OutlineElement * sib;

    for( sib = _sibling; sib && !sib->enabled(); sib = sib->_sibling );

    return sib;
}

const char * OutlineElement::string() const
//-----------------------------------------
{
    return NULL;
}

void OutlineElement::drawLine( WWindow * win, int & idx,
                                int width, int height )
//----------------------------------------------------------------
{
    OutlineElement * elm;
    WPoint           start( width * _level + width / 2, height * (idx + 1) );
    WPoint           end( start );
    WOrdinal         maxY = height * win->getRows();

    idx += 1;

    if( _state == ESExpanded ) {

        for( elm = _child; elm != NULL && idx < win->getRows() + 1;
            elm = elm->visibleSib() ) {

            end = WPoint( width * elm->_level,
                          height * idx + height / 2 );

            if( end.y() < maxY ) {
                win->drawLine( WPoint( start.x(), end.y() ), end, ColorBlack );
                elm->drawLine( win, idx, width, height );
            }

            if( elm->_lastSib ) {
                break;
            }
        }

        end.x( start.x() );
        end.y( minInt( end.y(), maxY ) );

        if( start.y() < maxY ) {
            win->drawLine( start, end, ColorBlack );
        }
    }
}


//------------------------------ Outline -------------------------------------

Outline::Outline( const char * text )
        : HotWindowList( text )
        , GlobalViewTree( menuManager() )
        , _findStack( NULL )
//----------------------------------------------------------------
{
    _sentinel = new OutlineElement;
    _queryConfig = new QueryConfig( this, "Find" );
    _findFilter = new KeySymbol;
    _loadFilter = new KeySymbol( WBRWinBase::optManager()->getQueryFilt() );

    onHotPress( this, (cbw) &Outline::toggleExpand );
    onDblClick( this, (cbw) &Outline::detailView );
    onChanged( this, (cbw) &Outline::changed );

    menuManager()->trackPopup( this, MMTree );
    viewManager()->registerForEvents( this );

    resetCache();
}

Outline::~Outline()
//-----------------
{
    delete _sentinel;       // deletes all children too
    delete _queryConfig;
    delete _loadFilter;
    delete _findFilter;
}

void Outline::resetOutline()
//--------------------------
// reset to initial state
{
    delete _sentinel;       // kill all elements
    _sentinel = new OutlineElement;
    resetCache();
    if( _findStack ) {
        _findStack->clear();
    }
}

bool Outline::paint()
//-------------------
{
    OutlineElement * elm;
    int              maxRows = getRows();
    int              index = -1 * _topIndex;
    WPoint           hotSize;
    WPoint           avg;
    WPoint           max;

    textMetrics( avg, max );

    if( count() ) {
        GlobalHotSpots->hotSpotSize( OutlineLeafUp, hotSize );  // rely on all being same size

        for( elm = element( 0 ); elm != NULL; elm = elm->visibleSib() ) {
            elm->drawLine( this, index, hotSize.x(), max.y() );

            if( index >= maxRows ) {
                break;
            }
        }

        return HotWindowList::paint();
    } else {
        ScreenDev dev;

        dev.open( this );
        dev.drawText( WPoint( 0, 0 ), emptyText() );
        dev.close();

        return true;
    }
}

void Outline::resetCache()
//------------------------
{
    _cacheElement = _sentinel;
    _cacheIndex = -1;
}

int Outline::count()
//------------------
{
    OutlineElement * elm;
    int              num = 0;

    for( elm = _sentinel->next(); elm != NULL; elm = elm->next() ) {
        num += 1;
    }

    return num;
}

const char * Outline::getString( int index )
//------------------------------------------
{
    OutlineElement * elm;

    elm = element( index );

    ASSERTION( elm != NULL );

    return elm->_name;
}

int Outline::getHotSpot( int index, bool pressed )
//------------------------------------------------
{
    OutlineElement * elm;

    elm = element( index );
    REQUIRE( elm != NULL, "Outline::getHotSpot elm" );

    switch( elm->state() ) {
    case OutlineElement::ESLeaf:
        return (pressed) ? OutlineLeafDown : OutlineLeafUp;
    case OutlineElement::ESCollapsed:
        return (pressed) ? OutlineCollapsedDown : OutlineCollapsedUp;
    case OutlineElement::ESExpanded:
        return (pressed) ? OutlineExpandedDown : OutlineExpandedUp;
    }

    return 0;
}

int Outline::getHotOffset( int index )
//------------------------------------
{
    OutlineElement * elm;
    WPoint          hotSize;

    GlobalHotSpots->hotSpotSize( OutlineLeafUp, hotSize );  // rely on all being same size

    elm = element( index );
    REQUIRE( elm != NULL, "Outline::getHotSpot elm" );

    return( elm->level() * hotSize.x() );
}

OutlineElement * Outline::element( int index )
//--------------------------------------------
{
    if( _cacheIndex > index ) {
        resetCache();
    }

    while( (_cacheIndex < index) && (_cacheElement != NULL) ) {
        _cacheElement = _cacheElement->next();
        _cacheIndex += 1;
    }

    if( _cacheIndex == index ) {
        return _cacheElement;
    } else {
        return NULL;
    }
}

void Outline::addLevel0( TreeNode * node )
//----------------------------------------
{
    OutlineElement * elm;

    elm = new OutlineElement( 0, node, _sentinel->_sibling );

    _sentinel->_sibling = elm;
}

void Outline::toggleExpand( WWindow * )
//-------------------------------------
{
    OutlineElement * elm;

    elm = element( selected() );
    REQUIRE( elm != NULL, "Outline::toggleExpand" );

    if( elm->state() != OutlineElement::ESLeaf ) {
        elm->toggleExpand();
        resetCache();
        reset();    // paranoia
    }
}

void Outline::detailView( WWindow * )
//-----------------------------------
{
    OutlineElement * elm;

    elm = element( selected() );
    REQUIRE( elm != NULL, "Outline::toggleExpand" );

    popDetail( elm->_symbol );
}

bool Outline::keyDown( WKeyCode code, WKeyState state )
//----------------------------------------------------
{
    bool used;

    used = key( code, state );

    if( !used ) {
        return HotWindowList::keyDown( code, state );
    } else {
        return used;
    }
};

void Outline::event( ViewEvent ve, View * )
//-----------------------------------------
{
    switch( ve ) {
    case VEBrowseFileChange:
        resetOutline();
        load();         // re-load
        break;
    case VEQueryFiltChange:
        query();
        break;
    default:
        NODEFAULT;
    }
}

ViewEvent Outline::wantEvents()
//-----------------------------
{
    return VEQueryFiltChange | VEBrowseFileChange;
}


static MIMenuID OutlineMenus[] = {
    MIMenuID( MMLocate, LMFind ),
    MIMenuID( MMLocate, LMFindNext ),
};

#define NumOutlineMenus (sizeof(OutlineMenus) / sizeof(MIMenuID) )

void Outline::setMenus( MenuManager * mgr )
//--------------------------------------------
{
    int i;

    for( i = 0; i < NumOutlineMenus; i += 1 ) {
        mgr->registerForMenu( this, OutlineMenus[ i ] );
    }

    menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), false );

    GlobalViewTree::setMenus( mgr );

// NYI    changed( NULL );      // disable menus
}

void Outline::unsetMenus( MenuManager * mgr )
//-------------------------------------------
{
    int i;

    for( i = 0; i < NumOutlineMenus; i += 1 ) {
        mgr->unRegister( OutlineMenus[ i ] );
    }
    GlobalViewTree::unsetMenus( mgr );
}

void Outline::menuSelected( const MIMenuID & id )
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
        default:
            NODEFAULT;
        }
        break;
    default:
        GlobalViewTree::menuSelected( id );
    }
}


void Outline::treeViewEvent( TreeViewEvent event )
//------------------------------------------------
{
    OutlineElement * elm;

    elm = element( selected() );

    switch( event ) {
    case ShowDetail:
        if( elm && elm->_symbol ) {
            popDetail( elm->_symbol );
        }
        break;
    case ShowDefinition:
        if( elm && elm->_symbol ) {
            browseTop->showSource( elm->_symbol );
        }
        break;
    case ShowReferences:
        if( elm && elm->_symbol ) {
            viewManager()->showDetailView( elm->_symbol, DetailView::ViewSourceRefs );
        }
        break;
    case ShowUsers:
        if( elm && elm->_symbol ) {
            viewManager()->showDetailView( elm->_symbol, DetailView::ViewSymbolRefs );
        }
        break;
    case ScrollToSelected:
        scrollToSelected();
        break;
    case SelectRoots:
        selectRoots();
        break;
    case ExpandOne:
        elm->expand( false );
        resetCache();
        reset();    // paranoia
        break;
    case ExpandBranch:
        elm->expand( true );
        resetCache();
        reset();    // paranoia
        break;
    case ExpandAll:
        for( elm = _sentinel->next(); elm != NULL; elm = elm->visibleSib() ) {
            elm->expand( true );
        }
        resetCache();
        reset();    // paranoia
        break;
    case CollapseBranch:
        if( elm->state() == OutlineElement::ESExpanded ) {
            elm->collapse( false );
            resetCache();
            reset();    // paranoia
        }
        break;
    case CollapseAll:
        for( elm = _sentinel->next(); elm != NULL; elm = elm->visibleSib() ) {
            elm->collapse( true );
        }
        resetCache();
        reset();    // paranoia
        break;
    default:
        NODEFAULT;
    }
}

void Outline::selectRoots()
//-------------------------
{
    OutlineRootSelect sel( this, _sentinel );

    if( sel.process() ) {
        scrollToSelected();
        resetCache();
        reset();
    }
}

GlobalViewTree::ExpandState Outline::state()
//------------------------------------------
{
    // convert from an OutlineElement::ElementState
    static ExpandState cvt[ 3 ] = { Leaf, Collapsed, Expanded };

    OutlineElement * elm;

    elm = element( selected() );

    if( elm == NULL ) {
        return NoSymbol;
    } else {
        return cvt[ elm->state() ];
    }
}

void Outline::findFirst()
//-----------------------
{
    if( _queryConfig->editFilter( _findFilter ) ) {
        if( _findStack == NULL ) {
            _findStack = new WCPtrOrderedVector<OutlineElement>;
        }

        _findStack->clear();
        _findStack->append( _sentinel );
        findNext();
    }
}

void Outline::findNext()
//----------------------
{
    BusyNotice busy( "Searching..." );
    OutlineElement * elm;
    int              i;

    elm = _findStack->removeLast();

    while( elm != NULL ) {
        _findStack->append( elm );
        if( elm->_child ) {
            elm = elm->_child;
        } else {
            elm = _findStack->removeLast(); // remove self from stack -- done

            while( elm->_lastSib ) {        // remove finished parents
                elm = _findStack->removeLast();
            }
            elm = elm->_sibling;
        }
        if( elm && _findFilter->matches( elm->symbol() ) ) {
            _findStack->append( elm );
            break;
        }
    }

    if( !elm ) {
        errMessage( "Not Found" );      // FIXME -- not an error, don't use errMessage
        _findStack->clear();
        menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), false );
    } else {
        // expand all the parent nodes of this one (they are stored
        // in the find stack), but don't expand this node itself
        for( i = _findStack->entries() - 1; i > 0; i -= 1 ) {
            (*_findStack)[ i - 1 ]->expand( false );
        }
        for( i = 0; i < count(); i += 1 ) {
            if( elm == element( i ) ) {
                select( i );
            }
        }
        resetCache();
        reset();
        scrollToSelected();
        menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), true );
    }
}

void Outline::query()
//-------------------
{
    delete _loadFilter;
    _loadFilter = new KeySymbol( optManager()->getQueryFilt() );
    resetOutline();
    load();             // re-load using new filters
    reset();            // paranoia
}
