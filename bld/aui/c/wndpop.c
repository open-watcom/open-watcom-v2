/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include "_aui.h"
#include <ctype.h>
#include <string.h>


static gui_ctl_id CalcMenuMaxId( const gui_menu_items *menus )
{
    gui_ctl_id  submaxid;
    gui_ctl_id  maxid;
    int         i;

    maxid = 0;
    for( i = 0; i < menus->num_items; i++ ) {
        if( maxid < menus->menu[i].id && (menus->menu[i].style & GUI_STYLE_MENU_SEPARATOR) == 0 )
            maxid = menus->menu[i].id;
        submaxid = CalcMenuMaxId( &menus->menu[i].child );
        if( maxid < submaxid ) {
            maxid = submaxid;
        }
    }
    return( maxid );
}

void NullPopupMenu( const gui_menu_struct *menu )
{
    int             i;
    gui_menu_items  *child;

    if( WndMain != NULL ) {
        for( i = 0; i < WndMainMenuPtr->num_items; i++ ) {
            if( (WndMainMenuPtr->menu[i].style & WND_MENU_POPUP) && WndMainMenuPtr->menu[i].child.menu == menu ) {
                child = &WndMainMenuPtr->menu[i].child;
                for( i = 0; i < child->num_items; i++ ) {
                    GUIDeleteMenuItem( WndMain->gui, child->menu[i].id, false );
                }
                *child = NoMenu;
                break;
            }
        }
    }
}

void    WndAddPopupMenu( a_window wnd )
{
    int                 i;
    gui_menu_struct     *menu;
    gui_menu_struct     *child;
    gui_ctl_id          maxid;

    if( WndMain != NULL ) {
        maxid = CalcMenuMaxId( WndMainMenuPtr );
        menu = WndMainMenuPtr->menu;
        for( i = 0; i < WndMainMenuPtr->num_items; i++ ) {
            if( menu->style & WND_MENU_POPUP ) {
                child = menu->child.menu;
                for( i = 0; i < menu->child.num_items; i++ ) {
                    GUIDeleteMenuItem( WndMain->gui, child->id, false );
                    child++;
                }
                menu->child = wnd->popup;
                WndPopupMenuPtr = menu;
                child = menu->child.menu;
                for( i = 0; i < menu->child.num_items; i++ ) {
                    if( (child->style & GUI_STYLE_MENU_SEPARATOR) && ( child->id == 0 ) ) {
                        child->id = ++maxid;
                    }
                    GUIAppendMenuToPopup( WndMain->gui, menu->id, child, false );
                    child++;
                }
                break;
            }
            menu++;
        }
    }
}

void    WndNullPopItem( a_window wnd )
{
    WndFree( wnd->popitem );
    wnd->popitem = WndMustAlloc( 1 );
    wnd->popitem[0] = '\0';
}

static void GoBackward( a_window wnd, wnd_coord *start, wnd_line_piece *line )
{
    int     ch;

    ch = UCHAR_VALUE( line->text[start->colidx] );
    if( isspace( ch ) || !WndIDChar( wnd, ch ) )
        return;
    while( start->colidx != 0 ) {
        start->colidx = WndPrevCharColIdx( line->text, start->colidx );
        ch = UCHAR_VALUE( line->text[start->colidx] );
        if( isspace( ch ) || !WndIDChar( wnd, ch ) ) {
            start->colidx += GUICharLen( ch );
            break;
        }
    }
}


static void GoForward( a_window wnd, wnd_coord *end, wnd_line_piece *line )
{
    int     ch;

    ch = UCHAR_VALUE( line->text[end->colidx] );
    if( isspace( ch ) || !WndIDChar( wnd, ch ) ) {
        end->colidx += GUICharLen( ch ) - 1;
        return;
    }
    for( ;; ) {
        end->colidx += GUICharLen( ch );
        ch = UCHAR_VALUE( line->text[end->colidx] );
        if( isspace( ch ) || !WndIDChar( wnd, ch ) ) {
            end->colidx--;
            break;
        }
    }
}

