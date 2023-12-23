/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#include "wglbl.h"
#include "wmenu.h"
#include "winfo.h"
#include "wribbon.h"
#include "wstrdup.h"
#include "wcopystr.h"
#include "wresall.h"
#include "wmsg.h"
#include "ldstr.h"
#include "sysall.rh"
#include "wrdll.h"
#include "w_menu.h"


/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static WMenuEntry   *WDummyMenuEntry = NULL;

void WInitDummyMenuEntry( void )
{
    WDummyMenuEntry = (WMenuEntry *)WRMemAlloc( sizeof( WMenuEntry ) );
    memset( WDummyMenuEntry, 0, sizeof( WMenuEntry ) );
    WDummyMenuEntry->item = ResNewMenuItem();
    WDummyMenuEntry->item->Item.Normal.ItemText = AllocRCString( W_MENUITEM );
    WDummyMenuEntry->item->Item.Normal.ItemID = 101;
}

void WFiniDummyMenuEntry( void )
{
    WFreeMenuEntry( WDummyMenuEntry );
    WDummyMenuEntry = NULL;
}

WMenuEditInfo *WAllocMenuEditInfo( void )
{
    WMenuEditInfo *einfo;

    einfo = (WMenuEditInfo *)WRMemAlloc( sizeof( WMenuEditInfo ) );

    if( einfo != NULL ) {
        memset( einfo, 0, sizeof( WMenuEditInfo ) );
        einfo->current_pos = LB_ERR;
    }

    return( einfo );
}

void WFreeMenuEditInfo( WMenuEditInfo *einfo )
{
    if( einfo != NULL ) {
        if( einfo->menu != NULL ) {
            WFreeMenu( einfo->menu );
            einfo->menu = NULL;
        }
        if( einfo->wsb != NULL ) {
            WDestroyStatusLine( einfo->wsb );
            einfo->wsb = NULL;
        }
        if( einfo->ribbon != NULL ) {
            WDestroyRibbon( einfo );
        }
        if( einfo->preview_window != (HWND)NULL && IsWindow( einfo->preview_window ) ) {
            DestroyWindow( einfo->preview_window );
            einfo->preview_window = (HWND)NULL;
        }
        if( einfo->edit_dlg != (HWND)NULL && IsWindow( einfo->edit_dlg ) ) {
            DestroyWindow( einfo->edit_dlg );
            einfo->edit_dlg = (HWND)NULL;
        }
        if( einfo->win != (HWND)NULL && IsWindow( einfo->win ) ) {
            SET_WNDINFO( einfo->win, 0 );
            DestroyWindow( einfo->win );
            einfo->win = (HWND)NULL;
        }
        if( einfo->file_name != NULL ) {
            WRMemFree( einfo->file_name );
        }
        WRMemFree( einfo );
    }
}

static char *WInitDataFromMenu( WMenuEntry *entry, char *data )
{
    char                *item_text;
    unsigned            item_flags;

    for( ; entry != NULL; entry = entry->next ) {
        if( entry->item->IsPopup ) {
            /* menu item flags */
            item_flags = entry->item->Item.Popup.ItemFlags & ~MENU_ENDMENU;
            if( entry->next == NULL ) {
                item_flags |= MENU_ENDMENU;
            }
            VALU16( data ) = item_flags;
            INCU16( data );
            item_text = entry->item->Item.Popup.ItemText;
        } else {
            /* menu item flags */
            if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
                item_flags = 0;
            } else {
                item_flags = (entry->item->Item.Normal.ItemFlags & ~MENU_ENDMENU) | MF_STRING;
            }
            if( entry->next == NULL ) {
                item_flags |= MENU_ENDMENU;
            }
            VALU16( data ) = item_flags;
            INCU16( data );
            /* menu item id */
            if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
                VALU16( data ) = 0;
            } else {
                VALU16( data ) = entry->item->Item.Normal.ItemID;
            }
            INCU16( data );
            item_text = entry->item->Item.Normal.ItemText;
        }

        /* menu item text */
        data += WRDataFromString( item_text, entry->is32bit, data );

        /* popup menu childs */
        if( entry->item->IsPopup ) {
            if( entry->child != NULL ) {
                data = WInitDataFromMenu( entry->child, data );
            } else {
                WDummyMenuEntry->is32bit = entry->is32bit;
                data = WInitDataFromMenu( WDummyMenuEntry, data );
            }
        }
    }

    return( data );
}

