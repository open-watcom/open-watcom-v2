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


#include <stddef.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "sortlist.h"
#include "strutil.h"
#include "wndsys.h"
#include "dbgprog.h"
#include "dbgimg.h"
#include "dbgwglob.h"
#include "dbgwinsp.h"
#include "dlgfile.h"


extern void             SetLastSym( char *to );

#define TITLE_SIZE      2

enum {
    PIECE_IMAGE,
    PIECE_SYMBOL,
    PIECE_DIP,
    PIECE_LAST
};

static gui_ord  Indents[PIECE_LAST];

#include "menudef.h"
static gui_menu_struct ImgMenu[] = {
    #include "menuimg.h"
};


static void CalcIndents( a_window *wnd )
{
    image_entry *img;
    gui_ord     max_image;
    gui_ord     max_symbol;
    gui_ord     curr;

    max_image = WndExtentX( wnd, LIT_DUI( Executable_File ) );
    max_symbol = WndExtentX( wnd, LIT_DUI( Debug_Information ) );
    for( img = DbgImageList; img != NULL; img = img->link ) {
        curr = WndExtentX( wnd, img->image_name );
        if( curr > max_image ) max_image = curr;
        curr = WndExtentX( wnd, ImgSymFileName( img, false ) );
        if( curr > max_symbol ) max_symbol = curr;
    }
    Indents[PIECE_SYMBOL] = max_image + 2*WndMaxCharX( wnd );
    Indents[PIECE_DIP] = Indents[PIECE_SYMBOL] + max_symbol + 4*WndMaxCharX( wnd );
}

static image_entry      *ImgGetImage( int row )
{
    image_entry *img;
    int         count;

    count = 0;
    if( row < 0 ) return( NULL );
    for( img = DbgImageList; img != NULL; img = img->link ) {
        if( count++ == row ) return( img );
    }
    return( NULL );
}

static void     ImgInit( a_window *wnd )
{
    ImgSort();
    CalcIndents( wnd );
    WndZapped( wnd );
}

static void     ImgMenuItem( a_window *wnd, gui_ctl_id id, int row, int piece )
{
    image_entry *img;
    char        *new_name;
    char        *old_name;

    piece=piece;
    img = ImgGetImage( row );
    switch( id ) {
    case MENU_INITIALIZE:
        WndMenuGrayAll( wnd );
        if( img != NULL ) {
            if( img->dip_handle == NO_MOD ) {
                WndMenuEnable( wnd, MENU_IMAGE_ADD_SYMBOLS, true );
            } else {
                WndMenuEnableAll( wnd );
            }
        }
        break;
    case MENU_IMAGE_ADD_SYMBOLS:
        // nyi - change sym_file
        if( img->deferred_symbols ) {
            ReLoadImgSymInfo( img );
        } else {
            new_name = DupStr( ImgSymFileName( img, true ) );
            if( !SymBrowse( &new_name ) ) {
                _Free( new_name );
            } else {
                UnLoadImgSymInfo( img, true );
                old_name = img->symfile_name;
                img->symfile_name = new_name;
                if( ReLoadImgSymInfo( img ) ) {
                    _Free( old_name );
                } else {
                    img->symfile_name = old_name;
                    _Free( new_name );
                }
            }
        }
        ImgInit( wnd );
        break;
    case MENU_IMAGE_DEL_SYMBOLS:
        UnLoadImgSymInfo( img, true );
        ImgInit( wnd );
        break;
    case MENU_IMAGE_SHOW_FUNCTIONS:
        WndGblFuncInspect( img->dip_handle );
        break;
    case MENU_IMAGE_SHOW_MODULES:
        WndModListInspect( img->dip_handle );
        break;
    case MENU_IMAGE_SHOW_GLOBALS:
        WndGblVarInspect( img->dip_handle );
        break;
    }
}

static WNDNUMROWS ImgNumRows;
static int ImgNumRows( a_window *wnd )
{
    image_entry *img;
    int         count;

    wnd=wnd;
    count = 0;
    for( img = DbgImageList; img != NULL; img = img->link ) {
        ++count;
    }
    return( count );
}

static WNDGETLINE ImgGetLine;
static  bool    ImgGetLine( a_window *wnd, int row, int piece,
                             wnd_line_piece *line )
{
    image_entry         *img;

    wnd=wnd;
    line->indent = Indents[ piece ];
    if( row < 0 ) {
        row += TITLE_SIZE;
        switch( row ) {
        case 0:
            line->tabstop = false;
            switch( piece ) {
            case PIECE_IMAGE:
                line->text = LIT_DUI( Executable_File );
                return( true );
            case PIECE_SYMBOL:
                line->text = LIT_DUI( Debug_Information );
                return( true );
            case PIECE_DIP:
                line->text = LIT_DUI( Debug_Information_Type );
                return( true );
            default:
                return( false );
            }
        case 1:
            if( piece != 0 ) return( false );
            SetUnderLine( wnd, line );
            return( true );
        default:
            return( false );
        }
    } else {
        line->tabstop = false;
        line->use_prev_attr = true;
        line->extent = WND_MAX_EXTEND;
        img = ImgGetImage( row );
        if( img == NULL ) return( false );
        switch( piece ) {
        case PIECE_IMAGE:
            line->text = img->image_name;
            line->tabstop = true;
            line->use_prev_attr = false;
            return( true );
        case PIECE_SYMBOL:
            line->text = ImgSymFileName( img, false );
            return( true );
        case PIECE_DIP:
            if( img->dip_handle == NO_MOD ) {
                line->text = " ";
            } else {
                line->text = (char *)ImageDIP( img->dip_handle );
            }
            return( true );
        }
    }
    return( false );
}

static WNDREFRESH ImgRefresh;
static void     ImgRefresh( a_window *wnd )
{
    ImgInit( wnd );
}


static WNDCALLBACK ImgEventProc;
static bool ImgEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        ImgInit( wnd );
        return( true );
    case GUI_RESIZE:
        ImgInit( wnd );
        return( true );
    case GUI_DESTROY:
        return( false );
    }
    return( false );
}

wnd_info ImgInfo = {
    ImgEventProc,
    ImgRefresh,
    ImgGetLine,
    ImgMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    WndFirstMenuItem,
    ImgNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_SYM_CHANGE,
    DefPopUp( ImgMenu )
};

extern WNDOPEN WndImgOpen;
extern a_window *WndImgOpen( void )
{
    return( DbgTitleWndCreate( LIT_DUI( WindowImages ), &ImgInfo, WND_IMAGE,
            NULL, &ImgIcon, TITLE_SIZE, true ) );
}
