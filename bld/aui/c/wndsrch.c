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
#include "rxwrap.h"
#include "wndregx.h"

bool    WndDoingSearch = FALSE;

void WndSetSrchItem( a_window *wnd, char *expr )
{
    GUIMemFree( wnd->searchitem );
    wnd->searchitem = GUIMemAlloc( strlen( expr ) + 1 );
    strcpy( wnd->searchitem, expr );
}

void *WndCompileRX( char *expr )
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


void WndSetMagicStr( char *str )
{
    char        *meta;
    char        *ignore;

    ignore = SrchIgnoreMagic;
    for( meta = SrchMetaChars; *meta; ++meta ) {
        if( strchr( str, *meta ) == NULL ) {
            *ignore++ = *meta;
        }
    }
    *ignore = '\0';
}

char *WndGetSrchMagicChars( void )
{
    return( SrchMagicChars );
}

void WndSetSrchMagicChars( char *str )
{
    strncpy( SrchMagicChars, str, MAX_MAGIC_STR );
    SrchMagicChars[MAX_MAGIC_STR] = '\0';
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

bool WndRXFind( void * _rx, char **pos, char **endpos )
{
    regexp * rx = (regexp *)_rx;

    if( (*pos)[0] == '\0' ) return( FALSE );
    if( !RegExec( rx, *pos, *endpos == NULL ) ) return( FALSE );
    *pos = rx->startp[0];
    *endpos = rx->endp[0];
    return( TRUE );
}

static void NotFound( a_window *wnd, regexp *rx, char *msg )
{
    Ring();
    WndNextRow( wnd, WND_NO_ROW, WND_RESTORE_ROW );
    WndStatusText( msg );
    WndFreeRX( rx );
    WndDoingSearch = FALSE;
    WndRepaint( wnd );
}

extern  bool    WndSearch( a_window *wnd, bool from_top, int direction )
{
    wnd_line_piece      line;
    regexp              *rx;
    char                *pos;
    char                *endpos;
    bool                wrap;
    int                 rows;
    bool                rc;
    bool                had_cache;
    char                *not_found;
    char                *top_of_window;
    char                *end_of_window;
    char                *search_wrapped;

    wnd_subpiece        next_occurence;
    wnd_subpiece        prev_occurence;
    wnd_subpiece        curr;
    wnd_coord           starting_pos;

    if( direction == 0 ) return( FALSE );
    if( wnd == NULL ) return( FALSE );
    if( wnd->searchitem == NULL ) return( FALSE );
    rx = WndCompileRX( wnd->searchitem );
    if( rx == NULL ) return( FALSE );
    not_found = WndLoadString( LITERAL_Not_Found );
    top_of_window = WndLoadString( LITERAL_Top_Of_Window );
    end_of_window = WndLoadString( LITERAL_End_Of_Window );
    search_wrapped = WndLoadString( LITERAL_Search_Wrapped );
    wrap = FALSE;
    starting_pos.piece = 0;
    starting_pos.col = direction > 0 ? -1 : WND_MAX_COL;
    if( from_top ) {
        curr.row = 0;
    } else if( WndHasCurrent( wnd ) ) {
        curr.row = WndVirtualRow( wnd, wnd->current.row );
        starting_pos.piece = wnd->current.piece;
        starting_pos.col = wnd->current.col;
    } else {
        curr.row = WndVirtualTop( wnd );
    }
    starting_pos.row = curr.row;
    WndNextRow( wnd, WND_NO_ROW, WND_SAVE_ROW );
    WndStatusText( "" );
    WndDoingSearch = TRUE;
    had_cache = WndSetCache( wnd, FALSE );
    for( ;; ) {
        if( curr.row < 0 ) {
            if( wrap ) {
                NotFound( wnd, rx, not_found );
                rc = FALSE;
                goto done;
            } else if( _Is( wnd, WSW_SEARCH_WRAP ) ) {
                rows = WndNumRows( wnd );
                if( rows == -1 ) {
                    WndRepaint( wnd );
                    WndScrollAbs( wnd, -wnd->title_size );
                    rows = WndScrollAbs( wnd, WND_MAX_ROW ) + WndRows( wnd );
                }
                curr.row = rows - 1;
                curr.col = 0;
                curr.piece = -1;
                wrap = TRUE;
                continue;
            } else {
                NotFound( wnd, rx, top_of_window );
                rc = FALSE;
                goto done;
            }
        }
        next_occurence.col = -1;
        prev_occurence.col = -1;
        for( curr.piece = 0;; ++curr.piece ) { // look for both next and prev match
            if( !WndGetLineAbs( wnd, curr.row, curr.piece, &line ) ) {
                if( curr.piece != 0 ) break;
                if( wrap ) {
                    NotFound( wnd, rx, not_found );
                    rc = FALSE;
                    goto done;
                } else if( _Is( wnd, WSW_SEARCH_WRAP ) ) {
                    curr.row = 0;
                    curr.col = 0;
                    curr.piece = -1;
                    wrap = TRUE;
                    continue;
                } else {
                    NotFound( wnd, rx, end_of_window );
                    rc = FALSE;
                    goto done;
                }
            }
            if( line.bitmap ) continue;
            pos = line.text;
            endpos = NULL;
            while( WndRXFind( rx, &pos, &endpos ) ) {
                curr.end = endpos - line.text;
                curr.col = pos - line.text;
                if( curr.piece < starting_pos.piece ) {
                    prev_occurence = curr;
                } else if( curr.piece > starting_pos.piece ) {
                    if( next_occurence.col == -1 ) {
                        next_occurence = curr;
                    }
                } else if( curr.col > starting_pos.col ) {
                    if( next_occurence.col == -1 ) {
                        next_occurence = curr;
                    }
                } else if( curr.col < starting_pos.col ) {
                    prev_occurence = curr;
                }
                ++pos;
            }
        }
        if( direction < 0 ) {
            next_occurence = prev_occurence;
        }
        if( next_occurence.col != -1 ) {
            WndDoingSearch = FALSE;
            WndKillCacheLines( wnd );
            WndDirtyCurr( wnd );
            WndNoSelect( wnd );
            WndNoCurrent( wnd );
            if( curr.row < WndVirtualTop( wnd ) ) {
                if( curr.row > wnd->rows / 2 ) {
                    WndRepaint( wnd );
                    WndScrollAbs( wnd, curr.row - wnd->rows / 2 );
                } else {
                    WndRepaint( wnd );
                    WndScrollAbs( wnd, -wnd->title_size );
                }
            } else if( curr.row >= WndVirtualBottom( wnd ) ) {
                WndRepaint( wnd );
                WndScrollAbs( wnd, curr.row - wnd->rows / 2 );
            }
            wnd->sel_start.row = WndScreenRow( wnd, curr.row );
            wnd->sel_start.piece = next_occurence.piece;
            wnd->sel_start.col = next_occurence.col;

            wnd->sel_end = wnd->sel_start;
            wnd->sel_end.col = next_occurence.end - 1;

            wnd->current.col = wnd->sel_end.col;
            wnd->current = wnd->sel_start;
            WndSetCurrCol( wnd );
            WndCurrVisible( wnd );
            WndDirtyCurr( wnd );
            WndFreeRX( rx );
            if( wrap ) WndStatusText( search_wrapped );
            rc = TRUE;
            goto done;
        }
        if( direction > 0 ) {
            if( wrap && curr.row > starting_pos.row ) {
                NotFound( wnd, rx, not_found );
                rc = FALSE;
                goto done;
            }
            starting_pos.col = -1;
            starting_pos.piece = 0;
            curr.row = WndNextRow( wnd, curr.row, 1 );
        } else {
            starting_pos.col = WND_MAX_COL;
            starting_pos.piece = WND_MAX_COL;
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
