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


#include <wcvector.h>

#include "wstd.h"
#include "assure.h"
#include "browse.h"
#include "gbvlist.h"
#include "gbvolinh.h"
#include "gbvolcll.h"
#include "gtclass.h"
#include "gtfunc.h"
#include "util.h"
#include "menuids.h"

#include "viewmgr.h"

//------------------------- EventReceiver -----------------------------------

ViewManager::EventReceiver::EventReceiver()
                : _receiver( NULL )
                , _wantsEvents( VENoEvent )
//-----------------------------------------
{
}

ViewManager::EventReceiver::EventReceiver( const EventReceiver & o )
                : _receiver( o._receiver )
                , _wantsEvents( o._wantsEvents )
//------------------------------------------------------------------
{
}

ViewManager::EventReceiver::EventReceiver( ViewEventReceiver * rec )
                : _receiver( rec )
                , _wantsEvents( rec->wantEvents() )
//------------------------------------------------------------------
{
}

bool ViewManager::EventReceiver::operator == ( const EventReceiver & o ) const
//----------------------------------------------------------------------------
{
    return( _receiver == o._receiver );
}

ViewManager::EventReceiver &
ViewManager::EventReceiver::operator=( const EventReceiver & o )
//--------------------------------------------------------------
{
    _receiver = o._receiver;
    _wantsEvents = o._wantsEvents;

    return *this;
}


//------------------------- ViewManager -----------------------------------

ViewManager::ViewManager()
        : _hasFocus( NULL )
        , _menuManager( NULL )
//----------------------------
{
    _detailViews = new WCPtrOrderedVector<DetailView>;
    _eventReceivers = new WCValOrderedVector<EventReceiver>;
    _killList = new WCValOrderedVector<View *>;
}

ViewManager::~ViewManager()
//-------------------------
{
    _detailViews->clear();
    delete _detailViews;

    _eventReceivers->clear();
    delete _eventReceivers;

    kill();
    delete _killList;
}

void ViewManager::kill()
//----------------------
{
    int         i;

    for( i = _killList->entries(); i > 0; --i ) {
        View *tmp = (*_killList)[i-1];
        _killList->removeLast();
        delete tmp;
    }
}


static MIMenuID ViewManagerMenus[] = {
    MIMenuID( MMView,           VMList ),
    MIMenuID( CMViewInherit,    VMInheritTree ),
    MIMenuID( CMViewInherit,    VMInheritOutline ),
    MIMenuID( CMViewCall,       VMCallTree ),
    MIMenuID( CMViewCall,       VMCallOutline ),
};

#define NumViewMgrMenus ( sizeof( ViewManagerMenus ) / sizeof( MIMenuID ) )

void ViewManager::setMenus( MenuManager * mgr )
//---------------------------------------------
{
    int i;

    _menuManager = mgr;

    for( i = 0; i < NumViewMgrMenus; i += 1 ) {
        _menuManager->registerForMenu( this, ViewManagerMenus[ i ] );
    }
}

void ViewManager::unsetMenus()
//----------------------------
{
    int i;

    for( i = 0; i < NumViewMgrMenus; i += 1 ) {
        _menuManager->unRegister( ViewManagerMenus[ i ] );
    }
}

void ViewManager::enableMenus( bool enabled )
//-------------------------------------------
{
    int i;

    if( _menuManager ) {
        for( i = 0; i < NumViewMgrMenus; i += 1 ) {
            _menuManager->enableMenu( ViewManagerMenus[ i ], enabled );
        }
    }
}


void ViewManager::menuSelected( const MIMenuID & id )
//---------------------------------------------------
{

    switch( id.mainID() ) {
    case MMView:
        switch( id.subID() ) {
        case VMList:
            showGlobalView( GlobalView::GlobalViewSymbolList );
            break;
        };
        break;

    case CMViewInherit:
        switch( id.subID() ) {
        case VMInheritTree:
            showGlobalView( GlobalView::GlobalViewInheritTree );
            break;
        case VMInheritOutline:
            showGlobalView( GlobalView::GlobalViewInheritOutline );
            break;
        };
        break;

    case CMViewCall:
        switch( id.subID() ) {
        case VMCallTree:
            showGlobalView( GlobalView::GlobalViewCallTree );
            break;

        case VMCallOutline:
            showGlobalView( GlobalView::GlobalViewCallOutline );
            break;
        }
        break;

    }
}

