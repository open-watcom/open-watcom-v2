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
* Description:  Window class definition.
*
****************************************************************************/

#include <ctype.h>

#include "wwindow.hpp"
#include "wvlist.hpp"
#include "wmenu.hpp"
#include "wcontrol.hpp"
#include "wpopmenu.hpp"
#include "wmenuitm.hpp"
#include "wwinmain.hpp"
#include "wkeydefs.hpp"
#include "wtoolbar.hpp"
#include "wbaritem.hpp"
#include "wmetrics.hpp"

/***************************************************************************/

WObjectMap      WWindow::_idMap;
WObjectMap      WWindow::_toolBarIdMap;
WObjectMap      WWindow::_popupIdMap;
unsigned        WWindow::_idMaster = 1;

/***************************************************************************/

WCLASS AccelKey : public WObject {
public:
    AccelKey( WKeyCode key, WObject* client, bcbk callback );
    ~AccelKey( void ) {}

    bool callClient( WKeyCode kc ) { return( (_client->*_callback)( kc ) ); }

    WKeyCode _key;
    WObject  *_client;
    bcbk     _callback;
};

AccelKey::AccelKey( WKeyCode key, WObject* client, bcbk callback )
    : _key( key )
    , _client( client )
    , _callback( callback )
{
}

/***************************************************************************/

