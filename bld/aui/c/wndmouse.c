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
* Description:  AUI mouse input processing.
*
****************************************************************************/


#include "auipvt.h"//

static bool                     LButtonIsDown;
static bool                     RButtonIsDown;
static bool                     LButtonDownOnHot;
static wnd_coord                MouseDownLoc;
static wnd_coord                MouseUpLoc;
static wnd_line_piece           MouseLine;
static wnd_row                  MouseRow;


wnd_row WndGetMouseRow( void )
{
    return( MouseRow );
}

static void WndSetMouseRow( a_window *wnd, void *parm )
{
    gui_point           point;

    GUI_GET_POINT( parm, point );
    if( point.x < 0 ) point.x = 0;
    if( point.y < 0 ) point.y = 0;
    MouseRow = GUIGetRow( wnd->gui, &point );
    if( MouseRow > wnd->rows ) {
        MouseRow = WND_NO_ROW;
    }
}

static bool FindPoint( a_window *wnd, void *parm, bool exact, wnd_coord *spot )
{
    return( WndSetPoint( wnd, parm, exact, spot, WND_NO_ROW, FALSE ) );
}

static void WndButtonChange( a_window *wnd, wnd_coord *point, bool down )
{
    char                old_row;

    old_row = wnd->u.button_down.row;
    if( down ) {
        wnd->u.button_down.row = point->row;
        wnd->u.button_down.piece = point->piece;
    } else {
        wnd->u.button_down.row = (char)-1;
    }
    if( old_row != wnd->u.button_down.row ) {
        WndKillCacheLines( wnd );
        WndDirtyScreenPiece( wnd, point );
    }
}


void WndLButtonDown( a_window *wnd, void *parm )
{
    MouseLine.hot = FALSE;
    if( !FindPoint( wnd, parm, TRUE, &MouseDownLoc ) ) return;
    if( !WndGetLine( wnd, MouseDownLoc.row, MouseDownLoc.piece, &MouseLine ) ) return;
    LButtonDownOnHot = MouseLine.hot;
    LButtonIsDown = TRUE;
    if( MouseLine.hot ) {
        WndInternalStatusText( MouseLine.hint );
        WndButtonChange( wnd, &MouseDownLoc, TRUE );
        return;
    }
    if( MouseLine.tabstop ) {
        WndDirtyCurr( wnd );
        wnd->current = MouseDownLoc;
        WndSetCurrCol( wnd );
        WndDirtyCurr( wnd );
    }
    if( _Is( wnd, WSW_LBUTTON_SELECTS ) ) {
        _Set( wnd, WSW_SELECTING );
        WndSelSetStart( wnd, parm );
    }
}

void WndLButtonUp( a_window *wnd, void *parm )
{
    WndResetStatusText();
    LButtonIsDown = FALSE;
    FindPoint( wnd, parm, TRUE, &MouseUpLoc );
    if( LButtonDownOnHot ) {
        if( FindPoint( wnd, parm, TRUE, &MouseUpLoc ) ) {
            WndGetLine( wnd, MouseUpLoc.row, MouseUpLoc.piece, &MouseLine );
            if( MouseLine.hot && MouseDownLoc.row == MouseUpLoc.row &&
                MouseDownLoc.piece == MouseUpLoc.piece ) {
                WndModify( wnd, MouseUpLoc.row, MouseUpLoc.piece );
            }
        }
        WndButtonChange( wnd, &MouseDownLoc, FALSE );
    } else if( _Is( wnd, WSW_LBUTTON_SELECTS ) ) {
        WndSelPopItem( wnd, parm, FALSE );
    }
}

void WndLDblClk( a_window *wnd, void *parm )
{
    if( LButtonDownOnHot ) {
        WndLButtonUp( wnd, parm );
    } else {
        LButtonIsDown = FALSE;
        if( MouseLine.static_text ) return;
//      if( !FindPoint( wnd, parm, TRUE, &MouseUpLoc ) ) return;
        FindPoint( wnd, parm, TRUE, &MouseUpLoc );
        WndSetMouseRow( wnd, parm );
        if( !MouseLine.tabstop && _Is( wnd, WSW_ONLY_MODIFY_TABSTOP ) ) return;
        WndModify( wnd, MouseUpLoc.row, MouseUpLoc.piece );
    }
}


void WndRButtonDown( a_window *wnd, void *parm )
{
    MouseLine.hot = FALSE;
    MouseLine.tabstop = FALSE;
    MouseLine.static_text = TRUE;
    RButtonIsDown = TRUE;
    if( _Is( wnd, WSW_RBUTTON_CHANGE_CURR ) ) {
        if( FindPoint( wnd, parm, _Is( wnd, WSW_MUST_CLICK_ON_PIECE ), &MouseDownLoc ) ) {
            WndGetLine( wnd, MouseDownLoc.row, MouseDownLoc.piece, &MouseLine );
            WndDirtyCurr( wnd );
            if( MouseLine.tabstop ) {
                wnd->current = MouseDownLoc;
                WndSetCurrCol( wnd );
            } else {
                WndNoCurrent( wnd );
            }
            WndDirtyCurr( wnd );
        } else {
            WndDirtyCurr( wnd );
            WndNoCurrent( wnd );
            WndDirtyCurr( wnd );
        }
    }
    if( _Is( wnd, WSW_RBUTTON_SELECTS ) ) {
        _Set( wnd, WSW_SELECTING );
        WndSelSetStart( wnd, parm );
    }
}

void WndRButtonUp( a_window *wnd, void *parm )
{
    gui_point           point;

    RButtonIsDown = FALSE;
//    if( !FindPoint( wnd, parm, FALSE, &MouseUpLoc ) ) return;
    FindPoint( wnd, parm, FALSE, &MouseUpLoc );
    WndSetMouseRow( wnd, parm );
    WndSelPopItem( wnd, parm, TRUE );
    GUI_GET_POINT( parm, point );
    SetWndMenuRow( wnd );
    if( WndMenuRow == WND_NO_ROW && _Is( wnd, WSW_MENU_ACCURATE_ROW ) ) {
        WndMenuRow = MouseUpLoc.row;
        WndMenuPiece = MouseUpLoc.piece;
    }
    WndInvokePopUp( wnd, &point, NULL );
}

bool WndMouseButtonIsDown( void )
{
    return( LButtonIsDown || RButtonIsDown );
}

bool WndIgnoreMouseMove( a_window *wnd )
{
    return( _Isnt( wnd, WSW_SELECTING ) && !WndMouseButtonIsDown() );
}

void WndMouseMove( a_window *wnd, void *parm )
{
    if( _Is( wnd, WSW_SELECTING ) ) {
        WndSelChange( wnd, parm );
    } else if( LButtonIsDown && LButtonDownOnHot ) {
        if( !FindPoint( wnd, parm, TRUE, &MouseUpLoc ) ||
            MouseUpLoc.row != MouseDownLoc.row ||
            MouseUpLoc.piece != MouseDownLoc.piece ) {
            WndButtonChange( wnd, &MouseDownLoc, FALSE );
        } else {
            WndButtonChange( wnd, &MouseDownLoc, TRUE );
        }
    }
}
