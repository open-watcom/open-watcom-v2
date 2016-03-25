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

#define NUM_ROWS 5000

static int W7NumRows( a_window *wnd )
{
    wnd=wnd;
    return( NUM_ROWS );
}

static bool    W7GetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    static char buff[20];

    wnd=wnd;

    if( row >= NUM_ROWS ) return( FALSE );
    if( piece != 0 ) return( FALSE );
    line->text = buff;
    itoa( row, buff, 10 );
    return( TRUE );
}


static void    W7Refresh( a_window *wnd )
{
    WndRepaint( wnd );
}

static wnd_info W7Info = {
    NoEventProc,
    W7Refresh,
    W7GetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    W7NumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    NoPopUp
};

a_window *W7Open( void )
{
    wnd_create_struct   info;

    WndInitCreateStruct( &info );
    info.scroll |= GUI_VTRACK;
    info.title = "Numbers";
    info.info = &W7Info;
    info.wndclass = CLASS_W1;
    info.extra = NULL;
    return( WndCreateWithStruct( &info ) );
}
