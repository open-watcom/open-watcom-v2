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

extern int              WndNumMenus;
extern gui_menu_struct  WndMainMenu[];
extern  gui_colour_set  WndColours[];
extern  int             WndNumColours;

gui_menu_struct *WndMainMenuPtr = WndMainMenu;
gui_menu_struct *WndPopupMenuPtr;

extern GUICALLBACK      WndMainEventProc;
extern char             *StrCopy(char *,char *);

static int              NumWindows;


extern  void    WndSetTitleSize( a_window *wnd, int size )
{
    wnd->title_size = size;
    WndRepaint( wnd );
}


extern  void    WndSetTitle( a_window *wnd, const char *title )
{
    GUISetWindowText( wnd->gui, title );
}

extern  int     WndGetTitle( a_window *wnd, char *buff, unsigned buff_len )
{
    return( GUIGetWindowText( wnd->gui, buff, buff_len ) );
}

static a_window *WndCreateWithStructBody( wnd_create_struct *info,
                                          gui_create_info *init )
{
    a_window    *wnd;
    gui_window  *gui;
    char        buff[256];
    int         size;

    if( info->title == NULL ) {
        buff[0] = '\0';
    } else {        // might be clobbered by create
        strcpy( buff, info->title );
    }
    if( info->title != NULL )
        strcpy( buff, info->title ); // might be clobbered by create
    size = sizeof( *wnd ) + ( WndMaxDirtyRects - 1 ) * sizeof( wnd->dirty ); //
    wnd = WndAlloc( size );
    if( wnd == NULL ) {
        WndFree( info->extra );
        WndNoMemory();
    }
    memset( wnd, 0, size );
    wnd->u.button_down.row = (char)-1;
    wnd->gui = NULL;
    wnd->info = info->info;
    wnd->class = info->class;
    wnd->extra = info->extra;
    wnd->title_size = info->title_size;
    wnd->rows = 1;      // just so it's not zero in init code
    WndNoSelect( wnd );
    WndNoCurrent( wnd );
    WndSetCurrCol( wnd );
    WndNullPopItem( wnd );
    wnd->dirtyrects= 0; // wndnoselect changes this!
    wnd->vscroll_pending = 0;
    wnd->hscroll_pending = -1;
    wnd->keypiece = WND_NO_PIECE;

    wnd->switches = WSW_SELECT_IN_TABSTOP | WSW_MUST_CLICK_ON_PIECE |
                    WSW_ALLOW_POPUP | WSW_SEARCH_WRAP | WSW_HIGHLIGHT_CURRENT |
                    WSW_ONLY_MODIFY_TABSTOP | WSW_MENU_ACCURATE_ROW;

    if( info->rect.width == 0 || info->rect.height == 0 ) {
        init->rect.x = 0;
        init->rect.y = 0;
        init->rect.width = WndMax.x;
        init->rect.height = WndMax.y;
    } else {
        init->rect.x = info->rect.x;
        init->rect.y = info->rect.y;
        init->rect.width = info->rect.width;
        init->rect.height = info->rect.height;
    }
    init->scroll = info->scroll;
    init->style = info->style;
    init->style |= GUI_VSCROLL_EVENTS;
    init->style &= ~GUI_HSCROLL_EVENTS;
    init->title = ( info->title == NULL ) ? NULL : "";
    if( WndMain != NULL ) {
        init->style |= GUI_VISIBLE;
        init->parent = WndMain->gui;
        init->num_menus = 0;
        init->menu = NULL;
    } else {
        init->style &= ~GUI_VISIBLE;
        init->scroll = GUI_NOSCROLL;
        init->num_menus = WndNumMenus;
        init->menu = WndMainMenu;
        init->parent = NULL;
    }
    if( init->style & GUI_POPUP ) {
        init->parent = NULL;
    }
    init->num_attrs = WndNumColours;
    init->colours = info->colour;
    init->call_back = WndMainEventProc;
    init->extra = wnd;

    _Set( wnd, WSW_ACTIVE );

    gui = GUICreateWindow( init );
    if( gui == NULL ) {
        WndFree( info->extra );
        WndFree( wnd );
        WndNoMemory();
        return( NULL );
    } else {
        if( buff[0] != '\0' ) {
            WndSetTitle( wnd, buff );
        }
    }
    ++NumWindows;
    return( wnd );
}

extern a_window *WndCreateWithStruct( wnd_create_struct *info )
{
    gui_create_info init;
    memset( &init, 0, sizeof( init ) );
    return( WndCreateWithStructBody( info, &init ) );
}

extern a_window *WndCreateWithStructAndMenuRes( wnd_create_struct *info,
                                                int resource_menu )
{
    gui_create_info init;
    memset( &init, 0, sizeof( init ) );
    init.resource_menu = resource_menu;
    return( WndCreateWithStructBody( info, &init ) );
}

extern void WndInitCreateStruct( wnd_create_struct *info )
{
    memset( info, 0, sizeof( *info ) );
    info->title = "";
    info->class = WND_NO_CLASS;
    info->style = ( GUI_GADGETS & ~GUI_CURSOR ) | GUI_CHANGEABLE_FONT;
    info->scroll = GUI_HSCROLL+GUI_VSCROLL+
                   GUI_HDRAG+GUI_VDRAG+
                   GUI_VROWS+GUI_HCOLS;
    info->colour = WndColours;
}


extern WNDCREATE        WndCreate;
extern a_window *WndCreate( char *title, wnd_info *wndinfo, wnd_class class,
                              void *extra )
{
    wnd_create_struct   info;

    WndInitCreateStruct( &info );
    info.title = title;
    info.info = wndinfo;
    info.class = class;
    info.extra = extra;
    return( WndCreateWithStruct( &info ) );
}

extern void     WndDestroy( a_window *wnd )
{
    WndEvent( wnd, GUI_DESTROY, NULL );
    GUIMemFree( wnd->searchitem );
    wnd->searchitem = NULL;
    WndFree( wnd->popitem );
    wnd->popitem = NULL;
    GUISetExtra( wnd->gui, NULL );
    WndFree( wnd );
    if( --NumWindows == 0 ) {
        WndFiniCacheLines();
        WndCleanUp();
    }
    if( wnd == WndMain ) WndMain = NULL;
}


extern WNDCLOSE WndClose;
extern void WndClose( a_window *wnd )
{
    bool        old;

    old = WndDoingRefresh; // doing refresh during destroy message mucks PM up
    WndDoingRefresh = TRUE;
    GUIDestroyWnd( wnd->gui );
    WndDoingRefresh = old;
}

void WndSetMainMenuText( gui_menu_struct *menu )
{
    if( WndMain != NULL ) {
        GUISetMenuText( WndMain->gui, menu->id, menu->label, FALSE );
    }
}

void WndSetIcon( a_window *wnd, gui_resource *icon )
{
    GUISetIcon( wnd->gui, icon );
}
