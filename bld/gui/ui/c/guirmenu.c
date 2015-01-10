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
    if( entry ) {
        if( entry->item ) {
            ResFreeMenuItem( entry->item );
        }
        GUIMemFree( entry );
    }
}

static void WFreeMenuEntries( GUIRMenuEntry *entry )
{
    GUIRMenuEntry *e;

    while( entry ) {
        e = entry;
        entry = entry->next;
        if( e->child ) {
            WFreeMenuEntries ( e->child );
        }
        WFreeMenuEntry( e );
    }
}

static int WMakeMenuItem( uint_8 **data, int *size, MenuItem **new )
{
    char                *text;
    char                *itext;
    int                 msize;
    int                 tlen;
    MenuFlags           normal_flags;

    if( !data || !*data || !size || !*size || !new ) {
        return( false );
    }

    *new = ResNewMenuItem();
    if( *new == NULL ) {
        return( false );
    }

    // Data may not be aligned -- need memcpy on UNIX platforms
    memcpy( &normal_flags, &((MenuItemNormal *)*data)->ItemFlags, sizeof( normal_flags ) );

    (*new)->Item.Normal.ItemFlags = normal_flags;
    (*new)->IsPopup = ( ( normal_flags & MENU_POPUP ) != 0 );
    msize = sizeof( MenuFlags );
    if( (*new)->IsPopup ) {
        text = (char *)(*data);
        text += msize;
    } else {
        uint_16 id;
        // Data may not be aligned -- need memcpy on UNIX platforms
        memcpy( &id, *data+msize, sizeof( uint_16 ) );
        if( !(normal_flags & ~MENU_ENDMENU ) && !id ) {
            (*new)->Item.Normal.ItemFlags |= MENU_SEPARATOR;
        }
        (*new)->Item.Normal.ItemID = id;
        msize += sizeof( uint_16 );
        text = (char *)(*data);
        text += msize;
    }
    tlen = strlen( text ) + 1;
    itext = (char *)GUIMemAlloc( tlen );
    if( !itext ) {
        *size = 0;
        return( false );
    }
    memcpy( itext, text, tlen );
    if( (*new)->IsPopup ) {
        (*new)->Item.Popup.ItemText = itext;
    } else {
        (*new)->Item.Normal.ItemText = itext;
    }
    text += tlen;
    msize += tlen;

    *data = (uint_8 *)text;
    if( *size >= msize ) {
        *size = *size - msize;
    } else {
        *size = 0;
        return( false );
    }

    return( true );
}

static int WAllocMenuEntry( uint_8 **data, int *size, GUIRMenuEntry **entry )
{
    int         ok;

    ok = ( data && *data && size && *size && entry );

    if( ok ) {
        *entry = (GUIRMenuEntry *)GUIMemAlloc( sizeof(GUIRMenuEntry) );
        ok = ( *entry != NULL );
    }

    if( ok ) {
        memset( *entry, 0, sizeof(GUIRMenuEntry) );
        ok = WMakeMenuItem( data, size, &(*entry)->item );
    }

    if( !ok ) {
        if( *entry ) {
            WFreeMenuEntry( *entry );
            *entry = NULL;
        }
    }

    return( ok );
}

static int WMakeMenuEntry( uint_8 **data, int *size,
                           GUIRMenuEntry *parent, GUIRMenuEntry **entry )
{
    GUIRMenuEntry       **current;
    GUIRMenuEntry       *prev;
    int                 ok;

    if( !entry || !data || !size ) {
        return( false );
    }

    *entry = NULL;
    ok = true;

    if( !*data || !*size ) {
        return( true );
    }

    current = entry;
    prev = NULL;

    while( ok && ( *size > 0 ) ) {
        ok = WAllocMenuEntry( data, size, current );
        if( ok ) {
            (*current)->parent = parent;
            (*current)->prev = prev;
            if( (*current)->item->IsPopup ) {
                ok = WMakeMenuEntry( data, size, *current,
                                     &((*current)->child) );
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

static GUIRMenuEntry *WMakeMenuFromData( uint_8 *data, int size )
{
    GUIRMenuEntry       *first;
    int                 ok;

    first = NULL;

    ok = ( data && size );

    if( ok ) {
        data += 2*sizeof(uint_16);
        size -= 2*sizeof(uint_16);
        ok = WMakeMenuEntry( &data, &size, NULL, &first );
    }

    if( !ok ) {
        if( first ) {
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

    while( entry ) {
        count++;
        entry = entry->next;
    }

    return( count );
}

void GUIFreeGUIMenuStruct( gui_menu_struct *entry, int num )
{
    int                 i;

    if( entry ) {
        for( i=0; i<num; i++ ) {
            if( entry[i].num_child_menus ) {
                GUIFreeGUIMenuStruct( entry[i].child, entry[i].num_child_menus );
            }
            if( entry[i].label ) {
                GUIMemFree( entry[i].label );
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

    ok = ( rentry && menu );

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
        for( i = 0, rentry = rmenu;
             ok && i < num_entries && rentry;
             i++, rentry = rentry->next ) {
            ok =  SetGUIMenuStruct( rentry, &menu[i] );
        }
    }

    if( !ok ) {
        if( menu ) {
            GUIMemFree( menu );
            menu = NULL;
        }
    }

    return( menu );
}

bool GUICreateMenuStructFromRes( int id, gui_menu_struct **menu, int *num )
{
    GUIRMenuEntry       *rmenu;
    uint_8              *data;
    int                 size;
    bool                ok;

    data = NULL;
    size = 0;
    rmenu = NULL;

    ok = ( menu && num );

    if( ok ) {
        ok = GUILoadMenuTemplate( id, (char **)&data, &size );
    }

    if( ok ) {
        rmenu = WMakeMenuFromData( data, size );
    }

    if( ok ) {
        *menu = MakeGUIMenuStruct( rmenu );
        *num = WCountMenuChildren( rmenu );
        ok = ( *menu != NULL );
    }

    if( !ok ) {
        if( *menu ) {
            GUIFreeGUIMenuStruct( *menu, *num );
            *menu = NULL;
        }
    }

    if( rmenu ) {
        WFreeMenuEntries( rmenu );
    }

    if( data ) {
        GUIMemFree( data );
    }

    return( ok );
}

