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


#include "wpickdlg.hpp"
#include "wpickbox.hpp"
#include "wpshbttn.hpp"
#include "wtext.hpp"
#include "wmetrics.hpp"

static  char    CancelText[] = "&Cancel";
static  char    OKText[] = "&OK";


WEXPORT WPickDialog::WPickDialog( WPickList& list, cbs getname,
                                  WWindow *parent, const char *text )
    : WDialog( parent, text )
    , _list( &list )
    , _getname( getname )
/********************/
{
    setDefaultQuitCode( -1 );
}


void WPickDialog::updateExtents( const char *t, int *w, int *h ) {
/****************************************************************/

    if( getTextExtentX( t ) > *w ) {
        *w = getTextExtentX( t );
    }
    if( getTextExtentY( t ) > *h ) {
        *h = getTextExtentY( t );
    }
}


void WPickDialog::initialize() {
/******************************/

    WPoint avg;
    WPoint max;
    textMetrics( avg, max );

    int x = WSystemMetrics::dialogFrameWidth();
    int y = WSystemMetrics::dialogFrameHeight();

    int w = 0;
    int h = 0;
    updateExtents( _promptText, &w, &h );
    w += avg.x() / 2;
    h += avg.y() / 2;
    _prompt = new WText( this, WRect( x, y, w, h ), _promptText );
    _prompt->show();
    y += h + avg.y()/2;

    w = 0;
    h = 0;
    WPoint org( x, y );
    _input = new WPickBox( *_list, _getname, this, org );
    _input->onDblClick( this, (cbw)&WPickDialog::picked );
    _input->show();

    WRect r;
    _input->getRectangle( r );
    y += r.h() + max.y();

    w = 0;
    h = 0;
    updateExtents( CancelText, &w, &h );
    updateExtents( OKText, &w, &h );
    w += avg.x() * 2;
    h += avg.y() / 2;

    WDefPushButton *bOk = new WDefPushButton( this, WRect( x, y, w, h ),
                                              OKText );
    bOk->onClick( this, (cbw)&WPickDialog::okButton );
    bOk->show();
    x += w + max.x();

    WPushButton *bCancel = new WPushButton( this, WRect( x, y, w, h ),
                                            CancelText );
    bCancel->onClick( this, (cbw)&WPickDialog::cancelButton );
    bCancel->show();

    shrink();
    centre();

    _input->setFocus();
    show();
}


void WEXPORT WPickDialog::picked( WWindow * ) {
/*********************************************/

    quit( _input->selected() );
}


void WEXPORT WPickDialog::okButton( WWindow * ) {
/***********************************************/

    quit( _input->selected() );
}


void WEXPORT WPickDialog::cancelButton( WWindow * ) {
/***************************************************/

    quit( -1 );
}


int WEXPORT WPickDialog::pickOne( const char *prompt ) {
/******************************************************/

    _promptText = prompt;
    return( process() );
}


#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

WEXPORT WPickDialog::~WPickDialog() {
/***********************************/

}
