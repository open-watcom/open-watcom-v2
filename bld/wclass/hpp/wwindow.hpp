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
* Description:  Window class prototype
*
****************************************************************************/

#ifndef wwindow_class
#define wwindow_class

#include "_windows.hpp"
#include "wkeydefs.hpp"
#include "wpaint.hpp"
#include "wmetrics.hpp"

typedef unsigned short  WMouseKeyFlags;
typedef unsigned short  WScrollPos;

// For defining WStyles for Windows:
// =================================

typedef unsigned long   WStyle;

#define _CreateStyleToWStyle( x )       (((WStyle)(x))<<16)
#define _ControlStyleToWStyle( x )      (((WStyle)(x))<<16)
#define _ScrollStyleToWStyle( x )       ((WStyle)(x))

#define _WStyleToCreateStyle( x )       ((gui_create_styles)((x & 0xffff0000)>>16))
#define _WStyleToControlStyle( x )      ((gui_control_styles)((x & 0xffff0000)>>16))
#define _WStyleToScrollStyle( x )       ((gui_scroll_styles)(x & 0x0000ffff))

// Window creation styles:

#define WStyleHScrollEvents     _CreateStyleToWStyle( GUI_HSCROLL_EVENTS )
#define WStyleVScrollEvents     _CreateStyleToWStyle( GUI_VSCROLL_EVENTS )
#define WStyleMinimize          _CreateStyleToWStyle( GUI_INIT_MINIMIZED )
#define WStyleMaximize          _CreateStyleToWStyle( GUI_INIT_MAXIMIZED )
#define WStyleSizeable          _CreateStyleToWStyle( GUI_RESIZEABLE )
#define WStylePopup             _CreateStyleToWStyle( GUI_POPUP )
#define WStyleSysMenu           _CreateStyleToWStyle( GUI_SYSTEM_MENU )
#define WStyleMinimizeBox       _CreateStyleToWStyle( GUI_MINIMIZE )
#define WStyleMaximizeBox       _CreateStyleToWStyle( GUI_MAXIMIZE )
#define WStyleCloseable         _CreateStyleToWStyle( GUI_CLOSEABLE )
#define WStyleTabGroup          _CreateStyleToWStyle( GUI_TAB_GROUP )
#define WStyleSimple            _CreateStyleToWStyle( GUI_NOFRAME )
#define WStyleSimpleFrame       _CreateStyleToWStyle( 0 )
#define WStyleDefault           (WStyleSysMenu | WStyleSizeable | \
                                 WStyleMinimizeBox |  WStyleMaximizeBox | \
                                 WStyleCloseable )

#define WStyleHScroll           _ScrollStyleToWStyle( GUI_HSCROLL )
#define WStyleVScroll           _ScrollStyleToWStyle( GUI_VSCROLL )
#define WStyleHDrag             _ScrollStyleToWStyle( GUI_HDRAG )
#define WStyleVDrag             _ScrollStyleToWStyle( GUI_VDRAG )
#define WStyleHTrack            _ScrollStyleToWStyle( GUI_HTRACK )
#define WStyleVTrack            _ScrollStyleToWStyle( GUI_VTRACK )
#define WStyleHCols             _ScrollStyleToWStyle( GUI_HCOLS )
#define WStyleVRows             _ScrollStyleToWStyle( GUI_VROWS )
#define WStyleHScrollAll        (WStyleHScroll | WStyleHDrag | \
                                 WStyleHTrack | WStyleHCols )
#define WStyleVScrollAll        (WStyleVScroll | WStyleVDrag | \
                                 WStyleVTrack | WStyleVRows )

// Window extended styles:

typedef unsigned long   WExStyle;

#define WExStyleDefault     0
#define WExStyle3DBorder    GUI_3D_BORDER

// Scroll bar identifiers:

typedef unsigned WScrollBar;

#define WScrollBarVertical      1
#define WScrollBarHorizontal    2

// Scroll notifications:

typedef unsigned WScrollNotification;

#define WScrollUp               1
#define WScrollPageUp           2
#define WScrollDown             3
#define WScrollPageDown         4
#define WScrollTop              5
#define WScrollBottom           6
#define WScrollVertical         7
#define WScrollLeft             8
#define WScrollPageLeft         9
#define WScrollRight            10
#define WScrollPageRight        11
#define WScrollFullLeft         12
#define WScrollFullRight        13
#define WScrollHorizontal       14

