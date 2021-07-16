/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "guiwind.h"
#include "guixutil.h"
#include "guisysme.h"
#include "guimenu.h"
#include "guihook.h"
#include "guirmenu.h"
#include <string.h>
#include "guitool.h"


extern UIMENUITEM       *GUIPopupMenu;

static gui_menu_struct GUISeparator = {
    NULL,       0,      GUI_STYLE_MENU_SEPARATOR
};

static void SetChecked( UIMENUITEM *menuitem, bool check )
{
    if( menuitem != NULL ) {
        if( check ) {
            menuitem->flags |= ITEM_CHECKED;
        } else {
            menuitem->flags &= ~ITEM_CHECKED;
        }
    }
}

static bool IsChecked( UIMENUITEM *menuitem )
{
    if( menuitem != NULL ) {
        return( (menuitem->flags & ITEM_CHECKED) != 0 );
    }
    return( false );
}

/*
 * MenuConvert -- returns a copy of text with the first amp removed and
 *                assigns *flags to indicate the location of the amp (base 0)
 *                and if checked or not
 */

static bool MenuConvert( const char *text, unsigned short *flags, char **new, bool checked )
{
    char        *end;
    size_t      length;
    size_t      end_length;
    char        *new_str;

    if( flags != NULL ) {
        *flags &= ~ITEM_CHAR_OFFSET; /* cancel prev info */
        if( checked ) {
            *flags |= ITEM_CHECKED;
        }
    }
    if( text == NULL ) {
        *new = NULL;
        return( true );
    }
    end = strchr( text, '&' );
    length = strlen( text );
    if( end == NULL ) {
        end_length = 0;
    } else {
        end_length = strlen( end );
    }
    if( flags != NULL ) {
        *flags |= ( length - end_length ) & ITEM_CHAR_OFFSET;
    }
    if( end == NULL ) {
        length++; /* no & so need room for NULL as it doesn't replace & */
    }
    new_str = (char *)GUIMemAlloc( length );
    *new = new_str;
    if( new_str == NULL ) {
        return( false );
    }
    if( length == end_length ) {
        end++; /* & at start */
        strcpy( new_str, end );
    } else {
        strncpy( new_str, text, length - end_length );
        if( end != NULL ) {
            end++;      /* past & */
            strncpy( new_str + length - end_length, end, end_length - 1 );
        }
        new_str[length - 1] = '\0';
    }
    return( true );
}

/*
 *  GUIFreeMenuItems -- frees exactly what was allocated by GUICreateMenuItems
 */

void GUIFreeMenuItems( UIMENUITEM *menuitems )
{
    int i;

    if( menuitems != NULL ) {
        for( i = 0; !MENUENDMARKER( menuitems[i] ); i++ ) {
            GUIMemFree( menuitems[i].name );
            if( menuitems[i].popup != NULL ) {
                GUIFreeMenuItems( menuitems[i].popup );
            }
        }
        GUIMemFree( menuitems );
    }
}

UIMENUITEM *GUIAllocMenuItems( int num_items )
{
    UIMENUITEM  *menuitems;

    menuitems = (UIMENUITEM *)GUIMemAlloc( sizeof( UIMENUITEM ) * ( num_items + 1 ) );
    if( menuitems != NULL ) {
        memset( menuitems, 0, sizeof( UIMENUITEM ) * ( num_items + 1 ) );
    }
    return( menuitems );
}

static bool GetMenu( int *depth, int num_items, UIMENUITEM *menuitems, gui_ctl_id id,
                     UIMENUITEM **pmenuitem, int *position, UIMENUITEM ***to_replace )
{
    int             i;
    int             num_popup_menus;

    for( i = 0; i < num_items; i++ ) {
        uiyield();
        if( menuitems[i].event == ID2EV( id ) ) {
            *pmenuitem = &menuitems[i];
            if( position != NULL ) {
                *position = i;
            }
            return( true );
        }
        if( depth != NULL ) {
            (*depth)++;
        }
        if( menuitems[i].popup != NULL ) {
            num_popup_menus = uimenuitemscount( menuitems[i].popup );
            if( GetMenu( depth, num_popup_menus, menuitems[i].popup, id, pmenuitem, position, to_replace ) ) {
                if( ( to_replace != NULL ) && ( *to_replace == NULL ) ) {
                    *to_replace = &menuitems[i].popup;
                }
                return( true );
            }
        }
        if( depth != NULL ) {
            (*depth)--;
        }
    }

    return( false );
}