static size_t WCalcMenuSize( WMenuEntry *entry )
{
    size_t      dsize;
    char        *text;

    if( entry == NULL ) {
        return( 0 );
    }

    dsize = 0;
    for( ; entry != NULL; entry = entry->next ) {
        if( entry->item->IsPopup ) {
            dsize += SIZEU16 + 1;
            text = entry->item->Item.Popup.ItemText;
            if( entry->child != NULL ) {
                dsize += WCalcMenuSize( entry->child );
            } else {
                WDummyMenuEntry->is32bit = entry->is32bit;
                dsize += WCalcMenuSize( WDummyMenuEntry );
            }
        } else {
            dsize += SIZEU16 + SIZEU16 + 1;
            text = entry->item->Item.Normal.ItemText;
        }
        dsize += WRCalcStrlen( text, entry->is32bit );
    }

    return( dsize );
}

void WMakeDataFromMenu( WMenu *menu, char **pdata, size_t *dsize )
{
    char *data;

    if( pdata != NULL && dsize != NULL ) {
        *dsize = SIZEU16 + SIZEU16 + WCalcMenuSize( menu->first_entry );
        if( *dsize != 0 ) {
            *pdata = data = WRMemAlloc( *dsize );
            if( data != NULL ) {
                /* menu resource header */
                VALU16( data ) = 0;
                INCU16( data );
                VALU16( data ) = 0;
                INCU16( data );
                WInitDataFromMenu( menu->first_entry, data );
            }
        } else {
            *pdata = NULL;
        }
    }
}

static bool WMakeMenuItemFromData( const char **pdata, size_t *dsize, MenuItem **pitem, bool is32bit )
{
    const char          *data;
    char                *itext;
    size_t              tlen;
    MenuFlags           item_flags;
    unsigned            item_id;
    MenuItem            *item;

    if( pdata == NULL || *pdata == NULL || dsize == NULL || *dsize == 0 || pitem == NULL ) {
        return( false );
    }

    *pitem = item = ResNewMenuItem();
    if( item == NULL ) {
        return( false );
    }

    data = *pdata;
    item_flags = VALU16( data );
    INCU16( data );
    item->IsPopup = ( (item_flags & MENU_POPUP) != 0 );
    if( !item->IsPopup ) {
        item_id = VALU16( data );
        INCU16( data );
        if( (item_flags & ~MENU_ENDMENU) == 0 && item_id == 0 ) {
            item_flags |= MENU_SEPARATOR;
        }
        item->Item.Normal.ItemID = item_id;
    }
    item->Item.Normal.ItemFlags = item_flags;
    itext = WRStringFromData( &data, is32bit );
    if( itext == NULL ) {
        *dsize = 0;
        return( false );
    }
    if( item->IsPopup ) {
        item->Item.Popup.ItemText = itext;
    } else {
        item->Item.Normal.ItemText = itext;
    }
    tlen = data - *pdata;
    *pdata = data;
    if( *dsize >= tlen ) {
        *dsize -= tlen;
        return( true );
    }
    *dsize = 0;
    return( false );

}

static bool WAllocMenuEntryFromData( const char **data, size_t *dsize, WMenuEntry **pentry, bool is32bit )
{
    bool        ok;
    WMenuEntry  *entry;

    ok = (data != NULL && *data != NULL && dsize != NULL && *dsize != 0 && pentry != NULL);

    if( ok ) {
        *pentry = entry = (WMenuEntry *)WRMemAlloc( sizeof( WMenuEntry ) );
        ok = (entry != NULL);
    }

    if( ok ) {
        memset( entry, 0, sizeof( WMenuEntry ) );
        entry->is32bit = is32bit;
        ok = WMakeMenuItemFromData( data, dsize, &entry->item, is32bit );
    }

    if( !ok ) {
        if( entry != NULL ) {
            WFreeMenuEntry( entry );
            *pentry = NULL;
        }
    }

    return( ok );
}

