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


#include "guiwind.h"
#include "guixutil.h"
#include "guisysme.h"
#include "guimenu.h"
#include "guihook.h"
#include "guirmenu.h"
#include <string.h>

extern  MENUITEM        *GUIPopupMenu;
extern bool GUIXCreateFixedToolbar( gui_window *wnd );

static gui_menu_struct GUISeparator = {
    NULL,       0,      GUI_SEPARATOR
};

static void SetChecked( MENUITEM *menu, bool check )
{
    if( menu != NULL ) {
        if( check ) {
            menu->flags |= ITEM_CHECKED;
        } else {
            menu->flags &= ~ITEM_CHECKED;
        }
    }
}

static bool IsChecked( MENUITEM *menu )
{
    if( menu != NULL ) {
        return( (menu->flags & ITEM_CHECKED) != 0 );
    }
    return( false );
}

/*
 * MenuConvert -- returns a copy of text with the first amp removed and
 *                assigns *flags to indicate the location of the amp (base 0)
 *                and if checked or not
 */

static bool MenuConvert( const char *text, unsigned short *flags, char **new,
                         bool checked )
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
        *flags |= ( char_offset & ITEM_CHAR_OFFSET );
    }
    if( end == NULL ) {
        length ++; /* no & so need room for NULL as it doesn't replace & */
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
            strncpy( new_str + length - end_length, end, end_length - 1);
        }
        new_str[length - 1] = '\0';
    }
    return( true );
}

/*
 *  GUIFreeMenuItems -- frees exactly what was allocated by GUICreateMenuItems
 */

void GUIFreeMenuItems( MENUITEM *menus )
{
    int j;

    if( menus == NULL ) return;
    for( j = 0; !MENUENDMARKER( menus[j] ); j++ ) {
        GUIMemFree( menus[j].name );
        if( menus[j].popup != NULL ) {
            GUIFreeMenuItems( menus[j].popup );
        }
    }
    GUIMemFree( menus );
}

MENUITEM *GUIAllocMenuItems( int num_menus )
{
    MENUITEM *menu;

    menu = (MENUITEM *)GUIMemAlloc( sizeof( MENUITEM ) * ( num_menus + 1 ) );
    if( menu != NULL ) {
        memset( menu, 0, sizeof( MENUITEM ) * ( num_menus + 1 ) );
    }
    return( menu );
}

/*
 * GetNumItems -- count the number of items in the menu array, excluding
 *                the end marker
 */

static int GetNumItems( MENUITEM *menu )
{
    int i;
    int prev_num;

    if( menu == NULL ) {
        return( 0 );
    }
    prev_num = 0;
    for( i = 0; !MENUENDMARKER( menu[i] ); i++ ) {
        prev_num++;
    }
    return( prev_num );
}

static bool GetMenu( int *depth, int num_menus, MENUITEM *menu, unsigned id,
                     MENUITEM **pmenu, int *index, MENUITEM ***to_replace )
{
    int i;
    int num_popup_menus;


    for( i = 0; i < num_menus; i++ ) {
        uiyield();
        if( menu[i].event == id ) {
            *pmenu = &menu[i];
            if( index != NULL ) {
                *index = i;
            }
            return( true );
        }
        if( depth ) {
            (*depth)++;
        }
        if( menu[i].popup != NULL ) {
            num_popup_menus = GetNumItems( menu[i].popup );
            if( GetMenu( depth, num_popup_menus, menu[i].popup, id,
                         pmenu, index, to_replace ) ) {
                if( ( to_replace != NULL ) && ( *to_replace == NULL ) ) {
                    *to_replace = &menu[i].popup;
                }
                return( true );
            }
        }
        if( depth ) {
            (*depth)--;
        }
    }


    return( false );
}

/*
 * GUIGetMenu -- get the MENUITEM * for the given id.  Return true if it
 *               was found in the vbarmenu.
 */