/*
 * GUIGetMenu -- get the UIMENUITEM * for the given id.  Return true if it
 *               was found in the vbarmenu.
 */

static bool GUIGetMenu( gui_window *wnd, gui_ctl_id id, UIMENUITEM **pmenuitem, int *position,
                        UIMENUITEM ***to_replace, bool floating )
{
    UIMENUITEM  **the_menu;
    int         num_items;
    bool        vbar;
    int         depth;

    if( to_replace != NULL ) {
        *to_replace = NULL;
    }
    if( position != NULL ) {
        *position = 0;
    }
    if( pmenuitem == NULL ) {
        return( false );
    }
    *pmenuitem = NULL;
    vbar = false;
    if( floating ) {
        the_menu = &GUIPopupMenu;
    } else {
        if( wnd->menu != NULL ) {
            the_menu = &wnd->menu;
        } else {
            if( wnd->vbarmenu == NULL ) {
                return( false );
            }
            the_menu = &wnd->vbarmenu->titles;
            vbar = true;
        }
    }
    num_items = uimenuitemscount( *the_menu );
    depth = 0;
    if( GetMenu( &depth, num_items, *the_menu, id, pmenuitem, position, to_replace ) ) {
        if( ( to_replace != NULL ) && ( *to_replace == NULL ) ) {
            *to_replace = the_menu;
        }
        if( vbar && depth > 0 ) {
            vbar = false;
        }
        return( vbar );
    }
    return( false );
}

int GUIAPI GUIGetMenuPopupCount( gui_window *wnd, gui_ctl_id id )
{
    UIMENUITEM  *menuitem;

    GUIGetMenu( wnd, id, &menuitem, NULL, NULL, false );
    if( menuitem != NULL )
        return( uimenuitemscount( menuitem->popup ) );
    return( -1 );
}

bool GUIAPI GUIEnableMenuItem( gui_window *wnd, gui_ctl_id id, bool enable, bool floating )
{
    UIMENUITEM  *menuitem;
    bool        vbar;

    vbar = GUIGetMenu( wnd, id, &menuitem, NULL, NULL, floating );
    if( menuitem == NULL ) {
        return( false );
    }
    if( enable ) {
        GUIChangeMenu( menuitem, GUI_STYLE_MENU_ENABLED );
    } else {
        GUIChangeMenu( menuitem, GUI_STYLE_MENU_GRAYED );
    }

    if( vbar ) {
        /* if the menu item changed was in the top bar of menus,
         * tell ui that it has changed.
         */
        uimenubar( wnd->vbarmenu );
    } else {
        uisetmenudesc();
    }

    return( true );
}

bool GUIAPI GUISetMenuText( gui_window *wnd, gui_ctl_id id, const char *text, bool floating )
{
    UIMENUITEM  *menuitem;
    bool        vbar;
    char        *new;
    bool        checked;

    vbar = GUIGetMenu( wnd, id, &menuitem, NULL, NULL, floating );
    if( menuitem == NULL ) {
        return( false );
    }
    checked = IsChecked( menuitem );
    if( !MenuConvert( text, &menuitem->flags, &new, checked ) ) {
        return( false );
    }
    GUIMemFree( menuitem->name );
    menuitem->name = new;
    if( vbar ) {
        /* if the menu item changed was in the top bar of menus,
         * tell ui that it has changed.
         */
        uimenubar( wnd->vbarmenu );
    } else {
        uisetmenudesc();
    }
    return( true );
}

bool GUIAPI GUICheckMenuItem( gui_window *wnd, gui_ctl_id id, bool check, bool floating )
{
    bool        vbar;
    UIMENUITEM  *menuitem;

    vbar = GUIGetMenu( wnd, id, &menuitem, NULL, NULL, floating );
    if( menuitem == NULL ) {
        return( false );
    }
    SetChecked( menuitem, check );
    if( vbar ) {
        /* if the menu item changed was in the top bar of menus,
         * tell ui that it has changed.
         */
        uimenubar( wnd->vbarmenu );
    } else {
        uisetmenudesc();
    }
    return( true );
}

void GUIChangeMenu( UIMENUITEM *menuitem, gui_menu_styles style )
{
    if( style & GUI_STYLE_MENU_GRAYED ) {
        menuitem->flags |= ITEM_GRAYED;
    } else {
        menuitem->flags &= ~ITEM_GRAYED;
    }
}

