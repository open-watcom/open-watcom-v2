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


#include "auipvt.h"//
#include <string.h>

wnd_info NoInfo = {
    NoEventProc,
    NoRefresh,
    NoGetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    NoNumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    0,
    NULL
};

a_window *WndNoOpen( void )
{
    return( NULL );
}

bool NoGetLine( a_window *wnd, int row, int piece, wnd_line_piece *line )
{
    wnd=wnd;row=row;piece=piece;line=line;
    return( FALSE );
}

void NoModify( a_window *wnd, int row, int piece )
{
    wnd=wnd;row=row;piece=piece;
}

void NoNotify( a_window *wnd, wnd_row row, int piece )
{
    wnd=wnd;row=row;piece=piece;
}

void    NoBegPaint( a_window *wnd, wnd_row row, int num )
{
    wnd=wnd;row=row;num=num;
}

void    NoEndPaint( a_window *wnd, wnd_row row, int num )
{
    wnd=wnd;row=row;num=num;
}

void NoRefresh( a_window * wnd )
{
    WndRepaint( wnd );
}

void NoMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
{
    wnd=wnd;id=id;row=row;piece=piece;
}


int NoScroll( a_window * wnd, int lines )
{
    wnd=wnd;lines=lines;
    return( 0 );
}

int NoNumRows( a_window * wnd )
{
    wnd=wnd;
    return( -1 );
}

int NoNextRow( a_window * wnd, int row, int inc )
{
    wnd=wnd;
    return( row + inc );
}

bool NoEventProc( a_window * wnd, gui_event event, void *parm )
{
    wnd=wnd;parm=parm;
    switch( event ) {
    case GUI_INIT_WINDOW:
    case GUI_RESIZE:
    case GUI_MOVE:
    case GUI_DESTROY:
        return( TRUE );
    default:
        return( FALSE );
    }
}


void WndMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
{
    wnd->info->menuitem( wnd, id, WndVirtualRow( wnd, row ), piece );
}


#define NUM_CACHE_ENTRIES 100 // NYI - just for now - testing mode

typedef struct {
    wnd_line_piece      line;
    a_window            *wnd;
    int                 row;
    int                 piece;
    char                *text;
} cache_line;

cache_line CacheLine[NUM_CACHE_ENTRIES];
int        CacheCurr;

void WndFiniCacheLines()
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        WndFree( CacheLine[i].text );
        CacheLine[i].text = NULL;
    }
}


void WndKillCacheEntries( a_window *wnd, int row, int piece )
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        if( CacheLine[i].wnd != wnd ) continue;
        if( CacheLine[i].row != row && row != WND_NO_ROW ) continue;
        if( CacheLine[i].piece != piece && piece != WND_NO_PIECE ) continue;
        CacheLine[i].wnd = NULL;
    }
}

void WndKillCacheLines( a_window *wnd )
{
    WndKillCacheEntries( wnd, WND_NO_ROW, WND_NO_PIECE );
}

static void DoSet( int i, a_window *wnd,
                   int row, int piece, wnd_line_piece *line )
{
    CacheLine[i].line = *line;
    CacheLine[i].wnd = wnd;
    CacheLine[i].row = row;
    CacheLine[i].piece = piece;
    WndFree( CacheLine[i].text );
    CacheLine[i].text = WndAlloc( strlen( line->text ) + 1 );
    strcpy( CacheLine[i].text, line->text );
}

static void SetCacheLine( a_window *wnd, int row,
                          int piece, wnd_line_piece *line )
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        if( CacheLine[i].wnd == wnd &&
            CacheLine[i].row == row &&
            CacheLine[i].piece == piece ) {
            DoSet( i, wnd, row, piece, line );
            return;
        }
    }
    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        if( CacheLine[i].wnd == NULL ) {
            DoSet( i, wnd, row, piece, line );
            return;
        }
    }
    ++CacheCurr;
    if( CacheCurr == NUM_CACHE_ENTRIES ) CacheCurr = 0;
    DoSet( CacheCurr, wnd, row, piece, line );
}