bool WEXPORT WWindow::processMsg( gui_event msg, void *parm )
{
    gui_point           point;
    gui_coord           size;
    unsigned            control_id;
    int                 scroll_position;

    switch( msg ) {
    case GUI_CLICKED: {
        GUI_GETID( parm, control_id );
        WMenuItem* menu = (WMenuItem*)WWindow::_idMap.findThis( (WHANDLE)control_id );
        if( menu != NULL ) {
            menu->picked();
            return( TRUE );
        }
        // a popup menu with no menu items will generate GUI_CLICKED
        // - simulate a GUI_INITMENUPOPUP
        WPopupMenu* pop = (WPopupMenu*)WWindow::_popupIdMap.findThis( (WHANDLE)control_id );
        if( pop != NULL ) {
            pop->popup();
            return( TRUE );
        }
        WToolBarItem* tool =(WToolBarItem*)WWindow::_toolBarIdMap.findThis( (WHANDLE)control_id );
        if( tool != NULL ) {
            tool->picked();
            return( TRUE );
        }
    }
    case GUI_CONTROL_CLICKED:
    case GUI_CONTROL_DCLICKED: {
        GUI_GETID( parm, control_id );
        WControl* control = getControl( control_id );
        if( control != NULL ) {
            return( control->processMsg( msg ) );
        } else {
            return( FALSE );
        }
    }
    case GUI_LBUTTONUP: {
        GUI_GET_POINT( parm, point );
        return( leftBttnUp( point.x, point.y, 0 ) );
    }
    case GUI_LBUTTONDOWN: {
        GUI_GET_POINT( parm, point );
        return( leftBttnDn( point.x, point.y, 0 ) );
    }
    case GUI_LBUTTONDBLCLK: {
        GUI_GET_POINT( parm, point );
        return( leftBttnDbl( point.x, point.y, 0 ) );
    }
    case GUI_RBUTTONUP: {
        GUI_GET_POINT( parm, point );
        return( rightBttnUp( point.x, point.y, 0 ) );
    }
    case GUI_CONTROL_RCLICKED: {
        GUI_GETID( parm, control_id );
        WControl* control = getControl( control_id );
        if( control != NULL ) {
            return( control->rightBttnUp( 0, 0, 0 ) );
        } else {
            return FALSE;
        }
    }
    case GUI_RBUTTONDOWN: {
        GUI_GET_POINT( parm, point );
        return( rightBttnDn( point.x, point.y, 0 ) );
    }
    case GUI_RBUTTONDBLCLK: {
        GUI_GET_POINT( parm, point );
        return( rightBttnDbl( point.x, point.y, 0 ) );
    }
    case GUI_MOUSEMOVE: {
        GUI_GET_POINT( parm, point );
        return( mouseMove( point.x, point.y, 0 ) );
    }
    case GUI_VSCROLL_NOTIFY: {
        return( scrollPosChanged( WScrollBarVertical ) );
    }
    case GUI_HSCROLL_NOTIFY: {
        return( scrollPosChanged( WScrollBarHorizontal ) );
    }
    case GUI_NOW_ACTIVE: {
        return( gettingFocus( NULL ) );
    }
    case GUI_NOT_ACTIVE: {
        return( losingFocus( NULL ) );
    }
    case GUI_KEYDOWN: {
        _keyHandled = FALSE;
        WWindow *p = this;
        while( p != NULL ) {
            _keyHandled = p->keyDown( ((gui_key_state *)parm)->key,
                                      ((gui_key_state *)parm)->state );
            if( _keyHandled ) break;
            p = p->parent();
        }
        return( _keyHandled );
    }
    case GUI_KEY_CONTROL: {
        return( FALSE );
    }
    case GUI_KEYUP: {
        // we don't care about GUI_KEYUP messages; however we want to
        // return whether the GUI_KEYDOWN message was handled or not
        return( _keyHandled );
    }
    case GUI_CLOSE: {
        return( reallyClose() );
    }
    case GUI_DESTROY: {
        setHandle( NULL );
        return( TRUE );
    }
    case GUI_ICONIFIED: {
        minimized();
        return( TRUE );
    }
    case GUI_MOVE:
        // BobP removed because it caused size problems
        // * could not move window to second monitor
        // * Window would jump
        // * project window would shrink
        //moved( 0, 0 );
        enumChildren();
        return( TRUE );
    case GUI_RESIZE: {
        GUI_GET_SIZE( parm, size );
        resized( size.x, size.y );
        enumChildren();
        return( TRUE );
    }
    case GUI_PAINT: {
        _painting = TRUE;
        GUI_GET_ROWS( parm, _firstDirtyRow, _numDirtyRows );
        bool ret = paint();
        _firstDirtyRow = 0;
        _numDirtyRows = 0;
        _painting = FALSE;
        return( ret );
    }
    case GUI_INITMENUPOPUP: {
        GUI_GETID( parm, control_id );
        WPopupMenu *pop = (WPopupMenu *)WWindow::_popupIdMap.findThis( (WHANDLE)control_id );
        pop->popup();
        return( TRUE );
    }
    case GUI_TOOLBAR_DESTROYED:
        if( !_toolBar->changed( WToolBarClosed ) ) {
            clearToolBar();
        }
        return( TRUE );
    case GUI_TOOLBAR_FLOATING:
        if( !_toolBar->changed( WToolBarFloating ) ) {
            enumChildren();
        }
        return( TRUE );
    case GUI_TOOLBAR_FIXED:
        if( !_toolBar->changed( WToolBarFixed ) ) {
            enumChildren();
        }
        return( TRUE );
    case GUI_SCROLL_UP:
        return( scrollNotify( WScrollUp, 0 ) );
    case GUI_SCROLL_PAGE_UP:
        return( scrollNotify( WScrollPageUp, 0 ) );
    case GUI_SCROLL_DOWN:
        return( scrollNotify( WScrollDown, 0 ) );
    case GUI_SCROLL_PAGE_DOWN:
        return( scrollNotify( WScrollPageDown, 0 ) );
    case GUI_SCROLL_TOP:
        return( scrollNotify( WScrollTop, 0 ) );
    case GUI_SCROLL_BOTTOM:
        return( scrollNotify( WScrollBottom, 0 ) );
    case GUI_SCROLL_VERTICAL:
        GUI_GET_SCROLL( parm, scroll_position );
        return( scrollNotify( WScrollVertical, scroll_position ) );
    case GUI_SCROLL_LEFT:
        return( scrollNotify( WScrollLeft, 0 ) );
    case GUI_SCROLL_PAGE_LEFT:
        return( scrollNotify( WScrollPageLeft, 0 ) );
    case GUI_SCROLL_RIGHT:
        return( scrollNotify( WScrollRight, 0 ) );
    case GUI_SCROLL_PAGE_RIGHT:
        return( scrollNotify( WScrollPageRight, 0 ) );
    case GUI_SCROLL_FULL_LEFT:
        return( scrollNotify( WScrollFullLeft, 0 ) );
    case GUI_SCROLL_FULL_RIGHT:
        return( scrollNotify( WScrollFullRight, 0 ) );
    case GUI_SCROLL_HORIZONTAL:
        GUI_GET_SCROLL( parm, scroll_position );
        return( scrollNotify( WScrollHorizontal, scroll_position ) );
    case GUI_STATUS_CLEARED:
        return( statusWindowCleared() );
    case GUI_QUERYENDSESSION:
        return( !queryEndSession() );
    case GUI_ENDSESSION:
        bool ending;
        GUI_GET_BOOL( parm, ending );
        endSession( ending );
    case GUI_ACTIVATEAPP:
        bool activated;
        GUI_GET_BOOL( parm, activated );
        return( appActivate( activated ) );
    case GUI_CONTEXTHELP:
        bool isactwin;
        GUI_GET_BOOL( parm, isactwin );
        return( contextHelp( isactwin ) );
    }
    return( TRUE );
}