static bool GUISetMenuItems( const gui_menu_items *menus, UIMENUITEM *menuitems )
{
    int     i;

    for( i = 0; i < menus->num_items; i++ ) {
        uiyield();
        if( menus->menu[i].style & GUI_STYLE_MENU_IGNORE )
            continue;
        if( menus->menu[i].style & GUI_STYLE_MENU_SEPARATOR ) {
            menuitems->flags |= ITEM_SEPARATOR;
        } else {
            if( !MenuConvert( menus->menu[i].label, &menuitems->flags, &menuitems->name, (menus->menu[i].style & GUI_STYLE_MENU_CHECKED) != 0 ) ) {
                return( false );
            }
            GUIChangeMenu( menuitems, menus->menu[i].style );
        }
        menuitems->event = ID2EV( menus->menu[i].id );
        menuitems++;
    }
    return( true );
}

int GUIGetNumIgnore( const gui_menu_items *menus )
{
    int     num_ignore;
    int     i;

    num_ignore = 0;
    for( i = 0; i < menus->num_items; i++ ) {
        if( menus->menu[i].style & GUI_STYLE_MENU_IGNORE ) {
            num_ignore++;
        }
    }
    return( num_ignore );
}

/*
 * GUICreateMenuItems -- converts the gui_menu_struct to an array of UIMENUITEM
 */

bool GUICreateMenuItems( const gui_menu_items *menus, UIMENUITEM **pmenuitems )
{
    int         num_ignore;
    UIMENUITEM  *menuitems;
    int         i;
    bool        ok;

    ok = true;
    *pmenuitems = NULL;
    if( menus->num_items > 0 ) {
        num_ignore = GUIGetNumIgnore( menus );
        if( menus->num_items > num_ignore ) {
            menuitems = GUIAllocMenuItems( menus->num_items - num_ignore );
            if( menuitems == NULL ) {
                ok = false;
            } else {
                *pmenuitems = menuitems;
                if( GUISetMenuItems( menus, menuitems ) ) {
                    for( i = 0; i < menus->num_items; i++ ) {
                        uiyield();
                        if( menus->menu[i].style & GUI_STYLE_MENU_IGNORE )
                            continue;
                        if( menus->menu[i].child.num_items > 0 ) {
                            ok = GUICreateMenuItems( &menus->menu[i].child, &menuitems->popup );
                            if( !ok ) {
                                break;
                            }
                        }
                        menuitems++;
                    }
                }
            }
        }
    }
    return( ok );
}

static VBARMENU *GUIAllocVBarMenu( void )
{
    VBARMENU    *vbarmenu;

    vbarmenu = (VBARMENU *)GUIMemAlloc( sizeof( VBARMENU ) );
    if( vbarmenu != NULL ) {
        vbarmenu->titles = NULL;
        vbarmenu->currmenu = -1;
    }
    return( vbarmenu );
}

static void GUIFreeVBarMenu( VBARMENU *vbarmenu )
{
    if( vbarmenu != NULL ) {
        uinomenus();
        if( vbarmenu->titles != NULL ) {
            GUIFreeMenuItems( vbarmenu->titles );
        }
        GUIMemFree( vbarmenu );
    }
}

/*
 * CreateVBarMenu -- converts the gui_menu_struct into a VBARMENU
 */

static bool CreateVBarMenu( gui_window *wnd, const gui_menu_items *menus, VBARMENU **pvbarmenu )
{
    VBARMENU    *vbarmenu;

    *pvbarmenu = NULL;
    if( menus->num_items > 0 ) {
        vbarmenu = GUIAllocVBarMenu();
        if( vbarmenu == NULL ) {
            return( false );
        }
        if( !GUICreateMenuItems( menus, &vbarmenu->titles ) ) {
            GUIFreeVBarMenu( vbarmenu );
            return( false );
        }
        GUIInitHint( wnd, menus, MENU_HINT );
        *pvbarmenu = vbarmenu;
    }
    return( true );
}

