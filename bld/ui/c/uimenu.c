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


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uidef.h"
#include "uimenu.h"
#include "uibox.h"
#include "uishift.h"
#include "uigchar.h"

#include "clibext.h"


#define         TABCHAR                 '\t'
#define         TITLE_OFFSET            2
#define         BETWEEN_TITLES          2

static          int                     BetweenTitles = BETWEEN_TITLES;

extern          EVENT                   Event;

static          VBARMENU                MenuList;
static          VBARMENU*               Menu;

static          DESCMENU                Describe[ MAX_MENUS ];
static          int                     NumMenus        = 0;

static          UI_WINDOW               BarWin;

static          EVENT                   menu_list[]      = {
                EV_FIRST_EDIT_CHAR, EV_LAST_EDIT_CHAR,
                EV_ALT_Q, EV_ALT_M,
                EV_SCROLL_PRESS, EV_CAPS_RELEASE,
                EV_NO_EVENT,
                EV_MOUSE_PRESS,
                EV_MOUSE_DRAG,
                EV_MOUSE_RELEASE,
                EV_ESCAPE,
                EV_RETURN,
                EV_CURSOR_LEFT,
                EV_CURSOR_RIGHT,
                EV_CURSOR_DOWN,
                EV_ALT_PRESS,
                EV_ALT_RELEASE,
                EV_FUNC(10),
                EV_NO_EVENT
};

static          char*                   alt             =
        "qwertyuiop\0\0\0\0asdfghjkl\0\0\0\0\0zxcvbnm";

static          bool                    InitMenuPopupPending = FALSE;

extern void uisetbetweentitles( int between )
{
    BetweenTitles = between;
}

extern char uialtchar( EVENT ev )
/*******************************/
{
    if( ( ev >= EV_ALT_Q ) && ( ev <= EV_ALT_M ) ) {
        return( alt[ ev - EV_ALT_Q ] );
    } else {
        return( '\0' );
    }
}


static void mstring( BUFFER *bptr, ORD row, ORD col, ATTR attr,
                             LPC_STRING string, int len )
/**************************************************************/
{
    SAREA       area;

    bstring( bptr, row, col, attr, string, len );
    area.row = row;
    area.col = col;
    area.height = 1;
    area.width = len;
    physupdate( &area );
}

static void mfill( BUFFER *bptr, ORD row, ORD col, ATTR attr,
                               char ch, int len, int height )
/***********************************************************/
{
    SAREA       area;

    area.row = row;
    area.col = col;
    area.width = len;
    area.height = height;
    while( height != 0 ) {
        bfill( bptr, row, col, attr, ch, len );
        ++row;
        --height;
    }
    physupdate( &area );
}

static void menutitle( int menu, bool current )
/*********************************************/
{
    register    DESCMENU*               desc;
    register    MENUITEM*               mptr;
    register    ATTR                    attr;
    register    ATTR                    chattr;

    desc = &Describe[ menu - 1 ];
    mptr = &Menu->titles[ menu - 1 ];
    if( MENUGRAYED(*mptr) ) {
        if( current ) {
            attr = UIData->attrs[ ATTR_CURR_INACTIVE ];
        } else {
            attr = UIData->attrs[ ATTR_INACTIVE ];
        }
        chattr = attr;
    } else {
        if( Menu->active ){
            if( current ){
                attr = UIData->attrs[ ATTR_CURR_ACTIVE ];
                chattr = UIData->attrs[ ATTR_HOT_CURR ];
            } else {
                attr = UIData->attrs[ ATTR_ACTIVE ];
                chattr = UIData->attrs[ ATTR_HOT ];
            }
        } else {
            attr = UIData->attrs[ ATTR_ACTIVE ];
            chattr = UIData->attrs[ ATTR_HOT_QUIET ];
        }
    }
    mstring( &UIData->screen, MENU_GET_ROW( desc ), desc->titlecol + TITLE_OFFSET,
             attr, mptr->name, desc->titlewidth );
    mstring( &UIData->screen, MENU_GET_ROW( desc ),
             desc->titlecol + TITLE_OFFSET + ( mptr->flags & ITEM_CHAR_OFFSET ),
             chattr, &mptr->name[ ( mptr->flags & ITEM_CHAR_OFFSET ) ], 1 );
}

