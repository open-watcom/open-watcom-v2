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

extern void W1Open();

extern WNDNUMROWS W4NumRows;

static gui_menu_struct W4PopUp[] = {
    { "&Say",       MENU_W2_SAY, GUI_ENABLED },
    { "&Top",       MENU_W2_TOP, GUI_ENABLED },
    { "&Open 1",    MENU_W2_OPEN1, GUI_ENABLED },
};

char * Stuff[] = {
        "Aardvark",
        "Aaron",
        "Abacus",
        "Balloon",
        "Bazaar",
        "Bizare",
        "Crazy",
        "Dashing",
        "Dastardly",
        "Deadly",
        "Driven",
        "Enough",
        "Hardly",
        "Justify",
        "Loony",
        "Mulroney",
        "Mystique",
        "Nuts",
        "Poor",
        "Queen",
        "Queer",
        "Quiet",
        "Schmooze",
        "Smart",
        "Stupid",
        "Vegtable",
        "Veritable",
        "Virtual",
        "Wobble",
        "Weeble",
        "Woozy",
        "Xray",
        "Xylophone",
        "Zap",
        "Znaimer",
        "Zoot Suit",
        "Zulu",
};

#define SIZE ArraySize( Stuff )

extern int W4NumRows( a_window *wnd )
{
    wnd=wnd;
    return( SIZE );
}

extern  WNDMODIFY W4Modify;
extern  void    W4Modify( a_window *wnd, int row, int piece )
{
    wnd=wnd;piece=piece;
    if( row < 0 ) {
        Say( "Shouldn't get this event" );
    } else {
        Say2( "Modify", Stuff[ row ] );
    }
}


#if 0
static char UiMapChar[] = { 0xC6, 0xEA, 0xC7, 0xD0,
                                        0xD1, 0xEB, 0xD2, 0xD3,
                                        0xD4, 0xCB, 0xCA, 0xC5,
                                        0xCC, 0xBA, 0xCE, 0xCD,
                                        0xDF, 0xDC, 0xFD, 0xF5, 0 };
#endif
extern WNDGETLINE W4GetLine;
extern  bool    W4GetLine( a_window *wnd, wnd_row row, int piece,
                             wnd_line_piece *line )
{
    static char buff[20];

    wnd=wnd;
    if( row == -2 ) {
        if( piece != 0 ) return( FALSE );
        line->text = "Title line 1";
        line->tabstop = FALSE;
        line->static_text = TRUE;
    } else if( row == -1 ) {
        if( piece != 0 ) return( FALSE );
        line->tabstop = FALSE;
        line->static_text = TRUE;
        #if 0
            line->text = UiMapChar;
        #else
            line->text = "";
            line->underline = TRUE;
            line->extent = WndWidth( wnd );
            line->indent = 0;
        #endif
    } else if( row >= SIZE ) {
        return( FALSE );
    } else {
        switch( piece ) {
        case 0:
            line->tabstop = TRUE;
            itoa( row, buff, 10 );
            line->text = buff;
            line->extent = WND_MAX_EXTEND;
            return( TRUE );
        case 1:
            line->tabstop = FALSE;
            line->use_prev_attr = TRUE;
            line->text = "";
            line->extent = WND_MAX_EXTEND;
            line->indent = 1000;
            return( TRUE );
        case 2:
            line->tabstop = FALSE;
            line->use_prev_attr = TRUE;
            line->text = Stuff[ row ];
            line->extent = WND_MAX_EXTEND;
            line->indent = 2000;
            return( TRUE );
        case 3:
            line->tabstop = FALSE;
            line->use_prev_attr = TRUE;
            line->text = "";
            line->extent = WND_MAX_EXTEND;
            line->indent = 3000;
            return( TRUE );
        default:
            return( FALSE );
        }
    }
    return( TRUE );
}


extern WNDREFRESH W4Refresh;
void    W4Refresh( a_window *wnd )
{
    WndRepaint( wnd );
}

extern  WNDMENU W4MenuItem;
extern void     W4MenuItem( a_window *wnd, unsigned id, int row, int piece )
{

    row=row;piece=piece;
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuGrayAll( wnd );
        if( row < 0 ) break;
        WndMenuEnableAll( wnd );
        break;
    case MENU_W2_SAY:
        Say2( "Say", WndPopItem( wnd ) );
        break;
    case MENU_W2_OPEN1:
        W1Open();
        break;
    case MENU_W2_TOP:
        WndScrollAbs( wnd, 0 );
        break;
    }
}

wnd_info W4Info = {
    NoEventProc,
    W4Refresh,
    W4GetLine,
    W4MenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    W4Modify,
    W4NumRows,
    NoNextRow,
    NoNotify,
    0,
    DefPopUp( W4PopUp )
};

extern WNDOPEN W4Open;
extern a_window *W4Open()
{
    a_window    *wnd;

    wnd = WndCreate( "", &W4Info, 0, NULL );
    if( wnd ) WndSetKey( wnd, 1 );
    return( wnd );
}