static bool InsertMenu( gui_window *wnd, const gui_menu_struct *menu, int position,
                        UIMENUITEM **pmenuitems, bool append_hint, hint_type type )
{
    int             num_items;
    UIMENUITEM      *newmenuitems;
    UIMENUITEM      *menuitems;
    gui_menu_items  menus;

    menuitems = *pmenuitems;
    num_items = uimenuitemscount( menuitems );
    if( ( position > num_items ) || ( position == -1 ) ) {
        position = num_items;
    }
    newmenuitems = (UIMENUITEM *)GUIMemAlloc( sizeof( UIMENUITEM ) * ( num_items + 2 ) );
    if( newmenuitems == NULL ) {
        return( false );
    }
    if( menuitems != NULL ) {
        memcpy( newmenuitems, menuitems, sizeof( UIMENUITEM ) * position );
        memcpy( &newmenuitems[position + 1], &menuitems[position], sizeof( UIMENUITEM ) * ( num_items - position + 1 ) );
    } else {
        memset( &newmenuitems[position + 1], 0, sizeof( UIMENUITEM ) );
    }
    menus.num_items = 1;
    menus.menu = (gui_menu_struct *)menu;
    memset( &newmenuitems[position], 0, sizeof( UIMENUITEM ) );
    if( !GUISetMenuItems( &menus, &newmenuitems[position] ) ) {
        GUIMemFree( newmenuitems );
        return( false );
    }
    if( !GUICreateMenuItems( &menu->child, &newmenuitems[position].popup ) ) {
        GUIMemFree( newmenuitems );
        return( false );
    }
    *pmenuitems = newmenuitems;
    GUIMemFree( menuitems );
    GUIMDIResetMenus( wnd, wnd->parent, &menus );
    if( append_hint ) {
        GUIAppendHintText( wnd, menu, type );
    }
    return( true );
}

/*
 * CreateMenus - creates menus for the windows
 */

static bool CreateMenus( gui_window *wnd, const gui_menu_items *menus,
                         gui_window *parent_wnd, gui_create_styles style )
{
    int     num_ignore;
    int     i;

    if( parent_wnd == NULL ) {
        if( !CreateVBarMenu( wnd, menus, &wnd->vbarmenu ) ) {
            return( false );
        }
    } else {
        if( style & GUI_SYSTEM_MENU ) {
            wnd->menu = GUIAllocMenuItems( NUM_SYSTEM_MENUS );
            if( wnd->menu == NULL ) {
                return( false );
            }
            if( !GUISetSystemMenu( wnd->menu, style ) ) {
                return( false );
            }
        }
        if( menus->num_items > 0 ) {
            num_ignore = GUIGetNumIgnore( menus );
            if( menus->num_items > num_ignore ) {
                if( style & GUI_SYSTEM_MENU ) {
                    if( !InsertMenu( wnd, &GUISeparator, -1, &wnd->menu, true, MENU_HINT ) ) {
                        return( false );
                    }
                }
                for( i = 0; i < menus->num_items; i++ ) {
                    uiyield();
                    if( menus->menu[i].style & GUI_STYLE_MENU_IGNORE )
                        continue;
                    if( !InsertMenu( wnd, &menus->menu[i], -1, &wnd->menu, true, MENU_HINT ) ) {
                        return( false );
                    }
                }
            }
        }
    }
    return( true );
}

bool GUICreateMenus( gui_window *wnd, gui_create_info *dlg_info )
{
    gui_menu_items      menus;
    bool                ret;

    if( dlg_info->resource_menu != NULL ) {
        ret = false;
        menus = NoMenu;
        if( GUICreateMenuStructFromRes( dlg_info->resource_menu, &menus ) ) {
            ret = CreateMenus( wnd, &menus, dlg_info->parent, dlg_info->style );
            GUIFreeGUIMenuStruct( &menus );
        }
    } else {
        ret = CreateMenus( wnd, &dlg_info->menus, dlg_info->parent, dlg_info->style );
    }
    return( ret );
}

/*
 * GUIFreeMenus -- frees everything allocated by CreateMenus
 */

void GUIFreeMenus( gui_window * wnd )
{
    GUIFreeVBarMenu( wnd->vbarmenu );
    wnd->vbarmenu = NULL;
    if( wnd->menu != NULL ) {
        GUIFreeMenuItems( wnd->menu );
        wnd->menu = NULL;
    }
}

void GUIAPI GUISetBetweenTitles( int between_titles )
{
    uisetbetweentitles( between_titles );
}

/*
 * GUIAppendMenuByIdx
 */

