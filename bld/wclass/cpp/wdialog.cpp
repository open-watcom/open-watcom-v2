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


#include "wdialog.hpp"
#include "wcontrol.hpp"
#include "wwindow.hpp"
#include "wmetrics.hpp"


WEXPORT WDialog::WDialog( WWindow* parent )
        : WWindow()
        , _text()
        , _quitCode( 0 )
/*****************/
{
    WRect       r;

    setParent( parent );
    WSystemMetrics::defaultRectangle( r );
    setAutosize( r );
}


WEXPORT WDialog::WDialog( WWindow* parent, const char *text )
        : WWindow()
        , _text( text )
        , _quitCode( 0 )
/***********************/
{
    WRect       r;

    setParent( parent );
    WSystemMetrics::defaultRectangle( r );
    setAutosize( r );
}


WEXPORT WDialog::WDialog( const WRect& r, const char *text )
        : WWindow()
        , _text( text )
        , _quitCode( 0 )
/***********************/
{
    setParent( NULL );
    setAutosize( r );
}


WEXPORT WDialog::WDialog( const char *text, const WRect& r )
        : WWindow()
        , _text( text )
        , _quitCode( 0 )
/***********************/
{
    setParent( NULL );
    setAutosize( r );
}


WEXPORT WDialog::WDialog( WWindow *parent, const WRect& r, const char *text )
        : WWindow()
        , _text( text )
        , _quitCode( 0 )
/***********************/
{
    setParent( parent );
    setAutosize( r );
}


WEXPORT WDialog::WDialog( WWindow *parent, const char *text, const WRect& r )
        : WWindow()
        , _text( text )
        , _quitCode( 0 )
/***********************/
{
    setParent( parent );
    setAutosize( r );
}


WEXPORT WDialog::~WDialog() {
/***************************/

    // make sure the dialog has not been closed using
    // the quit() member function
    if( handle() ) {
        GUICloseDialog( handle() );
    }
}

bool WEXPORT WDialog::processMsg( gui_event msg, void *parm ) {
/*************************************************************/

    switch( msg ) {
    case GUI_INIT_DIALOG:
        initialize();
        return( true );
    case GUI_DIALOG_ESCAPE:
        cancelButton( NULL );
        return( true );
    case GUI_DESTROY:
        _accelKeys.deleteContents();
        while( children().count() > 0 ) {
            delete children()[children().count() - 1];
        }
        setHandle( NULL );
        return( true );
    }
    return( WWindow::processMsg( msg, parm ) );
}


extern "C" bool DlgProc( gui_window *hwin, gui_event msg, void *parm ) {
/**********************************************************************/

    WDialog* win = (WDialog*)GUIGetExtra( hwin );
    if( msg == GUI_INIT_DIALOG ) {
        win->setHandle( hwin );
    }
    return( win->processMsg( msg, parm ) );
}


void WEXPORT WDialog::quit( int code ) {
/**************************************/

    _quitCode = code;
    GUICloseDialog( handle() );
}


void WDialog::doDialog( WWindow *parent ) {
/*****************************************/

    WRect               r;
    gui_create_info     create_info;

    autoPosition( r );
    gui_window *hparent = NULL;
    if( parent != NULL ) {
        hparent = parent->handle();
    }
    const char *ctext = _text;
    create_info.text = (char *)ctext;
    create_info.rect.x = r.x();
    create_info.rect.y = r.y();
    create_info.rect.width = r.w();
    create_info.rect.height = r.h();
    create_info.scroll = GUI_NOSCROLL;
    create_info.style = GUI_NONE;
    create_info.parent = hparent;
    create_info.num_menus = 0;
    create_info.menu = NULL;
    create_info.num_attrs = 0;
    create_info.colours = NULL;
    create_info.call_back = DlgProc;
    create_info.extra = this;
    create_info.icon = NULL;
    GUICreateDialog( &create_info, 0, NULL );
}


int WEXPORT WDialog::process( WWindow *parent ) {
/***********************************************/

// Process a dialog and use the given parent window for the parent of the
// dialog.

    setParent( parent );
    doDialog( parent );
    setParent( NULL );
    return( _quitCode );
}


int WEXPORT WDialog::process() {
/******************************/

    doDialog( parent() );
    return( _quitCode );
}


WWindow *WDialog::switchChild( WWindow *win, bool forward ) {
/***********************************************************/

    unsigned    control;

    if( GUIGetFocus( handle(), &control ) ) {
        // control has the focus
        WWindow* child = getControl( control );
        return( WWindow::switchChild( child, forward ) );
    } else if( this == win->parent() ) {
        // window inside a dialog has the focus
        // GUI won't generate a "losing focus" message so WCLASS
        // has to simulate one
        win->losingFocus( NULL );
        return( WWindow::switchChild( win, forward ) );
    }
    return( NULL );
}


bool WEXPORT WDialog::keyDown( WKeyCode kc, WKeyState ks ) {
/***********************************************************/

    if( kc == WKeyEnter ) {
        okButton( NULL );
        return( true );
    } else if( kc == WKeyEscape ) {
        cancelButton( NULL );
        return( true );
    }
    WWindow::keyDown( kc, ks );
    return( true ); // so we don't pass key messages to parents of dialogs
}