static bool WMakeMenuEntryFromData( const char **data, size_t *dsize, WMenuEntry *parent,
                            WMenuEntry **entry, bool is32bit )
{
    bool        ok;
    WMenuEntry  **current;
    WMenuEntry  *prev;

    if( entry == NULL || data == NULL || dsize == NULL ) {
        return( false );
    }

    *entry = NULL;
    ok = true;

    if( *data == NULL || *dsize == 0 ) {
        return( true );
    }

    current = entry;
    prev = NULL;

    while( ok && *dsize > 0 ) {
        ok = WAllocMenuEntryFromData( data, dsize, current, is32bit );
        if( ok ) {
            (*current)->parent = parent;
            (*current)->prev = prev;
            if( (*current)->item->IsPopup ) {
                ok = WMakeMenuEntryFromData( data, dsize, *current, &(*current)->child, is32bit );
            }
            if( (*current)->item->Item.Normal.ItemFlags & MENU_ENDMENU ) {
                break;
            }
            prev = *current;
            current = &(*current)->next;
        }
    }

    if( !ok ) {
        if( *entry != NULL ) {
            WFreeMenuEntries( *entry );
            *entry = NULL;
        }
    }

    return( ok );
}

WMenu *WMakeMenuFromInfo( WMenuInfo *info )
{
    WMenu       *menu;
    bool        ok;
    const char  *data;
    size_t      dsize;

    menu = NULL;

    ok = (info != NULL);

    if( ok ) {
        menu = (WMenu *)WRMemAlloc( sizeof( WMenu ) );
        ok = ( menu != NULL );
    }

    if( ok ) {
        menu->first_entry = NULL;
        if( info->data != NULL ) {
            ok = ( VALU16( info->data ) == 0 );
            if( !ok ) {
                WDisplayErrorMsg( W_NOMENUEX );
            }
        }
    }

    if( ok ) {
        menu->is32bit = info->is32bit;
        if( info->data != NULL ) {
            data = info->data + SIZEU16 + SIZEU16;
            dsize = info->data_size - SIZEU16 - SIZEU16;
            ok = WMakeMenuEntryFromData( &data, &dsize, NULL, &menu->first_entry, info->is32bit );
        }
    }

    if( ok ) {
        info->data = NULL;
        info->data_size = 0;
    } else {
        if( menu != NULL ) {
            WFreeMenu( menu );
            menu = NULL;
        }
    }

    return( menu );
}

void WFreeMenu( WMenu *menu )
{
    if( menu != NULL ) {
        WFreeMenuEntries( menu->first_entry );
        WRMemFree( menu );
    }
}

void WFreeMenuEntries( WMenuEntry *entry )
{
    WMenuEntry *next;

    for( ; entry != NULL; entry = next ) {
        next = entry->next;
        if( entry->child != NULL ) {
            WFreeMenuEntries( entry->child );
        }
        WFreeMenuEntry( entry );
    }
}

void WFreeMenuEntry( WMenuEntry *entry )
{
    if( entry != NULL ) {
        if( entry->item != NULL ) {
            ResFreeMenuItem( entry->item );
        }
        if( entry->symbol != NULL ) {
            WRMemFree( entry->symbol );
        }
        WRMemFree( entry );
    }
}

bool WRemoveMenuEntry( WMenu *menu, WMenuEntry *entry )
{
    if( menu == NULL || entry == NULL ) {
        return( false );
    }

    if( menu->first_entry == entry ) {
        menu->first_entry = entry->next;
    } else {
        if( entry->parent != NULL && entry->parent->child == entry ) {
            entry->parent->child = entry->next;
        }
    }
    if( entry->next != NULL ) {
        entry->next->prev = entry->prev;
    }
    if( entry->prev != NULL ) {
        entry->prev->next = entry->next;
    }

    return( true );
}

static bool WAddItemAtPos( HMENU hparent, int pos, WMenuEntry *entry )
{
    MenuFlags   flags;
    bool        ok;

    ok = (hparent != (HMENU)NULL && entry != NULL);

    if( ok ) {
        flags = entry->item->Item.Normal.ItemFlags;
        flags &= ~MENU_ENDMENU;
        if( flags & MENU_POPUP ) {
            entry->preview_popup = CreatePopupMenu();
            if( entry->preview_popup == (HMENU)NULL ) {
                return( false );
            }
            ok = InsertMenu( hparent, pos, MF_BYPOSITION | flags,
                             (UINT)(pointer_uint)entry->preview_popup,
                             entry->item->Item.Popup.ItemText ) != 0;
        } else if( flags & MENU_SEPARATOR ) {
            ok = InsertMenu( hparent, pos, MF_BYPOSITION | flags,
                             entry->preview_id, NULL ) != 0;
        } else {
            ok = InsertMenu( hparent, pos, MF_BYPOSITION | flags,
                             entry->preview_id,
                             entry->item->Item.Normal.ItemText ) != 0;
        }
    }

    return( ok );
}

