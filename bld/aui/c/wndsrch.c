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
#include "rxwrap.h"
#include "wndregx.h"

bool    WndDoingSearch = false;

void WndSetSrchItem( a_window wnd, const char *expr )
{
    GUIMemFree( wnd->searchitem );
    wnd->searchitem = GUIMemAlloc( strlen( expr ) + 1 );
    strcpy( wnd->searchitem, expr );
}

void *WndCompileRX( const char *expr )
{
    regexp      *rx;

    rx = RegComp( expr );
    if( RegExpError ) {
        WndFreeRX( rx );
        WndRXError( RegExpError );
        return( NULL );
    }
    return( rx );
}


void WndFreeRX( void *rx )
{
    WndFree( (regexp *)rx );
}


void WndSetMagicStr( const char *str )
{
    char        *meta;
    int     i;

    i = 0;
    for( meta = SrchMetaChars; i < MAX_MAGIC_STR && *meta != '\0'; ++meta ) {
        if( strchr( str, *meta ) == NULL ) {
            SrchIgnoreMagic[i++] = *meta;
        }
    }
    SrchIgnoreMagic[i] = '\0';
}

char *WndGetSrchMagicChars( void )
{
    return( SrchMagicChars );
}

void WndSetSrchMagicChars( const char *str, unsigned len )
{
    if( len > MAX_MAGIC_STR )
        len = MAX_MAGIC_STR;
    memcpy( SrchMagicChars, str, len );
    SrchMagicChars[len] = '\0';
}

bool WndGetSrchIgnoreCase( void )
{
    return( SrchIgnoreCase );
}

void WndSetSrchIgnoreCase( bool flag )
{
    SrchIgnoreCase = flag;
}

bool WndGetSrchRX( void )
{
    return( SrchRX );
}

void WndSetSrchRX( bool flag )
{
    SrchRX = flag;
}

bool WndRXFind( void *_rx, const char **pos, const char **endpos )
{
    regexp * rx = (regexp *)_rx;

    if( (*pos)[0] == '\0' )
        return( false );
    if( !RegExec( rx, *pos, *endpos == NULL ) )
        return( false );
    *pos = rx->startp[0];
    *endpos = rx->endp[0];
    return( true );
}

static void NotFound( a_window wnd, regexp *rx, const char *msg )
{
    Ring();
    WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
    WndStatusText( msg );
    WndFreeRX( rx );
    WndDoingSearch = false;
    WndSetRepaint( wnd );
}