extern "C" void EnumChildProc( gui_window *hwin, void * ) {
/*********************************************************/

    WWindow *win = (WWindow*)GUIGetExtra( hwin );
    win->autosize();
}


extern "C" void EnumControlProc( gui_window *hwin, unsigned id, void * ) {
/************************************************************************/

    WWindow *win = (WWindow*)GUIGetExtra( hwin );
    WControl* control = win->getControl( id );
    if( control != NULL ) {
        control->autosize();
    }
}


void WWindow::enumChildren() {
/****************************/

    GUIEnumChildWindows( _handle, EnumChildProc, NULL );
    GUIEnumControls( _handle, EnumControlProc, NULL );
}


extern "C" bool WinProc( gui_window *hwin, gui_event msg, void *parm ) {
/**********************************************************************/

    WWindow* win = (WWindow *)GUIGetExtra( hwin );
    if( msg == GUI_INIT_WINDOW ) {
        win->setHandle( hwin );
    }
    return( win->processMsg( msg, parm ) );
}


bool WWindow::mouseMove( int, int, WMouseKeyFlags ) {
/***************************************************/

    GUISetMouseCursor( _currCursor );
    return( FALSE );
}


void WWindow::makeWindow( const char *text, WStyle style, WExStyle exstyle ) {
/****************************************************************************/

    gui_create_info     create_info;
    unsigned long       gui_style;

    WRect r;
    autoPosition( r );
    gui_window *hparent = NULL;
    if( _parent != NULL ) {
        hparent = _parent->_handle;
    }
    create_info.text = (char *)text;
    create_info.rect.x = r.x();
    create_info.rect.y = r.y();
    create_info.rect.width = r.w();
    create_info.rect.height = r.h();
    create_info.scroll = _WStyleToScrollStyle( style );
    gui_style = GUI_INIT_INVISIBLE | GUI_VISIBLE | _WStyleToCreateStyle( style );
    gui_style |= exstyle;
    create_info.style = (gui_create_styles)gui_style;
    create_info.parent = hparent;
    create_info.num_menus = 0;
    create_info.menu = NULL;
    create_info.num_attrs = 0;
    create_info.colours = NULL;
    create_info.call_back = WinProc;
    create_info.extra = this;
    create_info.icon = NULL;
    _handle = GUICreateWindow( &create_info );
    if( _parent != NULL ) {
        _parent->addChild( this );
    }
}


WEXPORT WWindow::WWindow( WWindow *parent )
    : _painting( FALSE )
    , _firstDirtyRow( 0 )
    , _numDirtyRows( 0 )
    , _currCursor( GUI_ARROW_CURSOR )
    , _parent( parent )
    , _menu( NULL )
    , _popup( NULL )
    , _toolBar( NULL )
    , _handle( NULL ) {
/*********************/

}

WEXPORT WWindow::WWindow( const char *text, WStyle style, WExStyle exstyle )
    : _painting( FALSE )
    , _firstDirtyRow( 0 )
    , _numDirtyRows( 0 )
    , _currCursor( GUI_ARROW_CURSOR )
    , _parent( NULL )
    , _menu( NULL )
    , _popup( NULL )
    , _toolBar( NULL )
    , _handle( NULL ) {
/*********************/

    WSystemMetrics::defaultRectangle( _autosize );
    makeWindow( text, style, exstyle );
}


WEXPORT WWindow::WWindow( WWindow *parent, const char *text, WStyle style,
                          WExStyle exstyle )
    : _painting( FALSE )
    , _firstDirtyRow( 0 )
    , _numDirtyRows( 0 )
    , _currCursor( GUI_ARROW_CURSOR )
    , _parent( parent )
    , _menu( NULL )
    , _popup( NULL )
    , _toolBar( NULL )
    , _handle( NULL ) {
/*********************/

    WSystemMetrics::defaultRectangle( _autosize );
    makeWindow( text, style, exstyle );
}