static bool GUIGetMenu( gui_window *wnd, int id, MENUITEM **menu, int *index,
                        MENUITEM ***to_replace, bool floating )
{
    MENUITEM    **the_menu;
    bool        num_menus;
    bool        vbar;
    int         depth;

    if( to_replace != NULL ) {
        *to_replace = NULL;
    }
    if( index != NULL ) {
        *index = 0;
    }
    if( menu == NULL ) {
        return( false );
    } else {
        *menu = NULL;
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
    num_menus = GetNumItems( *the_menu );
    depth = 0;
    if( GetMenu( &depth, num_menus, *the_menu, id, menu, index, to_replace ) ) {
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

int GUIGetMenuPopupCount( gui_window *wnd, int id )
{
    MENUITEM    *menu;

    GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, NULL, NULL, false );
    if( menu && menu->popup ) {
        return( GetNumItems( menu->popup ) );
    } else {
        return( -1 );
    }
}

bool GUIEnableMenuItem( gui_window *wnd, int id, bool enable, bool floating )
{
    MENUITEM    *menu;
    bool        vbar;

    vbar = GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, NULL, NULL, floating );
    if( menu == NULL ) {
        return( false );
    }
    if( enable ) {
        GUIChangeMenu( menu, GUI_ENABLED );
    } else {
        GUIChangeMenu( menu, GUI_GRAYED );
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

bool GUISetMenuText( gui_window *wnd, int id, const char *text, bool floating )
{
    MENUITEM    *menu;
    bool        vbar;
    char        *new;
    bool        checked;

    vbar = GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, NULL, NULL, floating );
    if( menu == NULL ) {
        return( false );
    }
    checked = IsChecked( menu );
    if( !MenuConvert( text, &menu->flags, &new, checked ) ) {
        return( false );
    }
    GUIMemFree( menu->name );
    menu->name = new;
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

bool GUICheckMenuItem( gui_window *wnd, int id, bool check, bool floating )
{
    bool        vbar;
    MENUITEM    *menu;

    vbar = GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, NULL, NULL, floating );
    if( menu == NULL ) {
        return( false );
    }
    SetChecked( menu, check );
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

void GUIChangeMenu( MENUITEM *menu, gui_menu_styles style )
{
    if( style & GUI_GRAYED  ) {
        menu->flags |= ITEM_GRAYED;
    } else {
        menu->flags &= ~ITEM_GRAYED;
    }
}

static bool GUISetMenuItems( int num_menus, MENUITEM *menu,
                             gui_menu_struct *info )
{
    int i;
    int j;

    j = 0;
    for( i = 0; i < num_menus; i ++ ) {
        uiyield();
        if( !(info[i].style & GUI_IGNORE) ) {
            if( info[i].style & GUI_SEPARATOR ) {
                menu[j].flags |= ITEM_SEPARATOR;
            } else {
                if( !MenuConvert( info[i].label, &menu[j].flags, &menu[j].name,
                                  info[i].style & GUI_MENU_CHECKED ) ) {
                    return( false );
                }
                GUIChangeMenu( &menu[j], info[i].style );
            }
            menu[j].event = GUI_FIRST_USER_EVENT + info[i].id;
            j++;
        }
    }
    return( true );
}

int GUIGetNumIgnore( gui_menu_struct *info, int num_menus )
{
    int num_ignore;
    int i;

    num_ignore = 0;
    for( i = 0; i < num_menus; i++ ) {
        if( info[i].style & GUI_IGNORE ) {
            num_ignore++;
        }
    }
    return( num_ignore );
}

/*
 * GUICreateMenuItems -- converts the gui_menu_struct to an array of MENUITEM
 */

bool GUICreateMenuItems( int num_menus, gui_menu_struct *info,
                         MENUITEM **pmenu )
{
    int         num_ignore;
    int         i;
    int         j;
    MENUITEM    *menu;

    if( num_menus <= 0 ) {
        *pmenu = NULL;
        return( true );
    }
    num_ignore = GUIGetNumIgnore( info, num_menus );
    if( num_ignore >= num_menus ) {
        *pmenu = NULL;
        return( true );
    }
    *pmenu = GUIAllocMenuItems( num_menus - num_ignore );
    menu = *pmenu;
    if( menu == NULL ) {
        return( false );
    }
    if( !GUISetMenuItems( num_menus, menu, info ) ) {
        return( false );
    }
    j = 0;
    for( i = 0; i < num_menus; i++ ) {
        uiyield();
        if( !( info[i].style & GUI_IGNORE ) ) {
            if( info[i].num_child_menus > 0 ) {
                if( !GUICreateMenuItems( info[i].num_child_menus,
                                         info[i].child, &menu[j].popup ) ) {
                    return( false );
                }
            }
            j++;
        }
    }
    return( true );
}

bool GUIAllocVBarMenu( VBARMENU **pmenu )
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
    menu->menu = 1;
    *pmenu = menu;
    return( true );
}

/*
 * CreateVBarMenu -- converts the gui_menu_struct into a VBARMENU
 */

static bool CreateVBarMenu( gui_window *wnd, int num_menus,
                            gui_menu_struct *main_menu, VBARMENU **pmenu )
{
    VBARMENU    *vbarmenu;

    if( num_menus == 0 ) {
        *pmenu = NULL;
        return( true );
    }
    if( !GUIAllocVBarMenu( pmenu ) ) {
        *pmenu = NULL;
        return( false );
    }
    vbarmenu = *pmenu;
    if( !GUICreateMenuItems( num_menus, main_menu, &vbarmenu->titles ) ) {
        GUIFreeVBarMenu( vbarmenu );
        *pmenu = NULL;
        return( false );
    }
    GUIInitHint( wnd, num_menus, main_menu, MENU_HINT );
    return( true );
}

static bool InsertMenu( gui_window *wnd, gui_menu_struct *info, int offset,
                        MENUITEM **pmenu, bool append_hint, hint_type type )
{
    int         num_menus;
    MENUITEM    *new_menu;
    MENUITEM    *menu;

    menu = *pmenu;
    num_menus = GetNumItems( menu );
    if( ( num_menus < offset ) || ( offset == -1 ) ) {
        offset = num_menus;
    }
    new_menu = (MENUITEM *)GUIMemAlloc( sizeof( MENUITEM ) * ( num_menus + 2 ) );
    if( new_menu == NULL ) {
        return( false );
    }
    if( menu != NULL ) {
        memcpy( new_menu, menu, sizeof( MENUITEM ) * offset );
        memcpy( &new_menu[offset+1], &menu[offset],
                sizeof( MENUITEM ) * ( num_menus + 1 - offset ) );
    } else {
        memset( &new_menu[offset+1], 0, sizeof( MENUITEM ) );
    }
    memset( &new_menu[offset], 0, sizeof( MENUITEM ) );
    if( !GUISetMenuItems( 1, &new_menu[offset], info ) ) {
        GUIMemFree( new_menu );
        return( false );
    }
    if( !GUICreateMenuItems( info->num_child_menus, info->child,
                             &new_menu[offset].popup ) ) {
        return( false );
    }
    GUIMemFree( menu );
    *pmenu = new_menu;
    GUIMDIResetMenus( wnd, wnd->parent, 1, info );
    if( append_hint ) {
        GUIAppendHintText( wnd, info, type );
    }
    return( true );
}

/*
 * CreateMenus - creates menus for the windows
 */

static bool CreateMenus( gui_window *wnd, int num_menus, gui_menu_struct *menu,
                         gui_window *parent, gui_create_styles style )
{
    int num_ignore;
    int i;

    if( parent == NULL ) {
        if( !CreateVBarMenu( wnd, num_menus, menu, &wnd->vbarmenu ) ) {
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
        if( num_menus > 0 ) {
            num_ignore = GUIGetNumIgnore( menu->child, menu->num_child_menus );
        }
        if( num_menus > num_ignore ) {
            if( style & GUI_SYSTEM_MENU ) {
                if( !InsertMenu( wnd, &GUISeparator, -1, &wnd->menu, true,
                                 MENU_HINT ) ) {
                    return( false );
                }
            }
            for( i = 0; i < num_menus; i ++ ) {
                uiyield();
                if( !( menu[i].style & GUI_IGNORE ) ) {
                    if( !InsertMenu( wnd, &menu[i], -1, &wnd->menu, true,
                                     MENU_HINT ) ) {
                        return( false );
                    }
                }
            }
        }
    }
    return( true );
}

bool GUICreateMenus( gui_window *wnd,  gui_create_info *dlg_info )
{
    int                 num;
    gui_menu_struct     *menu;
    bool                ret;

    if( dlg_info->resource_menu ) {
        num = 0;
        menu = NULL;
        GUICreateMenuStructFromRes( dlg_info->resource_menu, &menu, &num );
        ret = CreateMenus( wnd, num, menu, dlg_info->parent, dlg_info->style );
        GUIFreeGUIMenuStruct( menu, num );
        return( ret );
    } else {
        return( CreateMenus( wnd, dlg_info->num_menus, dlg_info->menu,
                             dlg_info->parent, dlg_info->style ) );
    }
}

void GUIFreeVBarMenu( VBARMENU *menu )
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
 * GUIAppendMenuByOffset
 */

bool GUIAppendMenuByOffset( gui_window *wnd, int offset, gui_menu_struct *menu )
{
    int num_menus;

    if( wnd->vbarmenu != NULL ) {
        num_menus = GetNumItems( wnd->vbarmenu->titles );
        if( num_menus < ( offset + 1 ) ) {
            return( false );
        }
        return( InsertMenu( wnd, menu, -1,
                            &wnd->vbarmenu->titles[offset].popup,
                            true, MENU_HINT ) );
    }
    return( false );
}

static bool DeleteMenu( gui_window *wnd, unsigned id, MENUITEM **pmenu,
                        int index )
{
    int         prev_num;
    MENUITEM    *new_menu;
    MENUITEM    *sub;

    sub = *pmenu;
    prev_num = GetNumItems( sub );
    if( prev_num == 1 ) {
        new_menu = NULL;
    } else {
        new_menu = (MENUITEM *)GUIMemAlloc( sizeof( MENUITEM ) * prev_num );
        if( new_menu == NULL ) {
            return( false );
        }
        memcpy( new_menu, sub, sizeof( MENUITEM ) * index );
        memcpy( &new_menu[index], &sub[index+1], sizeof( MENUITEM ) *
                ( prev_num - index ) );
    }
    if( sub[index].popup != NULL ) {
        GUIFreeMenuItems( sub[index].popup );
    }
    GUIMemFree( sub[index].name );
    GUIMemFree( sub );
    *pmenu = new_menu;
    GUIDeleteHintText( wnd, id );
    GUIMDIDeleteMenuItem( id );
    return( true );
}

/*
 * GUIDeleteMenuItem -- delete the menu item with the given ID
 */

bool GUIDeleteMenuItem( gui_window *wnd, int id, bool floating )
{
    bool        vbar;
    MENUITEM    *menu;
    int         index;
    MENUITEM    **to_replace;

    vbar = GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, &index,
                       &to_replace, floating );
    if( menu == NULL ) {
        return( false );
    }
    if( !DeleteMenu( wnd, id, to_replace, index ) ) {
        return( false );
    }
    if( vbar ) {
        uimenubar( wnd->vbarmenu );
    } else if( !floating ) {
        uisetmenudesc();
    }
    return( true );
}

bool GUIDeleteToolbarMenuItem( gui_window *wnd, unsigned id )
{
    int         num_menus;
    MENUITEM    *menu;
    int         i;

    if( wnd->vbarmenu != NULL ) {
        id += GUI_FIRST_USER_EVENT;
        menu = wnd->vbarmenu->titles;
        num_menus = GetNumItems( menu );
        for( i = 0; i < num_menus; i++ ) {
            if( menu[i].event == id ) {
                if( !DeleteMenu( wnd, id, &wnd->vbarmenu->titles, i ) ) {
                    return( false );
                }
                uimenubar( wnd->vbarmenu );
                return( true );
            }
        }
    }
    return( false );
}

bool GUIResetMenus( gui_window *wnd, int num_menus, gui_menu_struct *menu )
{
    toolbarinfo *bar;


    bar = wnd->toolbar;
    if( bar != NULL ) {
        if( bar->fixed ) {
            wnd->toolbar = NULL;
        } else {
            bar = NULL;
        }
    }
    GUIFreeMenus( wnd );
    if( CreateMenus( wnd, num_menus, menu, wnd->parent, wnd->style ) ) {
        GUISetSystemMenuFlags( wnd );
        GUIMDIResetMenus( wnd, wnd->parent, num_menus, menu );
        if( wnd->vbarmenu != NULL ) {
            uimenubar( wnd->vbarmenu );
        }
        if( bar ) {
            wnd->toolbar = bar;
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
        style = GUI_ENABLED;
    } else {
        style = GUI_GRAYED;
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

bool GUIInsertMenu( gui_window *wnd, int offset, gui_menu_struct *menu, bool floating )
{
    if( floating ) {
        return( InsertMenu( wnd, menu, offset, &GUIPopupMenu, true, FLOAT_HINT ) );
    } else {
        if( wnd->parent != NULL ) {
            return( false );
        }
        if( wnd->vbarmenu == NULL ) {
            if( !CreateVBarMenu( wnd, 1, menu, &wnd->vbarmenu ) ) {
                return( false );
            }
        } else {
            if( !InsertMenu( wnd, menu, offset, &wnd->vbarmenu->titles, true, MENU_HINT ) ) {
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

bool GUIInsertMenuByID( gui_window *wnd, unsigned id, gui_menu_struct *info )
{
    bool        vbar;
    MENUITEM    *menu;
    int         index;
    MENUITEM    **to_replace;

    vbar = GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, &index,
                       &to_replace, false );
    if( menu == NULL ) {
        return( false );
    }
    if( !InsertMenu( wnd, info, index, to_replace, true, MENU_HINT ) ) {
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
            return( GUIInsertMenu( wnd, -1, menu, floating ) );
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

bool AddMenuItemToPopup( gui_window *wnd, unsigned id, int offset,
                         gui_menu_struct *info, bool floating )
{
    bool        vbar;
    MENUITEM    *menu;

    vbar = GUIGetMenu( wnd, id + GUI_FIRST_USER_EVENT, &menu, NULL, NULL, floating );
    if( menu == NULL ) {
        return( false );
    }
    if( !InsertMenu( wnd, info, offset, &menu->popup, false, MENU_HINT ) ) {
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

bool GUIAppendMenuToPopup( gui_window *wnd, unsigned id,
                           gui_menu_struct *info, bool floating )
{
    return( AddMenuItemToPopup( wnd, id, -1, info, floating ) );
}

bool GUIInsertMenuToPopup( gui_window *wnd, unsigned id, int offset,
                           gui_menu_struct *menu, bool floating )
{
    return( AddMenuItemToPopup( wnd, id, offset, menu, floating ) );
}

