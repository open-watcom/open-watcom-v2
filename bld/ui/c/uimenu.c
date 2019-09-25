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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uidef.h"
#include "uimenu.h"
#include "uibox.h"
#include "uishift.h"
#include "uigchar.h"

#include "clibext.h"


#define TABCHAR                 '\t'
#define TITLE_OFFSET            2
#define BETWEEN_TITLES          2

static  unsigned                BetweenTitles = BETWEEN_TITLES;

static  VBARMENU                MenuList;
static  VBARMENU                *Menu;

static  DESCMENU                Describe[MAX_MENUS];
static  int                     NumMenus = 0;

static  UI_WINDOW               BarWin;

static ui_event    menu_list[] = {
    EV_FIRST_EDIT_CHAR, EV_LAST_EDIT_CHAR,
    EV_ALT_Q,           EV_ALT_M,
    EV_SCROLL_PRESS,    EV_CAPS_RELEASE,
    __rend__,
    EV_MOUSE_PRESS,
    EV_MOUSE_DRAG,
    EV_MOUSE_RELEASE,
    EV_ESCAPE,
    EV_ENTER,
    EV_CURSOR_LEFT,
    EV_CURSOR_RIGHT,
    EV_CURSOR_DOWN,
    EV_ALT_PRESS,
    EV_ALT_RELEASE,
    EV_F10,
    __end__
};

static const char   *alt = "qwertyuiop\0\0\0\0asdfghjkl\0\0\0\0\0zxcvbnm";

static bool         InitMenuPopupPending = false;

void UIAPI uisetbetweentitles( unsigned between )
/***********************************************/
{
    BetweenTitles = between;
}

char UIAPI uialtchar( ui_event ui_ev )
/************************************/
{
    if( ( ui_ev >= EV_ALT_Q ) && ( ui_ev <= EV_ALT_M ) ) {
        return( alt[ui_ev - EV_ALT_Q] );
    } else {
        return( '\0' );
    }
}


static void mstring( BUFFER *bptr, ORD row, ORD col, ATTR attr, LPC_STRING string, uisize string_len )
/****************************************************************************************************/
{
    SAREA       area;

    bstring( bptr, row, col, attr, string, string_len );
    area.row = row;
    area.col = col;
    area.height = 1;
    area.width = string_len;
    physupdate( &area );
}

static void mfill( BUFFER *bptr, ORD row, ORD col, ATTR attr, char ch, uisize len, uisize height )
/************************************************************************************************/
{
    SAREA       area;

    area.row = row;
    area.col = col;
    area.width = len;
    area.height = height;
    for( ; height > 0; height-- ) {
        bfill( bptr, row, col, attr, ch, len );
        ++row;
    }
    physupdate( &area );
}

static void menutitle( int item, bool iscurritem )
/************************************************/
{
    DESCMENU            *desc;
    UIMENUITEM          *menuitem;
    ATTR                attr;
    ATTR                chattr;
    uisize              choffset;

    desc = &Describe[item];
    menuitem = &Menu->titles[item];
    if( MENUGRAYED( *menuitem ) ) {
        if( iscurritem ) {
            attr = UIData->attrs[ATTR_CURR_INACTIVE];
        } else {
            attr = UIData->attrs[ATTR_INACTIVE];
        }
        chattr = attr;
    } else {
        if( Menu->active ) {
            if( iscurritem ) {
                attr = UIData->attrs[ATTR_CURR_ACTIVE];
                chattr = UIData->attrs[ATTR_HOT_CURR];
            } else {
                attr = UIData->attrs[ATTR_ACTIVE];
                chattr = UIData->attrs[ATTR_HOT];
            }
        } else {
            attr = UIData->attrs[ATTR_ACTIVE];
            chattr = UIData->attrs[ATTR_HOT_QUIET];
        }
    }
    choffset = CHAROFFSET( *menuitem );
    mstring( &UIData->screen, MENU_GET_ROW( desc ), desc->titlecol + TITLE_OFFSET, attr, menuitem->name, desc->titlewidth );
    mstring( &UIData->screen, MENU_GET_ROW( desc ), desc->titlecol + TITLE_OFFSET + choffset, chattr, menuitem->name + choffset, 1 );
}

/*
 * GetNewPos -- calculate new position based on circular menu
 */