WEXPORT WWindow::WWindow( WWindow* parent, const WRect& r, const char *text,
                          WStyle style, WExStyle exstyle )
    : _painting( FALSE )
    , _firstDirtyRow( 0 )
    , _numDirtyRows( 0 )
    , _currCursor( GUI_ARROW_CURSOR )
    , _parent( parent )
    , _menu( NULL )
    , _popup( NULL )
    , _toolBar( NULL )
    , _autosize( r )
    , _handle( NULL ) {
/*********************/

    makeWindow( text, style, exstyle );
}


void WWindow::destroyWindow() {
/*****************************/

    _accelKeys.deleteContents();
    delete clearMenu();
    delete clearToolBar();
    while( _children.count() > 0 ) {
        delete _children[ _children.count()-1 ];
    }
    if( _parent != NULL ) {
        _parent->removeChild( this );
        _parent = NULL;
    }
}


WEXPORT WWindow::~WWindow() {
/***************************/

    destroyWindow();
    if( _handle ) {
        GUIDestroyWnd( _handle );
        _handle = NULL;
    }
}


WWindow* WEXPORT WWindow::hasFocus() {
/************************************/

    gui_window *hwnd = GUIGetFront();
    if( hwnd ) {
        return( (WWindow *)GUIGetExtra( hwnd ) );
    }
    return( NULL );
}


bool WEXPORT WWindow::setFocus() {
/********************************/

    if( handle() ) {
        GUIBringToFront( handle() );
        return( TRUE );
    } else {
        return( FALSE );
    }
}


void WEXPORT WWindow::addChild( WObject* child ) {
/************************************************/

    _children.add( child );
}


void WEXPORT WWindow::removeChild( WObject* child ) {
/***************************************************/

    _children.removeSame( child );
}


WWindow* WWindow::switchChild( WWindow* currChild, bool forward ) {
/*****************************************************************/

    int icount = _children.count();
    int index = _children.indexOfSame( currChild );
    int startIndex = index;
    if( index < 0 ) {
        return( NULL );
    }

    for( ;; ) {
        if( forward ) {
            index = (index + 1) % icount;
        } else {
            index = (index + icount - 1) % icount;
        }
        if( index == startIndex ) return( currChild );

        WWindow* w = (WWindow *)_children[ index ];

        if( w->isVisible() ) {
            if( w->setFocus() ) {
                return( w );
            }
        }
    }
}


WWindow* WWindow::nextChild( WWindow* w ) {
/*****************************************/

    int icount = _children.count();
    for( int i=0; i<icount; i++ ) {
        WWindow* nw = (WWindow*)_children[i];
        if( w == nw ) {
            if( i+1 < icount ) {
                return( (WWindow*)_children[ i+1 ] );
            }
            return( (WWindow*)_children[0] );
        }
    }
    return( NULL );
}


typedef struct key_map {
    WKeyCode    alt_key;
    char        ascii;
} key_map;

static key_map KeyMapping[] = {
    WKeyAltA, 'A',
    WKeyAltB, 'B',
    WKeyAltC, 'C',
    WKeyAltD, 'D',
    WKeyAltE, 'E',
    WKeyAltF, 'F',
    WKeyAltG, 'G',
    WKeyAltH, 'H',
    WKeyAltI, 'I',
    WKeyAltJ, 'J',
    WKeyAltK, 'K',
    WKeyAltL, 'L',
    WKeyAltM, 'M',
    WKeyAltN, 'N',
    WKeyAltO, 'O',
    WKeyAltP, 'P',
    WKeyAltQ, 'Q',
    WKeyAltR, 'R',
    WKeyAltS, 'S',
    WKeyAltT, 'T',
    WKeyAltU, 'U',
    WKeyAltV, 'V',
    WKeyAltW, 'W',
    WKeyAltX, 'X',
    WKeyAltY, 'Y',
    WKeyAltZ, 'Z',
    WKeyAlt0, '0',
    WKeyAlt1, '1',
    WKeyAlt2, '2',
    WKeyAlt3, '3',
    WKeyAlt4, '4',
    WKeyAlt5, '5',
    WKeyAlt6, '6',
    WKeyAlt7, '7',
    WKeyAlt8, '8',
    WKeyAlt9, '9',
};


static WKeyCode MapAccelKey( int key ) {
/**************************************/

    for( int i = 0; i < sizeof( KeyMapping ); ++i ) {
        if( KeyMapping[i].ascii == key ) {
            return( KeyMapping[i].alt_key );
        }
    }
    return( WKeyNone );
}


