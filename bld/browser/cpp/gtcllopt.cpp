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


#include <wpshbttn.hpp>
#include <wmsgdlg.hpp>

#include "brwhelp.h"
#include "gtcllopt.h"
#include "gtfnopt.h"
#include "optmgr.h"
#include "screendv.h"
#include "util.h"
#include "wbrwin.h"

const char * const Desc[ 7 ] = {
    "Called Once",
    "Called Twice",
    "Called Three Times",
    "Called Four Times",
    "Called Five+ Times",
    "Occurs Once",
    "Repeated",
};

GTCallOption::GTCallOption( const GTFunctionOpts & opt )
            : CallOptDlg( this )
            , WDialog( frame().r, frame().t )
            , _hasFocus( 0 )
//---------------------------------------------
{
    _options = new GTFunctionOpts( opt );
    _rects = new WRect[ 7 ];
}

GTCallOption::~GTCallOption()
//---------------------------
{
    delete _options;
    delete [] _rects;
}

void GTCallOption::initialize()
//-----------------------------
{
    rescale();
    move( frame().r );
    centre();

    _okButton =        new WDefPushButton( this, _okButtonR.r,        _okButtonR.t );
    _cancelButton =    new WPushButton(    this, _cancelButtonR.r,    _cancelButtonR.t );
    _modifyButton =    new WPushButton(    this, _modifyButtonR.r,    _modifyButtonR.t );
    _helpButton =      new WPushButton(    this, _helpButtonR.r,      _helpButtonR.t );

    _okButton->show();
    _cancelButton->show();
    _modifyButton->show();
    _helpButton->show();

    _okButton->onClick(     this, (cbw) GTCallOption::okButton );
    _cancelButton->onClick( this, (cbw) GTCallOption::cancelButton );
    _modifyButton->onClick( this, (cbw) GTCallOption::modifyButton );
    _helpButton->onClick(   this, (cbw) GTCallOption::helpButton );

    _okButton->setFocus();

    _rects[ 0 ] = _oneLineR.r;
    _rects[ 1 ] = _twoLineR.r;
    _rects[ 2 ] = _threeLineR.r;
    _rects[ 3 ] = _fourLineR.r;
    _rects[ 4 ] = _fiveLineR.r;
    _rects[ 5 ] = _occOnceBoxR.r;
    _rects[ 6 ] = _occManyBoxR.r;

    show();
}

void GTCallOption::okButton( WWindow * )
//--------------------------------------
{
    WBRWinBase::optManager()->setFunctionOpts( _options );
    quit( TRUE );
}

void GTCallOption::cancelButton( WWindow * )
//------------------------------------------
{
    WBRWinBase::optManager()->setFunctionOpts( NULL );
    quit( FALSE );
}

void GTCallOption::modifyButton( WWindow * )
//------------------------------------------
{
    PaintInfo * p;
    if( _hasFocus < 5 ) {
        p = &_options->numCalls( _hasFocus + 1 );
        GTLineOption mod( Desc[ _hasFocus ], this, p, TRUE );
        mod.process( this );
    } else {
        p = &_options->repeated( (bool) (_hasFocus - 5) );
        GTLineOption mod( Desc[ _hasFocus ], this, p, FALSE );
        mod.process( this );
    }
}

void GTCallOption::helpButton( WWindow * )
//----------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_CALL_TREE_OPTIONS );
}

bool GTCallOption::contextHelp( bool is_active_win )
//--------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_CALL_TREE_OPTIONS );
    }
    return( TRUE );
}

int GTCallOption::inRect( int x, int y )
//--------------------------------------
{
    int inr = -1;
    int i;

    for( i = 0; i < 7; i += 1 ) {
        WRect & r( _rects[ i ] );

        if( r.contains( x, y ) ) {
            inr = i;
            break;
        }
    }

    return inr;
}

bool GTCallOption::leftBttnDn( int x, int y, WMouseKeyFlags )
//-----------------------------------------------------------
{
    int focusTo = inRect( x, y );

    if( focusTo >= 0 && focusTo != _hasFocus ) {
        int prev_focus = _hasFocus;
        _hasFocus = focusTo;
        invalidateRect( _rects[ prev_focus ] );
        invalidateRect( _rects[ _hasFocus ] );
    }
    return TRUE;
}