int intern GetNewPos( int pos, int num_items )
{
    if( pos >= num_items ) {
        return( 0 );
    } else if( pos < 0 ) {
        return( num_items - 1 );
    } else {
        return( pos );
    }
}

void UIAPI uidisplaymenuitem( UIMENUITEM *menuitem, DESCMENU *desc, int item, bool iscurritem )
/*********************************************************************************************/
{
    uisize                  choffset;
    uisize                  len;
    char                    ch[1];
    char                    *tab_loc;
    uisize                  tab_loc_len;
    ORD                     start_col;
    char                    *str;
    ATTR                    attr;
    ATTR                    chattr;
    uisize                  str_len;

    if( !MENUGRAYED( *menuitem ) && uiinlists( menuitem->event ) ) {    /* active item */
        if( iscurritem ) {
            attr = UIData->attrs[ATTR_CURR_ACTIVE];
            chattr = UIData->attrs[ATTR_HOT_CURR];
        } else {
            attr = UIData->attrs[ATTR_ACTIVE];
            chattr = UIData->attrs[ATTR_HOT];
        }
    } else {
        if( iscurritem ) {
            attr = UIData->attrs[ATTR_CURR_INACTIVE];
        } else {
            attr = UIData->attrs[ATTR_INACTIVE];
        }
        chattr = attr;
    }
    len = 0;
    if( desc->area.width > 2 )
        len = desc->area.width - 2;
    if( MENUSEPARATOR( *menuitem ) ) {
        /* separator line */
        ch[0] = BOX_CHAR( SBOX_CHARS(), LEFT_TACK );
        mstring( &UIData->screen,
                desc->area.row + 1 + item,
                desc->area.col,
                UIData->attrs[ATTR_MENU], ch, 1 );
        mfill( &UIData->screen,
                desc->area.row + 1 + item,
                desc->area.col + 1,
                UIData->attrs[ATTR_MENU],
                BOX_CHAR( SBOX_CHARS(), HORIZ_LINE ),
                len, 1 );
        ch[0] = BOX_CHAR( SBOX_CHARS(), RIGHT_TACK );
        mstring( &UIData->screen,
                desc->area.row + 1 + item,
                desc->area.col + len + 1,
                UIData->attrs[ATTR_MENU], ch, 1 );
    } else {
        /* blank line */
        mfill( &UIData->screen,
                desc->area.row + 1 + item,
                desc->area.col + 1,
                attr, ' ', len, 1 );
        start_col = desc->area.col;
        if( desc->flags & MENU_HAS_CHECK ) {
            start_col++;
            if( len > 0 ) {
                len--;
            }
        }
        /* checkmark */
        if( menuitem->flags & ITEM_CHECKED ) {
            mfill( &UIData->screen,
                    desc->area.row + 1 + item,
                    start_col,
                    attr, UiGChar[UI_CHECK_MARK], 1, 1 );
        }
        /* > for popup */
        if( menuitem->popup != NULL ) {
            mfill( &UIData->screen,
                    desc->area.row + 1 + item,
                    start_col + len,
                    attr, UiGChar[UI_POPUP_MARK], 1, 1 );
        }
        if( desc->flags & MENU_HAS_POPUP ) {
            if( len > 0 ) {
                len--;
            }
        }
        str = menuitem->name;
        if( str != NULL ) {
            tab_loc_len = 0;
            tab_loc = strchr( str, TABCHAR );
            if( tab_loc != NULL ) {
                tab_loc++;
                if( tab_loc != NULL ) {
                    tab_loc_len = strlen( tab_loc ) + 1;
                }
            }
            str_len = strlen( str ) - tab_loc_len;
            if( desc->flags & MENU_HAS_TAB ) {
                if( str_len > MENU_GET_TAB_OFFSET( desc ) ) {
                    str_len = MENU_GET_TAB_OFFSET( desc ) - 1;
                }
            }
            /* text */
            mstring( &UIData->screen,
                    desc->area.row + 1 + item,
                    start_col + 2,
                    attr, str, str_len );
            /* tabbed text */
            if( tab_loc != NULL ) {
                mstring( &UIData->screen,
                        desc->area.row + 1 + item,
                        start_col + MENU_GET_TAB_OFFSET( desc ) + 2,
                        attr, tab_loc, tab_loc_len );
            }
            /* short cut key */
            choffset = CHAROFFSET( *menuitem );
            mstring( &UIData->screen,
                    desc->area.row + 1 + item,
                    start_col + choffset + 2,
                    chattr, str + choffset, 1 );
        }
    }
}