AccelKey *WWindow::findAccelKey( WKeyCode key ) {
/***********************************************/

    int icount = _accelKeys.count();
    for( int i = 0; i < icount; ++i ) {
        AccelKey *ak = (AccelKey *)_accelKeys[i];
        if( ak->_key == key ) {
            return( ak );
        }
    }
    return( NULL );
}


void WEXPORT WWindow::addAccelKey( WKeyCode key, WObject* client, bcbk cb ) {
/***************************************************************************/

    _accelKeys.add( new AccelKey( key, client, cb ) );
}


void WEXPORT WWindow::addAccelKey( int key, WObject* client, bcbk cb ) {
/**********************************************************************/

    WKeyCode    kc;

    if( isalpha( key ) ) {
        key = toupper( key );
    }
    kc = MapAccelKey( key );
    if( kc ) {
        _accelKeys.add( new AccelKey( kc, client, cb ) );
    }
}


void WEXPORT WWindow::removeAccelKey( WKeyCode key ) {
/****************************************************/

    AccelKey *ak = findAccelKey( key );
    if( ak != NULL ) {
        _accelKeys.removeSame( ak );
    }
}


void WEXPORT WWindow::close() {
/*****************************/

    if( reallyClose() ) {
        GUIDestroyWnd( handle() );
        setHandle( NULL );
    }
}


void WEXPORT WWindow::getText( char* textBuf, unsigned textLen ) {
/****************************************************************/

    GUIGetWindowText( _handle, textBuf, textLen );
}


void WEXPORT WWindow::getText( WString& str ) {
/*********************************************/

    unsigned len = getTextLength();
    char* t = new char[ len+1 ];
    getText( t, len+1 );
    str = t;
    delete [] t;
}


void WEXPORT WWindow::setText( const char *text ) {
/*************************************************/

    GUISetWindowText( _handle, (char *)text );
}


WMenu* WEXPORT WWindow::setMenu( WMenu* menu ) {
/**********************************************/

    WMenu* oldMenu = clearMenu();
    _menu = menu;
    _menu->attachMenu( this, 0 );
    return( oldMenu );
}


WMenu* WEXPORT WWindow::clearMenu() {
/***********************************/

    WMenu* oldMenu = _menu;
    if( _menu != NULL ) {
        _menu->detachMenu();
        _menu = NULL;
    }
    return( oldMenu );
}


WToolBar* WEXPORT WWindow::setToolBar( WToolBar *toolbar ) {
/**********************************************************/

    WToolBar* previous = clearToolBar();
    _toolBar = toolbar;
    _toolBar->attach( this );
    return( previous );
}


WToolBar* WEXPORT WWindow::clearToolBar() {
/*****************************************/

    WToolBar* curr = _toolBar;
    if( _toolBar != NULL ) {
        _toolBar->detach();
        _toolBar = NULL;
    }
    return( curr );
}


void WEXPORT WWindow::setPopup( WPopupMenu* popup ) {
/***************************************************/

    _popup = popup;
}


void WEXPORT WWindow::clearPopup() {
/**********************************/

    _popup = NULL;
}


void WEXPORT WWindow::insertPopup( WPopupMenu *pop, int index ) {
/***************************************************************/

    if( menu() == NULL ) return;
    menu()->insertPopup( pop, index );
    pop->attachMenu( this, 0 );
}


void WEXPORT WWindow::removePopup( WPopupMenu *pop ) {
/****************************************************/

    if( menu() == NULL ) return;
    menu()->removePopup( pop );
    pop->detachMenu();
}


/********************************************************/
void WEXPORT WWindow::shrink( int wBorder, int hBorder ) {

    if( _children.count() > 0 ) {
        WRect wr;
        getRectangle( wr, FALSE );
        int icount = _children.count();
        for( int i = 0; i < icount; i++ ) {
            WRect cr;
            ((WWindow*)_children[i])->getRectangle( cr, FALSE );
            if( i == 0 ) {
                wr.w( cr.x() + cr.w() );
                wr.h( cr.y() + cr.h() );
            } else {
                if( cr.x() + cr.w() > wr.w() ) {
                    wr.w( cr.x() + cr.w() );
                }
                if( cr.y() + cr.h() > wr.h() ) {
                    wr.h( cr.y() + cr.h() );
                }
            }
        }
        wr.w( wr.w() + wBorder + 2*frameWidth() );
        wr.h( wr.h() + hBorder + WSystemMetrics::captionSize() + 2*frameHeight() );
        move( wr );
    }
}