static void    WndSelPopPiece( a_window wnd, bool paint_immed )
{
    wnd_row             row;
    wnd_coord           *start;
    wnd_coord           *end;
    wnd_piece           piece;
    size_t              buff_size;
    char                *ptr;
    wnd_colidx          first_colidx;
    size_t              len;
    wnd_line_piece      line;

    WndClrSwitches( wnd, WSW_SELECTING | WSW_SELECTING_WITH_KEYBOARD );
    WndSelEnds( wnd, &start, &end );
    if( WndSwitchOff( wnd, WSW_SUBWORD_SELECT )
      || ( start->row == end->row && start->piece == end->piece && start->colidx == end->colidx ) ) {
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
            WndDirtyScreenRange( wnd, start, end->colidx );
        }
    }
    buff_size = 0;
    for( row = start->row; row <= end->row; ++row ) {
        for( piece = 0; ; ++piece ) {
            if( !WndGetLine( wnd, row, piece, &line ) )
                break;
            if( WndSelected( wnd, &line, row, piece, &first_colidx, &len ) ) {
                buff_size += len + 1;
            }
        }
    }
    WndFree( wnd->popitem );
    wnd->popitem = WndMustAlloc( buff_size + 2 );
    ptr = wnd->popitem;
    for( row = start->row; row <= end->row; ++row ) {
        for( piece = 0; ; ++piece ) {
            if( !WndGetLine( wnd, row, piece, &line ) )
                break;
            if( WndSelected( wnd, &line, row, piece, &first_colidx, &len ) ) {
                if( ptr != wnd->popitem )
                    *ptr++ = ' ';
                memcpy( ptr, line.text + first_colidx, len );
                ptr += len;
            }
        }
    }
    *ptr = '\0';
}


void    WndSelPopItem( a_window wnd, void *parm, bool paint_immed )
{
    wnd_coord   piece;

    if( !WndSelGetEndPiece( wnd, parm, &piece ) ) {
        WndNullPopItem( wnd );
        return;
    }
    wnd->sel_end = piece;
    WndSelPopPiece( wnd, paint_immed );
}


void    WndKeyPopItem( a_window wnd, bool paint_immed )
{
    if( !WndHasCurrent( wnd ) || WndSwitchOff( wnd, WSW_CHAR_CURSOR ) ) {
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


void    WndPopUp( a_window wnd, const gui_menu_struct *menu )
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
        wnd->current.colidx = 0; // just to be sure
        WndCurrToGUIPoint( wnd, &point );
        wnd->sel_end = wnd->current;
        wnd->sel_start = wnd->current;
    }
    WndNullPopItem( wnd );
    SetWndMenuRow( wnd );
    WndInvokePopUp( wnd, &point, menu );
}


void WndChangeMenuAll( gui_menu_items *menus, bool on, int bit )
{
    int     i;

    for( i = 0; i < menus->num_items; i++ ) {
        if( (menus->menu[i].style & GUI_STYLE_MENU_SEPARATOR) == 0 ) {
            WndChangeMenuAll( &menus->menu[i].child, on, bit );
            if( on ) {
                menus->menu[i].style |= bit;
            } else {
                menus->menu[i].style &= ~bit;
            }
        }
    }
}


static void MenuAll( a_window wnd, bool on, int bit )
{
    WndChangeMenuAll( &wnd->popup, on, bit );
}

void    WndMenuEnableAll( a_window wnd )
{
    MenuAll( wnd, false, GUI_STYLE_MENU_GRAYED );
}


void    WndMenuGrayAll( a_window wnd )
{
    MenuAll( wnd, true, GUI_STYLE_MENU_GRAYED );
}


void    WndMenuIgnoreAll( a_window wnd )
{
    MenuAll( wnd, true, GUI_STYLE_MENU_IGNORE );
}


void    WndMenuRespectAll( a_window wnd )
{
    MenuAll( wnd, false, GUI_STYLE_MENU_IGNORE );
}


static void DoMenuBitOn( gui_menu_items *menus, gui_ctl_id id, bool on, int bit )
{
    int     i;

    for( i = 0; i < menus->num_items; i++ ) {
        DoMenuBitOn( &menus->menu[i].child, id, on, bit );
        if( menus->menu[i].id == id ) {
            if( on ) {
                menus->menu[i].style |= bit;
            } else {
                menus->menu[i].style &= ~bit;
            }
        }
    }
}


static void MenuBitOn( a_window wnd, gui_ctl_id id, bool on, int bit )
{
    DoMenuBitOn( &wnd->popup, id, on, bit );
}

void    WndMenuCheck( a_window wnd, gui_ctl_id id, bool check )
{
    MenuBitOn( wnd, id, check, GUI_STYLE_MENU_CHECKED );
}


void    WndMenuEnable( a_window wnd, gui_ctl_id id, bool enable )
{
    MenuBitOn( wnd, id, !enable, GUI_STYLE_MENU_GRAYED );
}

void    WndMenuIgnore( a_window wnd, gui_ctl_id id, bool ignore )
{
    MenuBitOn( wnd, id, ignore, GUI_STYLE_MENU_IGNORE );
}