void uidrawmenu( UIMENUITEM *menuitems, DESCMENU *desc, int curritem )
{
    int         i;

    forbid_refresh();
    if( desc->area.height > 0 ) {
        drawbox( &UIData->screen, desc->area, SBOX_CHARS(), UIData->attrs[ATTR_MENU], false );
        for( i = 0; i < desc->area.height - 2; i++ ) {
            uidisplaymenuitem( &menuitems[i], desc, i, ( i == curritem ) );
        }
    }
    permit_refresh();
}

void UIAPI uiclosepopup( UI_WINDOW *wptr )
{
    closewindow( wptr );
    wptr->update_func = NULL;
}

void UIAPI uiopenpopup( DESCMENU *desc, UI_WINDOW *wptr )
{
    wptr->area = desc->area;
    wptr->priority = P_DIALOGUE;
    wptr->update_func = NULL;
    wptr->update_parm = NULL;
    openwindow( wptr );
}

static bool process_menuchar( int ch, int *pmenu )
{
    int                 i;
    UIMENUITEM          *menuitem;
    bool                handled;
    int                 hotchar;

    ch = tolower( ch );
    handled = false;
    menuitem = Menu->titles;
    for( i = 0; i < NumMenus; i++ ) {
        if( !MENUSEPARATOR( *menuitem ) && !MENUGRAYED( *menuitem ) ) {
            hotchar = menuitem->name[CHAROFFSET( *menuitem )];
            if( tolower( hotchar ) == ch ) {
                *pmenu = i;
                Menu->popuppending = true;
                handled = true;
                break;
            }
        }
        menuitem++;
    }
    return( handled );
}

static ui_event createpopup( DESCMENU *desc, ui_event *new_ui_ev )
{
    ui_event    ui_ev;
    UIMENUITEM  *menuitems;
    SAREA       keep_inside;
    SAREA       return_exclude;

    ui_ev = EV_NO_EVENT;
    if( MENUGRAYED( Menu->titles[Menu->currmenu] ) ) {
        menuitems = NULL;
    } else {
        menuitems = Menu->titles[Menu->currmenu].popup;
    }
    if( menuitems != NULL ) {
        keep_inside.row = 0;
        keep_inside.col = 0;
        keep_inside.width = UIData->width;
        keep_inside.height = UIData->height;

        return_exclude.row = 0;
        return_exclude.col = desc->titlecol;
        return_exclude.width = desc->titlewidth + 2;
        return_exclude.height = 1;

        uimenudisable( true );

        *new_ui_ev = uicreatesubpopupinarea( menuitems, desc, true, false,
            menuitems[0].event, &keep_inside, &BarWin.area, &return_exclude );
        uimenudisable( false );

        switch( *new_ui_ev ) {
        case EV_CURSOR_RIGHT :
        case EV_CURSOR_LEFT :
        case EV_ALT_PRESS :
        case EV_ESCAPE :
        case EV_MOUSE_DRAG :
        case EV_MOUSE_DRAG_R :
        case EV_MOUSE_PRESS:
        case EV_MOUSE_PRESS_R:
        case EV_MOUSE_RELEASE:
        case EV_MOUSE_RELEASE_R:
            break;
        default :
            ui_ev = *new_ui_ev;
        }
    }
    return( ui_ev );
}