#include "wstates.hpp"
#include "wres.hpp"
#include "wobject.hpp"
#include "wobjmap.hpp"
#include "wvlist.hpp"
#include "wrect.hpp"
#include "wstring.hpp"
#include "wcolour.hpp"
#include "wpoint.hpp"
#include "wlines.hpp"

WCLASS WMenu;
WCLASS WPopupMenu;
WCLASS WToolBar;
WCLASS WWindow;
WCLASS AccelKey;
WCLASS WControl;

typedef void (WObject::*cbw)( WWindow *w );
typedef bool (WObject::*bcbk)( gui_key k );

WCLASS WWindow : public WObject {
private:
    virtual bool reallyClose( void ) { return( TRUE ); }
    bool updateAutosize( void );
    void destroyWindow( void );
    AccelKey *findAccelKey( WKeyCode key );

private:
    gui_window              *_handle;
    WWindow                 *_parent;
    WMenu                   *_menu;
    WPopupMenu              *_popup;
    WToolBar                *_toolBar;
    WVList                  _children;
    WRect                   _autosize;
    gui_mouse_cursor        _prevCursor;
    gui_mouse_cursor        _currCursor;
    WOrdinal                _firstDirtyRow;
    int                     _numDirtyRows;
    bool                    _keyHandled;

protected:
    void makeWindow( const char *title, WStyle wstyle = 0, WExStyle wexstyle = 0 );
    void autoPosition( WRect& r );
    void setParent( WWindow *parent ) { _parent = parent; }
    void setAutosize( const WRect& r ) { _autosize = r; }
    WPopupMenu *popup() { return _popup; }
    WVList & children() { return _children; }
    WWindow *nextChild( WWindow *w );
    WRect getDefSize();
    void enumChildren( void );

protected:
    bool                    _painting;
    WVList                  _accelKeys;

public:
    WEXPORT WWindow( WWindow *parent = NULL );
    WEXPORT WWindow( const char *text, WStyle style = WStyleDefault,
                     WExStyle exstyle = WExStyleDefault );
    WEXPORT WWindow( WWindow *parent, const char *text, WStyle style = WStyleDefault,
                     WExStyle exstyle = WExStyleDefault );
    WEXPORT WWindow( WWindow *parent, const WRect& r, const char *text,
                     WStyle style = WStyleDefault, WExStyle exstyle = WExStyleDefault );
    WEXPORT ~WWindow();

    void WEXPORT addChild( WObject *child );
    void WEXPORT removeChild( WObject *child );
    void WEXPORT addAccelKey( WKeyCode key, WObject *client, bcbk cb );
    void addAccelKey( int key, WObject *client, bcbk cb );
    void WEXPORT removeAccelKey( WKeyCode key );
    virtual bool gettingFocus( WWindow * ) { return( FALSE ); }
    virtual bool losingFocus( WWindow * ) { return( FALSE ); }
    virtual bool WEXPORT keyDown( WKeyCode, WKeyState );
    virtual WControl * WEXPORT getControl( unsigned control_id );
    virtual bool appActivate( bool ) { return( FALSE ); }
    virtual bool contextHelp( bool ) { return( FALSE ); }
    void hookF1Key( bool );

    // mouse handling
    virtual bool WEXPORT mouseMove( int, int, WMouseKeyFlags );
    virtual bool WEXPORT leftBttnDn( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT leftBttnUp( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT leftBttnDbl( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT middleBttnDn( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT middleBttnUp( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT middleBttnDbl( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT rightBttnDn( int, int, WMouseKeyFlags ) { return( FALSE ); };
    virtual bool WEXPORT rightBttnUp( int, int, WMouseKeyFlags );
    virtual bool WEXPORT rightBttnDbl( int, int, WMouseKeyFlags ) { return( FALSE ); };

    // scrolling gear
    void WEXPORT setScrollRange( WScrollBar sb, int maxr );
    int WEXPORT getScrollRange( WScrollBar sb );
    void WEXPORT setScrollPos( WScrollBar sb, int pos );
    int  WEXPORT getScrollPos( WScrollBar sb );
    void WEXPORT setScrollTextRange( WScrollBar sb, int maxr );
    int WEXPORT getScrollTextRange( WScrollBar sb );
    void WEXPORT setScrollTextPos( WScrollBar sb, int pos );
    int  WEXPORT getScrollTextPos( WScrollBar sb );
    void WEXPORT scrollWindow( WScrollBar, int );
    virtual bool WEXPORT scrollPosChanged( WScrollBar sb );
    virtual bool WEXPORT scrollNotify( WScrollNotification, int );
    virtual bool WEXPORT statusWindowCleared( void ) { return( FALSE ); }

    void WEXPORT close( void );
    virtual bool WEXPORT paint() { return FALSE; }
    bool WEXPORT isPainting() { return _painting; }
    virtual void WEXPORT moved( WOrdinal width, WOrdinal height );
    virtual void WEXPORT resized( WOrdinal width, WOrdinal height );
    bool WEXPORT isMaximized() { return( GUIIsMaximized( _handle ) ); }
    bool WEXPORT isMinimized() { return( GUIIsMinimized( _handle ) ); }
    bool WEXPORT isIconic() { return( GUIIsMinimized( _handle ) ); }
    virtual void WEXPORT maximized() {}
    virtual void WEXPORT minimized() {}
    void WEXPORT startWait( void );
    void WEXPORT stopWait( void );
    virtual void WEXPORT autosize();
    virtual int WEXPORT getTextLength() {
        return GUIGetWindowTextLength( _handle );
    }
    virtual void WEXPORT getText( char *textbuf, unsigned length );
    virtual void WEXPORT getText( WString& str );
    virtual void WEXPORT setText( const char *text );
    virtual WMenu * WEXPORT setMenu( WMenu* menu );
    virtual WMenu * WEXPORT clearMenu();
    WMenu * WEXPORT menu() { return _menu; }
    virtual WToolBar * WEXPORT setToolBar( WToolBar *toolbar );
    virtual WToolBar * WEXPORT clearToolBar();
    WToolBar * WEXPORT toolBar() { return _toolBar; }
    virtual void WEXPORT setPopup( WPopupMenu *menu );
    virtual void WEXPORT clearPopup();
    virtual void WEXPORT insertPopup( WPopupMenu *pop, int index );
    virtual void WEXPORT removePopup( WPopupMenu *pop );
    void WEXPORT move( const WRect& r );
    void WEXPORT move( WOrdinal x, WOrdinal y );
    virtual void WEXPORT getRectangle( WRect& r, bool absolute=FALSE );
    virtual void WEXPORT getNormalRectangle( WRect& r );
    virtual void WEXPORT show( WWindowState state=WWinStateShow );
    virtual bool WEXPORT isVisible() { return GUIIsWindowVisible( _handle ); }
    void WEXPORT getClientRect( WRect & r, bool absolute=FALSE );
    void WEXPORT size( WOrdinal w, WOrdinal h );
    void WEXPORT minimumSize( WOrdinal w, WOrdinal h );
    void WEXPORT shrink( WOrdinal wBorder, WOrdinal hBorder );
    void WEXPORT shrink( void );
    void WEXPORT update( bool force=FALSE );
    virtual void WEXPORT setUpdates( bool b=TRUE );
    void WEXPORT erase() {}  // NYI
    int WEXPORT getRows( void ) {
        return( GUIGetNumRows( _handle ) );
    }
    int WEXPORT getRow( const WPoint & );
    void WEXPORT getRowPoint( int row, WPoint & );
    virtual void WEXPORT textMetrics( WPoint &, WPoint & );

    static WWindow * WEXPORT hasFocus();
    static WObjectMap WEXPORT _idMap;
    static WObjectMap WEXPORT _toolBarIdMap;
    static WObjectMap WEXPORT _popupIdMap;
    static unsigned WEXPORT _idMaster;

    void WEXPORT setIcon( WResource, char *ch_mode=NULL );
    virtual bool WEXPORT setFocus( void );
    gui_window * WEXPORT handle() { return _handle; }
    void setHandle( gui_window *handle ) { _handle = handle; }
    WWindow * WEXPORT parent() { return _parent; }
    WRect getAutosize( void ) { return _autosize; }
    virtual WWindow * switchChild( WWindow *currChild, bool forward );
    virtual bool WEXPORT processMsg( gui_event msg, void *parm );
    virtual int WEXPORT getTextExtentX( const char *text, int len ) {
        return( GUIGetExtentX( _handle, (char *)text, len ) );
    }
    virtual int WEXPORT getTextExtentX( const char *text ) {
        return( GUIGetExtentX( _handle, (char *)text, strlen( text ) ) );
    }
    virtual int WEXPORT getTextExtentY( const char *text ) {
        return( GUIGetExtentY( _handle, (char *)text ) );
    }
    virtual void displayFloatingPopup( WPopupMenu * );
    virtual WOrdinal WEXPORT frameWidth( void ) {
        return( WSystemMetrics::frameWidth() );
    }
    virtual WOrdinal WEXPORT frameHeight( void ) {
        return( WSystemMetrics::frameHeight() );
    }
    void centre( unsigned int xpct=50, unsigned int ypct=40 );

    virtual bool queryEndSession( void ) { return( TRUE ); }
    virtual void endSession( bool ) {}

    // painting functions

    void WEXPORT invalidate( void ) {
        GUIWndDirty( _handle );
    }
    void WEXPORT invalidateRow( int row ) {
        GUIWndDirtyRow( _handle, row );
    }
    void WEXPORT setSystemFont( bool fixed ) {
        GUISetSystemFont( _handle, fixed );
    }
    void WEXPORT invalidateRect( const WRect & );
    void WEXPORT getPaintRect( WRect & );
    Color WEXPORT backgroundColour( void );
    void WEXPORT changeBackground( WPaintAttr );
    void WEXPORT fillRect( const WRect &, Color );
    void WEXPORT fillRect( const WRect &, WPaintAttr );
    void WEXPORT drawRect( const WRect &, Color );
    void WEXPORT drawRect( const WRect &, WPaintAttr );
    void WEXPORT drawLine( const WPoint &, const WPoint &, WLineStyle, unsigned int, Color );
    void WEXPORT drawLine( const WPoint &, const WPoint &, WLineStyle, unsigned int, WPaintAttr );
    void WEXPORT drawLine( const WPoint &, const WPoint &, Color );
    void WEXPORT drawLine( const WPoint &, const WPoint &, WPaintAttr );
    void WEXPORT drawText( const WPoint &, const char * );
    void WEXPORT drawText( const WPoint &, const char *, Color, Color );
    void WEXPORT drawText( const WPoint &, const char *, WPaintAttr );
    void WEXPORT drawText( int, int, const char * );
    void WEXPORT drawText( int, int, const char *, Color, Color );
    void WEXPORT drawText( int, int, const char *, WPaintAttr );
    void WEXPORT drawText( const WPoint &, const char *, int );
    void WEXPORT drawText( const WPoint &, const char *, int, Color, Color );
    void WEXPORT drawText( const WPoint &, const char *, int, WPaintAttr );
    void WEXPORT drawText( int, int, const char *, int );
    void WEXPORT drawText( int, int, const char *, int, Color, Color );
    void WEXPORT drawText( int, int, const char *, int, WPaintAttr );
    void WEXPORT drawTextExtent( const WPoint &, const char *, Color, Color, int );
    void WEXPORT drawTextExtent( const WPoint &, const char *, WPaintAttr, int );
    void WEXPORT drawTextExtent( const WPoint &, const char *, int );
    void WEXPORT drawTextExtent( int, int, const char *, Color, Color, int );
    void WEXPORT drawTextExtent( int, int, const char *, WPaintAttr, int );
    void WEXPORT drawTextExtent( int, int, const char *, int );
    void WEXPORT drawTextExtent( const WPoint &, const char *, int, Color, Color, int );
    void WEXPORT drawTextExtent( const WPoint &, const char *, int, WPaintAttr, int );
    void WEXPORT drawTextExtent( const WPoint &, const char *, int, int );
    void WEXPORT drawTextExtent( int, int, const char *, int, Color, Color, int );
    void WEXPORT drawTextExtent( int, int, const char *, int, WPaintAttr, int );
    void WEXPORT drawTextExtent( int, int, const char *, int, int );
    void WEXPORT drawHotSpot( int, int, int );
    WOrdinal WEXPORT firstDirtyRow( void ) { return( _firstDirtyRow ); }
    int WEXPORT numDirtyRows( void ) { return( _numDirtyRows ); }
};

#endif
