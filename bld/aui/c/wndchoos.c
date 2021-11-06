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
* Description:  Window selection processing.
*
****************************************************************************/


#include "_aui.h"
#include <string.h>
#include <ctype.h>
#include "walloca.h"

#include "clibext.h"


void WndChooseEvent( a_window wnd, gui_event gui_ev, void *parm )
{
    gui_key     key;

    if( WndSwitchOff( wnd, WSW_CHOOSING ) )
        return;
    switch( gui_ev ) {
    case GUI_KEYDOWN:
    case GUI_KEYUP:
        GUI_GET_KEY( parm, key );
        if( WndKeyIsPrintChar( key ) )
            return;
        if( key == GUI_KEY_BACKSPACE )
            return;
        break;
    case GUI_RESIZE:
    case GUI_MOVE:
    case GUI_MOUSEMOVE:
    case GUI_PAINT:
        return;
    default :
        break;
    }
    WndDoneChoose( wnd );
}

void    WndDoneChoose( a_window wnd )
{
    WndClrSwitches( wnd, WSW_CHOOSING );
    wnd->keyindex = 0;
}


bool    WndKeyEscape( a_window wnd )
{
    if( WndKeyPiece( wnd ) == WND_NO_PIECE )
        return( false );
    WndNoSelect( wnd );
    wnd->keyindex = 0;
    return( true );
}


void    WndStartChoose( a_window wnd )
{
    WndSetSwitches( wnd, WSW_CHOOSING );
}

void    WndSayMatchMode( a_window wnd )
{
    char                *sofar_buff;
    wnd_line_piece      line;
    char                *match;
    size_t              match_len;

    match = WndLoadString( LITERAL_Match_Mode );
    match_len = strlen( match );
    if( WndSwitchOn( wnd, WSW_CHOOSING ) ) {
        sofar_buff = alloca( match_len + MAX_KEY_SIZE + 1 );
        memcpy( sofar_buff, match, match_len );
        WndGetLine( wnd, wnd->current.row, WndKeyPiece( wnd ), &line );
        strcpy( sofar_buff + match_len, line.text );
        sofar_buff[match_len + wnd->keyindex] = '\0';
        WndStatusText( sofar_buff );
    }
    WndFree( match );
}

static  bool    DoWndKeyChoose( a_window wnd, int ch )
{
    wnd_line_piece      line;
    wnd_row             row;
//    int                 len;
    int                 scrolled;
    wnd_coord           saved_curr;
    char                sofar[MAX_KEY_SIZE + 1];

    saved_curr = wnd->current;
    scrolled = 0;
    if( wnd->keyindex == 0 ) {
        WndDirtyCurr( wnd );
        scrolled = WndVScrollAbs( wnd, -wnd->title_rows );
        WndNoCurrent( wnd );
    }
    if( !WndHasCurrent( wnd ) ) {
        WndFirstCurrent( wnd );
    }
    WndGetLine( wnd, wnd->current.row, WndKeyPiece( wnd ), &line );
    strcpy( sofar, line.text );
    sofar[wnd->keyindex] = ch;
    sofar[wnd->keyindex + 1] = '\0';
    for( row = wnd->current.row; ; ++row ) {
        if( !WndGetLine( wnd, row, WndKeyPiece( wnd ), &line ) )
            break;
        if( !line.use_key )
            continue;
//        len = line.length;
        if( strnicmp( line.text, sofar, wnd->keyindex + 1 ) == 0 ) {
            if( !WndGetLine( wnd, row + 1, WndKeyPiece( wnd ), &line ) || line.length < wnd->keyindex
              || tolower( UCHAR_VALUE( line.text[wnd->keyindex] ) ) != tolower( ch ) ) {
                /* ??? */
            }
            wnd->keyindex++;
            WndDirtyCurr( wnd );
            if( row < wnd->rows ) {
                wnd->current.row = row;
            } else {
                scrolled = WndVScroll( wnd, row - wnd->rows / 2 );
                wnd->current.row = row - scrolled;
            }
            WndDirtyCurr( wnd );
            wnd->current.piece = WndKeyPiece( wnd );
            WndNoSelect( wnd );
            wnd->sel_start = wnd->current;
            wnd->sel_end = wnd->current;
            wnd->sel_start.colidx = 0;
            wnd->sel_end.colidx = wnd->keyindex - 1;
            return( true );
        }
    }
    WndVScroll( wnd, -scrolled );
    wnd->current = saved_curr;
    return( false );
}


bool    WndKeyChoose( a_window wnd, int ch )
{
    bool        rc;

    rc = DoWndKeyChoose( wnd, ch );
    return( rc );
}


bool    WndKeyRubOut( a_window wnd )
{
    int                 newindex;
    wnd_line_piece      line;
    int                 i;
    char                sofar[MAX_KEY_SIZE + 1];

    if( WndKeyPiece( wnd ) == WND_NO_PIECE )
        return( false );
    if( wnd->keyindex == 0 )
        return( false );
    WndGetLine( wnd, wnd->current.row, WndKeyPiece( wnd ), &line );
    strcpy( sofar, line.text );
    newindex = wnd->keyindex - 1;
    WndKeyEscape( wnd );
    for( i = 0; i < newindex; ++i ) {
        DoWndKeyChoose( wnd, UCHAR_VALUE( sofar[i] ) );
    }
    sofar[i] = '\0';
    return( true );
}
