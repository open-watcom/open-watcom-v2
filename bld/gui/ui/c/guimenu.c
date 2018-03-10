/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
    int         length;
    int         end_length;
    char        *new_str;
    int         char_offset;

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
        char_offset = length - end_length;
        *flags |= char_offset & ITEM_CHAR_OFFSET;
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
    int j;

    if( menuitems == NULL )
        return;
    for( j = 0; !MENUENDMARKER( menuitems[j] ); j++ ) {
        GUIMemFree( menuitems[j].name );
        if( menuitems[j].popup != NULL ) {
            GUIFreeMenuItems( menuitems[j].popup );
        }
    }
    GUIMemFree( menuitems );
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
    int             item;
    int             num_popup_menus;

    for( item = 0; item < num_items; item++ ) {
        uiyield();
        if( menuitems[item].event == ID2EV( id ) ) {
            *pmenuitem = &menuitems[item];
            if( position != NULL ) {
                *position = item;
            }
            return( true );
        }
        if( depth != NULL ) {
            (*depth)++;
        }
        if( menuitems[item].popup != NULL ) {
            num_popup_menus = uimenuitemscount( menuitems[item].popup );
            if( GetMenu( depth, num_popup_menus, menuitems[item].popup, id, pmenuitem, position, to_replace ) ) {
                if( ( to_replace != NULL ) && ( *to_replace == NULL ) ) {
                    *to_replace = &menuitems[item].popup;
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

static bool GUIGetMenu( gui_window *wnd, gui_ctl_id id, UIMENUITEM **menuitems, int *position,
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
    if( menuitems == NULL ) {
        return( false );
    } else {
        *menuitems = NULL;
    }
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
    if( GetMenu( &depth, num_items, *the_menu, id, menuitems, position, to_replace ) ) {
        if( ( to_replace != NULL ) && ( *to_replace == NULL ) ) {
            *to_replace = the_menu;
        }
        if ( vbar && depth ) {
            vbar = false;
        }
        return( vbar );
    }
    return( false );
}

int GUIGetMenuPopupCount( gui_window *wnd, gui_ctl_id id )
{
    UIMENUITEM  *menuitem;

    GUIGetMenu( wnd, id, &menuitem, NULL, NULL, false );
    if( menuitem != NULL && menuitem->popup ) {
        return( uimenuitemscount( menuitem->popup ) );
    } else {
        return( -1 );
    }
}

bool GUIEnableMenuItem( gui_window *wnd, gui_ctl_id id, bool enable, bool floating )
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

bool GUISetMenuText( gui_window *wnd, gui_ctl_id id, const char *text, bool floating )
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

bool GUICheckMenuItem( gui_window *wnd, gui_ctl_id id, bool check, bool floating )
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

static bool GUISetMenuItems( int num_items, UIMENUITEM *menuitems, gui_menu_struct *info )
{
    while( num_items-- > 0 ) {
        uiyield();
        if( (info->style & GUI_STYLE_MENU_IGNORE) == 0 ) {
            if( info->style & GUI_STYLE_MENU_SEPARATOR ) {
                menuitems->flags |= ITEM_SEPARATOR;
            } else {
                if( !MenuConvert( info->label, &menuitems->flags, &menuitems->name, (info->style & GUI_STYLE_MENU_CHECKED) != 0 ) ) {
                    return( false );
                }
                GUIChangeMenu( menuitems, info->style );
            }
            menuitems->event = ID2EV( info->id );
            menuitems++;
        }
        info++;
    }
    return( true );
}

int GUIGetNumIgnore( gui_menu_struct *info, int num_items )
{
    int     num_ignore;

    num_ignore = 0;
    while( num_items-- > 0 ) {
        if( info->style & GUI_STYLE_MENU_IGNORE ) {
            num_ignore++;
        }
        info++;
    }
    return( num_ignore );
}

/*
 * GUICreateMenuItems -- converts the gui_menu_struct to an array of UIMENUITEM
 */

bool GUICreateMenuItems( int num_items, gui_menu_struct *info, UIMENUITEM **pmenuitems )
{
    int         num_ignore;
    int         item;
    UIMENUITEM  *menuitems;

    if( num_items <= 0 ) {
        *pmenuitems = NULL;
        return( true );
    }
    num_ignore = GUIGetNumIgnore( info, num_items );
    if( num_ignore >= num_items ) {
        *pmenuitems = NULL;
        return( true );
    }
    menuitems = GUIAllocMenuItems( num_items - num_ignore );
    *pmenuitems = menuitems;
    if( menuitems == NULL ) {
        return( false );
    }
    if( !GUISetMenuItems( num_items, menuitems, info ) ) {
        return( false );
    }
    item = 0;
    while( num_items-- > 0 ) {
        uiyield();
        if( (info->style & GUI_STYLE_MENU_IGNORE) == 0 ) {
            if( info->num_child_menus > 0 ) {
                if( !GUICreateMenuItems( info->num_child_menus, info->child, &menuitems[item].popup ) ) {
                    return( false );
                }
            }
            item++;
        }
        info++;
    }
    return( true );
}

static bool GUIAllocVBarMenu( VBARMENU **pmenu )
{
    VBARMENU    *menu;

    if( pmenu == NULL ) {
        return( false );
    }
    menu = (VBARMENU *)GUIMemAlloc( sizeof( VBARMENU ) );
    if( menu == NULL ) {
        return( false );
    }
    menu->titles = NULL;
    menu->currmenu = 0;
    *pmenu = menu;
    return( true );
}

static void GUIFreeVBarMenu( VBARMENU *menu )
{
    if( menu != NULL ) {
        uinomenus();
        if( menu->titles != NULL ) {
            GUIFreeMenuItems( menu->titles );
        }
        GUIMemFree( menu );
    }
}

/*
 * CreateVBarMenu -- converts the gui_menu_struct into a VBARMENU
 */

static bool CreateVBarMenu( gui_window *wnd, int num_items,
                            gui_menu_struct *main_menu, VBARMENU **pmenu )
{
    VBARMENU    *vbarmenu;

    if( num_items == 0 ) {
        *pmenu = NULL;
        return( true );
    }
    if( !GUIAllocVBarMenu( pmenu ) ) {
        *pmenu = NULL;
        return( false );
    }
    vbarmenu = *pmenu;
    if( !GUICreateMenuItems( num_items, main_menu, &vbarmenu->titles ) ) {
        GUIFreeVBarMenu( vbarmenu );
        *pmenu = NULL;
        return( false );
    }
    GUIInitHint( wnd, num_items, main_menu, MENU_HINT );
    return( true );
}

static bool InsertMenu( gui_window *wnd, gui_menu_struct *info, int position,
                        UIMENUITEM **pmenuitems, bool append_hint, hint_type type )
{
    int         num_items;
    UIMENUITEM  *newmenuitems;
    UIMENUITEM  *menuitems;

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
        memcpy( &newmenuitems[position + 1], &menuitems[position],
                sizeof( UIMENUITEM ) * ( num_items + 1 - position ) );
    } else {
        memset( &newmenuitems[position + 1], 0, sizeof( UIMENUITEM ) );
    }
    memset( &newmenuitems[position], 0, sizeof( UIMENUITEM ) );
    if( !GUISetMenuItems( 1, &newmenuitems[position], info ) ) {
        GUIMemFree( newmenuitems );
        return( false );
    }
    if( !GUICreateMenuItems( info->num_child_menus, info->child, &newmenuitems[position].popup ) ) {
        GUIMemFree( newmenuitems );
        return( false );
    }
    *pmenuitems = newmenuitems;
    GUIMemFree( menuitems );
    GUIMDIResetMenus( wnd, wnd->parent, 1, info );
    if( append_hint ) {
        GUIAppendHintText( wnd, info, type );
    }
    return( true );
}

/*
 * CreateMenus - creates menus for the windows
 */

static bool CreateMenus( gui_window *wnd, int num_items, gui_menu_struct *menu,
                         gui_window *parent, gui_create_styles style )
{
    int     num_ignore;

    if( parent == NULL ) {
        if( !CreateVBarMenu( wnd, num_items, menu, &wnd->vbarmenu ) ) {
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
        num_ignore = 0;
        if( num_items > 0 ) {
            num_ignore = GUIGetNumIgnore( menu->child, menu->num_child_menus );
        }
        if( num_items > num_ignore ) {
            if( style & GUI_SYSTEM_MENU ) {
                if( !InsertMenu( wnd, &GUISeparator, -1, &wnd->menu, true, MENU_HINT ) ) {
                    return( false );
                }
            }
            while( num_items-- > 0 ) {
                uiyield();
                if( (menu->style & GUI_STYLE_MENU_IGNORE) == 0 ) {
                    if( !InsertMenu( wnd, menu, -1, &wnd->menu, true, MENU_HINT ) ) {
                        return( false );
                    }
                }
                menu++;
            }
        }
    }
    return( true );
}

bool GUICreateMenus( gui_window *wnd,  gui_create_info *dlg_info )
{
    int                 num_items;
    gui_menu_struct     *menu;
    bool                ret;

    if( dlg_info->resource_menu != NULL ) {
        num_items = 0;
        menu = NULL;
        GUICreateMenuStructFromRes( dlg_info->resource_menu, &menu, &num_items );
        ret = CreateMenus( wnd, num_items, menu, dlg_info->parent, dlg_info->style );
        GUIFreeGUIMenuStruct( menu, num_items );
        return( ret );
    } else {
        return( CreateMenus( wnd, dlg_info->num_items, dlg_info->menu, dlg_info->parent, dlg_info->style ) );
    }
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

void GUISetBetweenTitles( int between_titles )
{
    uisetbetweentitles( between_titles );
}

/*
 * GUIAppendMenuByIdx
 */

bool GUIAppendMenuByIdx( gui_window *wnd, int position, gui_menu_struct *menu )
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
    int         prev_num;
    UIMENUITEM  *newmenuitems;
    UIMENUITEM  *menuitems;

    menuitems = *pmenuitems;
    prev_num = uimenuitemscount( menuitems );
    if( prev_num == 1 ) {
        newmenuitems = NULL;
    } else {
        newmenuitems = (UIMENUITEM *)GUIMemAlloc( sizeof( UIMENUITEM ) * prev_num );
        if( newmenuitems == NULL ) {
            return( false );
        }
        memcpy( newmenuitems, menuitems, sizeof( UIMENUITEM ) * position );
        memcpy( &newmenuitems[position], &menuitems[position + 1], sizeof( UIMENUITEM ) * ( prev_num - position ) );
    }
    if( menuitems[position].popup != NULL ) {
        GUIFreeMenuItems( menuitems[position].popup );
    }
    *pmenuitems = newmenuitems;
    GUIMemFree( menuitems[position].name );
    GUIMemFree( menuitems );
    GUIDeleteHintText( wnd, id );
    GUIMDIDeleteMenuItem( id );
    return( true );
}

/*
 * GUIDeleteMenuItem -- delete the menu item with the given ID
 */

bool GUIDeleteMenuItem( gui_window *wnd, gui_ctl_id id, bool floating )
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
    int         item;
    int         num_items;
    UIMENUITEM  *menuitems;
    ui_event    ui_ev;

    if( wnd->vbarmenu != NULL ) {
        ui_ev = ID2EV( id );
        menuitems = wnd->vbarmenu->titles;
        num_items = uimenuitemscount( menuitems );
        for( item = 0; item < num_items; item++ ) {
            if( wnd->vbarmenu->titles[item].event == ui_ev ) {
                if( !DeleteMenu( wnd, id, &wnd->vbarmenu->titles, item ) ) {
                    break;
                }
                uimenubar( wnd->vbarmenu );
                return( true );
            }
        }
    }
    return( false );
}

bool GUIResetMenus( gui_window *wnd, int num_items, gui_menu_struct *menu )
{
    toolbarinfo *tbar;

    tbar = wnd->tbinfo;
    if( tbar != NULL ) {
        if( tbar->fixed ) {
            wnd->tbinfo = NULL;
        } else {
            tbar = NULL;
        }
    }
    GUIFreeMenus( wnd );
    if( CreateMenus( wnd, num_items, menu, wnd->parent, wnd->style ) ) {
        GUISetSystemMenuFlags( wnd );
        GUIMDIResetMenus( wnd, wnd->parent, num_items, menu );
        if( wnd->vbarmenu != NULL ) {
            uimenubar( wnd->vbarmenu );
        }
        if( tbar != NULL ) {
            wnd->tbinfo = tbar;
            GUIXCreateFixedToolbar( wnd );
        }
        return( true );
    }
    return( false );
}

bool GUIEnableMenus( gui_window *wnd, bool enable )
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

bool GUIInsertMenuByIdx( gui_window *wnd, int position, gui_menu_struct *menu, bool floating )
{
    if( floating ) {
        return( InsertMenu( wnd, menu, position, &GUIPopupMenu, true, FLOAT_HINT ) );
    } else {
        if( wnd->parent != NULL ) {
            return( false );
        }
        if( wnd->vbarmenu == NULL ) {
            if( !CreateVBarMenu( wnd, 1, menu, &wnd->vbarmenu ) ) {
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

bool GUIInsertMenuByID( gui_window *wnd, gui_ctl_id id, gui_menu_struct *info )
{
    bool        vbar;
    UIMENUITEM  *menuitem;
    int         position;
    UIMENUITEM  **to_replace;

    vbar = GUIGetMenu( wnd, id, &menuitem, &position, &to_replace, false );
    if( menuitem == NULL ) {
        return( false );
    }
    if( !InsertMenu( wnd, info, position, to_replace, true, MENU_HINT ) ) {
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

bool GUIAppendMenu( gui_window *wnd, gui_menu_struct *menu, bool floating )
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

bool GUIAppendToolbarMenu( gui_window *wnd, gui_menu_struct *menu, bool redraw )
{
    bool ok;

    ok = InsertMenu( wnd, menu, -1, &wnd->vbarmenu->titles, false, MENU_HINT );
    if ( ok && redraw ) {
        uimenubar( wnd->vbarmenu );
    }
    return( ok );
}

static bool AddMenuItemToPopup( gui_window *wnd, gui_ctl_id id, int position,
                         gui_menu_struct *info, bool floating )
{
    bool        vbar;
    UIMENUITEM  *menuitem;

    vbar = GUIGetMenu( wnd, id, &menuitem, NULL, NULL, floating );
    if( menuitem == NULL ) {
        return( false );
    }
    if( !InsertMenu( wnd, info, position, &menuitem->popup, false, MENU_HINT ) ) {
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

bool GUIAppendMenuToPopup( gui_window *wnd, gui_ctl_id id,
                           gui_menu_struct *info, bool floating )
{
    return( AddMenuItemToPopup( wnd, id, -1, info, floating ) );
}

bool GUIInsertMenuToPopup( gui_window *wnd, gui_ctl_id id, int position,
                           gui_menu_struct *menu, bool floating )
{
    return( AddMenuItemToPopup( wnd, id, position, menu, floating ) );
}

