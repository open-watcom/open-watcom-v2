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


#include <wvlist.hpp>
#include <wcmdlist.hpp>

#include "assure.h"
#include "busyntc.h"
#include "browse.h"
#include "dbmgr.h"
#include "death.h"
#include "dtview.h"
#include "gbvlist.h"
#include "hotspots.h"
#include "icons.h"
#include "keysym.h"
#include "menuids.h"
#include "module.h"
#include "optmgr.h"
#include "querycfg.h"
#include "screendv.h"
#include "symbol.h"
#include "util.h"
#include "viewmgr.h"
#include "brwhelp.h"

class GBVListPage : public WObject
{
public:
                     GBVListPage( SearchContext * ctxt, WVList * list );
                    ~GBVListPage();

    bool            operator==( const GBVListPage & ) const { return FALSE; }
    bool            operator==( WObject const & ) const { return FALSE; }

    SearchContext * _context;
    WVList *        _symbols;
};

// use a page size of 128

#define OFFSHIFT        (5)
#define IDXTOPAGE( a )  (((int)a) >> OFFSHIFT)
#define OFFSETMASK      (0x1f)
#define IDXTOOFF( a )   (((int)a) & OFFSETMASK)

GBVListPage::GBVListPage( SearchContext * ctxt, WVList * list )
            : _context( ctxt )
            , _symbols( list )
//-------------------------------------------------------------
{
}

GBVListPage::~GBVListPage()
//-------------------------
{
    _symbols->deleteContents();
    delete _symbols;
    delete _context;
}

GlobalViewList::GlobalViewList( const char * text )
            : HotWindowList( text, TRUE,
                             WBRWinStyleDefault
                                | (WStyleVScroll | WStyleVRows )
                                | WStyleVRows
                                | WStyleVScrollEvents
                                | WStyleHScrollAll )
            , _full( FALSE )
//-------------------------------------------------------------------
{
    _findFilter = new KeySymbol;
    _queryConfig = new QueryConfig( _findFilter, this, "Find" );
    _pages = new WVList;

    viewManager()->registerForEvents( this );
    onDblClick( this, (cbw) &GlobalViewList::toggleDetail );
    onHotPress( this, (cbw) &GlobalViewList::toggleDetail );
    onChanged( this, (cbw) &GlobalViewList::changed );
    setIcon( GBList );

    reLoad();
    show();
}

GlobalViewList::~GlobalViewList()
//-------------------------------
{
    _pages->deleteContents();
    delete _pages;
    delete _findFilter;
    delete _queryConfig;
}

bool GlobalViewList::contextHelp( bool is_active_win )
//---------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_LIST_VIEW );
    }
    return( TRUE );
}

bool GlobalViewList::reallyClose()
//--------------------------------
{
    viewManager()->viewDying( this );

    return TRUE;
}

bool GlobalViewList::gettingFocus( WWindow * )
//--------------------------------------------
{
    viewManager()->eventOccured( VEGettingFocus, this );
    return FALSE;
}

bool GlobalViewList::losingFocus( WWindow * )
//-------------------------------------------
{
    viewManager()->eventOccured( VELosingFocus, this );
    return FALSE;
}

bool GlobalViewList::paint()
//--------------------------
{
    if( getSymbol( 0 ) ) {
        return HotWindowList::paint();
    } else {
        ScreenDev dev;

        dev.open( this );
        dev.drawText( WPoint(0,0), "No symbols found with current query" );
        dev.close();
        return TRUE;
    }
}

void GlobalViewList::event( ViewEvent ve, View * view )
//-----------------------------------------------------
{
    if( ve == VEClose || ve == VECreate ) {
        if( view->identity() == VIDetailView ) {
            int             i;
            int             maxRows = getRows() + _topIndex;
            DetailView *    dtv = (DetailView *) view;
            dr_handle       handle = dtv->symHandle();

            for( i = _topIndex; i < maxRows; i += 1 ) {
                Symbol * sym = getSymbol( i );

                if( sym == NULL ) {
                    break;
                }
                if( handle == sym->getHandle() ) {
                    invalidateRow( i - _topIndex );
                    break;
                }
            }
        }
    } else {
        switch( ve ) {
        case VEBrowseFileChange:
        case VEQueryFiltChange:
            reLoad();
            break;
        }
    }
}

ViewEvent GlobalViewList::wantEvents()
//------------------------------------
{
    return VEBrowseFileChange | VEQueryFiltChange | VEClose | VECreate;
}

static MIMenuID ListMenus[] = {
    MIMenuID( MMDetail, DMDetail ),
    MIMenuID( MMDetail, DMDefinition ),
    MIMenuID( MMDetail, DMUsers ),
    MIMenuID( MMDetail, DMReferences ),

    MIMenuID( MMLocate, LMFind ),
    MIMenuID( MMLocate, LMFindNext ),
    MIMenuID( MMLocate, LMFindSelected ),
};

#define NumListMenus (sizeof(ListMenus) / sizeof(MIMenuID) )

void GlobalViewList::setMenus( MenuManager * mgr )
//------------------------------------------------
{
    int i;

    for( i = 0; i < NumListMenus; i += 1 ) {
        mgr->registerForMenu( this, ListMenus[ i ] );
    }

    menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), FALSE );

    changed( NULL );    // disable menus
}

void GlobalViewList::unsetMenus( MenuManager * mgr )
//--------------------------------------------------
{
    int i;

    for( i = 0; i < NumListMenus; i += 1 ) {
        mgr->unRegister( ListMenus[ i ] );
    }
}

