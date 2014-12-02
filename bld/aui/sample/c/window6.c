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

static char * Stuff[] = {
        "Line 1",
        "Line 2",
        "Line 3",
        "Line 4",
        "Line 5",
};

extern int W6NumRows( a_window *wnd )
{
    wnd=wnd;
    return( ArraySize( Stuff ) );
}

extern WNDGETLINE W6GetLine;
extern  bool    W6GetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    wnd=wnd;
    if( row >= ArraySize( Stuff ) ) return( FALSE );
    if( piece != 0 ) return( FALSE );
    line->text = Stuff[ row ];
    return( TRUE );
}


extern WNDREFRESH W6Refresh;
void    W6Refresh( a_window *wnd )
{
    WndRepaint( wnd );
}

wnd_metrics W6Metrics = { 3, 12, 0, 0 };

wnd_info W6Info = {
    NoEventProc,
    W6Refresh,
    W6GetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    W6NumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    NoPopUp
};

extern WNDOPEN W6Open;
extern a_window *W6Open()
{
    wnd_create_struct   info;
    a_window            *wnd;

    WndInitCreateStruct( &info );
    info.info = &W6Info;
    info.style |= GUI_INIT_INVISIBLE | GUI_POPUP;
    wnd = WndCreateWithStruct( &info );
    WndSetFontInfo( wnd, "-13 0 0 0 700 0 0 0 0 1 2 1 18 \"MS Serif\"" );
    WndForcePaint( wnd );
    WndShrinkToMouse( wnd, &W6Metrics );
    GUIShowWindow( WndGui( wnd ) );
    return( wnd );
}