static bool WInsertEntryIntoPreview( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    int         pos;
    HMENU       hmenu;
    WMenuEntry  *start;

    if( einfo == NULL || entry == NULL ) {
        return( false );
    }

    if( entry->parent != NULL ) {
        hmenu = entry->parent->preview_popup;
        start = entry->parent->child;
    } else {
        hmenu = GetMenu( einfo->preview_window );
        start = einfo->menu->first_entry;
    }

    if( hmenu == (HMENU)NULL ) {
        return( false );
    }

    pos = -1;
    for( ; start != NULL; start = start->next ) {
        pos++;
        if( start == entry ) {
            break;
        }
    }

    if( pos == -1 ) {
        return( false );
    }

    entry->preview_id = einfo->first_preview_id;
    einfo->first_preview_id++;
    if( einfo->first_preview_id == LAST_PREVIEW_ID ) {
        return( false );
    }

    if( !WAddItemAtPos( hmenu, pos, entry ) ) {
        return( false );
    }

    if( entry->parent == NULL ) {
        DrawMenuBar( einfo->preview_window );
    }

    return( true );
}

bool WInsertEntryIntoMenu( WMenuEditInfo *einfo, WMenuEntry *after,
                           WMenuEntry *parent, WMenuEntry *entry,
                           bool popup )
{
    bool ok;

    ok = (einfo != NULL && einfo->menu != NULL && entry != NULL);

    if( ok ) {
        if( after != NULL ) {
            if( popup ) {
                ok = WInsertEntryIntoMenu( einfo, NULL, after, entry, FALSE );
            } else {
                if( after->next != NULL ) {
                    after->next->prev = entry;
                }
                entry->next = after->next;
                entry->prev = after;
                after->next = entry;
                entry->parent = after->parent;
                ok = WInsertEntryIntoPreview( einfo, entry );
                if( !ok ) {
                    WRemoveMenuEntry( einfo->menu, entry );
                }
            }
        } else {
            if( parent != NULL ) {
                entry->next = parent->child;
                parent->child = entry;
            } else {
                entry->next = einfo->menu->first_entry;
                einfo->menu->first_entry = entry;
            }
            entry->prev = NULL;
            entry->parent = parent;
            ok = WInsertEntryIntoPreview( einfo, entry );
            if( !ok ) {
                WRemoveMenuEntry( einfo->menu, entry );
            }
        }
    }

    return( ok );
}

int WGetMenuEntryDepth( WMenuEntry *entry )
{
    int depth;

    depth = -1;
    for( ; entry != NULL; entry = entry->parent ) {
        depth++;
    }
    return( depth );
}

int WCountMenuChildren( WMenuEntry *entry )
{
    int count;

    count = 0;
    for( ; entry != NULL; entry = entry->next ) {
        count += WCountMenuChildren( entry->child );
        count++;
    }

    return( count );
}

static bool WResetPreviewID( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    bool    ok;

    for( ; entry != NULL; entry = entry->next ) {
        entry->preview_id = einfo->first_preview_id;
        einfo->first_preview_id++;
        if( einfo->first_preview_id == LAST_PREVIEW_ID ) {
            return( false );
        }
        if( entry->child != NULL ) {
            ok = WResetPreviewID( einfo, entry->child );
            if( !ok ) {
                break;
            }
        }
    }

    return( ok );
}

bool WResetPreviewIDs( WMenuEditInfo *einfo )
{
    if( einfo == NULL ) {
        return( false );
    }

    einfo->first_preview_id = FIRST_PREVIEW_ID;

    return( WResetPreviewID( einfo, einfo->menu->first_entry ) );
}