/******************************/
void WEXPORT WWindow::shrink() {

    WPoint      avg;
    WPoint      max;

    textMetrics( avg, max );
    shrink( avg.y(), avg.x() );
}
//
// updateAutosize()
//
// When       Who      Why
// ========== ======= ==================================
// 06-29-2007 BobP    This code causes the x,y window position
//                    to jump around.  Also, the project window
//                    redraws incorrectly.  I don't believe this
//                    code is needed
//
bool WEXPORT WWindow::updateAutosize() {
    WRect       rect;
    WRect       prect;
    bool        move;
    int         w_adjust = 0;
    int         h_adjust = 0;

    move = FALSE;
    getRectangle( rect );
    if( parent() ) {
        parent()->getClientRect( prect );
    }
    
    if( _autosize.x() >= 0 ) {
        if( rect.x() < 0 ) {
            // don't let the x co-ordinate change sign in case
            // the window is moved outside the client area
            _autosize.x( 0 );
            if( isMaximized() ) {
                w_adjust = rect.x();
            }
            move = TRUE;
        } else {
            _autosize.x( rect.x() );
        }
    }
    if( _autosize.y() >= 0 ) {
        if( rect.y() < 0 ) {
            // don't let the y co-ordinate change sign in case
            // the window is moved outside the client area
            _autosize.y( 0 );
            if( isMaximized() ) {
                h_adjust = rect.y();
            }
            move = TRUE;
        } else {
            _autosize.y( rect.y() );
        }
    }
 
    if( _autosize.w() >= 0 ) {
        _autosize.w( rect.w() + w_adjust );
    }
    if( _autosize.h() >= 0 ) {
        _autosize.h( rect.h() + h_adjust );
    }
    return( move );
}

//
void WEXPORT WWindow::moved( int, int ) {

    //if( updateAutosize() ) {
        // Don't let the window be moved to a location that
        // causes the x and/or y co-ordinates to change from
        // a positive value to a negative value
    //    autosize();
    //}
}
void WEXPORT WWindow::resized( int, int ) {

    //updateAutosize();
}

void WEXPORT WWindow::size( WOrdinal w, WOrdinal h ) {

    _autosize.w( w );
    _autosize.h( h );
    autosize();
}


void WEXPORT WWindow::move( WOrdinal x, WOrdinal y ) {

    _autosize.x( x );
    _autosize.y( y );
    autosize();
}


void WEXPORT WWindow::move( const WRect& r ) {

    _autosize = r;
    autosize();
}

void WEXPORT WWindow::getRectangle( WRect& r, bool absolute ) {
/*************************************************************/

    gui_rect    rr;

    if( absolute ) {
        GUIGetAbsRect( _handle, &rr );
    } else {
        GUIGetRect( _handle, &rr );
    }
    r.x( rr.x );
    r.y( rr.y );
    r.w( rr.width );
    r.h( rr.height );
}


void WEXPORT WWindow::getNormalRectangle( WRect& r ) {
/****************************************************/

    gui_rect    rr;

    GUIGetRestoredSize( _handle, &rr );
    r.x( rr.x );
    r.y( rr.y );
    r.w( rr.width );
    r.h( rr.height );
}


void WEXPORT WWindow::getClientRect( WRect& r, bool absolute ) {
/**************************************************************/

    gui_rect    rr;

    GUIGetClientRect( _handle, &rr );
    if( !absolute ) {
        r.x( 0 );
        r.y( 0 );
    } else {
        r.x( rr.x );
        r.y( rr.y );
    }
    r.w( rr.width );
    r.h( rr.height );
}


void WWindow::show( WWindowState state ) {
/****************************************/

    if( _handle ) {
        if( state == WWinStateHide ) {
            GUIHideWindow( _handle );
        } else if( state == WWinStateShow ) {
            GUIShowWindowNA( _handle );
        } else if( state == WWinStateShowNormal ) {
            GUIShowWindow( _handle );
        } else if( state == WWinStateMaximized ) {
            GUIMaximizeWindow( _handle );
        } else { // if( state == WWinStateMinimized )
            GUIMinimizeWindow( _handle );
        }
    }
}


