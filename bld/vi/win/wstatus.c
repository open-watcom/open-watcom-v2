/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Status line window.
*
****************************************************************************/


#include "vi.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "statwnd.h"
#include "wstatus.h"
#include <assert.h>
#include "winifini.h"

static int      capIndex = -1;
static short    *sections;
static statwnd  *sw = NULL;

window StatusBar = {
    &statusw_info,
    { 0, 0, 0, 0 }
};

void StatusWndSetSeparatorsWithArray( short *source, int num )
{
    status_block_desc   *list;
    int                 i;

    assert( num > 0 );
    list = MemAlloc( num * sizeof( status_block_desc ) );
    for( i = 0; i < num; i++ ) {
        list[i].separator_width = SEPERATOR_WIDTH;
        list[i].width = source[i];
        list[i].width_is_pixels = true;
    }
    StatusWndSetSeparators( sw, num, list );
    MemFree( list );
}

bool StatusHookProc( HWND, UINT, WPARAM, LPARAM );

bool StatusBarInit( void )
{
    bool    rc;

    rc = StatusWndInit( InstanceHandle, StatusHookProc, sizeof( LPVOID ), (HCURSOR)NULLHANDLE );
    sw = StatusWndStart();
#if defined( __NT__ )
    StatusWndChangeSysColors( GetSysColor( COLOR_BTNFACE ), GetSysColor( COLOR_BTNTEXT ),
#else
    StatusWndChangeSysColors( GetRGB( statusw_info.text_style.background ), GetRGB( statusw_info.text_style.foreground ),
#endif
                              GetSysColor( COLOR_BTNHIGHLIGHT ), GetSysColor( COLOR_BTNSHADOW ) );
    if( EditVars.NumStatusSections > 0 ) {
        StatusWndSetSeparatorsWithArray( EditVars.StatusSections, EditVars.NumStatusSections );
    }
    return( rc );
}

bool StatusBarFini( void )
{
    StatusWndDestroy( sw );
    StatusWndFini();
    return( true );
}

static int setCursor( short x )
{
    int     i;
    for( i = 0; i < EditVars.NumStatusSections; i++ ) {
        if( abs( x - (EditVars.StatusSections[i]) ) < MOUSE_ALLOWANCE ) {
            CursorOp( COP_STATMOVE );
            return( i );
        }
    }
    CursorOp( COP_ARROW );
    return( -1 );
}

static void processMouseMove( WPARAM w, LPARAM l )
{
    int         deep, delta, maxmove, movedby, i, next;
    short       x;
    int         secIndex;

    x = (short)LOWORD( l ) - CURSOR_CORRECT;
    w = w;

    if( capIndex == -1 ) {
        setCursor( x );
        return;
    }

    // algorithm simpler if we index into array w/ 'stops' at either end
    secIndex = capIndex + 1;

    // shove bars to left
    deep = 0;
    delta = sections[secIndex] - x;
    while( delta > 0 && secIndex - deep > 0 ) {
        next = secIndex - deep;
        maxmove = sections[next] - sections[next - 1] - BOUNDARY_WIDTH;
        movedby = 0;
        if( maxmove > 0 ) {
            movedby = maxmove;
            if( movedby > delta )
                movedby = delta;
            delta -= movedby;
        }
        for( i = next; i <= secIndex; i++ ) {
            sections[i] -= movedby;
        }
        deep++;
    }

    // shove bars to right
    deep = 0;
    delta = x - sections[secIndex];
    while( delta > 0 && secIndex + deep <= EditVars.NumStatusSections ) {
        next = secIndex + deep;
        maxmove = sections[next + 1] - sections[next] - BOUNDARY_WIDTH;
        movedby = 0;
        if( maxmove > 0 ) {
            movedby = maxmove;
            if( movedby > delta )
                movedby = delta;
            delta -= movedby;
        }
        for( i = secIndex; i <= next; i++ ) {
            sections[i] += movedby;
        }
        deep++;
    }

    StatusWndSetSeparatorsWithArray( sections + 1, EditVars.NumStatusSections );
    InvalidateRect( status_window_id, NULL, TRUE );
    UpdateWindow( status_window_id );
}

static void processLButtonDown( HWND hwnd, WPARAM w, LPARAM l )
{
    RECT        rect;

    w = w;
    capIndex = setCursor( (short)LOWORD( l ) - CURSOR_CORRECT );
    if( capIndex != -1 ) {
        SetCapture( hwnd );
        sections = MemAlloc( (EditVars.NumStatusSections + 2) * sizeof( short ) );
        GetClientRect( status_window_id, &rect );
        memcpy( sections + 1, EditVars.StatusSections, EditVars.NumStatusSections * sizeof( short ) );
        sections[0] = 0;
        sections[EditVars.NumStatusSections + 1] = rect.right - BOUNDARY_WIDTH + CURSOR_CORRECT;
    }
}

static void processLButtonUp( void )
{
    if( capIndex != -1 ) {
        CursorOp( COP_ARROW );
        ReleaseCapture();
        capIndex = -1;
        memcpy( EditVars.StatusSections, sections + 1, EditVars.NumStatusSections * sizeof( short ) );
        MemFree( sections );
    }
}

/*
 * StatusHookProc - handle messages for the status window
 */
bool StatusHookProc( HWND hwnd, UINT msg, WPARAM w, LPARAM l )
{
    w = w;
    l = l;
    switch( msg ) {
    case WM_CREATE:
        SET_WNDINFO( hwnd, (LONG_PTR)&StatusBar );
        break;
    case WM_SETFOCUS:
        SetFocus( root_window_id );
        return( true );
    case WM_MOUSEMOVE:
        processMouseMove( w, l );
        return( true );
    case WM_LBUTTONDOWN:
        processLButtonDown( hwnd, w, l );
        return( true );
    case WM_LBUTTONUP:
        processLButtonUp();
        return( true );
    }
    return( false );

} /* StatusHookProc */

/*
 * NewStatWindow - create a new status window
 */
window_id NewStatWindow( void )
{
    window_id   wid;
    RECT        size;

    size = StatusBar.def_area;
    size.left -= 1;
    size.right += 1;
    size.bottom += 1;
    wid = StatusWndCreate( sw, root_window_id, &size, InstanceHandle, NULL );
    return( wid );

} /* NewStatWindow */

/*
 * StatusLine - display text on a status line
 */
void StatusLine( int line, char *str, int format )
{
    HDC         hdc;
    font_type   font;
    HFONT       hfont;

    format = format;

    if( line != 1 ) {
        return;
    }
    if( !AllowDisplay || BAD_ID( status_window_id ) ) {
        return;
    }
    hdc = TextGetDC( status_window_id, WIN_TEXT_STYLE( &StatusBar ) );
    font = WIN_TEXT_FONT( &StatusBar );
    hfont = FontHandle( font );
    StatusWndDrawLine( sw, hdc, hfont, str, DT_ESC_CONTROLLED );
    TextReleaseDC( status_window_id, hdc );

} /* StatusLine */

/*
 * GetStatusHeight - get the height of a Win32 native status bar
 */
int GetStatusHeight( void )
{
    return( StatusWndGetHeight( sw ) );

} /* GetStatusHeight */