static ui_event process_menuevent( VSCREEN *vs, ui_event ui_ev )
/**************************************************************/
{
    int         i;
    int         oldmenu = NO_SELECT;
    ui_event    itemevent;
    ui_event    new_ui_ev;
    DESCMENU    *desc;
    int         menu;
    bool        select;
    ORD         mouserow;
    ORD         mousecol;
    bool        mouseon;

    new_ui_ev = ui_ev;
    if( iskeyboardchar( ui_ev ) ) {
        /* this allows alt numeric keypad stuff to not activate the menus */
        Menu->altpressed = false;
    }
    if( !isdialogue( vs ) ) {
        desc = &Describe[Menu->currmenu];
        new_ui_ev = EV_NO_EVENT; /* Moved here from "else" case below */
        if( Menu->popuppending ) {
            Menu->popuppending = false;
            itemevent = createpopup( desc, &ui_ev );
        } else {
            itemevent = EV_NO_EVENT;
        }
        if( Menu->active ) {
            oldmenu = menu = Menu->currmenu;
        } else {
            oldmenu = menu = NO_SELECT;
        }
        select = false;
        if( ui_ev == EV_ALT_PRESS && !Menu->ignorealt ) {
            Menu->altpressed = true;
        } else if( ui_ev == EV_ALT_RELEASE && Menu->altpressed ) {
            if( Menu->active ) {
                menu = NO_SELECT;
            } else {
                menu = 0;
                desc = &Describe[0];
            }
            Menu->altpressed = false;
        } else if( ui_ev == EV_F10 && UIData->f10menus ) {
            menu = 0;
            desc = &Describe[0];
        } else if( ui_ev == EV_MOUSE_PRESS_R || ui_ev == EV_MOUSE_PRESS_M  ) {
            new_ui_ev = ui_ev;
            menu = NO_SELECT;
            Menu->draginmenu = false;
        } else if( ( ui_ev == EV_MOUSE_PRESS ) ||
            ( ui_ev == EV_MOUSE_DRAG ) ||
            ( ui_ev == EV_MOUSE_REPEAT ) ||
            ( ui_ev == EV_MOUSE_RELEASE ) ||
            ( ui_ev == EV_MOUSE_DCLICK ) ) {
            uigetmouse( &mouserow, &mousecol, &mouseon );
            if( ( mouserow < uimenuheight() ) &&
                ( Menu->active ||
                  ui_ev == EV_MOUSE_PRESS || ui_ev == EV_MOUSE_DCLICK ||
                  ui_ev == EV_MOUSE_DRAG || ui_ev == EV_MOUSE_REPEAT ) ) {
                if( ui_ev == EV_MOUSE_DCLICK ) {
                    ui_ev = EV_MOUSE_PRESS;
                }
                menu = NO_SELECT;
                desc = Describe;
                for( i = 0; i < NumMenus; i++ ) {
                    if( ( MENU_GET_ROW( desc ) == mouserow ) && ( desc->titlecol <= mousecol )
                      && ( mousecol < desc->titlecol + desc->titlewidth + 2 ) ) {
                        Menu->draginmenu = true;
                        Menu->popuppending = true;
                        menu = i;
                        break;
                    }
                    desc++;
                }
            } else if( Menu->active || Menu->draginmenu ) {
                if( ( desc->area.col < mousecol )
                    && ( mousecol < desc->area.col + desc->area.width - 1 )
                    && ( mouserow < desc->area.row + desc->area.height - 1 )
                    && ( desc->area.row <= mouserow ) ) {
                    Menu->movedmenu = true;
                } else if( ui_ev == EV_MOUSE_PRESS  ) {
                    new_ui_ev = ui_ev;
                    menu = NO_SELECT;
                    Menu->draginmenu = false;
                } else if( ui_ev == EV_MOUSE_RELEASE ) {
                    menu = NO_SELECT;
                    Menu->draginmenu = false;
                }
            } else {
                new_ui_ev = ui_ev;
            }
            if( ui_ev != EV_MOUSE_RELEASE && menu != oldmenu ) {
                Menu->movedmenu = true;
            }
            if( ui_ev == EV_MOUSE_RELEASE ) {
                if( !Menu->movedmenu ) {
                    menu = NO_SELECT;
                } else {
                    select = true;
                }
                Menu->movedmenu = false;
            }
        } else if( uialtchar( ui_ev ) != '\0'  ) {
            if( process_menuchar( uialtchar( ui_ev ), &menu ) ) {
                desc = &Describe[menu];
                select = ( desc->area.height == 0 );
            }
            new_ui_ev = EV_NO_EVENT;
        } else if( Menu->active ) {
            switch( ui_ev ) {
            case EV_ESCAPE :
                menu = NO_SELECT;
                break;
            case EV_ENTER :
                if( menu != NO_SELECT ) {
                    Menu->popuppending = true;
                }
                break;
            case EV_CURSOR_LEFT :
                menu = GetNewPos( menu - 1, NumMenus );
                Menu->popuppending = true;
                desc = &Describe[menu];
                break;
            case EV_CURSOR_RIGHT :
                menu = GetNewPos( menu + 1, NumMenus );
                Menu->popuppending = true;
                desc = &Describe[menu];
                break;
            case EV_CURSOR_DOWN :
                Menu->popuppending = true;
                break;
            case EV_NO_EVENT :
                break;
            default :
                if( iseditchar( ui_ev ) ) {
                    if( process_menuchar( (unsigned char)ui_ev, &menu ) ) {
                        desc = &Describe[menu];
                        select = ( desc->area.height == 0 );
                        break;
                    }
                }
                if( itemevent != EV_NO_EVENT ) {
                    new_ui_ev = itemevent;
                    select = true;
                } else {
                    new_ui_ev = ui_ev;
                }
            }
        } else {
            new_ui_ev = ui_ev;
        }
        if( menu != oldmenu ) {
            if( menu != NO_SELECT && !Menu->active ) {
                new_ui_ev = EV_MENU_ACTIVE;
            }
            Menu->active = ( menu != NO_SELECT );
            if( oldmenu != NO_SELECT ) {
                menutitle( oldmenu, false );
            }
            if( menu != NO_SELECT ) {
                Menu->currmenu = menu;
                menutitle( menu, true );
            }
            if( menu == NO_SELECT || oldmenu == NO_SELECT ) {
                uimenutitlebar();
            }
        }
        if( Menu->active ) {
            if( itemevent == EV_NO_EVENT ) {
                if( MENUGRAYED( Menu->titles[menu] ) )  {
                    Menu->popuppending = false;
                } else {
                    itemevent = Menu->titles[menu].event;
                }
            }
            Menu->event = itemevent;
            if( select ) {
                new_ui_ev = Menu->event;
                Menu->active = false;
                uimenutitlebar();
            }
        }
    }
    if( ui_ev == EV_MOUSE_RELEASE ) {
        Menu->draginmenu = false;
    }
    if( Menu->ignorealt ) {
        Menu->ignorealt = false;
    }
    if( ( !Menu->active && ( oldmenu != NO_SELECT ) ) ||
        ( Menu->active && ( oldmenu != Menu->currmenu ) ) ) {
        if( ( Menu->currmenu != NO_SELECT ) && Menu->active ) {
            uimenucurr( &Menu->titles[Menu->currmenu] );
        } else {
            /* no current menu */
            uimenucurr( NULL );
        }
    }

    if( Menu->popuppending ) {
        InitMenuPopupPending = true;
    }

    return( new_ui_ev );
}

