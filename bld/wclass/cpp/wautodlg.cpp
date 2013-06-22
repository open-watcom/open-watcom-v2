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


#include "wautodlg.hpp"
#include "wtext.hpp"
#include "weditbox.hpp"
#include "wpshbttn.hpp"
#include "wfiledlg.hpp"
#include "wfilenam.hpp"
#include "wmetrics.hpp"

static  char    BrowseText[] = "&Browse...";
static  char    CancelText[] = "&Cancel";
static  char    OKText[] = "&OK";


WEXPORT WAutoDialog::WAutoDialog( WWindow *parent, const char *text,
                                  WStringList &prompts, unsigned editwidth )
    : WDialog( parent, text )
    , _prompts( &prompts )
    , _replys( NULL )
    , _editwidth( editwidth )
/*********************/
{

}


void WAutoDialog::updateExtents( const char *t, int *w, int *h ) {
/****************************************************************/

    if( getTextExtentX( t ) > *w ) {
        *w = getTextExtentX( t );
    }
    if( getTextExtentY( t ) > *h ) {
        *h = getTextExtentY( t );
    }
}


void WAutoDialog::initialize() {
/******************************/

    WText       *t;
    WPoint      avg;
    WPoint      max;
    int         i;

    setSystemFont( FALSE );
    int x = WSystemMetrics::dialogFrameWidth();
    int y = WSystemMetrics::dialogFrameHeight();
    textMetrics( avg, max );

    int t_w = 0;
    int t_h = 0;
    int icount = _prompts->count();
    for( i = 0; i < icount; i++ ) {
        updateExtents( _prompts->cStringAt( i ), &t_w, &t_h );
    }

    int b_w = 50 * avg.x() / 4;
    int b_h = 14 * avg.y() / 8;

    int p_w = _editwidth * avg.x();
    int p_h = max.y();
    t_w += avg.x() / 2;
    t_h += avg.y() / 2;
    p_w += avg.x() / 2;
    p_h += 2*max.y() / 3;
    int sp = max.x();

    for( i = 0; i < icount; i++ ) {
        WString s( _prompts->cStringAt( i ) );
        bool bro = FALSE;
        char* p = strchr( (char *)s.gets(), '=' );
        if( p != NULL ) { // this code depends on internals of WString!
            *p = '\0';
            p++;
            if( *p == '=' ) {
               bro = TRUE;
               p++;
            }
        }
        t = new WText( this, WRect( x, y + (p_h - t_h)/2, t_w, t_h ), s );
        t->show();
        WEditBox* e = new WEditBox( this,
                                    WRect( x + t_w + sp, y, p_w, p_h ), p );
        e->show();
        _inputs.add( e );
        if( bro ) {
            WPushButton* bBrowse = new WPushButton( this,
                WRect( x + t_w + p_w + 2*sp, y, b_w, b_h ), BrowseText );
            bBrowse->onClick( this, (cbw)&WAutoDialog::browseButton );
            bBrowse->setTagPtr( e );
            bBrowse->show();
        }
        y += t_h + t_h / 2;
    }

    WDefPushButton* bOk = new WDefPushButton( this, WRect( x, y, b_w, b_h ),
                                              OKText );
    bOk->onClick( this, (cbw)&WAutoDialog::okButton );
    bOk->show();

    WPushButton* bCancel = new WPushButton( this,
                    WRect( x + b_w + sp, y, b_w, b_h ), CancelText );
    bCancel->onClick( this, (cbw)&WAutoDialog::cancelButton );
    bCancel->show();

    shrink();
    centre();

    WEditBox* e = (WEditBox *)_inputs[0];
    e->select();
    e->setFocus();
    show();
}


void WEXPORT WAutoDialog::okButton( WWindow* ) {
/**********************************************/

    if( _replys != NULL ) {
        int icount = _prompts->count();
        for( int i=0; i<icount; i++ ) {
            WEditBox* e = (WEditBox*)_inputs[i];
            WString* s = new WString();
            e->getText( *s );
            _replys->add( s );
        }
    }
    quit( TRUE );
}


void WEXPORT WAutoDialog::cancelButton( WWindow* ) {
/**************************************************/

    quit( FALSE );
}


void WEXPORT WAutoDialog::browseButton( WWindow* b ) {
/****************************************************/

    WWindow* old = WWindow::hasFocus();
    WFileDialog bro( this );
    WFileName fn( bro.getOpenFileName( "", "Pick filename", WFOpenNew ) );
    if( fn.size() > 0 ) {
        WEditBox* e = (WEditBox*)((WPushButton*)b)->tagPtr();
        e->setText( fn );
    }
    old->setFocus();
}


bool WEXPORT WAutoDialog::getInput( WStringList &replys ) {
/*********************************************************/

    _replys = &replys;
    return( process() == TRUE );
}

#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

WEXPORT WAutoDialog::~WAutoDialog() {
/***********************************/

}

