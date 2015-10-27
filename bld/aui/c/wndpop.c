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


#include "auipvt.h"////
#include <ctype.h>
#include <string.h>

static gui_ctl_id CalcMaxId( gui_menu_struct *curr, int num_menus )
{
    int         i;
    gui_ctl_id  submax;
    gui_ctl_id  max;

    max = 0;
    for( i = 0; i < num_menus; ++i ) {
        if( curr->id > max && (curr->style & GUI_SEPARATOR) == 0 )
            max = curr->id;
        if( curr->num_child_menus != 0 ) {
            submax = CalcMaxId( curr->child, curr->num_child_menus );
            if( submax > max ) {
                max = submax;
            }
        }
        ++curr;
    }
    return( max );
}

void NullPopupMenu( gui_menu_struct *menu )
{
    int                 i,j;
    gui_menu_struct     *curr,*sub;

    if( WndMain == NULL )
        return;
    curr = WndMainMenuPtr;
    for( i = 0; i < WndNumMenus; ++i, ++curr ) {
        if( !( curr->style & WND_MENU_POPUP ) )
            continue;
        if( curr->child != menu )
            continue;
        sub = curr->child;
        for( j = 0; j < curr->num_child_menus; ++j ) {
            GUIDeleteMenuItem( WndMain->gui, sub->id, FALSE );
            ++sub;
        }
        curr->child = NULL;
        curr->num_child_menus = 0;
        break;
    }
}

extern  void    WndAddPopupMenu( a_window *wnd )
{
    int                 i,j;
    gui_menu_struct     *curr,*sub;
    gui_ctl_id          max;

    if( WndMain == NULL )
        return;
    curr = WndMainMenuPtr;
    max = CalcMaxId( WndMainMenuPtr, WndNumMenus );
    for( i = 0; i < WndNumMenus; ++i ) {
        if( curr->style & WND_MENU_POPUP ) {
            sub = curr->child;
            for( j = 0; j < curr->num_child_menus; ++j ) {
                GUIDeleteMenuItem( WndMain->gui, sub->id, FALSE );
                ++sub;
            }
            if( wnd->popupmenu == NULL ) {
                curr->num_child_menus = 0;
                curr->child = NULL;
            } else {
                curr->num_child_menus = wnd->num_popups;
                curr->child = wnd->popupmenu;
            }
            WndPopupMenuPtr = curr;
            sub = curr->child;
            for( j = 0; j < curr->num_child_menus; ++j ) {
                if( ( sub->style & GUI_SEPARATOR ) && ( sub->id == 0 ) ) {
                    sub->id = ++max;
                }
                GUIAppendMenuToPopup( WndMain->gui, curr->id, sub, FALSE );
                ++sub;
            }
            break;
        }
        ++curr;
    }
}

extern  void    WndNullPopItem( a_window *wnd )
{
    WndFree( wnd->popitem );
    wnd->popitem = WndMustAlloc( 1 );
    wnd->popitem[0] = '\0';
}

static void GoBackward( a_window *wnd, wnd_coord *start,
                    wnd_line_piece *line )
{
    char        ch;

    ch = line->text[ start->col ];
    if( isspace( ch ) || !WndIDChar( wnd, ch ) ) return;
    for( ;; ) {
        if( start->col == 0 ) return;
        start->col = WndPrevCharCol( line->text, start->col );
        ch = line->text[ start->col ];
        if( isspace( ch ) || !WndIDChar( wnd, ch ) ) {
            start->col += GUICharLen( ch );
            return;
        }
    }
}


static void GoForward( a_window *wnd, wnd_coord *end,
                       wnd_line_piece *line )
{
    char        ch;

    ch = line->text[ end->col ];
    if( isspace( ch ) || !WndIDChar( wnd, ch ) ) {
        end->col += GUICharLen( ch ) - 1;
        return;
    }
    for( ;; ) {
        end->col += GUICharLen( ch );
        ch = line->text[end->col];
        if( isspace( ch ) || !WndIDChar( wnd, ch ) ) {
            end->col--;
            return;
        }
    }
}