#if 0
ui_event uigeteventfrompos( ORD row, ORD col )
/********************************************/
{
    int                 i;
    DESCMENU*           desc;

    if( row < uimenuheight() ) {
        for( i = 0; i < NumMenus; i++ ) {
            desc = &Describe[i];
            if( ( MENU_GET_ROW( desc ) == row ) &&
                ( desc->titlecol <= col ) &&
                ( col < desc->titlecol + desc->titlewidth + 2 ) ) {
                return( Menu->event );
            }
        }
    }
    return( EV_NO_EVENT );
}
#endif

ui_event intern menuevent( VSCREEN *vs )
/**************************************/
{
    ui_event        new_ui_ev;
    ui_event        ui_ev;

    new_ui_ev = EV_NO_EVENT;

    if( InitMenuPopupPending ) {
        InitMenuPopupPending = false;
        if( Menu->titles[Menu->currmenu].popup != NULL ) {
            new_ui_ev = EV_MENU_INITPOPUP;
        }
    }

    if( new_ui_ev == EV_NO_EVENT ) {
        if( uimenuson() && !uimenuisdisabled() ) {
            uipushlist( menu_list );
            if( !Menu->active || isdialogue( vs ) ) {
                ui_ev = getprime( vs );
            } else {
                ui_ev = getprime( NULL );
            }
            switch( ui_ev ) {
            case EV_SCROLL_PRESS:
                Menu->scroll = true;
                break;
            case EV_SCROLL_RELEASE:
                Menu->scroll = false;
                break;
            case EV_NUM_PRESS:
                Menu->num = true;
                break;
            case EV_NUM_RELEASE:
                Menu->num = false;
                break;
            case EV_CAPS_PRESS:
                Menu->caps = true;
                break;
            case EV_CAPS_RELEASE:
                Menu->caps = false;
                break;
            default:
                new_ui_ev = process_menuevent( vs, ui_ev );
            }
            uipoplist( /* menu_list */ );
        } else {
            new_ui_ev = getprime( vs );
        }
    }

    return( new_ui_ev );
}