bool GUIAPI GUIAppendMenuByIdx( gui_window *wnd, int position, const gui_menu_struct *menu )
{
    int     num_items;

    if( wnd->vbarmenu != NULL ) {
        num_items = uimenuitemscount( wnd->vbarmenu->titles );
        if( num_items < ( position + 1 ) ) {
            return( false );
        }
        return( InsertMenu( wnd, menu, -1, &wnd->vbarmenu->titles[position].popup, true, MENU_HINT ) );
    }
    return( false );
}

static bool DeleteMenu( gui_window *wnd, gui_ctl_id id, UIMENUITEM **pmenuitems, int position )
{
    int         num_items;
    UIMENUITEM  *newmenuitems;
    UIMENUITEM  *menuitems;

    menuitems = *pmenuitems;
    num_items = uimenuitemscount( menuitems );
    if( num_items == 1 ) {
        newmenuitems = NULL;
    } else {
        newmenuitems = (UIMENUITEM *)GUIMemAlloc( sizeof( UIMENUITEM ) * num_items );
        if( newmenuitems == NULL ) {
            return( false );
        }
        memcpy( newmenuitems, menuitems, sizeof( UIMENUITEM ) * position );
        memcpy( &newmenuitems[position], &menuitems[position + 1], sizeof( UIMENUITEM ) * ( num_items - position ) );
    }
    if( menuitems[position].popup != NULL ) {
        GUIFreeMenuItems( menuitems[position].popup );
    }
    *pmenuitems = newmenuitems;
    GUIMemFree( menuitems[position].name );
    GUIMemFree( menuitems );
    GUIDeleteHintText( wnd, id, MENU_HINT );
    GUIMDIDeleteMenuItem( id );
    return( true );
}

/*
 * GUIDeleteMenuItem -- delete the menu item with the given ID
 */

bool GUIAPI GUIDeleteMenuItem( gui_window *wnd, gui_ctl_id id, bool floating )
{
    bool        vbar;
    UIMENUITEM  *menuitem;
    int         position;
    UIMENUITEM  **to_replace;

    vbar = GUIGetMenu( wnd, id, &menuitem, &position, &to_replace, floating );
    if( menuitem == NULL ) {
        return( false );
    }
    if( !DeleteMenu( wnd, id, to_replace, position ) ) {
        return( false );
    }
    if( vbar ) {
        uimenubar( wnd->vbarmenu );
    } else if( !floating ) {
        uisetmenudesc();
    }
    return( true );
}

bool GUIDeleteToolbarMenuItem( gui_window *wnd, gui_ctl_id id )
{
    int         i;
    int         num_items;
    UIMENUITEM  *menuitems;
    ui_event    ui_ev;

    if( wnd->vbarmenu != NULL ) {
        ui_ev = ID2EV( id );
        menuitems = wnd->vbarmenu->titles;
        num_items = uimenuitemscount( menuitems );
        for( i = 0; i < num_items; i++ ) {
            if( wnd->vbarmenu->titles[i].event == ui_ev ) {
                if( !DeleteMenu( wnd, id, &wnd->vbarmenu->titles, i ) ) {
                    break;
                }
                uimenubar( wnd->vbarmenu );
                return( true );
            }
        }
    }
    return( false );
}

bool GUIAPI GUIResetMenus( gui_window *wnd, const gui_menu_items *menus )
{
    toolbarinfo *tbar;

    tbar = wnd->tbar;
    if( tbar != NULL ) {
        if( tbar->fixed ) {
            wnd->tbar = NULL;
        } else {
            tbar = NULL;
        }
    }
    GUIFreeMenus( wnd );
    if( CreateMenus( wnd, menus, wnd->parent, wnd->style ) ) {
        GUISetSystemMenuFlags( wnd );
        GUIMDIResetMenus( wnd, wnd->parent, menus );
        if( wnd->vbarmenu != NULL ) {
            uimenubar( wnd->vbarmenu );
        }
        if( tbar != NULL ) {
            wnd->tbar = tbar;
            GUIXCreateFixedToolbar( wnd );
        }
        return( true );
    }
    return( false );
}

