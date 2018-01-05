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
    int         count;

    count = 0;
    for( ; entry != NULL; entry = entry->next ) {
        count++;
    }
    return( count );
}

void GUIFreeGUIMenuStruct( gui_menu_struct *entry, int num )
{
    int                 i;

    if( entry != NULL ) {
        for( i = 0; i < num; i++ ) {
            if( entry[i].num_child_menus > 0 ) {
                GUIFreeGUIMenuStruct( entry[i].child, entry[i].num_child_menus );
            }
            if( entry[i].label != NULL ) {
                GUIMemFree( (void *)entry[i].label );
            }
        }
        GUIMemFree( entry );
    }
}

static gui_menu_styles GetGUIMenuStyles( MenuFlags flags )
{
    gui_menu_styles     styles;

    styles = GUI_ENABLED;

    if( flags & MENU_CHECKED ) {
        styles |= GUI_MENU_CHECKED;
    }
    if( flags & MENU_GRAYED ) {
        styles |= GUI_GRAYED;
    }
    if( flags & MENU_SEPARATOR ) {
        styles |= GUI_SEPARATOR;
    }

    return( styles );
}

static gui_menu_struct *MakeGUIMenuStruct( GUIRMenuEntry *rmenu );

static bool SetGUIMenuStruct( GUIRMenuEntry *rentry, gui_menu_struct *menu )
{
    int                 num_submenus;
    bool                ok;

    ok = ( rentry != NULL && menu != NULL );

    if( ok ) {
        if( rentry->item->IsPopup ) {
            menu->label = GUIStrDup( rentry->item->Item.Popup.ItemText, NULL );
            menu->style = GetGUIMenuStyles( rentry->item->Item.Popup.ItemFlags );
            num_submenus = WCountMenuChildren( rentry->child );
            if( num_submenus ) {
                menu->num_child_menus = num_submenus;
                menu->child = MakeGUIMenuStruct( rentry->child );
                ok = ( menu->child != NULL );
            }
        } else {
            menu->label = GUIStrDup( rentry->item->Item.Normal.ItemText, NULL );
            menu->id = rentry->item->Item.Normal.ItemID;
            menu->style = GetGUIMenuStyles( rentry->item->Item.Normal.ItemFlags );
        }
    }

    return( ok );
}

static gui_menu_struct *MakeGUIMenuStruct( GUIRMenuEntry *rmenu )
{
    GUIRMenuEntry       *rentry;
    gui_menu_struct     *menu;
    int                 num_entries;
    int                 i;
    bool                ok;

    menu = NULL;

    num_entries = WCountMenuChildren( rmenu );
    ok = ( num_entries > 0 );

    if( ok ) {
        menu = GUIMemAlloc( num_entries * sizeof( gui_menu_struct ) );
        ok = ( menu != NULL );
    }

    if( ok ) {
        memset( menu, 0, num_entries * sizeof( gui_menu_struct ) );
        for( i = 0, rentry = rmenu; ok && i < num_entries && rentry; i++, rentry = rentry->next ) {
            ok =  SetGUIMenuStruct( rentry, menu + i );
        }
    }

    if( !ok ) {
        if( menu != NULL ) {
            GUIMemFree( menu );
            menu = NULL;
        }
    }

    return( menu );
}

bool GUICreateMenuStructFromRes( res_name_or_id menu_id, gui_menu_struct **menu, int *num )
{
    GUIRMenuEntry       *rmenu;
    bool                ok;

    rmenu = NULL;

    ok = ( menu != NULL && num != NULL );

    if( ok ) {
        ok = GUISeekMenuTemplate( menu_id );
    }

    if( ok ) {
        rmenu = WMakeMenuFromRes();
    }

    if( ok ) {
        *menu = MakeGUIMenuStruct( rmenu );
        *num = WCountMenuChildren( rmenu );
        ok = ( *menu != NULL );
    }

    if( !ok ) {
        if( *menu != NULL ) {
            GUIFreeGUIMenuStruct( *menu, *num );
            *menu = NULL;
        }
    }

    if( rmenu != NULL ) {
        WFreeMenuEntries( rmenu );
    }
    return( ok );
}