void UIAPI uidescmenu( UIMENUITEM *menuitems, DESCMENU *desc )
/************************************************************/
{
    int                 i;
    uisize              len;
    char*               tab_loc;
    uisize              tab_loc_len;
    uisize              to_add;

    desc->flags = 0;
    if( menuitems != NULL ) {
        desc->area.width = 0;
        tab_loc_len = 0;
        for( i = 0; !MENUENDMARKER( menuitems[i] ); i++ ) {
            if( !MENUSEPARATOR( menuitems[i] ) ) {
                len = strlen( menuitems[i].name );
                tab_loc = strchr( menuitems[i].name, TABCHAR );
                if( tab_loc != NULL ) {
                    desc->flags |= MENU_HAS_TAB;
                    tab_loc++;
                    if( tab_loc != NULL ) {
                        if( tab_loc_len < strlen( tab_loc ) )
                            tab_loc_len = strlen( tab_loc );
                        len -= strlen( tab_loc ); /* for text after TABCHAR */
                    }
                    len--;  /* for TABCHAR */
                }
                if( menuitems[i].flags & ITEM_CHECKED ) {
                    desc->flags |= MENU_HAS_CHECK;
                }
                if( menuitems[i].popup != NULL ) {
                    desc->flags |= MENU_HAS_POPUP;
                }
                if( desc->area.width < len ) {
                    desc->area.width = len;
                }
            }
        }
        to_add = 0;
        if( desc->flags & MENU_HAS_TAB ) {
            to_add += tab_loc_len + 1;
        }
        if( desc->flags & MENU_HAS_POPUP ) {
            to_add++;
        }
        if( desc->flags & MENU_HAS_CHECK ) {
            to_add++;
        }
        to_add += 4;
        if( desc->area.width > UIData->width - to_add )
            desc->area.width = UIData->width - to_add;
        MENU_SET_TAB_OFFSET( desc, desc->area.width + 1 );
        desc->area.width += to_add;
        desc->area.height = i + 2;
        if( desc->area.col + desc->area.width >= UIData->width ) {
            desc->area.col = UIData->width - desc->area.width;
        }
    } else {
        desc->area.height = 0;
    }
}

void UIAPI uimenutitlebar( void )
/*******************************/
{
    int         i;

    forbid_refresh();
    for( i = 0; i < NumMenus; i++ ) {
        menutitle( i, ( i == Menu->currmenu ) );
    }
    permit_refresh();
}

static void drawbar_update_fn( SAREA area, void *dummy )
/******************************************************/
{
    /* unused parameters */ (void)dummy;

    forbid_refresh();
    if( area.row < uimenuheight() ) {
        mfill( &UIData->screen, area.row, 0, UIData->attrs[ATTR_ACTIVE], ' ', UIData->width, area.height );
        uimenutitlebar();
    }
    permit_refresh();
}

bool UIAPI uienablepopupitem( int menuitem, int popupitem, bool enable )
/**********************************************************************/
{
    bool        prev;
    UIMENUITEM  *pitem;

    pitem = &Menu->titles[menuitem].popup[popupitem];
    prev = !MENUGRAYED( *pitem );
    if( enable ) {
        pitem->flags &= ~ITEM_GRAYED;
    } else {
        pitem->flags |= ITEM_GRAYED;
    }
    return( prev );
}


void UIAPI uimenuindicators( bool status )
/****************************************/
{
    Menu->indicators = status;
}

void UIAPI uisetmenudesc( void )
/******************************
 * this code facilitates the updating of menu's without constant redrawing
 */
{
    int                     i;
    uisize                  col_start;
    uisize                  col_end;
    uisize                  row;
    DESCMENU                *desc;
    UIMENUITEM              *menuitem;

    if( NumMenus > 0 ) {
        row = 1;
        col_end = col_start = 0;
        desc = Describe;
        menuitem = Menu->titles;
        for( i = 0; i < NumMenus; i++ ) {
            if( i > 0 ) {
                col_start = col_end;
                col_end += desc->titlewidth + BetweenTitles;
                if( col_end >= UIData->width ) {
                    col_start = 0;
                    row++;
                }
                desc++;
            }
            desc->area.row = row;
            desc->area.col = col_start;
            desc->titlecol = col_start;
            desc->titlewidth = ( menuitem->name != NULL ) ? strlen( menuitem->name ) : 0;
            col_end = col_start + desc->titlewidth + BetweenTitles;
            uidescmenu( menuitem->popup, desc );
            menuitem++;
        }
    }
}