extern  void    WndSelPopPiece( a_window *wnd, bool paint_immed )
{
    wnd_row             row;
    wnd_coord           *start;
    wnd_coord           *end;
    int                 piece;
    int                 buff_size;
    char                *ptr;
    int                 first;
    int                 len;
    wnd_line_piece      line;

    _Clr( wnd, WSW_SELECTING );
    _Clr( wnd, WSW_SELECTING_WITH_KEYBOARD );
    WndSelEnds( wnd, &start, &end );
    if( _Isnt( wnd, WSW_SUBWORD_SELECT ) ||
        ( start->row == end->row &&
          start->piece == end->piece && start->col == end->col ) ) {
        WndGetLine( wnd, start->row, start->piece, &line );
        GoBackward( wnd, start, &line );
        WndGetLine( wnd, end->row, end->piece, &line );
        GoForward( wnd, end, &line );
        if( end->row != start->row ) {
            GUIWndDirtyRow( wnd->gui, end->row );
            GUIWndDirtyRow( wnd->gui, start->row );
        } else if( paint_immed ) {
            GUIWndDirtyRow( wnd->gui, start->row );
        } else {
            WndDirtyScreenRange( wnd, start, end->col );
        }
    }
    buff_size = 0;
    for( row = start->row; row <= end->row; ++row ) {
        for( piece = 0; ; ++piece ) {
            if( !WndGetLine( wnd, row, piece, &line ) ) break;
            if( WndSelected( wnd, &line, row, piece, &first, &len ) ) {
                buff_size += len + 1;
            }
        }
    }
    WndFree( wnd->popitem );
    wnd->popitem = WndMustAlloc( buff_size+2 );
    ptr = wnd->popitem;
    for( row = start->row; row <= end->row; ++row ) {
        for( piece = 0; ; ++piece ) {
            if( !WndGetLine( wnd, row, piece, &line ) ) break;
            if( WndSelected( wnd, &line, row, piece, &first, &len ) ) {
                if( ptr != wnd->popitem ) *ptr++ = ' ';
                memcpy( ptr, line.text+first, len );
                ptr += len;
            }
        }
    }
    *ptr = '\0';
}


extern  void    WndSelPopItem( a_window *wnd, void *parm, bool paint_immed )
{
    wnd_coord   piece;

    if( !WndSelGetEndPiece( wnd, parm, &piece ) ) {
        WndNullPopItem( wnd );
        return;
    }
    wnd->sel_end = piece;
    WndSelPopPiece( wnd, paint_immed );
}


extern  void    WndKeyPopItem( a_window *wnd, bool paint_immed )
{
    if( !WndHasCurrent( wnd ) || _Isnt( wnd, WSW_CHAR_CURSOR ) ) {
        WndNullPopItem( wnd );
        return;
    }
    if( wnd->sel_start.row == WND_NO_ROW ) {
        WndNoSelect( wnd );
        wnd->sel_start = wnd->current;
    }
    if( wnd->sel_end.row == WND_NO_ROW ) {
        wnd->sel_end = wnd->current;
    }
    WndSelPopPiece( wnd, paint_immed );
}


extern  void    WndPopUp( a_window *wnd, gui_menu_struct *menu )
{
    gui_point           point;

    point.x = 0;
    point.y = 0;
    wnd->sel_end.row = 0;
    if( !WndHasCurrent( wnd ) ) {
        WndFirstCurrent( wnd );
    }
    if( WndHasCurrent( wnd ) ) {
        WndNoSelect( wnd );
        wnd->current.col = 0; // just to be sure
        WndCurrToGUIPoint( wnd, &point );
        wnd->sel_end = wnd->current;
        wnd->sel_start = wnd->current;
    }
    WndNullPopItem( wnd );
    SetWndMenuRow( wnd );
    WndInvokePopUp( wnd, &point, menu );
}