void UIAPI uidisplayitem( MENUITEM *menu, DESCMENU *desc, int item, bool curr )
/******************************************************************************/
{
    bool                    active;
    ORD                     choffset;
    int                     len;
    char                    ch;
    char*                   tab_loc;
    int                     tab_len;
    ORD                     start_col;
    char*                   str;
    ATTR                    attr;
    ATTR                    chattr;
    int                     str_len;

    active = !MENUGRAYED(*menu) && uiinlist( menu->event );
    if( active ) {
        if( curr ) {
            attr = UIData->attrs[ ATTR_CURR_ACTIVE ];
            chattr = UIData->attrs[ ATTR_HOT_CURR ];
        } else {
            attr = UIData->attrs[ ATTR_ACTIVE ];
            chattr = UIData->attrs[ ATTR_HOT ];
        }
    } else {
        if( curr ) {
            attr = UIData->attrs[ ATTR_CURR_INACTIVE ];
        } else {
            attr = UIData->attrs[ ATTR_INACTIVE ];
        }
        chattr = attr;
    }
    if( item > 0 ) {
        len = desc->area.width - 2;
        str = menu->name;
        if( MENUSEPARATOR( *menu ) ) {
            ch = UiGChar[ UI_SBOX_LEFT_TACK ];
            mstring( &UIData->screen,
                    (ORD) desc->area.row + item,
                    (ORD) desc->area.col,
                     UIData->attrs[ATTR_MENU], &ch, 1 );
            mfill( &UIData->screen,
                    (ORD) desc->area.row + item,
                    (ORD) desc->area.col + 1,
                    UIData->attrs[ATTR_MENU],
                    UiGChar[ UI_SBOX_HORIZ_LINE ],
                    len, 1 );
            ch = UiGChar[ UI_SBOX_RIGHT_TACK ];
            mstring( &UIData->screen,
                    (ORD) desc->area.row + item,
                    (ORD) desc->area.col + len + 1,
                    UIData->attrs[ATTR_MENU], &ch, 1 );
        } else {
            if( len < 0 ) {
                len = 0;
            }
            choffset = ( menu->flags & ITEM_CHAR_OFFSET );
            mfill( &UIData->screen,                     /* blank line */
                    (ORD) desc->area.row + item,
                    (ORD) desc->area.col + 1,
                    attr, ' ', len, 1 );
            if( desc->flags & MENU_HAS_CHECK ) {
                start_col = desc->area.col + 1;
                len--;
            } else {
                start_col = desc->area.col;
            }
            if( menu->flags & ITEM_CHECKED ) {
                mfill( &UIData->screen,                 /* checkmark */
                       (ORD) desc->area.row + item,
                       (ORD) start_col,
                       attr, UiGChar[ UI_CHECK_MARK], 1, 1 );
            }
            if( menu->popup != NULL ) {
                mfill( &UIData->screen,                 /* > for popup */
                       (ORD) desc->area.row + item,
                       (ORD) start_col + len,
                       attr, UiGChar[ UI_POPUP_MARK], 1, 1 );
            }
            if( desc->flags & MENU_HAS_POPUP ) {
                len--;
            }
            if( str != NULL ) {
                tab_loc = strchr( str, TABCHAR );
                if( tab_loc != NULL ) {
                    tab_loc++;
                    if( tab_loc != NULL ) {
                        tab_len = strlen( tab_loc ) + 1;
                    } else {
                        tab_len = 0;
                    }
                } else {
                    tab_len = 0;
                }
                str_len = strlen( str ) - tab_len;
                if( desc->flags & MENU_HAS_TAB ) {
                    if( str_len > TAB_OFFSET( desc ) ) {
                        str_len = TAB_OFFSET( desc ) - 1;
                    }
                }
                /* text */
                mstring( &UIData->screen, (ORD) desc->area.row + item,
                         (ORD) start_col + 2, attr, str, str_len );
                if( tab_loc != NULL ) {
                    mstring( &UIData->screen,           /* tabbed text */
                             (ORD) desc->area.row + item,
                             (ORD) start_col + TAB_OFFSET( desc ) + 2,
                             attr, tab_loc, tab_len );
                }
                mstring( &UIData->screen,               /* short cut key */
                         (ORD) desc->area.row + item,
                         (ORD) start_col + choffset + 2,
                         chattr, &str[choffset], 1 );
            }
        }
    }
}