static bool FindCacheLine( a_window *wnd, int row,
                           int piece, wnd_line_piece *line )
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        if( CacheLine[i].wnd == wnd &&
            CacheLine[i].row == row &&
            CacheLine[i].piece == piece ) {
                *line = CacheLine[i].line;
                line->text = CacheLine[i].text;
                return( TRUE );
        }
    }
    return( FALSE );
}


bool WndSetCache( a_window *wnd, bool on )
{
    bool        old;

    old = _Is( wnd, WSW_CACHE_LINES );
    if( on ) {
        _Set( wnd, WSW_CACHE_LINES );
    } else {
        _Clr( wnd, WSW_CACHE_LINES );
    }
    return( old );
}

bool WndGetLine( a_window *wnd, int row, int piece, wnd_line_piece *line )
{
    int         virtual_row;
    bool        success;

    virtual_row = WndVirtualRow( wnd, row );
    if( _Is( wnd, WSW_CACHE_LINES ) ) {
        if( FindCacheLine( wnd, virtual_row, piece, line ) ) return( TRUE );
    }
    line->attr = WndPlainAttr;
    line->indent = 0;
    line->static_text = FALSE;
    line->tabstop = TRUE;
    line->hot = FALSE;
    line->extent = WND_NO_EXTEND;
    line->master_tabstop = FALSE;
    line->underline = FALSE;
    line->draw_bar = FALSE;
    line->vertical_line = FALSE;
    line->draw_hook = FALSE;
    line->draw_line_hook = FALSE;
    line->bitmap = FALSE;
    line->use_piece0_attr = FALSE;
    line->use_prev_attr = FALSE;
    line->use_key = TRUE;
    line->text = "";
    line->hint = "";
    if( virtual_row < -wnd->title_size ) return( FALSE );
    if( row == wnd->u.button_down.row && piece == wnd->u.button_down.piece ) {
        _Set( wnd, WSW_ALTERNATE_BIT );
    }
    success = wnd->info->getline( wnd, virtual_row, piece, line );
    _Clr( wnd, WSW_ALTERNATE_BIT );
    if( success ) {
        if( !(line->bitmap|line->vertical_line|line->draw_hook|line->draw_line_hook|line->draw_bar) ) {
            line->length = strlen( line->text );
        }
        if( virtual_row > wnd->max_row ) wnd->max_row = virtual_row;
    }
    if( success ) {
        if( _Is( wnd, WSW_CACHE_LINES ) ) {
            SetCacheLine( wnd, virtual_row, piece, line );
        }
    }
    return( success );
}


bool WndGetLineAbs( a_window *wnd, int row, int piece, wnd_line_piece *line )
{
    return( WndGetLine( wnd, WndScreenRow( wnd, row ), piece, line ) );
}


void WndFirstMenuItem( a_window *wnd, int row, int piece )
{
    WndRowPopUp( wnd, &wnd->popupmenu[ 0 ], row, piece );
}

#if 0
wnd_row WndNoRow( a_window *wnd )
{
    return( WndVirtualRow( wnd, WND_NO_ROW ) );
}
#endif

void    WndModify( a_window *wnd, int row, int piece )
{
    wnd->info->modify( wnd, WndVirtualRow( wnd, row ), piece );
}

void    WndNotify( a_window *wnd, int row, int piece )
{
    wnd->info->notify( wnd, WndVirtualRow( wnd, row ), piece );
}

void    WndBegPaint( a_window *wnd, int row, int num )
{
    wnd->info->begpaint( wnd, WndVirtualRow( wnd, row ), num );
}


void    WndEndPaint( a_window *wnd, int row, int num )
{
    wnd->info->endpaint( wnd, WndVirtualRow( wnd, row ), num );
}


void WndRefresh( a_window * wnd )
{
    wnd->info->refresh( wnd );
    WndKillCacheLines( wnd );
}


bool WndEvent( a_window * wnd, gui_event event, void *parm )
{
    return( wnd->info->event( wnd, event, parm ) );
}


int WndNumRows( a_window * wnd )
{
    return( wnd->info->numrows( wnd ) + wnd->title_size );
}


int WndNextRow( a_window * wnd, int row, int inc )
{
    return( wnd->info->nextrow( wnd, row, inc ) );
}
