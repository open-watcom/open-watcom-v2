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


#include "wstatwin.hpp"
#include "wwindow.hpp"


WEXPORT WStatWindow::WStatWindow( WWindow* parent, const WRect& r,
                                  const char *defText )
    : WWindow( parent )
    , _defText( defText )
    , _text( defText ) {
/**********************/

    _fg_bg.fore = GUI_BLACK;
    _fg_bg.back = GUI_GREY;
    GUICreateStatusWindow( parent->handle(), r.x(), r.h(), &_fg_bg );
    const char *text = _defText;
    GUIDrawStatusText( parent->handle(), (char *)text );
}


WEXPORT WStatWindow::~WStatWindow() {
/***********************************/

   GUICloseStatusWindow( parent()->handle() );
}


void WStatWindow::setStatus( const char *text ) {
/***********************************************/

    if( text != NULL ) {
        _text = text;
    } else {
        _text = _defText;
    }
    const char *txt = _text;
    GUIDrawStatusText( parent()->handle(), (char *)txt );
}


bool WStatWindow::statusWindowCleared( void ) {
/*********************************************/

    const char *txt = _defText;
    GUIDrawStatusText( parent()->handle(), (char *)txt );
    return( TRUE );
}
