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



#include "app.h"

#define NUM_ROWS        60

extern WNDNUMROWS W5NumRows;

static int W5Scroll( a_window *wnd, int lines )
{
    int         old_top;
    int         new_top;

    old_top = WndTop( wnd );
    new_top = old_top + lines;
    if( new_top >= NUM_ROWS ) new_top = NUM_ROWS-1;
    if( new_top < 0 ) new_top = 0;
    WndSetTop( wnd, new_top );
    WndSetVScrollRange( wnd, WndRows( wnd ) + NUM_ROWS - 1 );
    WndSetThumbPos( wnd, new_top );
    return( new_top - old_top );
}


static bool    W5GetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    static char buff[20];

    if( row >= NUM_ROWS ) return( false );
    if( piece == 0 ) {
        itoa( row, buff, 10 );
        line->text = buff;
    } else if( piece == 1 ) {
        line->text = "This is text";
        line->extent = WND_MAX_EXTEND;
        line->indent = 5*WndAvgCharX( wnd );
    } else {
        return( false );
    }
    return( true );
}


static void W5Refresh( a_window *wnd )
{
    WndRepaint( wnd );
}

static wnd_info W5Info = {
    NoEventProc,
    W5Refresh,
    W5GetLine,
    NoMenuItem,
    W5Scroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    NoNumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    NoPopUp
};

a_window *W5Open( void )
{
    a_window    *wnd;
    wnd = WndCreate( "Just some more text", &W5Info, 0, NULL );
    WndRepaint( wnd );
    WndScrollAbs( wnd, 0 );
    WndScrollAbs( wnd, NUM_ROWS );
    WndScrollAbs( wnd, NUM_ROWS/2 );
    return( wnd );
}
