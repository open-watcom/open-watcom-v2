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
* Description:  Load menu data from resources.
*
****************************************************************************/


#include "guiwind.h"
#include <string.h>
#include <stdlib.h>
#include "watcom.h"
#include "layer0.h"
#include "resmenu.h"
#include "resmenu.h"
#include "guiutil.h"
#include "filefmt.h"
#include "resdiag.h"
#include "resmenu.h"
#include "guildstr.h"
#include "guirmenu.h"


typedef struct GUIRMenuEntry {
    MenuItem                    *item;
    struct GUIRMenuEntry        *next;
    struct GUIRMenuEntry        *prev;
    struct GUIRMenuEntry        *parent;
    struct GUIRMenuEntry        *child;
} GUIRMenuEntry;

static void WFreeMenuEntry( GUIRMenuEntry *entry )
{
    if( entry != NULL ) {
        if( entry->item != NULL ) {
            ResFreeMenuItem( entry->item );
        }
        GUIMemFree( entry );
    }
}

static void WFreeMenuEntries( GUIRMenuEntry *entry )
{
    GUIRMenuEntry *next;

    for( ; entry != NULL; entry = next ) {
        next = entry->next;
        if( entry->child != NULL ) {
            WFreeMenuEntries ( entry->child );
        }
        WFreeMenuEntry( entry );
    }
}

static bool WMakeMenuItem( MenuItem **new )
{
    if( new == NULL ) {
        return( false );
    }

    *new = ResNewMenuItem();
    if( *new == NULL ) {
        return( false );
    }

    return( !GUIResReadMenuItem( *new ) );
}

static bool WAllocMenuEntry( GUIRMenuEntry **entry )
{
    bool    ok;

    ok = ( entry != NULL );

    if( ok ) {
        *entry = (GUIRMenuEntry *)GUIMemAlloc( sizeof( GUIRMenuEntry ) );
        ok = ( *entry != NULL );
    }

    if( ok ) {
        memset( *entry, 0, sizeof( GUIRMenuEntry ) );
        ok = WMakeMenuItem( &(*entry)->item );
    }

    if( !ok ) {
        if( *entry != NULL ) {
            WFreeMenuEntry( *entry );
            *entry = NULL;
        }
    }

    return( ok );
}

static bool WMakeMenuEntry( GUIRMenuEntry *parent, GUIRMenuEntry **entry )
{
    GUIRMenuEntry       **current;
    GUIRMenuEntry       *prev;
    bool                ok;

    if( entry == NULL ) {
        return( false );
    }

    *entry = NULL;
    ok = true;

    current = entry;
    prev = NULL;

    while( ok ) {
        ok = WAllocMenuEntry( current );
        if( ok ) {
            (*current)->parent = parent;
            (*current)->prev = prev;
            if( (*current)->item->IsPopup ) {
                ok = WMakeMenuEntry( *current, &((*current)->child) );
            }
            if( (*current)->item->Item.Normal.ItemFlags & MENU_ENDMENU ) {
                break;
            }
            prev = *current;
            current = &((*current)->next);
        }
    }

    if( !ok ) {
        if( *entry ) {
            WFreeMenuEntries( *entry );
            *entry = NULL;
        }
    }

    return( ok );
}

static GUIRMenuEntry *WMakeMenuFromRes( void )
{
    GUIRMenuEntry       *first;
    bool                ok;
    MenuHeader          menuh;

    first = NULL;

    ok = !GUIResReadMenuHeader( &menuh );

    if( ok ) {
        ok = WMakeMenuEntry( NULL, &first );
    }

    if( !ok ) {
        if( first != NULL ) {
            WFreeMenuEntries( first );
            first = NULL;
        }
    }

    return( first );
}

static int WCountMenuChildren( GUIRMenuEntry *entry )
{
    int         num_items;

    num_items = 0;
    for( ; entry != NULL; entry = entry->next ) {
        num_items++;
    }
    return( num_items );
}

void GUIFreeGUIMenuStruct( gui_menu_items *menus )
{
    int         i;

    if( menus->num_items > 0 ) {
        for( i = 0; i < menus->num_items; i++ ) {
            GUIFreeGUIMenuStruct( &menus->menu[i].child );
            if( menus->menu[i].label != NULL ) {
                GUIMemFree( (void *)menus->menu[i].label );
            }
        }
        GUIMemFree( menus->menu );
    }
}

static gui_menu_styles GetGUIMenuStyles( MenuFlags flags )
{
    gui_menu_styles     styles;

    styles = GUI_STYLE_MENU_ENABLED;

    if( flags & MENU_CHECKED ) {
        styles |= GUI_STYLE_MENU_CHECKED;
    }
    if( flags & MENU_GRAYED ) {
        styles |= GUI_STYLE_MENU_GRAYED;
    }
    if( flags & MENU_SEPARATOR ) {
        styles |= GUI_STYLE_MENU_SEPARATOR;
    }

    return( styles );
}

static bool MakeGUIMenuStruct( GUIRMenuEntry *rmenu, gui_menu_items *menus )
{
    gui_menu_struct     *menu;
    gui_menu_struct     *menuitem;
    int                 num_items;
    bool                ok;

    menu = NULL;
    num_items = WCountMenuChildren( rmenu );
    ok = ( num_items > 0 );
    if( ok ) {
        menu = GUIMemAlloc( num_items * sizeof( gui_menu_struct ) );
        if( menu == NULL ) {
            ok = false;
        }
    }

    if( ok ) {
        menuitem = menu;
        for( ; rmenu != NULL; rmenu = rmenu->next ) {
            menuitem->hinttext = NULL;
            menuitem->child = NoMenu;
            if( rmenu->item->IsPopup ) {
                menuitem->label = GUIStrDup( rmenu->item->Item.Popup.ItemText, NULL );
                menuitem->id = 0;
                menuitem->style = GetGUIMenuStyles( rmenu->item->Item.Popup.ItemFlags );
                ok = MakeGUIMenuStruct( rmenu->child, &menuitem->child );
                if( !ok ) {
                    GUIMemFree( menu );
                    return( ok );
                }
            } else {
                menuitem->label = GUIStrDup( rmenu->item->Item.Normal.ItemText, NULL );
                menuitem->id = rmenu->item->Item.Normal.ItemID;
                menuitem->style = GetGUIMenuStyles( rmenu->item->Item.Normal.ItemFlags );
            }
            menuitem++;
        }
        menus->menu = menu;
        menus->num_items = num_items;
    }

    return( ok );
}

bool GUICreateMenuStructFromRes( res_name_or_id menu_id, gui_menu_items *menus )
{
    GUIRMenuEntry       *rmenu;
    bool                ok;

    rmenu = NULL;

    ok = ( menus != NULL );
    if( ok ) {
        ok = GUISeekMenuTemplate( menu_id );
    }

    if( ok ) {
        rmenu = WMakeMenuFromRes();
    }

    if( ok ) {
        ok = MakeGUIMenuStruct( rmenu, menus );
    }

    if( !ok ) {
        GUIFreeGUIMenuStruct( menus );
    }

    if( rmenu != NULL ) {
        WFreeMenuEntries( rmenu );
    }
    return( ok );
}