void GlobalViewList::menuSelected( const MIMenuID & id )
//------------------------------------------------------
{
    switch( id.mainID() ) {
    case MMDetail:
        if( selected() >= 0 ) {
            Symbol * sym = getSymbol( selected() );
            switch( id.subID() ) {
            case DMDetail:
                popDetail( sym );
                break;
            case DMDefinition:
                browseTop->showSource( sym );
                break;
            case DMReferences:
                viewManager()->showDetailView( sym, DetailView::ViewSourceRefs );
                break;
            case DMUsers:
                viewManager()->showDetailView( sym, DetailView::ViewSymbolRefs );
                break;
            }
        }
        break;
    case MMLocate:
        switch( id.subID() ) {
        case LMFind:
            findFirst();
            break;

        case LMFindNext:
            findNext();
            break;

        case LMFindSelected:
            scrollToSelected();
            break;
        }
        break;
    }
}

void GlobalViewList::findFirst()
//------------------------------
{
    Symbol * sym;
    BusyNotice busy( "Searching..." );

    if( _queryConfig->process() ) {
        for( _lastFound = 0; ; _lastFound ++ ) {
            sym = getSymbol( _lastFound );

            if( sym == NULL || _findFilter->matches( sym ) ) {
                break;
            }
        }

        if( sym == NULL ) {
            errMessage( "Matching symbol not found" );
            menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), FALSE );
            _lastFound = -1;
        } else {
            select( _lastFound );
            menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), TRUE );
        }
    }
}

void GlobalViewList::findNext()
//-----------------------------
{
    Symbol * sym;
    BusyNotice busy( "Searching..." );

    ASSERTION( _lastFound >= 0 );

    for( _lastFound += 1; ; _lastFound += 1 ) {
        sym = getSymbol( _lastFound );
        if( sym == NULL || _findFilter->matches( sym ) ) {
            break;
        }
    }

    if( sym == NULL ) {
        errMessage( "No more matching symbols" );
        menuManager()->enableMenu( MIMenuID(MMLocate,LMFindNext), FALSE );
        _lastFound = -1;
    } else {
        select( _lastFound );
    }
}

int GlobalViewList::count()
//-------------------------
{
    ASSERTION( full() );

    int           num;
    GBVListPage * page;

    num = (_pages->count() - 1) * (OFFSETMASK + 1);

    if( _pages->count() ) {
        page = (GBVListPage *) (*_pages)[ _pages->count() - 1 ];
        num += page->_symbols->count();
    }

    return num;
}


Symbol * GlobalViewList::getSymbol( int index )
//---------------------------------------------
// FIXME -- have to cast away constness
{
    int             page = IDXTOPAGE( index );
    int             off = IDXTOOFF( index );
    Module *        mod = dbManager()->module();
    KeySymbol *     key = (KeySymbol *) &optManager()->getQueryFilt();
    SearchContext * prev;
    SearchContext * ctxt;
    WVList *        list;
    GBVListPage *   prevPage;
    GBVListPage *   currPage;

    if( index < 0 ) {
        return NULL;
    }

    try {
        while( !full() && _pages->count() <= page ) {
            if( page < 0 || page > _pages->count() ) {
                return NULL;
            } else {
                prevPage = page ? (GBVListPage *)(*_pages)[ page - 1 ] : NULL;
                prev = prevPage ? prevPage->_context : NULL;
            }
            list = new WVList;
            ctxt = mod->findSymbols( list, key, prev, OFFSETMASK + 1 );
            _pages->add( new GBVListPage( ctxt, list ) );

            if( ctxt == NULL ) {
                _full = TRUE;
            }
        }

        if( page < _pages->count() ) {
            currPage = (GBVListPage *)(*_pages)[ page ];
            if( off < currPage->_symbols->count() ) {
                return (Symbol *) (*currPage->_symbols)[ off ];
            }
        }

    } catch( CauseOfDeath cause ) {
        IdentifyAssassin( cause );
    }

    return NULL;
}

const char * GlobalViewList::getString( int index )
//-------------------------------------------------
{
    Symbol * sym = getSymbol( index );

    return sym ? sym->scopedName() : NULL;
}

int GlobalViewList::getHotSpot( int index, bool pressed )
//-------------------------------------------------------
{
    Symbol * sym = getSymbol( index );

    return sym ? sym->getHotSpot( pressed ) : InvalidHotSpot;
}

void GlobalViewList::reLoad()
//---------------------------
{
    _pages->deleteContents();
    _full = FALSE;
    _topIndex = 0;
    _selected = -1;
    reset();
}

void GlobalViewList::toggleDetail( WWindow * )
//--------------------------------------------
{
    WRect           rect;
    int             sel = selected();
    Symbol *        sym;
    DetailView *    dtv;

    sym = getSymbol( sel );
    if( sym ) {
        dtv = viewManager()->findDetailView( sym, DetailView::ViewDetail );

        if( !dtv ) {
            popDetail( sym );
        } else {
            dtv->aboutToClose();
            viewManager()->kill();
        }
    }
}

void GlobalViewList::changed( WWindow * )
//---------------------------------------
{
    int      sel = selected();
    bool     enable = FALSE;
    Symbol * sym = getSymbol( sel );

    if( sym ) {
        enable = TRUE;
    }

    menuManager()->enableMenu( MIMenuID( MMDetail, DMDetail ), enable );
    menuManager()->enableMenu( MIMenuID( MMDetail, DMDefinition ),
                                enable && browseTop->canEdit() );
    menuManager()->enableMenu( MIMenuID( MMDetail, DMReferences ), enable );
    menuManager()->enableMenu( MIMenuID( MMDetail, DMUsers ), enable );

    menuManager()->enableMenu( MIMenuID( MMLocate, LMFindSelected ), enable );
}