void WndChangeMenuAll( gui_menu_struct *menu, int num_popups, bool on, int bit )
{
    int                 i;

    for( i = 0; i < num_popups; ++i ) {
        if( menu[i].style & GUI_SEPARATOR ) continue;
        if( menu[i].num_child_menus != 0 ) {
            WndChangeMenuAll( menu[i].child, menu[i].num_child_menus, on, bit );
        }
        if( on ) {
            menu[i].style |= bit;
        } else {
            menu[i].style &= ~bit;
        }
    }
}


static void MenuAll( a_window *wnd, bool on, int bit )
{
    WndChangeMenuAll( wnd->popupmenu, wnd->num_popups, on, bit );
}

extern  void    WndMenuEnableAll( a_window *wnd )
{
    MenuAll( wnd, FALSE, GUI_GRAYED );
}


extern  void    WndMenuGrayAll( a_window *wnd )
{
    MenuAll( wnd, TRUE, GUI_GRAYED );
}


extern  void    WndMenuIgnoreAll( a_window *wnd )
{
    MenuAll( wnd, TRUE, GUI_IGNORE );
}


extern  void    WndMenuRespectAll( a_window *wnd )
{
    MenuAll( wnd, FALSE, GUI_IGNORE );
}


static void DoMenuBitOn( gui_menu_struct *menu, int num_popups, gui_ctl_id id, bool on, int bit )
{
    int         i;

    for( i = 0; i < num_popups; ++i ) {
        if( menu[i].num_child_menus != 0 ) {
            DoMenuBitOn( menu[i].child, menu[i].num_child_menus, id, on, bit );
        }
        if( menu[i].id == id ) {
            if( on ) {
                menu[i].style |= bit;
            } else {
                menu[i].style &= ~bit;
            }
        }
    }
}


static void MenuBitOn( a_window *wnd, gui_ctl_id id, bool on, int bit )
{
    DoMenuBitOn( wnd->popupmenu, wnd->num_popups, id, on, bit );
}

extern  void    WndMenuCheck( a_window *wnd, gui_ctl_id id, bool check )
{
    MenuBitOn( wnd, id, check, GUI_MENU_CHECKED );
}


extern  void    WndMenuEnable( a_window *wnd, gui_ctl_id id, bool enable )
{
    MenuBitOn( wnd, id, !enable, GUI_GRAYED );
}

extern  void    WndMenuIgnore( a_window *wnd, gui_ctl_id id, bool ignore )
{
    MenuBitOn( wnd, id, ignore, GUI_IGNORE );
}


void WndCreateFloatingPopup( a_window *wnd, gui_point *point,
                             char num_popups, gui_menu_struct *menu,
                             gui_ctl_id *last_popup )
{
    gui_point   mouse;

    if( !GUIIsGUI() ) {
        WndResetStatusText(); // wipe out 'Busy'
    }
    if( point == NULL ) {
        if( !GUIGetMousePosn( wnd->gui, &mouse ) ) {
            mouse.x = 0;
            mouse.y = 0;
        }
        point = &mouse;
    }
    GUICreateFloatingPopup( wnd->gui, point, num_popups, menu, GUI_TRACK_BOTH, last_popup );
}


void    WndInvokePopUp( a_window *wnd, gui_point *point, gui_menu_struct *menu )
{
    gui_ctl_id  dummy;

    if( _Isnt( wnd, WSW_ALLOW_POPUP ) ) return;
    WndMenuItem( wnd, MENU_INITIALIZE, WndMenuRow, WndMenuPiece );
    if( menu != NULL && wnd->num_popups == 1 ) {
        if( menu->style & GUI_GRAYED ) {
            Ring();
        } else {
            WndMenuItem( wnd, wnd->popupmenu[ 0 ].id, WndMenuRow, WndMenuPiece );
        }
    } else if( wnd->num_popups != 0 ) {
        if( menu == NULL ) {
            WndCreateFloatingPopup( wnd, point, wnd->num_popups, wnd->popupmenu, &wnd->last_popup );
        } else if( menu->style & GUI_GRAYED ) {
            Ring();
        } else if( menu->child == NULL ) {
            WndMenuItem( wnd, menu->id, WndMenuRow, WndMenuPiece );
        } else {
            WndCreateFloatingPopup( wnd, point, menu->num_child_menus, menu->child, &dummy );
        }
    }
}


