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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgadget.h"
#include "guidlg.h"
#include "strutil.h"

extern void StartupErr(char *);
extern int WndNumColours;
extern a_window *WndMain;

#if 0
#define _EXPIRY_YEAR    1988
#define _EXPIRY_MONTH   5
#define _EXPIRY_DAY     1
#endif

extern int      FingMessageSize;
extern char     *AboutMessage[];
static gui_coord BitmapSize;
static gui_ord  Width;
static gui_ord  Height;

#define TOP_BLANK( wnd ) ( GUIIsGUI() ? 2 : ( ( WndRows(wnd) - FingMessageSize ) / 2 ) )

#ifdef _EXPIRY_YEAR
static bool ChkDate()
{
    struct date_st { char day; char month; int year; } date;
    extern struct date_st getdate();
    date = getdate();
    if( date.year  < _EXPIRY_YEAR  ) return( TRUE );
    if( date.year  > _EXPIRY_YEAR  ) return( FALSE );
    if( date.month < _EXPIRY_MONTH ) return( TRUE );
    if( date.month > _EXPIRY_MONTH ) return( FALSE );
    if( date.day   > _EXPIRY_DAY   ) return( FALSE );
    return( TRUE );
}
#endif

static a_window *WndFing = NULL;

void FingClose()
{
    a_window    *wnd;

    if( WndFing != NULL ) {
        wnd = WndFing;
        WndFing = NULL;
        WndClose( wnd );
    }
}


static WNDNUMROWS FingNumRows;
static int FingNumRows( a_window *wnd )
{
    wnd=wnd;
    return( TOP_BLANK( wnd ) + FingMessageSize + GUIIsGUI() );
}


static WNDGETLINE FingGetLine;
static  bool    FingGetLine( a_window *wnd, int row, int piece,
                            wnd_line_piece *line )
{
    if( piece != 0 ) return( FALSE );
    row -= TOP_BLANK( wnd );
    if( row < 0 ) {
        line->text = " ";
        return( TRUE );
    }
    if( row >= FingMessageSize ) {
        if( !GUIIsGUI() || piece != 0 ) return( FALSE );
        row -= FingMessageSize;
        switch( row ) {
        case 0:
            line->text = " ";
            return( TRUE );
        case 1:
            SetGadgetLine( wnd, line, GADGET_SPLASH );
            line->indent = ( Width - BitmapSize.x ) / 2;
            return( TRUE );
        default:
            return( FALSE );
        }
    }
    line->text = AboutMessage[ row ];
    line->indent = ( Width - WndExtentX( wnd, line->text ) ) / 2;
    return( TRUE );
}

static WNDCALLBACK FingEventProc;
static bool FingEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    gui_colour_set      *colours;

    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        if( GUIIsGUI() ) {
            colours = GUIGetWindowColours( WndGui( wnd ) );
            colours[GUI_BACKGROUND].fore = GUI_BRIGHT_CYAN;
            colours[GUI_BACKGROUND].back = GUI_BRIGHT_CYAN;
            colours[WND_PLAIN].fore = GUI_BLACK;
            colours[WND_PLAIN].back = GUI_BRIGHT_CYAN;
            GUISetWindowColours( WndGui( wnd ), WndNumColours, colours );
            GUIMemFree( colours );
        }
        return( TRUE );
    }
    return( FALSE );
}

static wnd_info FingInfo = {
    FingEventProc,
    NoRefresh,
    FingGetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    FingNumRows,
    NoNextRow,
    NoNotify,
    NULL,
    0,
    NoPopUp,
};

void FingOpen()
{
    wnd_create_struct   info;
    int                 i;
    gui_ord             extent;

#ifdef _EXPIRY_YEAR
    if( !ChkDate() ) {
        StartupErr( LIT( Tst_Per_Exp ) );
    }
#endif
    WndInitCreateStruct( &info );
    info.title = NULL;
    info.info = &FingInfo;
    info.class = WND_NO_CLASS;
    info.extra = NULL;
    if( GUIIsGUI() ) {
        WndGetGadgetSize( GADGET_SPLASH, &BitmapSize );
        Width = Height = 0;
        for( i = 0; i < FingMessageSize; ++i ) {
            extent = WndExtentX( WndMain, AboutMessage[ i ] );
            if( extent > Width ) Width = extent;
        }
        if( BitmapSize.x >= Width ) Width = BitmapSize.x;
        Width += 4*WndMaxCharX( WndMain );
        Height = ( FingMessageSize + 5 ) * WndMaxCharY( WndMain );
        Height += BitmapSize.y;
    } else {
        Width = Height = WND_APPROX_SIZE;
    }
    info.rect.x = ( WND_APPROX_SIZE - Width ) / 2;
    info.rect.y = ( WND_APPROX_SIZE - Height ) / 2;
    info.rect.width = Width;
    info.rect.height = Height;
    info.style |= GUI_POPUP | GUI_NOFRAME;
    info.scroll = GUI_NOSCROLL;
    WndFing = WndCreateWithStruct( &info );
    if( WndFing == NULL ) return;
    WndRepaint( WndFing );
}
