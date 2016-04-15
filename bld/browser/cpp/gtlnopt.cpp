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
#include <wtext.hpp>
#include <wmsgdlg.hpp>

#include "brwhelp.h"
#include "gtlnopt.h"
#include "screendv.h"
#include "util.h"
#include "wbrwin.h"

#define _NumStyles      5
#define _StyleColumn    0

static struct {
    uint        _thk;
    WLineStyle  _stl;
} Styles[ _NumStyles ] = {
    { 1, LS_PEN_SOLID },
    { 1, LS_PEN_DASH },
    { 1, LS_PEN_DOT },
    { 1, LS_PEN_DASHDOT },
    { 1, LS_PEN_DASHDOTDOT },
/* line thickness not supported under PM
    { 2, LS_PEN_SOLID },
    { 3, LS_PEN_SOLID },
    { 4, LS_PEN_SOLID },
    { 5, LS_PEN_SOLID },
    { 6, LS_PEN_SOLID },
*/
};

#define _NumColours     5

static Color Colours[ _NumColours ] = {
    ColorBlack,
    ColorRed,
    ColorGreen,
    ColorBlue,
    CUSTOM_RGB(255,255,0),      // yellow
};

GTLineOption::GTLineOption( const char * text,
                            GTLineEditor * parent,
                            PaintInfo * inf, bool line )
            : GTLineOptDlg( this )
            , WDialog( frame().r, text )
            , _parent( parent )
            , _info( inf )
            , _line( line )
            , _inStyle( _line )
//---------------------------------------------------
{
}

GTLineOption::~GTLineOption()
//---------------------------
{
    _parent->endEdit();
}

bool GTLineOption::paint()
//------------------------
{
    ScreenDev   dev;
    PaintInfo   black( ColorBlack, 1, LS_PEN_SOLID );
    PaintInfo   gray( ColorDarkGray, 1, LS_PEN_DOT );
    PaintInfo   pinf;
    WRect       r;
    int         i;

    dev.open( this );

    dev.setPaintInfo( &black );

    r = _exampleTextR.r;
    dev.drawText( WPoint( r.x(), r.y() ), _exampleTextR.t );

    if( _line ) {
        r = _styleTextR.r;
        dev.drawText( WPoint( r.x(), r.y() ), _styleTextR.t );
    }

    r = (_line) ? _colourTextR.r : _styleTextR.r;
    dev.drawText( WPoint( r.x(), r.y() ), _colourTextR.t );

    if( _line ) {
        dev.rectangle( _styleBoxR.r );
        dev.rectangle( _colourBoxR.r );
    } else {
        r = _colourBoxR.r;
        r.x( _styleBoxR.r.x() );
        r.w( _colourBoxR.r.x() - _styleBoxR.r.x() + _colourBoxR.r.w() );
        dev.rectangle( r );
    }

    getPinfo( pinf );

    dev.setPaintInfo( &pinf );
    if( _line ) {
        dev.moveTo( _exampleLineR.r.x(),
                    _exampleLineR.r.y() + _exampleLineR.r.h() / 2 );
        dev.lineTo( _exampleLineR.r.x() + _exampleLineR.r.w(),
                    _exampleLineR.r.y() + _exampleLineR.r.h() / 2 );
    } else {
        WRect r( _exampleRectR.r );
        r.x( r.x() + r.w() / 4 );
        r.w( r.w() - r.w() / 2 );
        r.y( r.y() + r.h() / 4 );
        r.h( r.h() - r.h() / 2 );

        dev.rectangle( r );
    }

    if( _line ) {
        for( i = 0; i < _NumStyles; i += 1 ) {
            WRect     r( _styleRects[ i ] );
            PaintInfo p( ColorBlack, Styles[ i ]._thk, Styles[ i ]._stl );

            dev.setPaintInfo( &p );
            dev.moveTo( r.x(), r.y() + r.h() / 2 );
            dev.lineTo( r.x() + r.w(), r.y() + r.h() / 2 );

            if( i == _style ) {
                if( _inStyle ) {
                    dev.setPaintInfo( &black );
                } else {
                    dev.setPaintInfo( &gray );
                }
                dev.rectangle( _styleRects[ i ] );
            }
        }
    }

    for( i = 0; i < _NumColours; i += 1 ) {
        WRect r( _colrRects[ i ] );
        PaintInfo p( Colours[i], 1, LS_PEN_SOLID );

        dev.setPaintInfo( &p );
        dev.moveTo( r.x(), r.y() + r.h() / 2 );
        dev.lineTo( r.x() + r.w(), r.y() + r.h() / 2 );

        if( i == _colour ) {
            if( _inStyle ) {
                dev.setPaintInfo( &gray );
            } else {
                dev.setPaintInfo( &black );
            }
            dev.rectangle( _colrRects[ i ] );
        }
    }

    dev.close();

    return true;
}