extern void uidrawmenu( MENUITEM *menu, DESCMENU *desc, int curr )
{
    register    int             item;

    forbid_refresh();
    if( desc->area.height > 0 ) {
        drawbox( &UIData->screen, desc->area, (char *)&UiGChar[ UI_SBOX_TOP_LEFT ],
                 UIData->attrs[ATTR_MENU], FALSE );
        for( item = 1 ; item < desc->area.height - 1 ; ++item ) {
            uidisplayitem( &menu[ item - 1 ], desc, item, item == curr );
        }
    }
    permit_refresh();
}

void UIAPI uiclosepopup( UI_WINDOW *window )
{
    closewindow( window );
    window->update = NULL;
}

void UIAPI uiopenpopup( DESCMENU *desc, UI_WINDOW *window )
{
    window->area = desc->area;
    window->priority = P_DIALOGUE;
    window->update = NULL;
    window->parm = NULL;
    openwindow( window );
}

static int process_char( int ch, DESCMENU **desc, int *menu, bool *select )
{
    register    int                     index;
    register    MENUITEM*               itemptr;
    register    int                     handled;
    register    int                     hotchar;

    ch = tolower( ch );
    handled = FALSE;
    itemptr = Menu->titles;
    for( index = 0 ; !MENUENDMARKER( itemptr[ index ] ); ++index ) {
        if( !MENUSEPARATOR( itemptr[index] ) &&
            !MENUGRAYED ( itemptr[index] ) ) {
            hotchar = (itemptr[ index ].name)
                       [ ( itemptr[ index ].flags & ITEM_CHAR_OFFSET ) ];
            if( tolower( hotchar ) == ch ) {
                *desc = &Describe[ index ];
                *menu = index + 1;
                *select = ( (*desc)->area.height == 0 );
                Menu->popuppending = TRUE;
                handled = TRUE;
                break;
            }
        }
    }
    return( handled );
}

static EVENT createpopup( DESCMENU *desc, EVENT *newevent )
{
    EVENT       itemevent;
    MENUITEM    *curr_menu;
    SAREA       keep_inside;
    SAREA       return_exclude;

    itemevent = EV_NO_EVENT;
    if( MENUGRAYED(Menu->titles[ Menu->menu - 1]) ) {
        curr_menu = NULL;
    } else {
        curr_menu = Menu->titles[ Menu->menu - 1].popup;
    }
    if( curr_menu != NULL ) {
        keep_inside.row = 0;
        keep_inside.col = 0;
        keep_inside.width = UIData->width;
        keep_inside.height = UIData->height;

        return_exclude.row = 0;
        return_exclude.col = desc->titlecol;
        return_exclude.width = desc->titlewidth + 2;
        return_exclude.height = 1;

        uimenudisable( TRUE );

        *newevent = uicreatesubpopupinarea( curr_menu, desc, TRUE, FALSE,
                                            curr_menu[0].event, &keep_inside,
                                            &BarWin.area, &return_exclude );
        uimenudisable( FALSE );

        switch( *newevent ) {
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
            itemevent = *newevent;
        }
    }
    return( itemevent );
}