DetailView * ViewManager::findDetailView( const Symbol * sym,
                                            DetailView::ViewType vt )
//-------------------------------------------------------------------
{
    return findDetailView( sym->getHandle(), vt );
}

DetailView * ViewManager::findDetailView( dr_handle hdl, DetailView::ViewType vt )
//--------------------------------------------------------------------------------
{
    int             i;
    DetailView *    testView;

    for( i = 0; i < _detailViews->entries(); i += 1 ) {
        testView = (*_detailViews)[ i ];

        if( ( testView->symHandle() == hdl ) && ( testView->viewType() == vt ) ) {
            return testView;
        }
    }

    return NULL;
}

void ViewManager::showDetailView( const Symbol * sym, DetailView::ViewType vt )
//-----------------------------------------------------------------------------
{
    DetailView * dtv;

    dtv = findDetailView( sym, vt );

    if( !dtv ) {
        dtv = DetailView::createView( sym, vt );

        #if DEBUG   // FIXME -- TAKE OUT AS SOON AS TEXTREE, STRUCVIEW ARE VIEWS
        if( dtv == NULL ) {
            return;
        }
        #endif

        _detailViews->append( dtv );

        eventOccured( VECreate, dtv );
        eventOccured( VEGettingFocus, dtv );
    }

    REQUIRE( dtv != NULL, "ViewManager::showDetailView -- dtv == NULL" );

    dtv->showWin( WWinStateShowNormal );
    dtv->setFocusWin();
}

void ViewManager::showGlobalView( GlobalView::GlobalViewType vt )
//---------------------------------------------------------------
{
    GlobalView * gbv;

    gbv = GlobalView::createView( vt );

    REQUIRE( gbv != NULL, "ViewManager::showGlobalView -- gbv == NULL" );

    eventOccured( VECreate, gbv );
    eventOccured( VEGettingFocus, gbv );
}

void ViewManager::registerForEvents( ViewEventReceiver * rec )
//------------------------------------------------------------
{
    _eventReceivers->append( EventReceiver( rec ) );
}

void ViewManager::unregister( ViewEventReceiver * rec )
//-----------------------------------------------------
{
    _eventReceivers->remove( EventReceiver( rec ) );
}

void ViewManager::sendEvent( ViewEvent ve, View * view )
//------------------------------------------------------
{
    int i;

    for( i = 0; i < _eventReceivers->entries(); i += 1 ) {
        EventReceiver & er( (*_eventReceivers)[ i ] );

        if( er._wantsEvents & ve ) {
            er._receiver->event( ve, view );
        }
    }
}

void ViewManager::eventOccured( ViewEvent ve, View * view )
//---------------------------------------------------------
// call kill on VEGettingFocus and VELosingFocus events
// since under OS/2, these occur before the window is
// destroyed!!
{
    int i;

    // perform special actions on some events //
    switch( ve ) {
    case VEGettingFocus:
        if( view != _hasFocus ) {
            _hasFocus = view;
            sendEvent( ve, view );
        }
        break;

    case VELosingFocus:
        if( view == _hasFocus ) {
            sendEvent( ve, view );
        }
        _hasFocus = NULL;
        break;

    case VEBrowseFileChange:
        /* the loop has to be in reverse order since the aboutToClose
         * call removes the receiver from the array
         */
        for( i = _detailViews->entries(); i > 0; i -= 1 ) {
            DetailView * dtv = (*_detailViews)[ i - 1 ];

            dtv->aboutToClose();
        }
        sendEvent( ve, view );
        kill();

        break;

    default:
        sendEvent( ve, view );
        kill();
    }
}

void ViewManager::viewDying( View * view )
//----------------------------------------
{
    if( view->identity() == VIDetailView ) {
        _detailViews->remove( (DetailView *) view );
    }

    unregister( view );

    eventOccured( VELosingFocus, view );
    eventOccured( VEClose, view );

    _killList->append( view );      // will be deleted next event
}