void GTLineOption::initialize()
//-----------------------------
{
    int i;
    setSystemFont( false );
    rescale();
    move( frame().r );
    centre();

    _style = 0;
    for( i = 0; i < _NumStyles; i += 1 ) {
        if( _info->getStyle() == Styles[ i ]._stl &&
            _info->getThickness() == Styles[ i ]._thk ) {
            _style = i;
            break;
        }
    }

    _colour = 0;
    for( i = 0; i < _NumColours; i += 1 ) {
        if( _info->getColour() == Colours[ i ] ) {
            _colour = i;
            break;
        }
    }

    _okButton =         new WDefPushButton( this, _okButtonR.r,         _okButtonR.t );
    _cancelButton =     new WPushButton(    this, _cancelButtonR.r,     _cancelButtonR.t );
    _helpButton =       new WPushButton(    this, _helpButtonR.r,       _helpButtonR.t );

    _okButton->show();
    _cancelButton->show();
    _helpButton->show();

    _okButton->onClick(     this, (cbw) GTLineOption::okButton );
    _cancelButton->onClick( this, (cbw) GTLineOption::cancelButton );
    _helpButton->onClick(   this, (cbw) GTLineOption::helpButton );

    _styleRects[ 0 ] =_stl1RectR.r;
    _styleRects[ 1 ] =_stl2RectR.r;
    _styleRects[ 2 ] =_stl3RectR.r;
    _styleRects[ 3 ] =_stl4RectR.r;
    _styleRects[ 4 ] =_stl5RectR.r;

/* thick lines not supported by PM
    _styleRects[ 5 ] =_thick1RectR.r;
    _styleRects[ 6 ] =_thick2RectR.r;
    _styleRects[ 7 ] =_thick3RectR.r;
    _styleRects[ 8 ] =_thick4RectR.r;
    _styleRects[ 9 ] =_thick5RectR.r;
*/

    _colrRects[ 0 ] =_colr1RectR.r;
    _colrRects[ 1 ] =_colr2RectR.r;
    _colrRects[ 2 ] =_colr3RectR.r;
    _colrRects[ 3 ] =_colr4RectR.r;
    _colrRects[ 4 ] =_colr5RectR.r;

    if( !_line ) {
        WOrdinal add = _colourBoxR.r.x() - _styleBoxR.r.x();

        for( i = 0; i < _NumColours; i += 1 ) {
            _colrRects[ i ].x( _styleRects[ i ].x() );
            _colrRects[ i ].w( _colrRects[ i ].w() + add );
        }
    }

    _okButton->setFocus();

    show();
}

void GTLineOption::updateRects( WRect *prev_rect )
//------------------------------------------------
{
    if( _line ) {
        invalidateRect( _exampleLineR.r );
    } else {
        invalidateRect( _exampleRectR.r );
    }
    invalidateRect( *prev_rect );
    if( _inStyle ) {
        invalidateRect( _styleRects[ _style ] );
    } else {
        invalidateRect( _colrRects[ _colour ] );
    }
}

bool GTLineOption::leftBttnDn( int x, int y, WMouseKeyFlags )
//-----------------------------------------------------------
{
    int         i;
    WRect       *prev_rect;

    if( _line ) {
        for( i = 0; i < _NumStyles; i += 1 ) {
            if( _styleRects[ i ].contains( x, y ) ) {
                prev_rect = &_styleRects[ _style ];
                _style = i;
                _inStyle = true;
                updateRects( prev_rect );
                return true;
            }
        }
    }

    for( i = 0; i < _NumColours; i += 1 ) {
        if( _colrRects[ i ].contains( x, y ) ) {
            prev_rect = &_colrRects[ _colour ];
            _colour = i;
            _inStyle = false;
            updateRects( prev_rect );
            return true;
        }
    }

    return true;
}

bool GTLineOption::leftBttnDbl( int x, int y, WMouseKeyFlags )
//------------------------------------------------------------
{
    int i;

    if( _line ) {
        for( i = 0; i < _NumStyles; i += 1 ) {
            if( _styleRects[ i ].contains( x, y ) ) {
                if( i == _style ) {
                    okButton( NULL );
                    return true;
                }
            }
        }
    }

    for( i = 0; i < _NumColours; i += 1 ) {
        if( _colrRects[ i ].contains( x, y ) ) {
            if( i == _colour ) {
                okButton( NULL );
                return true;
            }
            return true;
        }
    }

    return true;
}

bool GTLineOption::keyDown( WKeyCode kc, WKeyState ks )
//-----------------------------------------------------
{
    WRect       *prev_rect;

    if( _inStyle ) {
        prev_rect = &_styleRects[ _style ];
    } else {
        prev_rect = &_colrRects[ _colour ];
    }
    switch( kc ) {
    case WKeyUp:
        if( _inStyle ) {
            _style -= 1;
        } else {
            _colour -= 1;
        }
        _colour = (_colour + _NumColours) % _NumColours;
        _style = (_style + _NumStyles) % _NumStyles;
        updateRects( prev_rect );
        return true;
    case WKeyDown:
        if( _inStyle ) {
            _style += 1;
        } else {
            _colour += 1;
        }
        _colour = (_colour + _NumColours) % _NumColours;
        _style = (_style + _NumStyles) % _NumStyles;
        updateRects( prev_rect );
        return true;
    case WKeyLeft:
    case WKeyRight:
        if( _line ) {
            _inStyle = !_inStyle;
            updateRects( prev_rect );
        }
        return true;
    }
    return( WDialog::keyDown( kc, ks ) );
}

void GTLineOption::getPinfo( PaintInfo & pinf )
//---------------------------------------------
{
    pinf = PaintInfo( Colours[ _colour ],
                      Styles[ _style ]._thk, Styles[ _style ]._stl );
}


void GTLineOption::okButton( WWindow * )
//--------------------------------------
{
    getPinfo( *_info );
    _parent->setInfo( _info );
    quit( true );
}

void GTLineOption::cancelButton( WWindow * )
//------------------------------------------
{
    _parent->setInfo( NULL );
    quit( false );
}

void GTLineOption::helpButton( WWindow * )
//----------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_LINE_OPTIONS );
}

bool GTLineOption::contextHelp( bool is_active_win )
//--------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_LINE_OPTIONS );
    }
    return( true );
}
