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


#include "wflashp.hpp"
#include "wtimer.hpp"
#include "wmetrics.hpp"
#include "wpshbttn.hpp"

WFlashPage::WFlashPage( WWindow* parent, WHotSpots* hs, int idx, int interval )
    : WWindow( parent, NULL, WStyleSimple | WStylePopup )
    , _idx( idx )
    , _width( 0 )
    , _height( 0 )
    , _hotSpot( hs )
    , _flashTimer( NULL )
//-------------------------------------------------------------------------
{
    init( interval, NULL );
}

WFlashPage::WFlashPage( WWindow* parent, WHotSpots* hs, int idx, const char* title, int interval )
    : WWindow( parent, title, WStyleSimpleFrame | WStylePopup )
    , _idx( idx )
    , _width( 0 )
    , _height( 0 )
    , _hotSpot( hs )
    , _flashTimer( NULL )
//-------------------------------------------------------------------------
{
    init( interval, title );
}

WFlashPage::WFlashPage( WWindow* parent, WHotSpots* hs, int idx, const char* title, const char* text[], int interval )
    : WWindow( parent, title, WStyleSimpleFrame | WStylePopup )
    , _idx( idx )
    , _width( 0 )
    , _height( 0 )
    , _hotSpot( hs )
    , _flashTimer( NULL )
//-------------------------------------------------------------------------
{
    if( text ) {
        for( int i=0; text[i]; i++ ) {
            int w = getTextExtentX( text[i] );
            if( _width < w ) _width = w;
            _height += getTextExtentY( text[i] );
            _text.add( new WString( text[ i ] ) );
        }
    }
    init( interval, title );
}

void WFlashPage::init( int interval, const char *title )
{
    WPoint hotSize;
    _hotSpot->hotSpotSize( _idx, hotSize );
    if( _width < hotSize.x() ) _width = hotSize.x();
    _height += hotSize.y();

    if( interval == INTERVAL_OK_BUTTON ) {
        static const char ok[] = { "OK" };
        int w = getTextExtentX( ok ) * 3;
        int h = getTextExtentY( ok ) * 3/2;
        int xoff = (_width - w) / 2;
        int yoff = _height + h / 2;
        WDefPushButton* bOk = new WDefPushButton( this, WRect( xoff, yoff, w, h), ok );
        bOk->onClick( this, (cbw)&WFlashPage::okButton );
        bOk->setFocus();
        bOk->show();
        _height += 2*h;
    } else {
        if( _width - hotSize.x() ) {
            _height += WSystemMetrics::captionSize();
        }
    }

    WOrdinal wheight = _height;
    if( title ) {
        wheight += WSystemMetrics::captionSize();
    }
    WOrdinal wwidth = _width;

    move( 0, 0 );
    size( wwidth, wheight );
    centre();
    show();
    update();   // force the paint method.
    if( interval > 0 ) {
        _flashTimer = new WTimer( this, (cbt)&WFlashPage::flashTimer );
        _flashTimer->start( (WORD)interval, 1 );
    }
}

WFlashPage::~WFlashPage()
//-----------------------
{
    _text.deleteContents();
    delete _flashTimer;
}

void WFlashPage::flashTimer( WTimer*, DWORD )
//-------------------------------------------
{
    delete this;
}

void WFlashPage::okButton( WButton* )
{
    delete this;
}

bool WFlashPage::paint()
//-----------------------
{
    for( int i=0; i<_text.count(); i++ ) {
        const char* text = _text.cStringAt( i );
        int off = (_width - getTextExtentX( text )) / 2;
        drawTextExtent( i, off, text, WPaintAttrBackground, _width );
    }
    WPoint hotSize;
    _hotSpot->hotSpotSize( _idx, hotSize );
    drawHotSpot( _idx, _text.count(), (_width - hotSize.x())/2 );
    return TRUE;
}