bool    WndSearch( a_window wnd, bool from_top, int direction )
{
    wnd_line_piece      line;
    regexp              *rx;
    const char          *pos;
    const char          *endpos;
    bool                wrap;
    wnd_row             rows;
    bool                rc;
    bool                had_cache;
    char                *not_found;
    char                *top_of_window;
    char                *end_of_window;
    char                *search_wrapped;

    wnd_rect            next_occurence;
    wnd_rect            prev_occurence;
    wnd_rect            curr;
    wnd_coord           starting_pos;

    if( direction == 0 )
        return( false );
    if( wnd == NULL )
        return( false );
    if( wnd->searchitem == NULL )
        return( false );
    rx = WndCompileRX( wnd->searchitem );
    if( rx == NULL )
        return( false );
    not_found = WndLoadString( LITERAL_Not_Found );
    top_of_window = WndLoadString( LITERAL_Top_Of_Window );
    end_of_window = WndLoadString( LITERAL_End_Of_Window );
    search_wrapped = WndLoadString( LITERAL_Search_Wrapped );
    wrap = false;
    starting_pos.piece = 0;
    starting_pos.colidx = ( direction > 0 ) ? -1 : WND_MAX_COLIDX;
    if( from_top ) {
        curr.row = 0;
    } else if( WndHasCurrent( wnd ) ) {
        curr.row = WndVirtualRow( wnd, wnd->current.row );
        starting_pos.piece = wnd->current.piece;
        starting_pos.colidx = wnd->current.colidx;
    } else {
        curr.row = WndVirtualTop( wnd );
    }
    starting_pos.row = curr.row;
    WndNextRow( wnd, WND_NO_ROW, WND_SAVE_ROW );
    WndStatusText( "" );
    WndDoingSearch = true;
    had_cache = WndSetCache( wnd, false );
    for( ;; ) {
        if( curr.row < 0 ) {
            if( wrap ) {
                NotFound( wnd, rx, not_found );
                rc = false;
                goto done;
            } else if( WndSwitchOn( wnd, WSW_SEARCH_WRAP ) ) {
                rows = WndNumRows( wnd );
                if( rows == -1 ) {
                    WndSetRepaint( wnd );
                    WndVScrollAbs( wnd, -wnd->title_rows );
                    rows = WndVScrollAbs( wnd, WND_MAX_ROW ) + WndRows( wnd );
                }
                curr.row = rows - 1;
                curr.colidx = 0;
                curr.piece = -1;
                wrap = true;
                continue;
            } else {
                NotFound( wnd, rx, top_of_window );
                rc = false;
                goto done;
            }
        }
        next_occurence.colidx = -1;
        prev_occurence.colidx = -1;
        for( curr.piece = 0;; ++curr.piece ) { // look for both next and prev match
            if( !WndGetLineAbs( wnd, curr.row, curr.piece, &line ) ) {
                if( curr.piece != 0 )
                    break;
                if( wrap ) {
                    NotFound( wnd, rx, not_found );
                    rc = false;
                    goto done;
                } else if( WndSwitchOn( wnd, WSW_SEARCH_WRAP ) ) {
                    curr.row = 0;
                    curr.colidx = 0;
                    curr.piece = -1;
                    wrap = true;
                    continue;
                } else {
                    NotFound( wnd, rx, end_of_window );
                    rc = false;
                    goto done;
                }
            }
            if( line.bitmap )
                continue;
            endpos = NULL;
            for( pos = line.text; WndRXFind( rx, &pos, &endpos ); pos++ ) {
                curr.end_colidx = (wnd_colidx)( endpos - line.text );
                curr.colidx = (wnd_colidx)( pos - line.text );
                if( curr.piece < starting_pos.piece ) {
                    prev_occurence = curr;
                } else if( curr.piece > starting_pos.piece ) {
                    if( next_occurence.colidx == -1 ) {
                        next_occurence = curr;
                    }
                } else if( curr.colidx > starting_pos.colidx ) {
                    if( next_occurence.colidx == -1 ) {
                        next_occurence = curr;
                    }
                } else if( curr.colidx < starting_pos.colidx ) {
                    prev_occurence = curr;
                }
            }
        }
        if( direction < 0 ) {
            next_occurence = prev_occurence;
        }
        if( next_occurence.colidx != -1 ) {
            WndDoingSearch = false;
            WndKillCacheLines( wnd );
            WndDirtyCurr( wnd );
            WndNoSelect( wnd );
            WndNoCurrent( wnd );
            if( curr.row < WndVirtualTop( wnd ) ) {
                WndSetRepaint( wnd );
                if( curr.row > wnd->rows / 2 ) {
                    WndVScrollAbs( wnd, curr.row - wnd->rows / 2 );
                } else {
                    WndVScrollAbs( wnd, -wnd->title_rows );
                }
            } else if( curr.row >= WndVirtualBottom( wnd ) ) {
                WndSetRepaint( wnd );
                WndVScrollAbs( wnd, curr.row - wnd->rows / 2 );
            }
            wnd->sel_start.row = WndScreenRow( wnd, curr.row );
            wnd->sel_start.piece = next_occurence.piece;
            wnd->sel_start.colidx = next_occurence.colidx;

            wnd->sel_end = wnd->sel_start;
            wnd->sel_end.colidx = next_occurence.end_colidx - 1;

            wnd->current.colidx = wnd->sel_end.colidx;
            wnd->current = wnd->sel_start;
            WndSetCurrCol( wnd );
            WndCurrVisible( wnd );
            WndDirtyCurr( wnd );
            WndFreeRX( rx );
            if( wrap )
                WndStatusText( search_wrapped );
            rc = true;
            goto done;
        }
        if( direction > 0 ) {
            if( wrap && curr.row > starting_pos.row ) {
                NotFound( wnd, rx, not_found );
                rc = false;
                goto done;
            }
            starting_pos.colidx = -1;
            starting_pos.piece = 0;
            curr.row = WndNextRow( wnd, curr.row, 1 );
        } else {
            starting_pos.colidx = WND_MAX_COLIDX;
            starting_pos.piece = WND_MAX_PIECE;
            curr.row = WndNextRow( wnd, curr.row, -1 );
        }
        curr.piece = 0;
    }
done:;
    WndSetCache( wnd, had_cache );
    WndFree( not_found );
    WndFree( top_of_window );
    WndFree( end_of_window );
    WndFree( search_wrapped );
    return( rc );
}
