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
* Description:  Window selection processing.
*
****************************************************************************/


#include "auipvt.h"
#include <string.h>
#include <ctype.h>
#include "walloca.h"

#include "clibext.h"


void WndChooseEvent( a_window *wnd, gui_event event, void *parm )
{
    gui_key     key;

    if( _Isnt( wnd, WSW_CHOOSING ) )
        return;
    switch( event ) {
    case GUI_KEYDOWN:
    case GUI_KEYUP:
        GUI_GET_KEY( parm, key );
        if( GUI_IS_ASCII( key ) && WndKeyChar( key ) )
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

extern  void    WndDoneChoose( a_window *wnd )
{
    _Clr( wnd, WSW_CHOOSING );
    wnd->keyindex = 0;
}


extern  bool    WndKeyEscape( a_window *wnd )
{
    if( wnd->keypiece == WND_NO_PIECE )
        return( FALSE );
    WndNoSelect( wnd );
    wnd->keyindex = 0;
    return( TRUE );
}


extern  void    WndStartChoose( a_window *wnd )
{
    _Set( wnd, WSW_CHOOSING );
}

extern  void    WndSayMatchMode( a_window *wnd )
{
    char                *sofar_buff;
    wnd_line_piece      line;
    char                *match;

#define _SOFAR (sofar_buff + strlen( match ) - 1)

    match = WndLoadString( LITERAL_Match_Mode );
    sofar_buff = alloca( MAX_KEY_SIZE + strlen( match ) + 1 );
    if( _Is( wnd, WSW_CHOOSING ) ) {
        strcpy( sofar_buff, match );
        WndGetLine( wnd, wnd->current.row, wnd->keypiece, &line );
        strcpy( _SOFAR, line.text );
        _SOFAR[wnd->keyindex] = '\0';
        WndStatusText( sofar_buff );
    }
    WndFree( match );
}

static  bool    DoWndKeyChoose( a_window *wnd, unsigned key )
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
        scrolled = WndScrollAbs( wnd, -wnd->title_size );
        WndNoCurrent( wnd );
    }
    if( !WndHasCurrent( wnd ) ) {
        WndFirstCurrent( wnd );
    }
    WndGetLine( wnd, wnd->current.row, wnd->keypiece, &line );
    strcpy( sofar, line.text );
    sofar[wnd->keyindex] = key;
    sofar[wnd->keyindex + 1] = '\0';
    for( row = wnd->current.row; ; ++row ) {
        if( !WndGetLine( wnd, row, wnd->keypiece, &line ) )
            break;
        if( !line.use_key )
            continue;
//        len = line.length;
        if( strnicmp( line.text, sofar, wnd->keyindex + 1 ) == 0 ) {
            if( !WndGetLine( wnd, row + 1, wnd->keypiece, &line ) ||
                line.length < wnd->keyindex ||
                tolower( line.text[wnd->keyindex] ) != tolower( key ) ) {
            }
            wnd->keyindex++;
            WndDirtyCurr( wnd );
            if( row < wnd->rows ) {
                wnd->current.row = row;
            } else {
                scrolled = WndScroll( wnd, row - wnd->rows / 2 );
                wnd->current.row = row - scrolled;
            }
            WndDirtyCurr( wnd );
            wnd->current.piece = wnd->keypiece;
            WndNoSelect( wnd );
            wnd->sel_start = wnd->current;
            wnd->sel_end = wnd->current;
            wnd->sel_start.col = 0;
            wnd->sel_end.col = wnd->keyindex - 1;
            return( TRUE );
        }
    }
    WndScroll( wnd, -scrolled );
    wnd->current = saved_curr;
    return( FALSE );
}


extern  bool    WndKeyChoose( a_window *wnd, unsigned key )
{
    bool        rc;

    rc = DoWndKeyChoose( wnd, key );
    return( rc );
}


extern  bool    WndKeyRubOut( a_window *wnd )
{
    int                 newindex;
    wnd_line_piece      line;
    int                 i;
    char                sofar[MAX_KEY_SIZE + 1];

    if( wnd->keypiece == WND_NO_PIECE )
        return( FALSE );
    if( wnd->keyindex == 0 )
        return( FALSE );
    WndGetLine( wnd, wnd->current.row, wnd->keypiece, &line );
    strcpy( sofar, line.text );
    newindex = wnd->keyindex - 1;
    WndKeyEscape( wnd );
    for( i = 0; i < newindex; ++i ) {
        DoWndKeyChoose( wnd, sofar[i] );
    }
    sofar[i] = '\0';
    return( TRUE );
}