/******************************************/
void WWindow::autoPosition( WRect& cRect ) {
// Automatically reposition the given co-ordinates for a window relative
// to its parent window.

/*
    +------------------------------------------+
    |             |                            |
    |             y                            |
    |             |<-------- -x -------------->|
    |<---- x ---->+-----------------+          |
    |         |   |                 |          |
    |         |   |                 h          |
    |         |   |                 |          |
    |        -y   +--------w--------+<-- -w -->|
    |         |                     |          |
    |         |                     -h         |
    |         |                     |          |
    +------------------------------------------+
*/
    WRect pRect;
    if( _parent != NULL ) {
        _parent->getClientRect( pRect );
    }

    if( _autosize.x() < 0 ) {
        cRect.x( pRect.w() + _autosize.x() );
    } else {
        cRect.x( _autosize.x() );
    }

    if( _autosize.y() < 0 ) {
        cRect.y( pRect.h() + _autosize.y() );
    } else {
        cRect.y( _autosize.y() );
    }

    if( _autosize.w() < 0 ) {
        cRect.w( pRect.w() - cRect.x() + _autosize.w() );
    } else {
        cRect.w( _autosize.w() );
    }

    if( _autosize.h() < 0 ) {
        cRect.h( pRect.h() - cRect.y() + _autosize.h() );
    } else {
        cRect.h( _autosize.h() );
    }
    if( cRect.h() < 0 ) cRect.h( 0 );
    if( cRect.w() < 0 ) cRect.w( 0 );
    if( cRect.x() < 0 ) cRect.x( 0 );
    if( cRect.y() < 0 ) cRect.y( 0 );
}

void WEXPORT WWindow::autosize() {

    if( (_handle == NULL) || isIconic() ) return;
    WRect cRect;
    autoPosition( cRect );
    gui_rect c;
    c.x = cRect.x();
    c.y = cRect.y();
    c.width = cRect.w();
    c.height = cRect.h();
    GUIResizeWindow( _handle, &c );
}