int UIAPI uimenuitemscount( UIMENUITEM *menuitems )
/*************************************************/
{
    int         count;

    count = 0;
    if( menuitems != NULL ) {
        while( !MENUENDMARKER( menuitems[count] ) ) {
            count++;
        }
    }
    return( count );
}

VBARMENU * UIAPI uimenubar( VBARMENU *bar )
/*****************************************/
{
    VBARMENU    *prevMenu;

    if( NumMenus > 0 ) {
        closewindow( &BarWin );
        NumMenus = 0;
    }
    prevMenu = Menu;
    Menu = bar;
    /* resetting old_shift is a bit kludgy but it's either this or */
    /* rewrite a bunch of code that somebody else wrote - yuk      */
    /* UIData->old_shift = 0;                                      */
    if( Menu != NULL ) {
        Menu->active = false;
        Menu->draginmenu = false;
        Menu->indicators = true;
        Menu->altpressed = false;
        Menu->ignorealt = false;
        Menu->movedmenu = false;
        Menu->popuppending = false;
        Menu->disabled = false;
        NumMenus = uimenuitemscount( Menu->titles );
        if( NumMenus > MAX_MENUS )
            NumMenus = MAX_MENUS;
        uisetmenudesc();
        BarWin.area.row = 0;
        BarWin.area.col = 0;
        BarWin.area.height = uimenuheight();
        BarWin.area.width = UIData->width;
        BarWin.priority = P_MENU;
        BarWin.update_func = drawbar_update_fn;
        BarWin.update_parm = NULL;
        openwindow( &BarWin );
        InitMenuPopupPending = false;
    }
    return( prevMenu );
}

bool UIAPI uimenuson( void )
/**************************/
{
    return( Menu != NULL );
}

unsigned UIAPI uimenuheight( void )
/*********************************/
{
    if( NumMenus > 0 )
        return( MENU_GET_ROW( &Describe[NumMenus - 1] ) + 1 );
    return( 0 );
}

void UIAPI uimenudisable( bool disabled )
/***************************************/
{
    if( uimenuson() ) {
        Menu->disabled = disabled;
    }
}

bool UIAPI uimenuisdisabled( void )
/*********************************/
{
    return( uimenuson() && Menu->disabled );
}

bool UIAPI uimenugetaltpressed( void )
/************************************/
{
    return( uimenuson() && Menu->altpressed );
}

void UIAPI uimenusetaltpressed( bool altpressed )
/***********************************************/
{
    if( uimenuson() ) {
        Menu->altpressed = altpressed;
    }
}

void UIAPI uinomenus( void )
/***************************/
{
    uimenubar( NULL );
}


void UIAPI uimenus( UIMENUITEM *menuitems, UIMENUITEM **popupitems, ui_event hot )
/********************************************************************************/
{
    int             i;

    /* unused parameters */ (void)hot;

    uimenubar( NULL );
    for( i = 0; !MENUENDMARKER( menuitems[i] ); i++ ) {
        menuitems[i].popup = popupitems[i];
    }
    MenuList.titles = menuitems;
    MenuList.currmenu = 0;
    uimenubar( &MenuList );
}

void UIAPI uiactivatemenus( void )
/*********************************/
{
    if( Menu != NULL ) {
        if( !Menu->active ) {
            Menu->altpressed = true;
            process_menuevent( NULL, EV_ALT_RELEASE );
        }
    }
}

void UIAPI uiignorealt( void )
/*****************************/
{
    if( Menu != NULL ) {
        Menu->ignorealt = true;
    }
}

bool UIAPI uigetcurrentmenu( UIMENUITEM *currmenuitem )
/*****************************************************/
{
    if( Menu->currmenu != NO_SELECT ) {
        *currmenuitem = Menu->titles[Menu->currmenu];
    }
    return( Menu->currmenu != NO_SELECT );
}