bool GTCallOption::leftBttnDbl( int x, int y, WMouseKeyFlags )
//------------------------------------------------------------
{
    if( _hasFocus == inRect( x, y ) ) {
        modifyButton( NULL );
    }
    return TRUE;
}

#define COL_1   5
#define COL_2   2

bool GTCallOption::keyDown( WKeyCode kc, WKeyState ks )
//-----------------------------------------------------
{
    int         prev_focus = _hasFocus;

    switch( kc ) {
    case WKeyUp:
        if( _hasFocus < COL_1 ) {
            _hasFocus = (_hasFocus - 1 + COL_1) % COL_1;
        } else {
            _hasFocus = COL_1 + (_hasFocus - COL_1 - 1 + COL_2) % COL_2;
        }
        invalidateRect( _rects[ prev_focus ] );
        invalidateRect( _rects[ _hasFocus ] );
        return TRUE;
    case WKeyDown:
        if( _hasFocus < COL_1 ) {
            _hasFocus = (_hasFocus + 1) % COL_1;
        } else {
            _hasFocus = COL_1 + (_hasFocus - COL_1 + 1) % COL_2;
        }
        invalidateRect( _rects[ prev_focus ] );
        invalidateRect( _rects[ _hasFocus ] );
        return TRUE;
    case WKeyLeft:
    case WKeyRight:
        if( _hasFocus < COL_1 ) {
            _hasFocus = COL_1;
        } else {
            _hasFocus = 0;
        }
        invalidateRect( _rects[ prev_focus ] );
        invalidateRect( _rects[ _hasFocus ] );
        return TRUE;
    }
    return( WDialog::keyDown( kc, ks ) );
}

static void OutRect( const ControlRect & rect, ScreenDev & dev )
//--------------------------------------------------------------
{
    WPoint      p( rect.r.x(), rect.r.y() );

    dev.drawText( p, rect.t );
}

bool GTCallOption::paint()
//------------------------
{
    int         i;
    ScreenDev   dev;
    PaintInfo   black( ColorBlack, 1, LS_PEN_SOLID );
    PaintInfo   gray( ColorDarkGray, 1, LS_PEN_DOT );

    dev.open( this );

    dev.setPaintInfo( &black );
    dev.rectangle( _numCallsBoxR.r );
    dev.rectangle( _repetitionBoxR.r );

    OutRect( _numCallsTextR, dev );
    OutRect( _repetitionTextR, dev );
    OutRect( _oneTextR, dev );
    OutRect( _twoTextR, dev );
    OutRect( _threeTextR, dev );
    OutRect( _fourTextR, dev );
    OutRect( _fiveTextR, dev );
    OutRect( _occOnceTextR, dev );
    OutRect( _occManyTextR, dev );

    for( i = 1; i <= 5; i += 1 ) {
        WRect &     r( _rects[ i - 1 ] );
        PaintInfo   p( _options->numCalls( i ) );

        dev.setPaintInfo( &p );

        dev.moveTo( r.x(), r.y() + r.h() / 2 );
        dev.lineTo( r.x() + r.w(), r.y() + r.h() / 2 );

        if( (i - 1) == _hasFocus ) {
            dev.setPaintInfo( &gray );
            dev.rectangle( _rects[ i - 1 ] );
        }
    }

    for( i = 0; i < 2; i += 1 ) {
        WRect       r;
        PaintInfo   p( _options->repeated( (bool) i ) );

        dev.setPaintInfo( &p );

        r.x( _rects[ i + 5 ].x() + _rects[ i + 5 ].w() / 4 );
        r.w( _rects[ i + 5 ].w() / 2 );
        r.y( _rects[ i + 5 ].y() + _rects[ i + 5 ].h() / 4 );
        r.h( _rects[ i + 5 ].h() / 2 );

        dev.rectangle( r );

        if( (i + 5) == _hasFocus ) {
            dev.setPaintInfo( &gray );
            dev.rectangle( _rects[ i + 5 ] );
        }
    }

    dev.close();

    return TRUE;
}

void GTCallOption::endEdit()
//-----------------------------
// line editor informs us it is dead
// so don't access it any more
{
}

void GTCallOption::setInfo( PaintInfo * p )
//--------------------------------------------
// line editor wishes to be terminated (suicidal?)
// and may or may not have changed the PaintInfo
{
    if( p ) {
        invalidate();
    }
}
