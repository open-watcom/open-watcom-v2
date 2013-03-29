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


#include "weditdlg.hpp"
#include "weditbox.hpp"
#include "wpshbttn.hpp"
#include "wmetrics.hpp"

static  char    CancelText[] = "&Cancel";
static  char    OKText[] = "&OK";


WEXPORT WEditDialog::WEditDialog( WWindow* parent, const char* text )
    : WDialog( parent, text )
    , _reply( NULL ) {
/********************/

}


void WEditDialog::updateExtents( const char *t, int *w, int *h ) {
/****************************************************************/

    if( getTextExtentX( t ) > *w ) {
        *w = getTextExtentX( t );
    }
    if( getTextExtentY( t ) > *h ) {
        *h = getTextExtentY( t );
    }
}


void WEditDialog::initialize() {
/******************************/

    WPoint avg;
    WPoint max;
    setSystemFont( FALSE );
    textMetrics( avg, max );

    int x = WSystemMetrics::dialogFrameWidth();
    int y = WSystemMetrics::dialogFrameHeight();

    int w = 64 * avg.x();
    int h = 8 * (max.y() + avg.y() / 2);

    _edit = new WMultiLineEditBox( this, WRect( x, y, w, h ) );
    _edit->show();
    y += h + avg.y() / 2;

    w = 50 * avg.x() / 4;
    h = 14 * avg.y() / 8;

    WDefPushButton *bOk = new WDefPushButton( this, WRect( x, y, w, h ), OKText );
    bOk->onClick( this, (cbw)&WEditDialog::okButton );
    bOk->show();
    x += w + max.x();

    WPushButton *bCancel = new WPushButton( this, WRect( x, y, w, h ), CancelText );
    bCancel->onClick( this, (cbw)&WEditDialog::cancelButton );
    bCancel->show();

    shrink();
    centre();

    _edit->setText( *_reply );
    _edit->select( 0, 0 );
    _edit->setFocus();
    show();
}


void WEXPORT WEditDialog::okButton( WWindow* ) {
/**********************************************/

    _edit->getText( *_reply );
    quit( TRUE );
}


void WEXPORT WEditDialog::cancelButton( WWindow* ) {
/**************************************************/

    quit( FALSE );
}


bool WEXPORT WEditDialog::edit( WString& reply ) {
/************************************************/

    _reply = &reply;
    return( process() == TRUE );
}


#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

WEXPORT WEditDialog::~WEditDialog() {
/***********************************/

}