bool GUIAPI GUIEnableMenus( gui_window *wnd, bool enable )
{
    int                 i;
    gui_menu_styles     style;
    SAREA               screen;

    if( wnd->vbarmenu == NULL ) {
        return( false );
    }
    if( enable ) {
        style = GUI_STYLE_MENU_ENABLED;
    } else {
        style = GUI_STYLE_MENU_GRAYED;
    }
    for( i = 0; !MENUENDMARKER( wnd->vbarmenu->titles[i] ); i++ ) {
        GUIChangeMenu( &wnd->vbarmenu->titles[i], style );
    }
    screen.row = 0; /* leave this 0! */
    screen.col = 0;
    screen.height = YMIN;
    screen.width = XMAX;
    uidirty( screen );
    return( true );
}

bool GUIAPI GUIInsertMenuByIdx( gui_window *wnd, int position, const gui_menu_struct *menu, bool floating )
{
    if( floating ) {
        return( InsertMenu( wnd, menu, position, &GUIPopupMenu, true, FLOAT_HINT ) );
    } else {
        if( wnd->parent != NULL ) {
            return( false );
        }
        if( wnd->vbarmenu == NULL ) {
            gui_menu_items  menus;

            menus.num_items = 1;
            menus.menu = (gui_menu_struct *)menu;
            if( !CreateVBarMenu( wnd, &menus, &wnd->vbarmenu ) ) {
                return( false );
            }
        } else {
            if( !InsertMenu( wnd, menu, position, &wnd->vbarmenu->titles, true, MENU_HINT ) ) {
                return( false );
            }
        }
        uimenubar( wnd->vbarmenu );
    }

    return( true );
}

/*
 * GUIInsertMenuByID -- insert the given menu before the menu item with
 *                      the given id
 */

bool GUIAPI GUIInsertMenuByID( gui_window *wnd, gui_ctl_id id, const gui_menu_struct *menu )
{
    bool        vbar;
    UIMENUITEM  *menuitem;
    int         position;
    UIMENUITEM  **to_replace;

    vbar = GUIGetMenu( wnd, id, &menuitem, &position, &to_replace, false );
    if( menuitem == NULL ) {
        return( false );
    }
    if( !InsertMenu( wnd, menu, position, to_replace, true, MENU_HINT ) ) {
        return( false );
    }
    if( vbar ) {
        uimenubar( wnd->vbarmenu );
    } else {
        uisetmenudesc();
    }
    return( true );
}

/*
 * GUIAppendMenu -- append the given menu item to the top level menu of
 *                  the given window
 */

bool GUIAPI GUIAppendMenu( gui_window *wnd, const gui_menu_struct *menu, bool floating )
{
    if( floating ) {
        return( InsertMenu( wnd, menu, -1, &GUIPopupMenu, true, FLOAT_HINT ) );
    } else {
        if( wnd->parent == NULL ) {
            return( GUIInsertMenuByIdx( wnd, -1, menu, floating ) );
        } else {
            return( InsertMenu( wnd, menu, -1, &wnd->menu, true, MENU_HINT ) );
        }
    }
}

bool GUIAppendToolbarMenu( gui_window *wnd, const gui_menu_struct *menu, bool redraw )
{
    bool ok;

    ok = InsertMenu( wnd, menu, -1, &wnd->vbarmenu->titles, false, MENU_HINT );
    if( ok && redraw ) {
        uimenubar( wnd->vbarmenu );
    }
    return( ok );
}

static bool AddMenuItemToPopup( gui_window *wnd, gui_ctl_id id, int position,
                         const gui_menu_struct *menu, bool floating )
{
    bool        vbar;
    UIMENUITEM  *menuitem;

    vbar = GUIGetMenu( wnd, id, &menuitem, NULL, NULL, floating );
    if( menuitem == NULL ) {
        return( false );
    }
    if( !InsertMenu( wnd, menu, position, &menuitem->popup, false, MENU_HINT ) ) {
        return( false );
    }
    if( vbar ) {
        uimenubar( wnd->vbarmenu );
    } else if( !floating ) {
        uisetmenudesc();
    }
    return( true );
}

/*
 * GUIAppenMenuToPopup -- append info to the popup of the menu item with the
 *                        given ID
 */

bool GUIAPI GUIAppendMenuToPopup( gui_window *wnd, gui_ctl_id id,
                           const gui_menu_struct *menu, bool floating )
{
    return( AddMenuItemToPopup( wnd, id, -1, menu, floating ) );
}

bool GUIAPI GUIInsertMenuToPopup( gui_window *wnd, gui_ctl_id id, int position,
                           const gui_menu_struct *menu, bool floating )
{
    return( AddMenuItemToPopup( wnd, id, position, menu, floating ) );
}
