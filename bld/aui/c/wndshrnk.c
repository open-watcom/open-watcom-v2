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

void WndShrinkToMouse( a_window *wnd, wnd_metrics *metrics )
{
    gui_rect            rect,client;
    gui_point           mouse;
    gui_coord           round;
    gui_ord             max_width;
    gui_ord             min_width;
    int                 rows;

    GUIGetRect( wnd->gui, &rect );
    GUIGetClientRect( wnd->gui, &client );
    max_width = metrics->max_cols * WndAvgCharX( wnd );
    min_width = metrics->min_cols * WndAvgCharX( wnd );
    rect.width = wnd->max_indent + rect.width - client.width;
    if( max_width != 0 && rect.width > max_width ) {
        rect.width = max_width;
    }
    if( min_width != 0 && rect.width < min_width ) {
        rect.width = min_width;
    }
    if( rect.width > WndMax.x ) rect.width = WndMax.x;
    rows = wnd->max_row + 1;
    if( metrics->max_rows != 0 && rows > metrics->max_rows ) {
        rows = metrics->max_rows;
    }
    if( metrics->min_rows != 0 && rows < metrics->min_rows ) {
        rows = metrics->min_rows;
    }
    if( rows > rect.height/WndMaxCharY(wnd) ) {
        rows = rect.height / WndMaxCharY(wnd);
    }
    rect.height = rows * WndMaxCharY(wnd) + rect.height - client.height;
    if( rect.height > WndMax.y ) rect.height = WndMax.y;
    GUIGetMousePosn( WndMain->gui, &mouse );
    if( mouse.x < 0 ) mouse.x = 0;
    if( mouse.y < 0 ) mouse.y = 0;
    if( rect.width / 2 > mouse.x ) {
        rect.x = 0;
    } else if( ( rect.width / 2 ) + mouse.x > WndMax.x ) {
        rect.x = WndMax.x - rect.width;
    } else {
        rect.x = mouse.x - rect.width / 2;
    }
    if( mouse.y + rect.height > WndMax.y ) {
        rect.y = WndMax.y - rect.height;
    } else {
        rect.y = mouse.y;
        round.y = WndMaxCharY( wnd ) / 2;
        round.x = 0;
        GUITruncToPixel( &round );
        rect.y -= round.y;
    }
    GUISetRestoredSize( wnd->gui, &rect );
}

void WndMinimizeWindow( a_window *wnd )
{
    GUIMinimizeWindow( wnd->gui );
}

void WndMaximizeWindow( a_window *wnd )
{
    GUIMaximizeWindow( wnd->gui );
}

bool WndIsMinimized( a_window *wnd )
{
    gui_rect    rect;

    if( GUIIsMinimized( wnd->gui ) ) return( true );
    GUIGetClientRect( wnd->gui, &rect );
    if( rect.width == 0 && rect.height == 0 ) return( true ); // work around GUI bug
    return( false );
}

bool WndIsMaximized( a_window *wnd )
{
    return( GUIIsMaximized( wnd->gui ) );
}

void WndRestoreWindow( a_window *wnd )
{
    GUIRestoreWindow( wnd->gui );
}

void WndGetRect( a_window *wnd, gui_rect *rect )
{
    GUIGetRect( wnd->gui, rect );
}

void WndShowWindow( a_window *wnd )
{
    GUIShowWindow( wnd->gui );
}

void WndResizeWindow( a_window *wnd, gui_rect *rect )
{
    GUISetRestoredSize( wnd->gui, rect );
}

void WndShowAll()
{
    gui_window  *gui;
    gui_window  **list;
    gui_window  **pcurr;
    int         count;

    // this is a kludge since UI brings windows to the front on show
    count = 0;
    for( gui = GUIGetFront(); gui != NULL; gui = GUIGetNextWindow( gui ) ) {
        ++count;
    }
    list = WndAlloc( count * sizeof( gui ) );
    pcurr = list;
    for( gui = GUIGetFront(); gui != NULL; gui = GUIGetNextWindow( gui ) ) {
        if( WndMain != NULL && WndMain->gui == gui ) continue;
        *pcurr++ = gui;
    }
    while( pcurr > list ) {
        GUIShowWindow( *--pcurr );
    }
    WndFree( list );
}

