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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"
#include <string.h>

int WndCharCol( char *buff, int col )
{
    char        *end,*curr;

    end = buff + col;
    for( curr = buff; curr < end; curr += GUICharLen( *curr ) ) ;
    if( curr > end ) col--;
    return( col );
}

int WndLastCharCol( wnd_line_piece *line )
{
    return( line->length == 0 ? 0 : WndCharCol( line->text, line->length-1 ) );
}

int WndPrevCharCol( char *buff, int idx )
{
    return( idx == 0 ? 0 : WndCharCol( buff, idx - 1 ) );
}

char *WndPrevChar( char *buff, char *curr )
{
    return( buff + WndPrevCharCol( buff, curr - buff ) );
}

void WndCurrToGUIPoint( a_window *wnd, gui_point *point )
{
    WndCoordToGUIPoint( wnd, &wnd->current, point );
}

extern  void    WndCoordToGUIPoint( a_window *wnd,
                                    wnd_coord *where, gui_point *point )
{
    wnd_line_piece      line;

    point->x = 0;
    point->y = 0;
    if( where->row == WND_NO_ROW ) return;
    if( !WndGetLine( wnd, where->row, where->piece, &line ) ) return;
    point->x = line.indent + GUIGetExtentX( wnd->gui, line.text, where->col );
    point->y = where->row * wnd->max_char.y;
}

extern  bool    WndPieceIsTab( a_window *wnd, int row, int piece )
{
    wnd_line_piece      line;

    return( WndGetLine( wnd, row, piece, &line ) && line.tabstop );
}


extern  bool    WndPieceIsHot( a_window *wnd, int row, int piece )
{
    wnd_line_piece      line;

    return( WndGetLine( wnd, row, piece, &line ) && line.hot );
}


extern  a_window        *WndFindActive()
{
    a_window    *wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( _Is( wnd, WSW_ACTIVE ) ) return( wnd );
    }
    return( WndMain );
}


extern  bool    WndValid( a_window *check )
{
    a_window    *wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( check == wnd ) return( TRUE );
    }
    return( FALSE );
}


void WndDirty( a_window *wnd )
{
    if( wnd == NULL ) {
        GUIWndDirty( NULL );
    } else {
        GUIWndDirty( wnd->gui );
    }
}


void WndZapped( a_window *wnd )
{
    WndNoCurrent( wnd );
    WndNoSelect( wnd );
    WndRepaint( wnd );
}

extern gui_ord  WndExtentX( a_window *wnd, const char *string )
{
    return( GUIGetExtentX( wnd->gui, string, strlen( string ) ) );
}


extern  a_window        *WndFindClass( a_window *first, wnd_class class )
{
    a_window    *wnd;

    for( wnd = WndNext( first ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( wnd->class == class ) return( wnd );
    }
    return( NULL );
}

void WndSetWndMax()
{
    gui_rect    rect;

    if( WndIsMinimized( WndMain ) ) return;
    GUIGetClientRect( WndMain->gui, &rect );
    WndMax.x = rect.width;
    WndMax.y = rect.height;
    WndMainResized();
}


a_window        *WndNext( a_window *wnd )
{
    gui_window  *gui;

    if( wnd == NULL ) {
        gui = GUIGetFront();
    } else {
        gui = GUIGetNextWindow( wnd->gui );
    }
    if( gui == NULL ) return( NULL );
    return( GUIGetExtra( gui ) );
}

extern void WndForAllClass( wnd_class class, void (*rtn)( a_window * ) )
{
    a_window    *wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( class == WND_NO_CLASS || WndClass( wnd ) == class ) {
            rtn( wnd );
        }
    }
}