static bool WModifyItemAtPos( HMENU hparent, int pos, WMenuEntry *entry )
{
    MenuFlags   flags;
    bool        ok;

    ok = (hparent != (HMENU)NULL && entry != NULL);

    if( ok ) {
        flags = entry->item->Item.Normal.ItemFlags;
        flags &= ~MENU_ENDMENU;
        if( flags & MENU_POPUP ) {
            ok = ModifyMenu( hparent, pos, MF_BYPOSITION | flags,
                             (UINT_PTR)entry->preview_popup,
                             entry->item->Item.Popup.ItemText ) != 0;
        } else if( flags & MENU_SEPARATOR ) {
            // do nothing
        } else {
            ok = ModifyMenu( hparent, pos, MF_BYPOSITION | flags,
                             entry->preview_id,
                             entry->item->Item.Normal.ItemText ) != 0;
        }
    }

    return( ok );
}

static bool WAddToPreviewMenu( HMENU hparent, WMenuEntry *entry )
{
    bool        ok;
    int         pos;

    ok = (hparent != (HMENU)NULL && entry != NULL);

    pos = 0;
    for( ; ok && entry != NULL; entry = entry->next ) {
        ok = WAddItemAtPos( hparent, pos, entry );
        if( ok && entry->item->IsPopup ) {
            if( entry->child != NULL ) {
                ok = WAddToPreviewMenu( entry->preview_popup, entry->child );
            }
        }
        pos++;
    }

    return( ok );
}

HMENU WCreatePreviewMenu( WMenuEditInfo *einfo )
{
    HMENU       hmenu;

    if( einfo == NULL ) {
        return( (HMENU)NULL );
    }

    hmenu = CreateMenu();
    if( hmenu == (HMENU)NULL ) {
        return( hmenu );
    }

    WAddToPreviewMenu( hmenu, einfo->menu->first_entry );

    return( hmenu );
}

WMenuEntry *WFindEntryFromPreviewID( WMenuEntry *entry, WORD id )
{
    WMenuEntry  *found;

    for( ; entry != NULL; entry = entry->next ) {
        if( entry->preview_id == id ) {
            return( entry );
        }
        if( entry->child != NULL ) {
            found = WFindEntryFromPreviewID( entry->child, id );
            if( found != NULL ) {
                return( found );
            }
        }
    }

    return( NULL );
}

WMenuEntry *WFindEntryFromPreviewPopup( WMenuEntry *entry, HMENU hpopup )
{
    WMenuEntry  *found;

    for( ; entry != NULL; entry = entry->next ) {
        if( entry->preview_popup == hpopup ) {
            return( entry );
        }
        if( entry->child != NULL ) {
            found = WFindEntryFromPreviewPopup( entry->child, hpopup );
            if( found != NULL ) {
                return( found );
            }
        }
    }

    return( NULL );
}

bool WFindEntryLBPos( WMenuEntry *start, WMenuEntry *entry, LRESULT *pos )
{
    for( ; start != NULL; start = start->next ) {
        *pos = *pos + 1;
        if( start == entry ) {
            return( true );
        }
        if( start->child != NULL ) {
            if ( WFindEntryLBPos( start->child, entry, pos ) ) {
                return( true );
            }
        }
    }

    return( false );
}

bool WModifyEntryInPreview( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    int         pos;
    HMENU       hmenu;
    WMenuEntry  *start;

    if( einfo == NULL || entry == NULL ) {
        return( false );
    }

    if( entry->parent != NULL ) {
        hmenu = entry->parent->preview_popup;
        start = entry->parent->child;
    } else {
        hmenu = GetMenu( einfo->preview_window );
        start = einfo->menu->first_entry;
    }

    if( hmenu == (HMENU)NULL ) {
        return( false );
    }

    pos = -1;
    for( ; start != NULL; start = start->next ) {
        pos++;
        if( start == entry ) {
            break;
        }
    }

    if( pos == -1 ) {
        return( false );
    }

    if( !WModifyItemAtPos( hmenu, pos, entry ) ) {
        return( false );
    }

    if( entry->parent == NULL ) {
        DrawMenuBar( einfo->preview_window );
    }

    return( true );
}