void WndCreateFloatingPopup( a_window wnd, gui_point *point, const gui_menu_items *menus, gui_ctl_id *last_popup )
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
    GUICreateFloatingPopup( wnd->gui, point, menus, GUI_TRACK_BOTH, last_popup );
}


void    WndInvokePopUp( a_window wnd, gui_point *point, const gui_menu_struct *menu )
{
    gui_ctl_id  dummy;

    if( WndSwitchOff( wnd, WSW_ALLOW_POPUP ) )
        return;
    WndMenuItem( wnd, MENU_INITIALIZE, WndMenuRow, WndMenuPiece );
    if( menu != NULL && WndNumPopups( wnd ) == 1 ) {
        if( menu->style & GUI_STYLE_MENU_GRAYED ) {
            Ring();
        } else {
            WndMenuItem( wnd, WndPopupMenu( wnd )[0].id, WndMenuRow, WndMenuPiece );
        }
    } else if( WndNumPopups( wnd ) > 0 ) {
        if( menu == NULL ) {
            WndCreateFloatingPopup( wnd, point, &wnd->popup, &wnd->last_popup );
        } else if( menu->style & GUI_STYLE_MENU_GRAYED ) {
            Ring();
        } else if( menu->child.menu == NULL ) {
            WndMenuItem( wnd, menu->id, WndMenuRow, WndMenuPiece );
        } else {
            WndCreateFloatingPopup( wnd, point, &menu->child, &dummy );
        }
    }
}


static void WndGetPopPoint( a_window wnd, gui_point *point )
{
    WndKeyPopItem( wnd, true );
    if( wnd->sel_start.row != WND_NO_ROW ) {
        WndCoordToGUIPoint( wnd, &wnd->sel_end, point );
    } else {
        WndCoordToGUIPoint( wnd, &wnd->current, point );
    }
}

void    WndKeyPopUp( a_window wnd, const gui_menu_struct *menu )
{
    gui_point           point;

    WndGetPopPoint( wnd, &point );
    SetWndMenuRow( wnd );
    WndInvokePopUp( wnd, &point, menu );
}


void    WndRowPopUp( a_window wnd, const gui_menu_struct *menu, wnd_row row, wnd_piece piece )
{
    gui_point           point;

    WndGetPopPoint( wnd, &point );
    WndMenuRow = WndScreenRow( wnd, row );
    WndMenuPiece = piece;
    WndInvokePopUp( wnd, &point, menu );
}


void    WndSetMainMenu( gui_menu_items *menus )
{
    if( WndMain != NULL ) {
        WndMainMenuPtr = menus;
        GUIResetMenus( WndMain->gui, menus );
    }
}

void WndClick( a_window wnd, gui_ctl_id id )
{
    if( !WndMainMenuProc( wnd, id ) ) {
        wnd = WndFindActive();
        if( wnd != NULL ) {
            WndMenuItem( wnd, id, WndMenuRow, WndMenuPiece );
        }
    }
}

static void WndSetPopupBits( a_window wnd, const gui_menu_struct *menu )
{
    int                 i;
    gui_menu_struct     *child;

    child = menu->child.menu;
    for( i = 0; i < menu->child.num_items; i++ ) {
        if( (child->style & GUI_STYLE_MENU_SEPARATOR) == 0 ) {
            GUIEnableMenuItem( WndMain->gui, child->id, wnd && (child->style & GUI_STYLE_MENU_GRAYED) == 0, false );
            GUICheckMenuItem( WndMain->gui, child->id, wnd && (child->style & GUI_STYLE_MENU_CHECKED) != 0, false );
            WndSetPopupBits( wnd, child );
        }
        child++;
    }
}


void WndSetPopup( gui_ctl_id id )
{
    a_window    wnd;

    if( WndMain != NULL && WndPopupMenuPtr != NULL ) {
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
}

void WndEnableMainMenu( gui_ctl_id id, bool enable )
{
    if( WndMain != NULL && WndMainMenuPtr != NULL ) {
        DoMenuBitOn( WndMainMenuPtr, id, !enable, GUI_STYLE_MENU_GRAYED );
        GUIEnableMenuItem( WndMain->gui, id, enable, false );
    }
}

void WndCheckMainMenu( gui_ctl_id id, bool check )
{
    if( WndMain != NULL && WndMainMenuPtr != NULL ) {
        DoMenuBitOn( WndMainMenuPtr, id, check, GUI_STYLE_MENU_CHECKED );
        GUICheckMenuItem( WndMain->gui, id, check, false );
    }
}

#if 0
// keep commented out until GUI supports this properly
void WndSetHintText( a_window wnd, gui_ctl_id id, char *text )
{
    GUISetMenuHintText( wnd->gui, id, text );
}
#endif
