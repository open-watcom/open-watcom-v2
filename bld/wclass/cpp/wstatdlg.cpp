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


#include "wstatdlg.hpp"
#include "wcontrol.hpp"

WEXPORT WStatDialog::WStatDialog( WWindow * prt, WDialogId dialog_id )
    : WDialog( prt )
    , _dialog_id( dialog_id ) {
/*****************************/

}


void WStatDialog::addControl( WControl * control ) {
/**************************************************/

    _controls.add( control );
}

WControl * WEXPORT WStatDialog::getControl( unsigned control_id ) {
/*****************************************************************/

    unsigned    i;
    WControl *  test;

    for( i = 0; i < _controls.count(); i += 1 ) {
        test = (WControl *)_controls[ i ];
        if( test->controlId() == control_id ) {
            return test;
        }
    }

    return NULL;
}

extern "C" bool DlgProc( gui_window *hwin, gui_event msg, void *parm );

void WStatDialog::doDialog( WWindow *parent ) {
/*********************************************/

    gui_create_info     create_info;

    gui_window *hparent = NULL;
    if( parent != NULL ) {
        hparent = parent->handle();
    }


    // blank unused fields
    create_info.text = NULL;
    create_info.rect.x = 0;
    create_info.rect.y = 0;
    create_info.rect.width = 0;
    create_info.rect.height = 0;

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

    GUICreateResDialog( &create_info, _dialog_id );
}

void WEXPORT WStatDialog::getCtrlText( unsigned control_id,
                                       WString & str ) {
/******************************************************/

    char *text = GUIGetText( handle(), control_id );
    WString t( text );
    GUIMemFree( text );
    str = t;
}

void WEXPORT WStatDialog::getCtrlText( unsigned control_id,
                                       char* buff, unsigned len ) {
/*****************************************************************/

    char * text;
    int    text_len;

    text = GUIGetText( handle(), control_id );

    if( text == NULL ) {
        *buff = NULLCHAR;
    } else {
        text_len = strlen( text );
        if( text_len > len - 1 ) text_len = len - 1;
        memcpy( buff, text, text_len );
        buff[text_len] = NULLCHAR;
        GUIMemFree( text );
    }
}

int  WEXPORT WStatDialog::getCtrlTextLength( unsigned control_id ) {
/******************************************************************/

    char *text = GUIGetText( handle(), control_id );
    if( text == NULL ) return( 0 );
    int len = strlen( text );
    GUIMemFree( text );
    return( len );
}

void WEXPORT WStatDialog::setCtrlText( unsigned control_id,
                                       const char *text ) {
/*********************************************************/

    GUISetText( handle(), control_id, (char *)text );
}

bool WEXPORT WStatDialog::isCtrlEnabled( unsigned control_id ) {
/**************************************************************/

    return( GUIIsControlEnabled( handle(), control_id ) );
}

void WEXPORT WStatDialog::enableCtrl( unsigned control_id, bool state ) {
/***********************************************************************/

    GUIEnableControl( handle(), control_id, state );
}

void WEXPORT WStatDialog::getCtrlRect( unsigned control_id, WRect & r ) {
/***********************************************************************/

    gui_rect    rr;

    GUIGetControlRect( handle(), control_id, &rr );
    r.x( rr.x );
    r.y( rr.y );
    r.w( rr.width );
    r.h( rr.height );
}