static void WndGetPopPoint( a_window *wnd, gui_point *point )
{
    WndKeyPopItem( wnd, TRUE );
    if( wnd->sel_start.row != WND_NO_ROW ) {
        WndCoordToGUIPoint( wnd, &wnd->sel_end, point );
    } else {
        WndCoordToGUIPoint( wnd, &wnd->current, point );
    }
}

extern  void    WndKeyPopUp( a_window *wnd, gui_menu_struct *menu )
{
    gui_point           point;

    WndGetPopPoint( wnd, &point );
    SetWndMenuRow( wnd );
    WndInvokePopUp( wnd, &point, menu );
}


extern  void    WndRowPopUp( a_window *wnd, gui_menu_struct *menu, int row, int piece )
{
    gui_point           point;

    WndGetPopPoint( wnd, &point );
    WndMenuRow = WndScreenRow( wnd, row );
    WndMenuPiece = piece;
    WndInvokePopUp( wnd, &point, menu );
}


extern  void    WndSetMainMenu( gui_menu_struct *menu, int num_menus )
{
    if( WndMain == NULL ) return;
    WndMainMenuPtr = menu;
    WndNumMenus = num_menus;
    GUIResetMenus( WndMain->gui, WndNumMenus, WndMainMenuPtr );
}

void WndClick( a_window *wnd, gui_ctl_id id )
{
    if( !WndMainMenuProc( wnd, id ) ) {
        wnd = WndFindActive();
        if( wnd != NULL ) {
            WndMenuItem( wnd, id, WndMenuRow, WndMenuPiece );
        }
    }
}

static void WndSetPopupBits( a_window *wnd, gui_menu_struct *menu )
{
    int                 i;
    gui_menu_struct     *curr;

    curr = menu->child;
    for( i = 0; i < menu->num_child_menus; ++i ) {
        if( !( curr->style & GUI_SEPARATOR ) ) {
            GUIEnableMenuItem( WndMain->gui, curr->id, wnd && (curr->style & GUI_GRAYED) == 0, FALSE );
            GUICheckMenuItem( WndMain->gui, curr->id, wnd && (curr->style & GUI_CHECKED) != 0, FALSE );
            if( curr->child ) WndSetPopupBits( wnd, curr );
        }
        ++curr;
    }
}


extern void WndSetPopup( gui_ctl_id id )
{
    a_window    *wnd;

    if( WndMain == NULL )
        return;
    if( WndPopupMenuPtr == NULL )
        return;
    if( WndPopupMenuPtr->id != id )
        return;
    if( WndMouseButtonIsDown() )
        return;
    wnd = WndFindActive();
    if( wnd == WndMain )
        wnd = NULL;
    WndMenuRow = WND_NO_ROW;
    WndMenuPiece = WND_NO_PIECE;
    if( wnd != NULL ) {
        SetWndMenuRow( wnd );
        WndMenuItem( wnd, MENU_INITIALIZE, WndMenuRow, WndMenuPiece );
    }
    WndSetPopupBits( wnd, WndPopupMenuPtr );
}

extern void WndEnableMainMenu( gui_ctl_id id, bool enable )
{
    if( WndMain == NULL || WndMainMenuPtr == NULL )
        return;
    DoMenuBitOn( WndMainMenuPtr, WndNumMenus, id, !enable, GUI_GRAYED );
    GUIEnableMenuItem( WndMain->gui, id, enable, FALSE );
}

extern void WndCheckMainMenu( gui_ctl_id id, bool check )
{
    if( WndMain == NULL || WndMainMenuPtr == NULL )
        return;
    DoMenuBitOn( WndMainMenuPtr, WndNumMenus, id, check, GUI_MENU_CHECKED );
    GUICheckMenuItem( WndMain->gui, id, check, FALSE );
}

#if 0
// keep commented out until GUI supports this properly
extern void WndSetHintText( a_window *wnd, gui_ctl_id id, char *text )
{
    GUISetHintText( wnd->gui, id, text );
}
#endif
