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


#include "wlistbox.hpp"
#include "wstring.hpp"


WEXPORT WListBox::WListBox( WWindow* parent, const WRect& r, WStyle wstyle )
    : WControl( parent, GUI_LISTBOX, r, NULL, wstyle )
    , _changedClient( NULL )
    , _changed( NULL )
    , _dblClickClient( NULL )
    , _dblClick( NULL ) {
/***********************/

}


WEXPORT WListBox::WListBox( WStatDialog* parent, unsigned id, WStyle wstyle )
    : WControl( parent, id, wstyle )
    , _changedClient( NULL )
    , _changed( NULL )
    , _dblClickClient( NULL )
    , _dblClick( NULL ) {
/***********************/

}


WEXPORT WListBox::~WListBox() {
/*****************************/

}


bool WListBox::gettingFocus( WWindow* ) {
/***************************************/

    if( count() > 0  && selected() < 0 ) {
        select( 0 );
    }
    return FALSE;
}


void WEXPORT WListBox::onChanged( WObject* client, cbw changed ) {
/****************************************************************/

    _changedClient = client;
    _changed = changed;
}


void WEXPORT WListBox::onDblClick( WObject* client, cbw click ) {
/***************************************************************/

    _dblClickClient = client;
    _dblClick = click;
}


int WListBox::findString( int index, const char *str ) {
/******************************************************/

    int len = strlen( str );
    int icount = count();
    if( icount > 0 ) {
        if( index < 0 ) index = icount-1;
        int i = index;
        i++;
        for( ; i != index; i++) {
            if( i >= icount ) i = 0;
            WString s;
            getString( i, s );
            for( int k=0; isspace( s[k] ); k++ );
            if( strnicmp( str, &s[k], len ) == 0 ) {
                return i;
            }
        }
    }
    return -1;
}


bool WListBox::processMsg( gui_event msg ) {
/******************************************/

    switch( msg ) {
    case GUI_CONTROL_CLICKED:
        if( _changedClient && _changed ) {
            (_changedClient->*_changed)( this );
            return( TRUE );
        }
        break;
    case GUI_CONTROL_DCLICKED:
        if( _dblClickClient && _dblClick ) {
            (_dblClickClient->*_dblClick)( this );
            return( TRUE );
        }
        break;
    }
    return( FALSE );
}


bool WEXPORT WListBox::keyDown( WKeyCode key, WKeyState ) {
/*********************************************************/

    switch( key ) {
        case WKeyEnter: {
            if( _dblClickClient && _dblClick ) {
                (_dblClickClient->*_dblClick)( this );
                return TRUE;
            }
        }
    }
    return( FALSE );
}


void WEXPORT WListBox::getString( int index, WString& str ) {
/***********************************************************/

    char        *list_item;

    list_item = GUIGetListItem( parent()->handle(), controlId(), index );
    WString s( list_item );
    GUIFree( list_item );
    str = s;
}


int WEXPORT WListBox::insertString( const char *s, int index ) {
/**************************************************************/

    int         newIndex;

    if( index < 0 ) {
        GUIAddText( parent()->handle(), controlId(), (char *)s );
        newIndex = count() - 1;
    } else {
        GUIInsertText( parent()->handle(), controlId(), index, (char *)s );
        newIndex = index;
    }
    if( count() == 1 ) {
        select( 0 );
    }
    return newIndex;
}


void WEXPORT WListBox::deleteString( int index ) {
/************************************************/

    GUIDeleteItem( parent()->handle(), controlId(), index );
}


void WEXPORT WListBox::reset() {
/******************************/

    GUIClearList( parent()->handle(), controlId() );
}


int WEXPORT WListBox::count() {
/******************************/

    return( GUIGetListSize( parent()->handle(), controlId() ) );
}


int WEXPORT WListBox::selected() {
/********************************/

    return( GUIGetCurrSelect( parent()->handle(), controlId() ) );
}


void WEXPORT WListBox::select( int index ) {
/******************************************/

    GUISetCurrSelect( parent()->handle(), controlId(), index );
}


void WEXPORT WListBox::setTopIndex( int index ) {
/***********************************************/

    GUISetTopIndex( parent()->handle(), controlId(), index );
}


int WEXPORT WListBox::topIndex() {
/********************************/

    return( GUIGetTopIndex( parent()->handle(), controlId() ) );
}


void WEXPORT WListBox::setExtent( WOrdinal extent ) {
/***************************************************/

    GUISetHorizontalExtent( parent()->handle(), controlId(), extent );
}


void WEXPORT WListBox::setTagPtr( int index, void* tagPtr ) {
/***********************************************************/

    GUISetListItemData( parent()->handle(), controlId(), index, tagPtr );
}


void* WEXPORT WListBox::tagPtr( int index ) {
/*******************************************/

    return( GUIGetListItemData( parent()->handle(), controlId(), index ) );
}
