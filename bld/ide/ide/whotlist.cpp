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


#include "whotlist.hpp"
#include <limits.h>     // NYI -- only for scroll bandage
#include <wkeydefs.hpp>


WHotSpotList::WHotSpotList( WWindow * prt, const WRect & r, const char * text, WHotSpots* hs,
                            WStyle wstyle, WExStyle wexstyle )
    : WWindow( prt, r, text, wstyle, wexstyle )
    , _topIndex( 0 )
    , _selected( -1 )
    , _leftDown( FALSE )
    , _hotPressIdx( -1 )
    , _inHotZone( FALSE )
    , _changedClient( NULL )
    , _changed( NULL )
    , _dblClickClient( NULL )
    , _dblClick( NULL )
    , _hotPressClient( NULL )
    , _hotPress( NULL )
    , _hs( hs )
//--------------------------------------------------------------
{
    changeBackground( WPaintAttrControlBackground );
}

bool WHotSpotList::gettingFocus( WWindow* )
//-----------------------
{
    return FALSE;
}

void WHotSpotList::adjustScrollBars()
{
    int scrollr = 0;
    int cnt = count();
    if( cnt > getRows() ) {
        scrollr = cnt;
    }
    setScrollTextRange( WScrollBarVertical, scrollr );
    setScrollTextPos( WScrollBarVertical, _topIndex );
    setScrollRange( WScrollBarHorizontal, width() );
}

void WHotSpotList::resized( WOrdinal w, WOrdinal h )
//-----------------------
{
    int         newtop;

    WWindow::resized( w, h );
    newtop = count() - getRows();
    if( newtop < 0 ) newtop = 0;
    if( newtop < _topIndex ) {
        performScroll( newtop, TRUE );
    }
    adjustScrollBars();
}

bool WHotSpotList::paint()
//-----------------------
{
    int offset;
    int maxRows = numDirtyRows() + _topIndex + firstDirtyRow();
    int numElem = count();
    int extent;
    WRect r;

    getClientRect( r );

    for( int i = _topIndex + firstDirtyRow(); i < maxRows && i < numElem; i += 1 ) {
        const char * str = getString( i );

        int hotSpot;
        if( i == _hotPressIdx && _inHotZone ) {
            hotSpot = getHotSpot( i, TRUE );
        } else {
            hotSpot = getHotSpot( i, FALSE );
        }

        if( _hs != NULL ) {
            WPoint hotSize;
            _hs->hotSpotSize( hotSpot, hotSize );
            offset = getHotOffset( i ) + hotSize.x();
            drawHotSpot( hotSpot, i - _topIndex, getHotOffset( i ) );
        } else {
            offset = 0;
        }
        extent = r.w();
        if( width() > extent ) extent = width();
        if( i == _selected ) {
            drawTextExtent( i - _topIndex, offset, str, WPaintAttrMenuActive,
                            extent );
        } else {
            drawTextExtent( i - _topIndex, offset, str, WPaintAttrControlBackground,
                            extent );
        }
    }

    return TRUE;
}