bool WMakeClipDataFromMenuEntry( WMenuEntry *entry, char **data, size_t *dsize )
{
    WMenu       menu;
    WMenuEntry  save;
    bool        ok;

    ok = (entry != NULL && data != NULL && dsize != NULL);

    if( ok ) {
        memcpy( &save, entry, sizeof( WMenuEntry ) );
        menu.is32bit = entry->is32bit;
        menu.first_entry = entry;
        entry->next = NULL;
        entry->prev = NULL;
        entry->parent = NULL;
        WMakeDataFromMenu( &menu, data, dsize );
        ok = (*data != NULL && *dsize != 0);
        if( ok ) {
            (*data)[0] = entry->is32bit;
        }
        memcpy( entry, &save, sizeof( WMenuEntry ) );
    }

    return( ok );
}

WMenuEntry *WMakeMenuEntryFromClipData( const char *data, size_t dsize )
{
    WMenuEntry  *entry;
    bool        is32bit;
    bool        ok;

    entry = NULL;

    ok = (data != NULL && dsize != 0);

    if( ok ) {
        is32bit = data[0];
        data += SIZEU16 + SIZEU16;
        dsize -= SIZEU16 + SIZEU16;
        ok = WMakeMenuEntryFromData( &data, &dsize, NULL, &entry, is32bit );
    }

    if( !ok ) {
        if( entry != NULL ) {
            WFreeMenuEntries( entry );
            entry = NULL;
        }
    }

    return( entry );
}

bool WResolveMenuEntries( WMenuEditInfo *einfo )
{
    if( einfo->menu == NULL ) {
        return( false );
    }

    if( einfo->menu->first_entry == NULL ) {
        return( true );
    }

    return( WResolveEntries( einfo->menu->first_entry, einfo->info->symbol_table ) );
}

bool WResolveEntries( WMenuEntry *entry, WRHashTable *symbol_table )
{
    if( entry == NULL || symbol_table == NULL ) {
        return( false );
    }

    for( ; entry != NULL; entry = entry->next ) {
        if( entry->child != NULL ) {
            WResolveEntries( entry->child, symbol_table );
        }
        WResolveEntrySymbol( entry, symbol_table );
    }

    return( true );
}

bool WResolveEntrySymbol( WMenuEntry *entry, WRHashTable *symbol_table )
{
    uint_16             id;
    MenuFlags           flags;
    WRHashValueList     *vlist;
    bool                ok;

    vlist = NULL;

    ok = (entry != NULL && symbol_table != NULL);

    if( ok ) {
        if( entry->item->IsPopup ) {
            return( true );
        }
        flags = entry->item->Item.Normal.ItemFlags;
        if( flags & MENU_SEPARATOR ) {
            return( true );
        }
        id = entry->item->Item.Normal.ItemID;
        vlist = WRLookupValue( symbol_table, id );
        ok = (vlist != NULL && vlist->next == NULL);
    }

    if( ok ) {
        if( entry->symbol != NULL ) {
            WRMemFree( entry->symbol );
        }
        entry->symbol = WStrDup( vlist->entry->name );
        ok = (entry->symbol != NULL);
    }

    if( vlist != NULL ) {
        WRValueListFree( vlist );
    }

    return( ok );
}

bool WResolveMenuSymIDs( WMenuEditInfo *einfo )
{
    if( einfo->menu == NULL ) {
        return( false );
    }

    if( einfo->menu->first_entry == NULL ) {
        return( true );
    }

    return( WResolveSymIDs( einfo->menu->first_entry, einfo->info->symbol_table ) );
}

bool WResolveSymIDs( WMenuEntry *entry, WRHashTable *symbol_table )
{
    if( entry == NULL || symbol_table == NULL ) {
        return( false );
    }

    for( ; entry != NULL; entry = entry->next ) {
        if( entry->child != NULL ) {
            WResolveSymIDs( entry->child, symbol_table );
        }
        WResolveEntrySymIDs( entry, symbol_table );
    }

    return( true );
}

bool WResolveEntrySymIDs( WMenuEntry *entry, WRHashTable *symbol_table )
{
    MenuFlags           flags;
    WRHashValue         hv;
    bool                ok;

    ok = (entry != NULL && symbol_table != NULL);

    if( ok ) {
        if( entry->item->IsPopup ) {
            return( true );
        }
        flags = entry->item->Item.Normal.ItemFlags;
        if( flags & MENU_SEPARATOR ) {
            return( true );
        }
        ok = WRLookupName( symbol_table, entry->symbol, &hv );
    }

    if( ok ) {
        entry->item->Item.Normal.ItemID = (uint_16)hv;
    }

    return( ok );
}
