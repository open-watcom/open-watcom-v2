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


#include "_aui.h"
#include <string.h>


wnd_colidx WndCharColIdx( const char *buff, wnd_colidx colidx )
{
    const char  *end;
    const char  *curr;

    end = buff + colidx;
    for( curr = buff; curr < end; curr += GUICharLen( UCHAR_VALUE( *curr ) ) )
        ;
    if( curr > end )
        colidx--;
    return( colidx );
}

wnd_colidx WndLastCharColIdx( wnd_line_piece *line )
{
    return( ( line->length > 0 ) ? WndCharColIdx( line->text, (wnd_colidx)( line->length - 1 ) ) : 0 );
}

wnd_colidx WndPrevCharColIdx( const char *buff, wnd_colidx colidx )
{
    return( colidx > 0 ? WndCharColIdx( buff, colidx - 1 ) : 0 );
}

const char *WndPrevChar( const char *buff, const char *curr )
{
    return( buff + WndPrevCharColIdx( buff, (wnd_colidx)( curr - buff ) ) );
}

void WndCurrToGUIPoint( a_window wnd, gui_point *point )
{
    WndCoordToGUIPoint( wnd, &wnd->current, point );
}

void    WndCoordToGUIPoint( a_window wnd, wnd_coord *where, gui_point *point )
{
    wnd_line_piece      line;

    point->x = 0;
    point->y = 0;
    if( where->row == WND_NO_ROW )
        return;
    if( !WndGetLine( wnd, where->row, where->piece, &line ) )
        return;
    point->x = line.indent + GUIGetExtentX( wnd->gui, line.text, where->colidx );
    point->y = where->row * wnd->max_char.y;
}

bool    WndPieceIsTab( a_window wnd, wnd_row row, wnd_piece piece )
{
    wnd_line_piece      line;

    return( WndGetLine( wnd, row, piece, &line ) && line.tabstop );
}


bool    WndPieceIsHot( a_window wnd, wnd_row row, wnd_piece piece )
{
    wnd_line_piece      line;

    return( WndGetLine( wnd, row, piece, &line ) && line.hot );
}


a_window        WndFindActive( void )
{
    a_window    wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( WndSwitchOn( wnd, WSW_ACTIVE ) ) {
            return( wnd );
        }
    }
    return( WndMain );
}


bool    WndValid( a_window check )
{
    a_window    wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( check == wnd ) {
            return( true );
        }
    }
    return( false );
}


void WndDirty( a_window wnd )
{
    if( wnd == NULL ) {
        GUIWndDirty( NULL );
    } else {
        GUIWndDirty( wnd->gui );
    }
}


void WndZapped( a_window wnd )
{
    WndNoCurrent( wnd );
    WndNoSelect( wnd );
    WndSetRepaint( wnd );
}

gui_ord  WndExtentX( a_window wnd, const char *string )
{
    return( GUIGetExtentX( wnd->gui, string, strlen( string ) ) );
}


a_window        WndFindClass( a_window first, wnd_class wndclass )
{
    a_window    wnd;

    for( wnd = WndNext( first ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( wnd->wndclass == wndclass ) {
            return( wnd );
        }
    }
    return( NULL );
}

void WndSetWndMax( void )
{
    gui_rect    rect;

    if( WndIsMinimized( WndMain ) )
        return;
    GUIGetClientRect( WndMain->gui, &rect );
    WndMax.x = rect.width;
    WndMax.y = rect.height;
    WndMainResized();
}


a_window        WndNext( a_window wnd )
{
    gui_window  *gui;

    if( wnd == NULL ) {
        gui = GUIGetFront();
    } else {
        gui = GUIGetNextWindow( wnd->gui );
    }
    if( gui == NULL )
        return( NULL );
    return( GUIGetExtra( gui ) );
}

void WndForAllClass( wnd_class wndclass, void (*rtn)( a_window ) )
{
    a_window    wnd;

    for( wnd = WndNext( NULL ); wnd != NULL; wnd = WndNext( wnd ) ) {
        if( wndclass == WND_NO_CLASS || WndClass( wnd ) == wndclass ) {
            rtn( wnd );
        }
    }
}

