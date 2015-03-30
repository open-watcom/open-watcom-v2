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


#include "wcontrol.hpp"
#include "wwindow.hpp"
#include "wstatdlg.hpp"
#include "wpopmenu.hpp"


WEXPORT WControl::WControl( WWindow* parent, gui_control_class control_class,
                            const WRect& r, const char *text, WStyle style )
                : WWindow( parent ) {
/***********************************/

    gui_control_info    control_info;
    WRect               rr;
    unsigned long       gui_style;

    if( parent == NULL )
        return;
    setAutosize( r );
    autoPosition( rr );
    _id = WWindow::_idMaster++;
    control_info.control_class = control_class;
    control_info.text = (char *)text;
    control_info.rect.x = rr.x();
    control_info.rect.y = rr.y();
    control_info.rect.width = rr.w();
    control_info.rect.height = rr.h();
    control_info.parent = parent->handle();
    control_info.scroll = _WStyleToScrollStyle( style );
    gui_style = GUI_CONTROL_INIT_INVISIBLE | _WStyleToControlStyle( style );
    if( (control_class != GUI_STATIC) && (control_class != GUI_GROUPBOX) ) {
        gui_style |= GUI_TAB_GROUP;
    }
    control_info.style = (gui_control_styles)gui_style;
    control_info.id = _id;
    WWindow::_idMap.setThis( this, (WHANDLE)(pointer_int)_id );
    parent->addChild( this );
    GUIAddControl( &control_info, NULL, NULL );
}

WEXPORT WControl::WControl( WStatDialog* parent, gui_ctl_id id, WStyle wstyle )
                : WWindow( parent )
                , _id( id )
                , _style( wstyle )
/***********************************/
{
    parent->addControl( this );
}


WEXPORT WControl::~WControl() {
/*****************************/

    WWindow::_idMap.clearThis( this );
}


void WEXPORT WControl::autosize() {
/*********************************/

    WRect cRect;
    autoPosition( cRect );
    gui_rect c;
    c.x = cRect.x();
    c.y = cRect.y();
    c.width = cRect.w();
    c.height = cRect.h();
    GUIResizeControl( parent()->handle(), controlId(), &c );
}


void WEXPORT WControl::getText( char* buff, size_t len ) {
/********************************************************/

    char *text = GUIGetText( parent()->handle(), controlId() );
    if( text == NULL ) {
        *buff = NULLCHAR;
    } else {
        size_t text_len = strlen( text );
        if( text_len > len - 1 )
            text_len = len - 1;
        memcpy( buff, text, text_len );
        buff[text_len] = NULLCHAR;
        GUIMemFree( text );
    }
}


void WEXPORT WControl::getText( WString& str ) {
/**********************************************/

    char *text = GUIGetText( parent()->handle(), controlId() );
    WString t( text );
    GUIMemFree( text );
    str = t;
}


size_t WEXPORT WControl::getTextLength() {
/****************************************/

    char *text = GUIGetText( parent()->handle(), controlId() );
    if( text == NULL )
        return( 0 );
    size_t len = strlen( text );
    GUIMemFree( text );
    return( len );
}


void WEXPORT WControl::setText( const char* text ) {
/**************************************************/

    GUISetText( parent()->handle(), controlId(), text );
}


bool WEXPORT WControl::isEnabled() {
/**********************************/

    return( GUIIsControlEnabled( parent()->handle(), controlId() ) );
}


void WEXPORT WControl::enable( bool state ) {
/*******************************************/

    GUIEnableControl( parent()->handle(), controlId(), state );
}


void WEXPORT WControl::getRectangle( WRect& r, bool absolute ) {
/**************************************************************/

    gui_rect    rr;

    _unused( absolute );
    GUIGetControlRect( parent()->handle(), controlId(), &rr );
    r.x( rr.x );
    r.y( rr.y );
    r.w( rr.width );
    r.h( rr.height );
}


void WEXPORT WControl::show( WWindowState state ) {
/*************************************************/

    if( state == WWinStateHide ) {
        GUIHideControl( parent()->handle(), controlId() );
    } else {
        GUIShowControl( parent()->handle(), controlId()  );
    }
}


bool WEXPORT WControl::isHidden() {
/*********************************/

    return( !GUIIsControlVisible( parent()->handle(), controlId() ) );
}


bool WEXPORT WControl::setFocus() {
/*********************************/

    gui_control_class   control;

    GUIGetControlClass( parent()->handle(), controlId(), &control );
    if( control == GUI_STATIC )
        return( false );
    if( control == GUI_GROUPBOX )
        return( false );
    return( GUISetFocus( parent()->handle(), controlId() ) );
}


void WEXPORT WControl::setUpdates( bool start_update ) {
/******************************************************/

    GUIControlSetRedraw( parent()->handle(), controlId(), start_update );
    if( start_update ) {
        update( true );
    }
}


void WEXPORT WControl::textMetrics( WPoint &avg, WPoint &max ) {
/**************************************************************/

    parent()->textMetrics( avg, max );
}
