/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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

wnd_info NoInfo = {
    NoWndEventProc,
    NoRefresh,
    NoGetLine,
    NoMenuItem,
    NoVScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    NoNumRows,
    NoNextRow,
    NoNotify,
    NoChkUpdate,
    NoPopUp
};

a_window WndNoOpen( void )
{
    return( NULL );
}

bool NoGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    /* unused parameters */ (void)wnd; (void)row; (void)piece; (void)line;

    return( false );
}

void NoModify( a_window wnd, wnd_row row, wnd_piece piece )
{
    /* unused parameters */ (void)wnd; (void)row; (void)piece;
}

void NoNotify( a_window wnd, wnd_row row, wnd_piece piece )
{
    /* unused parameters */ (void)wnd; (void)row; (void)piece;
}

void    NoBegPaint( a_window wnd, wnd_row row, int num )
{
    /* unused parameters */ (void)wnd; (void)row; (void)num;
}

void    NoEndPaint( a_window wnd, wnd_row row, int num )
{
    /* unused parameters */ (void)wnd; (void)row; (void)num;
}

void NoRefresh( a_window wnd )
{
    WndSetRepaint( wnd );
}

void NoMenuItem( a_window wnd, gui_ctl_id id, wnd_row row, wnd_piece piece )
{
    /* unused parameters */ (void)wnd; (void)id; (void)row; (void)piece;
}


int NoVScroll( a_window wnd, int lines )
{
    /* unused parameters */ (void)wnd; (void)lines;

    return( 0 );
}

wnd_row NoNumRows( a_window wnd )
{
    /* unused parameters */ (void)wnd;

    return( -1 );
}

wnd_row NoNextRow( a_window wnd, wnd_row row, int inc )
{
    /* unused parameters */ (void)wnd;

    return( row + inc );
}

bool NoWndEventProc( a_window wnd, gui_event gui_ev, void *parm )
{
    /* unused parameters */ (void)wnd; (void)parm;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
    case GUI_RESIZE:
    case GUI_MOVE:
    case GUI_DESTROY:
        return( true );
    default:
        return( false );
    }
}


void WndMenuItem( a_window wnd, gui_ctl_id id, wnd_row row, wnd_piece piece )
{
    wnd->info->menuitem( wnd, id, WndVirtualRow( wnd, row ), piece );
}


#define NUM_CACHE_ENTRIES 100 // NYI - just for now - testing mode

typedef struct {
    wnd_line_piece      line;
    a_window            wnd;
    wnd_row             row;
    wnd_piece           piece;
    char                *text;
} cache_line;

cache_line CacheLine[NUM_CACHE_ENTRIES];
int        CacheCurr;

void WndFiniCacheLines( void )
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        WndFree( CacheLine[i].text );
        CacheLine[i].text = NULL;
    }
}


void WndKillCacheEntries( a_window wnd, wnd_row row, wnd_piece piece )
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        if( CacheLine[i].wnd != wnd )
            continue;
        if( CacheLine[i].row != row && row != WND_NO_ROW )
            continue;
        if( CacheLine[i].piece != piece && piece != WND_NO_PIECE )
            continue;
        CacheLine[i].wnd = NULL;
    }
}

void WndKillCacheLines( a_window wnd )
{
    WndKillCacheEntries( wnd, WND_NO_ROW, WND_NO_PIECE );
}

static void DoSet( int i, a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    CacheLine[i].line = *line;
    CacheLine[i].wnd = wnd;
    CacheLine[i].row = row;
    CacheLine[i].piece = piece;
    WndFree( CacheLine[i].text );
    CacheLine[i].text = WndAlloc( strlen( line->text ) + 1 );
    strcpy( CacheLine[i].text, line->text );
}

static void SetCacheLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
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
    if( CacheCurr == NUM_CACHE_ENTRIES )
        CacheCurr = 0;
    DoSet( CacheCurr, wnd, row, piece, line );
}