static EVENT intern process_menuevent( VSCREEN *vptr, EVENT ev )
/**************************************************************/
{
    register    int                     index;
    register    int                     oldmenu = 0;
    register    EVENT                   itemevent;
    auto        EVENT                   newevent;
    auto        DESCMENU*               desc;
    auto        int                     menu;
    auto        bool                    select;
    auto        ORD                     mouserow;
    auto        ORD                     mousecol;
    auto        int                     mouseon;

    newevent = ev;
    if( ev > EV_NO_EVENT && ev <= EV_LAST_KEYBOARD ){
        /* this allows alt numeric keypad stuff to not activate the menus */
        Menu->altpressed = FALSE;
    }
    if( isdialogue( vptr ) == FALSE ) {
        if( NumMenus > 0 ) {
            desc = &Describe[ Menu->menu - 1 ];
            newevent = EV_NO_EVENT; /* Moved here from "else" case below */
            if( Menu->popuppending ) {
                Menu->popuppending = FALSE;
                itemevent = createpopup( desc, &ev );
            } else {
                itemevent = EV_NO_EVENT;
            }
            if( Menu->active ) {
                oldmenu = menu = Menu->menu;
            } else {
                oldmenu = menu = 0;
            }
            select = FALSE;
            if( ev == EV_ALT_PRESS && !Menu->ignorealt ){
                Menu->altpressed = TRUE;
            } else if( ev == EV_ALT_RELEASE && Menu->altpressed ){
                if( Menu->active ){
                    menu = 0;
                } else {
                    desc = &Describe[ 0 ];
                    menu = 1;
                }
                Menu->altpressed = FALSE;
            } else if( ev == EV_FUNC( 10 ) && UIData->f10menus ){
                desc = &Describe[ 0 ];
                menu = 1;
            } else if( ev == EV_MOUSE_PRESS_R  ||
                       ev == EV_MOUSE_PRESS_M  ){
                newevent = ev;
                menu = 0;
                Menu->draginmenu = FALSE;
            } else if( ( ev == EV_MOUSE_PRESS ) ||
                ( ev == EV_MOUSE_DRAG ) ||
                ( ev == EV_MOUSE_REPEAT ) ||
                ( ev == EV_MOUSE_RELEASE ) ||
                ( ev == EV_MOUSE_DCLICK ) ) {
                uigetmouse( &mouserow, &mousecol, &mouseon );
                if( ( mouserow < uimenuheight() ) &&
                    ( Menu->active  ||
                      ev == EV_MOUSE_PRESS  || ev == EV_MOUSE_DCLICK  ||
                      ev == EV_MOUSE_DRAG || ev == EV_MOUSE_REPEAT ) ) {
                    if( ev == EV_MOUSE_DCLICK ) {
                        ev = EV_MOUSE_PRESS;
                    }
                    menu = 0;
                    for( index = 0 ; !MENUENDMARKER( Menu->titles[ index ] ); ++index ) {
                        desc = &Describe[ index ];
                        if( ( MENU_GET_ROW( desc ) == mouserow ) &&
                            ( desc->titlecol <= mousecol ) &&
                            ( mousecol < desc->titlecol + desc->titlewidth + 2 ) ) {
                            Menu->draginmenu = TRUE;
                            Menu->popuppending = TRUE;
                            menu = index + 1;
                            break;
                        }
                    }
                } else if( Menu->active || Menu->draginmenu ) {
                    if( ( desc->area.col < mousecol )
                        && ( mousecol < desc->area.col + desc->area.width - 1 )
                        && ( mouserow < desc->area.row + desc->area.height - 1 )
                        && ( desc->area.row <= mouserow ) ) {
                        Menu->movedmenu = TRUE;
                    } else if( ev == EV_MOUSE_PRESS  ) {
                        newevent = ev;
                        menu = 0;
                        Menu->draginmenu = FALSE;
                    } else if( ev == EV_MOUSE_RELEASE ) {
                        menu = 0;
                        Menu->draginmenu = FALSE;
                    }
                } else {
                    newevent = ev;
                }
                if( ev != EV_MOUSE_RELEASE && menu != oldmenu ){
                    Menu->movedmenu = TRUE;
                }
                if( ev == EV_MOUSE_RELEASE ) {
                    if( !Menu->movedmenu ) {
                        menu = 0;
                    } else {
                        select = TRUE;
                    }
                    Menu->movedmenu = FALSE;
                }
            } else if( uialtchar( ev ) != '\0'  ) {
                process_char( uialtchar( ev ), &desc, &menu, &select );
                newevent = EV_NO_EVENT;
            } else if( Menu->active ) {
                switch( ev ) {
                case EV_ESCAPE :
                    menu = 0;
                    break;
                case EV_RETURN :
                    if( menu > 0 ) {
                        Menu->popuppending = TRUE;
                    }
                    break;
                case EV_CURSOR_LEFT :
                    menu -= 1;
                    if( menu == 0 ){
                        menu = NumMenus;
                    }
                    Menu->popuppending = TRUE;
                    desc = &Describe[ menu - 1 ];
                    break;
                case EV_CURSOR_RIGHT :
                    menu += 1;
                    if( menu > NumMenus ){
                        menu = 1;
                    }
                    Menu->popuppending = TRUE;
                    desc = &Describe[ menu - 1 ];
                    break;
                case EV_CURSOR_DOWN :
                    Menu->popuppending = TRUE;
                    break;
                case EV_NO_EVENT :
                    break;
                default :
                    if( ev <= EV_LAST_KEYBOARD ) {
                        if( process_char( ev, &desc, &menu, &select ) ) {
                            break;
                        }
                    }
                    if( itemevent != EV_NO_EVENT ) {
                        newevent = itemevent;
                        select = TRUE;
                    } else {
                        newevent = ev;
                    }
                }
            } else {
                newevent = ev;
            }
            if( menu != oldmenu ) {
                if( menu > 0 && !Menu->active ) {
                    newevent = EV_MENU_ACTIVE;
                }
                Menu->active = ( menu > 0 );
                if( oldmenu > 0 ) {
                    menutitle( oldmenu, FALSE );
                }
                if( menu > 0 ) {
                    Menu->menu = menu;
                    menutitle( menu, TRUE );
                }
                if( menu == 0 || oldmenu == 0 ) {
                    uimenutitlebar();
                }
            }
            if( Menu->active ) {
                if( itemevent == EV_NO_EVENT ) {
                    if( MENUGRAYED(Menu->titles[ menu-1]) )  {
                        Menu->popuppending = FALSE;
                    } else {
                        itemevent = Menu->titles[ menu-1 ].event;
                    }
                }
                Menu->event = itemevent;
                if( select ) {
                    newevent = Menu->event;
                    Menu->active = FALSE;
                    uimenutitlebar();
                }
            }
        }
    }
    if( ev == EV_MOUSE_RELEASE ){
        Menu->draginmenu = FALSE;
    }
    if( Menu->ignorealt ){
        Menu->ignorealt = FALSE;
    }
    if( ( !Menu->active && ( oldmenu != 0 ) ) ||
        ( Menu->active && ( oldmenu != Menu->menu ) ) ) {
        if( ( Menu->menu > 0 ) && Menu->active ) {
            uimenucurr( &Menu->titles[Menu->menu - 1 ] );
        } else {
            /* no current menu */
            uimenucurr( NULL );
        }
    }

    if ( Menu->popuppending ) {
        InitMenuPopupPending = TRUE;
    }

    return( newevent );
}

