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


#include "wbaritem.hpp"
#include "wtoolbar.hpp"
#include "wwindow.hpp"


WEXPORT WToolBarItem::WToolBarItem( const char *text, WResource tool,
                                    WObject *client, cbtbi pick,
                                    const char *htext, const char *tip )
    : _parent( NULL )
    , _client( client )
    , _pick( pick ) {
/*******************/

    _toolbar.label = (char *)text;
    _toolbar.bitmap = tool;
    _toolbar.hinttext = (char *)htext;
    _toolbar.tip = (char *)tip;
    _toolbar.id = WWindow::_idMaster++;
    WWindow::_toolBarIdMap.setThis( this, (WHANDLE)(unsigned)_toolbar.id );
}


WEXPORT WToolBarItem::~WToolBarItem() {
/*************************************/

    if( parent() ) {
        parent()->removeTool( this );
    }
    WWindow::_toolBarIdMap.clearThis( this );
}


void WEXPORT WToolBarItem::picked() {
/***********************************/

    if( _client && _pick ) {
        (_client->*_pick)( this );
    }
}