bool WHotSpotList::mouseMove( int x, int y, WMouseKeyFlags )
//---------------------------------------------------------
{
    if( _leftDown ) {
        int row = getRow( WPoint( x, y ) );
        int oldSel = _selected;

        if( row < 0 ) row = 0;
        if( row > getRows() - 1 ) {
            row = getRows() - 1;
        }

        row += _topIndex;

        if( _hs != NULL && _hotPressIdx >= 0 ) {

            WPoint hotSize;
            _hs->hotSpotSize( getHotSpot( _hotPressIdx, FALSE ), hotSize );
            int hotOffset = getHotOffset( _hotPressIdx );

            if( row == _hotPressIdx
                && x >= hotOffset
                && x <= hotOffset + hotSize.x() ) {

                if( !_inHotZone ) {
                    _inHotZone = TRUE;
                    invalidateRow( _hotPressIdx - _topIndex );
                }
            } else {
                if( _inHotZone ) {
                    _inHotZone = FALSE;
                    invalidateRow( _hotPressIdx - _topIndex );
                }
            }
        } else {
            if( row >= count() ) {
                row = count() - 1;
            }

            if( _selected != row ) {
                _selected = row;

                invalidateRow( oldSel - _topIndex );
                invalidateRow( _selected - _topIndex );
                scrollToSelected();
            }
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

bool WHotSpotList::leftBttnDn( int x, int y, WMouseKeyFlags f )
//------------------------------------------------------------
{
    int row = getRow( WPoint( x, y ) ) + _topIndex;

    if( row < 0 ) row = 0;
    if( row >= count() ) row = count() - 1;
    if( row < 0 ) {     // count == 0
        return FALSE;
    }

    if( _hs != NULL ) {
        WPoint  hotSize;
        _hs->hotSpotSize( getHotSpot( row, FALSE ), hotSize );
        int hotOffset = getHotOffset( row );

        if( x > hotOffset && x < hotOffset + hotSize.x() ) {
            _hotPressIdx = row;
        }
    }

    _leftDown = TRUE;

    mouseMove( x, y, f );

    return TRUE;
}

bool WHotSpotList::leftBttnUp( int x, int y, WMouseKeyFlags f )
//------------------------------------------------------------
{
    mouseMove( x, y, f );
    _leftDown = FALSE;

    if( _inHotZone && getRow( WPoint( x, y ) ) + _topIndex == _hotPressIdx ) {
        // have to set false before calling invalidateRow().
        _inHotZone = FALSE;

        if( _selected != _hotPressIdx ) {
            int oldSel = _selected;
            _selected = _hotPressIdx;
            invalidateRow( oldSel - _topIndex );
        }

        if( _hotPressClient && _hotPress )
            (_hotPressClient->*_hotPress)( this );

        invalidateRow( _selected - _topIndex );
    }

    _inHotZone = FALSE;
    _hotPressIdx = -1;

    changed();

    return TRUE;
}

bool WHotSpotList::leftBttnDbl( int x, int y, WMouseKeyFlags )
//-----------------------------------------------------------
{
    int row = getRow( WPoint( x, y ) ) + _topIndex;

    if( _selected == row ) {
        if( _dblClickClient && _dblClick ) {
            (_dblClickClient->*_dblClick)( this );
        }
    }
    invalidateRow( _selected - _topIndex );

    return TRUE;
}

bool WHotSpotList::rightBttnDn( int x, int y, WMouseKeyFlags f )
//------------------------------------------------------------
{
    int row = getRow( WPoint( x, y ) ) + _topIndex;
    if( row >=0 && row < count() ) {
        select( row );
        reset();
    }
    return WWindow::rightBttnDn( x, y, f );
}

void WHotSpotList::onChanged( WObject* obj, cbw changed )
//------------------------------------------------------
{
    _changedClient = obj;
    _changed = changed;
}

void WHotSpotList::onDblClick( WObject* obj, cbw dblClick )
//--------------------------------------------------------
{
    _dblClickClient = obj;
    _dblClick = dblClick;
}

void WHotSpotList::onHotPress( WObject* obj, cbw hotPress )
//--------------------------------------------------------
{
    _hotPressClient = obj;
    _hotPress = hotPress;
}

int WHotSpotList::selected()
//-------------------------
{
    return _selected;
}

void WHotSpotList::setSelected( int index )
//-----------------------------------------
{
    if( index < 0 ) {
        _selected = 0;
    } else {
        _selected = index;
    }
}

void WHotSpotList::select( int index )
//-----------------------------------
{
    int         oldsel;

    oldsel = _selected;
    _selected = index;
    invalidateRow( oldsel - _topIndex );
    invalidateRow( _selected - _topIndex );
}

void WHotSpotList::reset()
//-----------------------
{
    adjustScrollBars();
    invalidate();
}

void WHotSpotList::scrollToSelected()
//----------------------------------
{
    int nRows = getRows();

    if( _selected < _topIndex ) {
        performScroll( _selected, TRUE );
    }
    if( _selected > _topIndex + nRows - 1 ) {
        performScroll( _selected - nRows + 1, TRUE );
    }
}

void WHotSpotList::changed()
//-------------------------
{
    if( _changedClient && _changed ) {
        (_changedClient->*_changed)( this );
    }
}

void WHotSpotList::performScroll( int pos, bool absolute )
//--------------------------------------------------------
{
    if( absolute ) {
        _topIndex = pos;
    } else {
        _topIndex += pos;
    }

    int cnt = count();
    int nRows = getRows();

    if( _topIndex < 0 || cnt <= nRows ) {
        _topIndex = 0;
    } else if( _topIndex >= cnt - nRows ) {
        _topIndex = cnt - nRows;
    }
    setScrollTextPos( WScrollBarVertical, _topIndex );
}

bool WHotSpotList::scrollNotify( WScrollNotification sn, int diff )
//----------------------------------------------------------------
{
    switch( sn ) {
        case WScrollUp:
            performScroll( -1 );
            return TRUE;

        case WScrollPageUp:
            performScroll( -1 * getRows() + 1 );
            return TRUE;

        case WScrollDown:
            performScroll( 1 );
            return TRUE;

        case WScrollPageDown:
            performScroll( getRows() - 1 );
            return TRUE;

        case WScrollVertical:
            performScroll( diff );
            return TRUE;
    }

    return FALSE;
}

bool WHotSpotList::keyDown(  WKeyCode key, WKeyState state )
//---------------------------------------------------------
{
    int nRows = getRows();
    int oldSel = _selected;

    if( state == WKeyStateNone ) {
        switch( key ) {
        case WKeyPageup:
            _selected -= nRows - 1;
            if( _selected < 0 ) _selected = 0;

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _topIndex );
                invalidateRow( _selected - _topIndex );
            }
            scrollToSelected();
            changed();
            return TRUE;

        case WKeyPagedown:
            _selected += nRows - 1;
            if( _selected >= count() ) {
                _selected = count() - 1;
            }

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _topIndex );
                invalidateRow( _selected - _topIndex );
            }
            scrollToSelected();
            changed();
            return TRUE;

        case WKeyUp:
            _selected -= 1;
            if( _selected < 0 ) _selected = 0;

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _topIndex );
                invalidateRow( _selected - _topIndex );
            }
            scrollToSelected();
            changed();
            return TRUE;

        case WKeyDown:
            _selected += 1;
            if( _selected >= count() ) {
                _selected = count() - 1;
            }

            if( oldSel != _selected ) {
                invalidateRow( oldSel - _topIndex );
                invalidateRow( _selected - _topIndex );
            }
            scrollToSelected();
            changed();
            return TRUE;

        case WKeyEnter:
            if( count() != 0 && _dblClickClient && _dblClick ) {
                (_dblClickClient->*_dblClick)( this );
                invalidateRow( _selected - _topIndex );
            }
            return TRUE;

        case WKeySpace:
            if( _hotPressClient && _hotPress ) {
                (_hotPressClient->*_hotPress)( this );
                return TRUE;
            }
            break;
        case WKeyLeft:
            {
                int col;
                col = getScrollTextPos( WScrollBarHorizontal );
                if( col > 0 ) {
                    setScrollTextPos( WScrollBarHorizontal, col - 1 );
                }
            }
            break;
        case WKeyRight:
            {
                int col;
                col = getScrollTextPos( WScrollBarHorizontal );
                setScrollTextPos( WScrollBarHorizontal, col + 1 );
            }
            break;

        }
    }
    return FALSE;
}


#ifdef __WATCOMC__
// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9
#endif

WHotSpotList::~WHotSpotList()
//-------------------------
{
}

