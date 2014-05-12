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


#include "winpdlg.hpp"
#include "wtext.hpp"
#include "weditbox.hpp"
#include "wpshbttn.hpp"
#include "wfiledlg.hpp"
#include "wfilenam.hpp"
#include "wmetrics.hpp"

static  char    BrowseText[] = "&Browse...";
static  char    CancelText[] = "&Cancel";
static  char    OKText[] = "&OK";


WEXPORT WInputDialog::WInputDialog( WWindow *parent, const char *text )
    : WDialog( parent, text )
    , _reply( NULL )
    , _browseDialog( NULL ) {
/***************************/

}


WEXPORT WInputDialog::~WInputDialog() {
/*************************************/

    delete _browseDialog;
}


void WInputDialog::updateExtents( const char *t, int *w, int *h ) {
/*****************************************************************/

    if( getTextExtentX( t ) > *w ) {
        *w = getTextExtentX( t );
    }
    if( getTextExtentY( t ) > *h ) {
        *h = getTextExtentY( t );
    }
}


void WInputDialog::initialize() {
/*******************************/

    WPoint avg;
    WPoint max;
    textMetrics( avg, max );
    int sp = max.x();

    int x = WSystemMetrics::dialogFrameWidth();
    int y = WSystemMetrics::dialogFrameHeight();

    int p_w = 0;
    int p_h = 0;
    updateExtents( _promptText, &p_w, &p_h );
    p_w += avg.x() / 2;
    p_h += avg.y() / 2;
    int r_w = 32 * avg.x();
    int r_h = max.y() + 2*max.y() / 3;
    updateExtents( *_reply, &r_w, &r_h );

    _prompt = new WText( this, WRect( x, y + (r_h - p_h)/2, p_w, p_h ), _promptText );
    _prompt->show();
    _input = new WEditBox( this, WRect( x + p_w + sp, y, r_w, r_h ), *_reply );
    _input->show();
    y += p_h + max.y();

    int b_w = 0;
    int b_h = 0;
    updateExtents( BrowseText, &b_w, &b_h );
    updateExtents( CancelText, &b_w, &b_h );
    updateExtents( OKText, &b_w, &b_h );
    b_w += avg.x() * 2;
    b_h += avg.y() / 2;
    WDefPushButton *bOk = new WDefPushButton( this, WRect( x, y, b_w, b_h ),
                                              OKText );
    bOk->onClick( this, (cbw)&WInputDialog::okButton );
    bOk->show();
    x += b_w + max.x();

    WPushButton *bCancel = new WPushButton( this, WRect( x, y, b_w, b_h ),
                                            CancelText );
    bCancel->onClick( this, (cbw)&WInputDialog::cancelButton );
    bCancel->show();
    x += b_w + max.x();

    if( _browseDialog ) {
        WPushButton *bBrowse = new WPushButton( this,
                                                WRect( x, y, b_w, b_h ),
                                                BrowseText );
        bBrowse->onClick( this, (cbw)&WInputDialog::browseButton );
        bBrowse->show();
    }

    shrink();
    centre();

    _input->select();
    _input->setFocus();
    show();
}


void WEXPORT WInputDialog::setBrowse( const char *filter, bool multi_select ) {
/*****************************************************************************/

    _browseDialog = new WFileDialog( this, filter );
    _multiSelect = multi_select;
}


void WEXPORT WInputDialog::okButton( WWindow * ) {
/************************************************/

    _input->getText( *_reply );
    quit( ( _reply->size() > 0 ) );
}


void WEXPORT WInputDialog::cancelButton( WWindow* ) {
/***************************************************/

    quit( false );
}


void WEXPORT WInputDialog::browseButton( WWindow* ) {
/***************************************************/

    WFileName   fn;

    WWindow *old = WWindow::hasFocus();
    if( _multiSelect ) {
        WFileNameList   fn_list;
        _browseDialog->getOpenFileName( fn_list, "", "Pick filename", WFOpenNew );
        fn = fn_list.cString( 0, -1 );
    } else {
        fn = _browseDialog->getOpenFileName( "", "Pick filename", WFOpenNew );
    }
    if( fn.size() > 0 ) {
        _input->setText( fn );
    }
    old->setFocus();
}


bool WEXPORT WInputDialog::getInput( WString& reply, const char *prompt ) {
/*************************************************************************/

    _reply = &reply;
    _promptText = prompt;
    return( process() == (int)true );
}