static bool FindCacheLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    int         i;

    for( i = 0; i < NUM_CACHE_ENTRIES; ++i ) {
        if( CacheLine[i].wnd == wnd &&
            CacheLine[i].row == row &&
            CacheLine[i].piece == piece ) {
                *line = CacheLine[i].line;
                line->text = CacheLine[i].text;
                return( true );
        }
    }
    return( false );
}


bool WndSetCache( a_window wnd, bool on )
{
    bool        old;

    old = WndSwitchOn( wnd, WSW_CACHE_LINES );
    if( on ) {
        WndSetSwitches( wnd, WSW_CACHE_LINES );
    } else {
        WndClrSwitches( wnd, WSW_CACHE_LINES );
    }
    return( old );
}

bool WndGetLine( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    wnd_row     virtual_row;
    bool        success;

    virtual_row = WndVirtualRow( wnd, row );
    if( WndSwitchOn( wnd, WSW_CACHE_LINES ) ) {
        if( FindCacheLine( wnd, virtual_row, piece, line ) ) {
            return( true );
        }
    }
    line->attr = WndPlainAttr;
    line->indent = 0;
    line->static_text = false;
    line->tabstop = true;
    line->hot = false;
    line->extent = WND_NO_EXTEND;
    line->master_tabstop = false;
    line->underline = false;
    line->draw_bar = false;
    line->vertical_line = false;
    line->draw_hook = false;
    line->draw_line_hook = false;
    line->bitmap = false;
    line->use_piece0_attr = false;
    line->use_prev_attr = false;
    line->use_key = true;
    line->text = "";
    line->hint = "";
    if( virtual_row < -wnd->title_rows )
        return( false );
    if( row == wnd->button_down.row && piece == wnd->button_down.piece ) {
        WndSetSwitches( wnd, WSW_ALTERNATE_BIT );
    }
    success = wnd->info->getline( wnd, virtual_row, piece, line );
    WndClrSwitches( wnd, WSW_ALTERNATE_BIT );
    if( success ) {
        if( !(line->bitmap|line->vertical_line|line->draw_hook|line->draw_line_hook|line->draw_bar) ) {
            line->length = strlen( line->text );
        }
        if( wnd->max_row < virtual_row ) {
            wnd->max_row = virtual_row;
        }
    }
    if( success ) {
        if( WndSwitchOn( wnd, WSW_CACHE_LINES ) ) {
            SetCacheLine( wnd, virtual_row, piece, line );
        }
    }
    return( success );
}


bool WndGetLineAbs( a_window wnd, wnd_row row, wnd_piece piece, wnd_line_piece *line )
{
    return( WndGetLine( wnd, WndScreenRow( wnd, row ), piece, line ) );
}


void WndFirstMenuItem( a_window wnd, wnd_row row, wnd_piece piece )
{
    WndRowPopUp( wnd, &WndPopupMenu( wnd )[0], row, piece );
}

#if 0
wnd_row WndNoRow( a_window wnd )
{
    return( WndVirtualRow( wnd, WND_NO_ROW ) );
}
#endif

void    WndModify( a_window wnd, wnd_row row, wnd_piece piece )
{
    wnd->info->modify( wnd, WndVirtualRow( wnd, row ), piece );
}

void    WndNotify( a_window wnd, wnd_row row, wnd_piece piece )
{
    wnd->info->notify( wnd, WndVirtualRow( wnd, row ), piece );
}

void    WndBegPaint( a_window wnd, wnd_row row, int num )
{
    wnd->info->begpaint( wnd, WndVirtualRow( wnd, row ), num );
}


void    WndEndPaint( a_window wnd, wnd_row row, int num )
{
    wnd->info->endpaint( wnd, WndVirtualRow( wnd, row ), num );
}


void WndRefresh( a_window wnd )
{
    wnd->info->refresh( wnd );
    WndKillCacheLines( wnd );
}


wnd_row WndNumRows( a_window wnd )
{
    return( wnd->info->numrows( wnd ) + wnd->title_rows );
}


wnd_row WndNextRow( a_window wnd, wnd_row row, int inc )
{
    return( wnd->info->nextrow( wnd, row, inc ) );
}