bool WEXPORT WWindow::keyDown( WKeyCode key, WKeyState ) {
/********************************************************/

    AccelKey *ak = findAccelKey( key );
    if( ak != NULL ) {
        if( ak->callClient( ak->_key ) ) {
            return( TRUE );
        }
    }
    if( key == WKeyTab ) {
        if( switchChild( WWindow::hasFocus(), TRUE ) ) {
            return( TRUE );
        }
    } else if( key == WKeyShiftTab ) {
        if( switchChild( WWindow::hasFocus(), FALSE ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

WControl * WWindow::getControl( unsigned id ) {
/*********************************************/

    return (WControl *)_idMap.findThis( (WHANDLE)id );
}


void WWindow::displayFloatingPopup( WPopupMenu *popup ) {
/*******************************************************/

// Assumption: _menu will always be NULL for controls

    if( _menu ) {
        popup->track( this );
    } else if( _parent ) {
        parent()->displayFloatingPopup( popup );
    }
}


bool WEXPORT WWindow::rightBttnUp( int x, int y, WMouseKeyFlags flags ) {
/***********************************************************************/

    if( _popup == NULL ) {
        if( _parent != NULL ) {
            return( _parent->rightBttnUp( x, y, flags ) );
        }
        return( FALSE );
    }

    // simulate GUI_INITMENUPOPUP for floating popup menus since
    // GUI won't generate this message for floating popup menus
    _popup->popup();

    displayFloatingPopup( _popup );
    return( TRUE );
}


void WEXPORT WWindow::setScrollRange( WScrollBar sb, int maxr ) {
/***************************************************************/

    if( sb == WScrollBarVertical ) {
        GUISetVScrollRange( _handle, maxr );
    } else {
        GUISetHScrollRange( _handle, maxr );
    }
}


int WEXPORT WWindow::getScrollRange( WScrollBar sb ) {
/****************************************************/

    if( sb == WScrollBarVertical ) {
        return( GUIGetVScrollRange( _handle ) );
    } else {
        return( GUIGetHScrollRange( _handle ) );
    }
}


void WEXPORT WWindow::setScrollPos( WScrollBar sb, int pos ) {
/************************************************************/

    if( sb == WScrollBarVertical ) {
        GUISetVScroll( _handle, pos );
    } else {
        GUISetHScroll( _handle, pos );
    }
}


int  WEXPORT WWindow::getScrollPos( WScrollBar sb ) {
/***************************************************/

    if( sb == WScrollBarVertical ) {
        return( GUIGetVScroll( _handle ) );
    } else {
        return( GUIGetHScroll( _handle ) );
    }
}


void WEXPORT WWindow::setScrollTextRange( WScrollBar sb, int maxr ) {
/*******************************************************************/

    if( sb == WScrollBarVertical ) {
        GUISetVScrollRangeRows( _handle, maxr );
    } else {
        GUISetHScrollRangeCols( _handle, maxr );
    }
}


int WEXPORT WWindow::getScrollTextRange( WScrollBar sb ) {
/********************************************************/

    if( sb == WScrollBarVertical ) {
        return( GUIGetVScrollRangeRows( _handle ) );
    } else {
        return( GUIGetHScrollRangeCols( _handle ) );
    }
}


void WEXPORT WWindow::setScrollTextPos( WScrollBar sb, int pos ) {
/****************************************************************/

    if( sb == WScrollBarVertical ) {
        GUISetVScrollRow( _handle, pos );
    } else {
        GUISetHScrollCol( _handle, pos );
    }
}


int  WEXPORT WWindow::getScrollTextPos( WScrollBar sb ) {
/*******************************************************/

    if( sb == WScrollBarVertical ) {
        return( GUIGetVScrollRow( _handle ) );
    } else {
        return( GUIGetHScrollCol( _handle ) );
    }
}


bool WEXPORT WWindow::scrollNotify( WScrollNotification, int ) {
/**************************************************************/

    return( FALSE );
}


void WEXPORT WWindow::scrollWindow( WScrollBar sb, int scroll_size ) {
/********************************************************************/

    if( sb == WScrollBarVertical ) {
        GUIDoVScroll( handle(), scroll_size );
    } else {
        GUIDoHScroll( handle(), scroll_size );
    }
}


bool WEXPORT WWindow::scrollPosChanged( WScrollBar ) {
/****************************************************/

    return( FALSE );
}


void WEXPORT WWindow::startWait() {
/*********************************/

    if( _currCursor != GUI_HOURGLASS_CURSOR ) {
        _prevCursor = _currCursor;
        _currCursor = GUI_HOURGLASS_CURSOR;
        GUISetMouseCursor( _currCursor );
    }
}


void WEXPORT WWindow::stopWait() {
/********************************/

    if( _currCursor == GUI_HOURGLASS_CURSOR ) {
        _currCursor = _prevCursor;
        GUISetMouseCursor( _currCursor );
    }
}


void WEXPORT WWindow::update( bool ) {
/************************************/

    if( _handle != NULL ) {
        GUIWndDirty( _handle );
    }
}


void WEXPORT WWindow::setUpdates( bool start_update ) {
/*****************************************************/

    GUISetRedraw( _handle, start_update );
    if( start_update ) {
        update( TRUE );
    }
}


void WEXPORT WWindow::setIcon( WResource bitmap, char *ch_mode ) {
/****************************************************************/

    gui_resource        resource;

    resource.res = bitmap;
    resource.chars = ch_mode;
    GUISetIcon( _handle, &resource );
}


int WEXPORT WWindow::getRow( const WPoint &point ) {
/**************************************************/

    gui_point   p;

    p.x = point.x();
    p.y = point.y();
    return GUIGetRow( _handle, &p );
}


void WEXPORT WWindow::getRowPoint( int row, WPoint &point ) {
/***********************************************************/

    gui_point   p;

    GUIGetPoint( _handle, 0, row, &p );
    point.x( p.x );
    point.y( p.y );
}


void WEXPORT WWindow::textMetrics( WPoint &avg, WPoint &max ) {
/*************************************************************/

    gui_text_metrics    tm;

    GUIGetTextMetrics( _handle, &tm );
    avg.x( tm.avg.x );
    avg.y( tm.avg.y );
    max.x( tm.max.x );
    max.y( tm.max.y );
}


void WWindow::centre( unsigned int xpct, unsigned int ypct ) {
/************************************************************/
    WRect       backRect;
    WRect       foreRect;

    if( parent() ) {
        parent()->getClientRect( backRect, TRUE );
    } else {
        WSystemMetrics::screenCoordinates( backRect );
    }

    getRectangle( foreRect, TRUE );

    WOrdinal    newX;
    WOrdinal    newY;

    newX = WOrdinal(( backRect.w() - foreRect.w() ) * (long)xpct / 100);
    newY = WOrdinal(( backRect.h() - foreRect.h() ) * (long)ypct / 100);

    foreRect.x( newX );
    foreRect.y( newY );

    gui_rect c;
    c.x = foreRect.x();
    c.y = foreRect.y();
    c.width = foreRect.w();
    c.height = foreRect.h();
    GUIResizeWindow( _handle, &c );
}

void WWindow::hookF1Key( bool set ) {
    if( set ) {
        GUIHookF1();
    } else {
        GUIUnHookF1();
    }
}