#if 0
EVENT uigeteventfrompos( ORD row, ORD col )
/*****************************************/
{
    unsigned            index;
    DESCMENU*           desc;

    if( row < uimenuheight() ) {
        for( index = 0 ; !MENUENDMARKER( Menu->titles[ index ] ); ++index ) {
            desc = &Describe[ index ];
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

EVENT intern menuevent( VSCREEN *vptr )
/*************************************/
{
    register    EVENT                   newevent;
    register    EVENT                   ev;

    newevent = EV_NO_EVENT;

    if ( InitMenuPopupPending ) {
        InitMenuPopupPending = FALSE;
        if( Menu->titles[ Menu->menu - 1].popup != NULL ) {
            newevent = EV_MENU_INITPOPUP;
        }
    }

    if( newevent == EV_NO_EVENT ) {
        if ( uimenuson() && !uimenuisdisabled() ) {
            uipushlist( menu_list );
            if( ( Menu->active == FALSE ) || isdialogue( vptr ) ) {
                ev = getprime( vptr );
            } else {
                ev = getprime( NULL );
            }
            switch( ev ) {
            case EV_SCROLL_PRESS:
                Menu->scroll = TRUE;
                break;
            case EV_SCROLL_RELEASE:
                Menu->scroll = FALSE;
                break;
            case EV_NUM_PRESS:
                Menu->num = TRUE;
                break;
            case EV_NUM_RELEASE:
                Menu->num = FALSE;
                break;
            case EV_CAPS_PRESS:
                Menu->caps = TRUE;
                break;
            case EV_CAPS_RELEASE:
                Menu->caps = FALSE;
                break;
            default:
                newevent = process_menuevent( vptr, ev );
            }
            uipoplist();
        } else {
            newevent = getprime( vptr );
        }
    }

    return( newevent );
}


void UIAPI uidescmenu( MENUITEM *iptr, DESCMENU *desc )
/******************************************************/
{
    register    int                     item;
    register    int                     len;
    register    char*                   tab_loc;
    register    int                     tab_length;
                int                     to_add;

    desc->flags = 0;
    if( iptr != NULL ) {
        desc->area.width = 0;
        tab_length = 0;
        for( item = 0 ; !MENUENDMARKER( *iptr ) ; ++item ) {
            if( !MENUSEPARATOR( *iptr) ) {
                len = strlen( iptr->name );
                tab_loc = strchr( iptr->name, TABCHAR );
                if( tab_loc != NULL ) {
                    desc->flags |= MENU_HAS_TAB;
                    tab_loc++;
                    if( tab_loc != NULL ) {
                        if( tab_length < strlen( tab_loc ) )
                            tab_length = strlen( tab_loc );
                        len -= strlen( tab_loc ); /* for text after TABCHAR */
                    }
                    len--;  /* for TABCHAR */
                }
                if( iptr->flags & ITEM_CHECKED ) {
                    desc->flags |= MENU_HAS_CHECK;
                }
                if( iptr->popup != NULL ) {
                    desc->flags |= MENU_HAS_POPUP;
                }
                if( desc->area.width < len ) {
                    desc->area.width = len;
                }
            }
            ++iptr;
        }
        to_add = 0;
        if( desc->flags & MENU_HAS_TAB ) {
            to_add += tab_length + 1;
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
        desc->flags |= ( ( desc->area.width + 1 ) & MENU_TAB_OFFSET );
        desc->area.width += to_add;
        desc->area.height = (ORD) item + 2;
        if( desc->area.col + desc->area.width >= UIData->width ) {
            desc->area.col = UIData->width - desc->area.width;
        }
    } else {
        desc->area.height = 0;
    }
}

static void descmenu( int menu, DESCMENU *desc )
{
    MENUITEM*           nptr;
    MENUITEM*           iptr;
    unsigned            next;
    #define             MENUSTRLEN(x)   ((x) ? (ORD) strlen((x)) : (ORD) 0)

    --menu;
    iptr = Menu->titles[ menu ].popup;
    desc->area.row = 1;
    desc->area.col = 0;
    nptr = Menu->titles;
    for( ;; ) {
        next =  (ORD)desc->area.col +  MENUSTRLEN( nptr->name ) + BetweenTitles;
        if( next >= UIData->width ) {
            next -= desc->area.col;
            desc->area.col = 0;
            desc->area.row++;
        }
        if( menu == 0 ) break;
        desc->area.col = next;
        --menu;
        ++nptr;
    }
    desc->titlecol = desc->area.col;
    desc->titlewidth = MENUSTRLEN( nptr->name );
    uidescmenu( iptr, desc );
    /* Have to call this here since uidescmenu initializes field */
    MENU_SET_ROW( desc, desc->area.row - 1 );
}

void uimenutitlebar( void )
{
    register    int                     menu;

    forbid_refresh();
    for( menu = 1; menu <= NumMenus; ++menu ) {
        menutitle( menu, menu == Menu->menu );
    }
    permit_refresh();
}

static void drawbar( SAREA area, void *dummy )
/********************************************/
{
    forbid_refresh();
    dummy = dummy;
    if( area.row < uimenuheight() ) {
        mfill( &UIData->screen, area.row, 0,
           UIData->attrs[ ATTR_ACTIVE ], ' ', UIData->width, area.height );
        uimenutitlebar();
    }
    permit_refresh();
}

bool uienablemenuitem( unsigned menu, unsigned item, bool enable )
{
    bool        prev;
    MENUITEM    *pitem;

    pitem = &Menu->titles[ menu - 1 ].popup[ item - 1 ];
    prev = ( ( pitem->flags & ITEM_GRAYED ) == 0 );
    if( enable ) {
        pitem->flags &= ~ITEM_GRAYED;
    } else {
        pitem->flags |= ITEM_GRAYED;
    }
    return( prev );
}


void UIAPI uimenuindicators( bool status )
/*****************************************/
{
    Menu->indicators = status;
}

/* this code was split out of uimenubar to facilitate the updating of
 * menu's without constant redrawing
 */
void UIAPI uisetmenudesc( void )
/*******************************/
{
    register int  count;

    count = NumMenus;
    for( ; count > 0 ; --count ) {
        descmenu( count, &Describe[ count - 1 ] );
    }
}

VBARMENU* UIAPI uimenubar( VBARMENU *bar )
/*****************************************/
{
    register    int                     count;
    register    MENUITEM*               menus;
    register    VBARMENU*               prevMenu;

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
        Menu->active = FALSE;
        Menu->draginmenu = FALSE;
        Menu->indicators = TRUE;
        Menu->altpressed = FALSE;
        Menu->ignorealt = FALSE;
        Menu->movedmenu = FALSE;
        Menu->popuppending = FALSE;
        Menu->disabled = FALSE;
        count = 0;
        for( menus = Menu->titles; !MENUENDMARKER( *menus ); ++menus ) {
            if( ++count >= MAX_MENUS ) {
                break;
            }
        }
        NumMenus = count;
        uisetmenudesc();
        BarWin.area.row = 0;
        BarWin.area.col = 0;
        BarWin.area.height = uimenuheight();
        BarWin.area.width = UIData->width;
        BarWin.priority = P_MENU;
        BarWin.update = drawbar;
        BarWin.parm = NULL;
        openwindow( &BarWin );
        InitMenuPopupPending = FALSE;
    }
    return( prevMenu );
}

bool UIAPI uimenuson( void )
/***************************/
{
    return( Menu != NULL );
}

unsigned UIAPI uimenuheight( void )
/**********************************/
{
    if( Menu == NULL ) return( 0 );
    return( MENU_GET_ROW( &Describe[ NumMenus - 1 ] ) + 1 );
}

void UIAPI uimenudisable( bool disabled )
/****************************************/
{
    if ( uimenuson() ) {
        Menu->disabled = disabled;
    }
}

bool UIAPI uimenuisdisabled( void )
/**********************************/
{
    return( uimenuson() && Menu->disabled );
}

bool UIAPI uimenugetaltpressed( void )
/*************************************/
{
    return( uimenuson() && Menu->altpressed );
}

void UIAPI uimenusetaltpressed( bool altpressed )
/************************************************/
{
    if ( uimenuson() ) {
        Menu->altpressed = altpressed;
    }
}

void UIAPI uinomenus( void )
/***************************/
{
    uimenubar( NULL );
}


void UIAPI uimenus( MENUITEM *menus, MENUITEM **items, EVENT hot )
/*****************************************************************/
{
    register    int                     index;

    uimenubar( NULL );
    MenuList.titles = menus;
    for( index = 0 ; !MENUENDMARKER( menus[ index ] ); ++index ) {
        menus[index].popup = items[index];
    }
    MenuList.menu = 1;
    uimenubar( &MenuList );
}

void UIAPI uiactivatemenus( void )
/*********************************/
{
    if( Menu != NULL ) {
        if( !Menu->active ){
            Menu->altpressed = TRUE;
            process_menuevent( NULL, EV_ALT_RELEASE );
        }
    }
}

void UIAPI uiignorealt( void )
/*****************************/
{
    if( Menu != NULL ) {
        Menu->ignorealt = TRUE;
    }
}

int UIAPI uigetcurrentmenu( MENUITEM *menu )
{
    if( Menu->menu ) {
        *menu = Menu->titles[ Menu->menu - 1];
    }
    return( Menu->menu != 0 );
}

