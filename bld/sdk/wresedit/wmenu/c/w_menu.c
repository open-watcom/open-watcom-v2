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


#include "commonui.h"
#include <string.h>
#include "watcom.h"
#include "wglbl.h"
#include "wmenu.h"
#include "winfo.h"
#include "wribbon.h"
#include "wstrdup.h"
#include "wcopystr.h"
#include "wresall.h"
#include "wmsg.h"
#include "ldstr.h"
#include "rcstr.gh"
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
static void *WInitDataFromMenu( WMenuEntry *, void * );
static size_t WCalcMenuSize( WMenuEntry * );
static bool WMakeMenuEntryFromData( void **, size_t *, WMenuEntry *, WMenuEntry **, bool );
static bool WAllocMenuEntryFromData( void **, size_t *, WMenuEntry **, bool );
static bool WMakeMenuItemFromData( void **data, size_t *size, MenuItem **new, bool );
static bool WInsertEntryIntoPreview( WMenuEditInfo *, WMenuEntry * );

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

WMenuEditInfo *WAllocMenuEInfo( void )
{
    WMenuEditInfo *einfo;

    einfo = (WMenuEditInfo *)WRMemAlloc( sizeof( WMenuEditInfo ) );

    if( einfo != NULL ) {
        memset( einfo, 0, sizeof( WMenuEditInfo ) );
        einfo->current_pos = -1;
    }

    return( einfo );
}

void WFreeMenuEInfo( WMenuEditInfo *einfo )
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
            SetWindowLong( einfo->win, 0, (LONG)0 );
            DestroyWindow( einfo->win );
            einfo->win = (HWND)NULL;
        }
        if( einfo->file_name != NULL ) {
            WRMemFree( einfo->file_name );
        }
        WRMemFree( einfo );
    }
}

void WMakeDataFromMenu( WMenu *menu, void **data, size_t *size )
{
    char *tdata;

    if( data != NULL && size != NULL ) {
        *size = WCalcMenuSize( menu->first_entry ) + 2 * sizeof(WORD);
        if( *size != 0 ) {
            *data = WRMemAlloc( *size );
            if( *data != NULL ) {
                tdata = *data;
                memset( tdata, 0, 2 * sizeof( WORD ) );
                tdata = tdata + 2 * sizeof( WORD );
                WInitDataFromMenu( menu->first_entry, tdata );
            }
        } else {
            *data = NULL;
        }
    }
}

bool WMakeMenuItemFromData( void **data, size_t *size, MenuItem **new, bool is32bit )
{
    MenuItemNormal      *normal;
    char                *text;
    char                *itext;
    size_t              msize;
    size_t              tlen;
    size_t              itlen;

    if( data == NULL || *data == NULL || size == NULL || *size == 0 || new == NULL ) {
        return( false );
    }

    *new = ResNewMenuItem();
    if( *new == NULL ) {
        return( false );
    }

    normal = (MenuItemNormal *)*data;
    (*new)->Item.Normal.ItemFlags = normal->ItemFlags;
    (*new)->IsPopup = ((normal->ItemFlags & MENU_POPUP) != 0);
    msize = sizeof( MenuFlags );
    if( (*new)->IsPopup ) {
        text = (char *)(*data);
        text += msize;
    } else {
        if( !(normal->ItemFlags & ~MENU_ENDMENU) && !normal->ItemID ) {
            (*new)->Item.Normal.ItemFlags |= MENU_SEPARATOR;
        }
        (*new)->Item.Normal.ItemID = normal->ItemID;
        msize += sizeof( uint_16 );
        text = (char *)(*data);
        text += msize;
    }
    tlen = WRStrlen( text, is32bit ) + 1;
    if( is32bit ) {
        tlen++;
    }

    if( is32bit ) {
        itext = NULL;
        WRunicode2mbcs( text, &itext, &itlen );
    } else {
        itext = (char *)WRMemAlloc( tlen );
        if( itext != NULL ) {
            memcpy( itext, text, tlen );
        }
    }

    if( itext == NULL ) {
        *size = 0;
        return( false );
    }

    if( (*new)->IsPopup ) {
        (*new)->Item.Popup.ItemText = itext;
    } else {
        (*new)->Item.Normal.ItemText = itext;
    }
    text += tlen;
    msize += tlen;

    *data = text;
    if( *size >= msize ) {
        *size = *size - msize;
    } else {
        *size = 0;
        return( false );
    }

    return( true );
}

bool WAllocMenuEntryFromData( void **data, size_t *size, WMenuEntry **entry, bool is32bit )
{
    bool        ok;

    ok = (data != NULL && *data != NULL && size != NULL && *size != 0 && entry != NULL);

    if( ok ) {
        *entry = (WMenuEntry *)WRMemAlloc( sizeof( WMenuEntry ) );
        ok = (*entry != NULL);
    }

    if( ok ) {
        memset( *entry, 0, sizeof( WMenuEntry ) );
        (*entry)->is32bit = is32bit;
        ok = WMakeMenuItemFromData( data, size, &(*entry)->item, is32bit );
    }

    if( !ok ) {
        if( *entry != NULL ) {
            WFreeMenuEntry( *entry );
            *entry = NULL;
        }
    }

    return( ok );
}

bool WMakeMenuEntryFromData( void **data, size_t *size, WMenuEntry *parent,
                            WMenuEntry **entry, bool is32bit )
{
    bool        ok;
    WMenuEntry  **current;
    WMenuEntry  *prev;

    if( entry == NULL || data == NULL || size == NULL ) {
        return( FALSE );
    }

    *entry = NULL;
    ok = true;

    if( *data == NULL || *size == 0 ) {
        return( TRUE );
    }

    current = entry;
    prev = NULL;

    while( ok && *size > 0 ) {
        ok = WAllocMenuEntryFromData( data, size, current, is32bit );
        if( ok ) {
            (*current)->parent = parent;
            (*current)->prev = prev;
            if( (*current)->item->IsPopup ) {
                ok = WMakeMenuEntryFromData( data, size, *current,
                                             &(*current)->child, is32bit );
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
    void        *data;
    size_t      size;

    menu = NULL;

    ok = (info != NULL);

    if( ok ) {
        menu = (WMenu *)WRMemAlloc( sizeof( WMenu ) );
        ok = (menu != NULL);
    }

    if( ok ) {
        menu->first_entry = NULL;
        if( info->data != NULL ) {
            ok = (((WORD *)info->data)[0] == 0);
            if( !ok ) {
                WDisplayErrorMsg( W_NOMENUEX );
            }
        }
    }

    if( ok ) {
        menu->is32bit = info->is32bit;
        if( info->data != NULL ) {
            data = (char *)info->data + 2 * sizeof( WORD );
            size = info->data_size - 2 * sizeof( WORD );
            ok = WMakeMenuEntryFromData( &data, &size, NULL,
                                         &menu->first_entry, info->is32bit );
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

void *WInitDataFromMenu( WMenuEntry *entry, void *tdata )
{
    uint_16             *word;
    size_t              tlen;
    char                *item_text;
    char                *text;

    while( entry != NULL ) {
        word = (uint_16 *)tdata;
        if( entry->item->IsPopup ) {
            *word = entry->item->Item.Popup.ItemFlags & ~MENU_ENDMENU;
            if( entry->next == NULL ) {
                *word |= MENU_ENDMENU;
            }
            text = tdata;
            text = text + sizeof( uint_16 );
            item_text = entry->item->Item.Popup.ItemText;
        } else {
            if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
                *word = 0;
            } else {
                *word = entry->item->Item.Normal.ItemFlags & ~MENU_ENDMENU;
                *word |= MF_STRING;
            }
            if( entry->next == NULL ) {
                *word |= MENU_ENDMENU;
            }
            word++;
            if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
                *word = 0;
            } else {
                *word = entry->item->Item.Normal.ItemID;
            }
            text = tdata;
            text = text + 2 * sizeof( uint_16 );
            item_text = entry->item->Item.Normal.ItemText;
        }

        if( entry->is32bit ) {
            tlen = 0;
            if( item_text != NULL ) {
                if( WRmbcs2unicode( item_text, NULL, &tlen ) ) {
                    if( !WRmbcs2unicodeBuf( item_text, text, tlen ) ) {
                        tlen = 0;
                    }
                }
            }
            if( tlen == 0 ) {
                tlen = 2;
                text[0] = '\0';
                text[1] = '\0';
            }
        } else {
            if( item_text != NULL ) {
                tlen = strlen( item_text ) + 1;
                memcpy( text, item_text, tlen );
            } else {
                tlen = 1;
                text[0] = '\0';
            }
        }

        text += tlen;
        tdata = text;

        if( entry->item->IsPopup ) {
            if( entry->child != NULL ) {
                tdata = WInitDataFromMenu( entry->child, tdata );
            } else {
                WDummyMenuEntry->is32bit = entry->is32bit;
                tdata = WInitDataFromMenu( WDummyMenuEntry, tdata );
            }
        }
        entry = entry->next;
    }

    return( tdata );
}

size_t WCalcMenuSize( WMenuEntry *entry )
{
    size_t      size;
    size_t      tlen;
    char        *text;

    if( entry == NULL ) {
        return( 0 );
    }

    size = 0;

    while( entry != NULL ) {
        if( entry->item->IsPopup ) {
            size += sizeof( MenuFlags ) + 1;
            text = entry->item->Item.Popup.ItemText;
            if( entry->child != NULL ) {
                size += WCalcMenuSize( entry->child );
            } else {
                WDummyMenuEntry->is32bit = entry->is32bit;
                size += WCalcMenuSize( WDummyMenuEntry );
            }
        } else {
            size += sizeof( MenuFlags ) + sizeof( uint_16 ) + 1;
            text = entry->item->Item.Normal.ItemText;
        }

        if( entry->is32bit ) {
            tlen = 0;
            if( text != NULL ) {
                if( !WRmbcs2unicode( text, NULL, &tlen ) ) {
                    tlen = 0;
                }
            }
            if( tlen == 0 ) {
                tlen = 2;
            }
        } else {
            if( text != NULL ) {
                tlen = strlen( text ) + 1;
            } else {
                tlen = 1;
            }
        }

        size += tlen;
        entry = entry->next;
    }

    return( size );
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
    WMenuEntry *e;

    while( entry != NULL ) {
        e = entry;
        entry = entry->next;
        if( e->child != NULL ) {
            WFreeMenuEntries( e->child );
        }
        WFreeMenuEntry( e );
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
        return( FALSE );
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

    return( TRUE );
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

    while( entry != NULL ) {
        depth++;
        entry = entry->parent;
    }

    return( depth );
}

int WCountMenuChildren( WMenuEntry *entry )
{
    int count;

    count = 0;

    while( entry != NULL ) {
        count += WCountMenuChildren( entry->child );
        count++;
        entry = entry->next;
    }

    return( count );
}

static bool WResetPreviewID( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    bool    ok;

    ok = true;

    while( ok && entry != NULL ) {
        entry->preview_id = einfo->first_preview_id;
        einfo->first_preview_id++;
        if( einfo->first_preview_id == LAST_PREVIEW_ID ) {
            return( FALSE );
        }
        if( entry->child != NULL ) {
            ok = WResetPreviewID( einfo, entry->child );
        }
        entry = entry->next;
    }

    return( ok );
}

bool WResetPreviewIDs( WMenuEditInfo *einfo )
{
    if( einfo == NULL ) {
        return( FALSE );
    }

    einfo->first_preview_id = FIRST_PREVIEW_ID;

    return( WResetPreviewID( einfo, einfo->menu->first_entry ) );
}

static bool WAddItemAtPos( HMENU parent, int pos, WMenuEntry *entry )
{
    MenuFlags   flags;
    bool        ok;

    ok = (parent != (HMENU)NULL && entry != NULL);

    if( ok ) {
        flags = entry->item->Item.Normal.ItemFlags;
        flags &= ~MENU_ENDMENU;
        if( flags & MENU_POPUP ) {
            entry->preview_popup = CreatePopupMenu();
            if( entry->preview_popup == (HMENU)NULL ) {
                return( FALSE );
            }
            ok = InsertMenu( parent, pos, MF_BYPOSITION | flags,
                             (UINT)(pointer_int)entry->preview_popup,
                             entry->item->Item.Popup.ItemText ) != 0;
        } else if( flags & MENU_SEPARATOR ) {
            ok = InsertMenu( parent, pos, MF_BYPOSITION | flags,
                             entry->preview_id, NULL ) != 0;
        } else {
            ok = InsertMenu( parent, pos, MF_BYPOSITION | flags,
                             entry->preview_id,
                             entry->item->Item.Normal.ItemText ) != 0;
        }
    }

    return( ok );
}

static bool WModifyItemAtPos( HMENU parent, int pos, WMenuEntry *entry )
{
    MenuFlags   flags;
    bool        ok;

    ok = (parent != (HMENU)NULL && entry != NULL);

    if( ok ) {
        flags = entry->item->Item.Normal.ItemFlags;
        flags &= ~MENU_ENDMENU;
        if( flags & MENU_POPUP ) {
            ok = ModifyMenu( parent, pos, MF_BYPOSITION | flags,
                             (UINT_PTR)entry->preview_popup,
                             entry->item->Item.Popup.ItemText ) != 0;
        } else if( flags & MENU_SEPARATOR ) {
            // do nothing
        } else {
            ok = ModifyMenu( parent, pos, MF_BYPOSITION | flags,
                             entry->preview_id,
                             entry->item->Item.Normal.ItemText ) != 0;
        }
    }

    return( ok );
}

static bool WAddToPreviewMenu( HMENU parent, WMenuEntry *entry )
{
    bool        ok;
    int         pos;

    ok = (parent != (HMENU)NULL && entry != NULL);

    pos = 0;
    while( ok && entry != NULL ) {
        ok = WAddItemAtPos( parent, pos, entry );
        if( ok && entry->item->IsPopup ) {
            if( entry->child != NULL ) {
                ok = WAddToPreviewMenu( entry->preview_popup, entry->child );
            }
        }
        entry = entry->next;
        pos++;
    }

    return( ok );
}

HMENU WCreatePreviewMenu( WMenuEditInfo *einfo )
{
    HMENU       menu;

    if( einfo == NULL ) {
        return( FALSE );
    }

    menu = CreateMenu();
    if( menu == (HMENU)NULL ) {
        return( FALSE );
    }

    WAddToPreviewMenu( menu, einfo->menu->first_entry );

    return( menu );
}

WMenuEntry *WFindEntryFromPreviewID( WMenuEntry *entry, WORD id )
{
    WMenuEntry  *found;

    while( entry != NULL ) {
        if( entry->preview_id == id ) {
            return( entry );
        }
        if( entry->child != NULL ) {
            found = WFindEntryFromPreviewID( entry->child, id );
            if( found != NULL ) {
                return( found );
            }
        }
        entry = entry->next;
    }

    return( NULL );
}

WMenuEntry *WFindEntryFromPreviewPopup( WMenuEntry *entry, HMENU popup )
{
    WMenuEntry  *found;

    while( entry != NULL ) {
        if( entry->preview_popup == popup ) {
            return( entry );
        }
        if( entry->child != NULL ) {
            found = WFindEntryFromPreviewPopup( entry->child, popup );
            if( found != NULL ) {
                return( found );
            }
        }
        entry = entry->next;
    }

    return( NULL );
}

bool WFindEntryLBPos( WMenuEntry *start, WMenuEntry *entry, box_pos *pos )
{
    while( start != NULL ) {
        *pos = *pos + 1;
        if( start == entry ) {
            return( TRUE );
        }
        if( start->child != NULL ) {
            if ( WFindEntryLBPos( start->child, entry, pos ) ) {
                return( TRUE );
            }
        }
        start = start->next;
    }

    return( FALSE );
}

bool WInsertEntryIntoPreview( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    int         pos;
    HMENU       menu;
    WMenuEntry  *start;

    if( einfo == NULL || entry == NULL ) {
        return( FALSE );
    }

    if( entry->parent != NULL ) {
        menu = entry->parent->preview_popup;
        start = entry->parent->child;
    } else {
        menu = GetMenu( einfo->preview_window );
        start = einfo->menu->first_entry;
    }

    if( menu == (HMENU)NULL ) {
        return( FALSE );
    }

    pos = -1;
    while( start != NULL ) {
        pos++;
        if( start == entry ) {
            break;
        }
        start = start->next;
    }

    if( pos == -1 ) {
        return( FALSE );
    }

    entry->preview_id = einfo->first_preview_id;
    einfo->first_preview_id++;
    if( einfo->first_preview_id == LAST_PREVIEW_ID ) {
        return( FALSE );
    }

    if( !WAddItemAtPos( menu, pos, entry ) ) {
        return( FALSE );
    }

    if( entry->parent == NULL ) {
        DrawMenuBar( einfo->preview_window );
    }

    return( TRUE );
}

bool WModifyEntryInPreview( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    int         pos;
    HMENU       menu;
    WMenuEntry  *start;

    if( einfo == NULL || entry == NULL ) {
        return( FALSE );
    }

    if( entry->parent != NULL ) {
        menu = entry->parent->preview_popup;
        start = entry->parent->child;
    } else {
        menu = GetMenu( einfo->preview_window );
        start = einfo->menu->first_entry;
    }

    if( menu == (HMENU)NULL ) {
        return( FALSE );
    }

    pos = -1;
    while( start != NULL ) {
        pos++;
        if( start == entry ) {
            break;
        }
        start = start->next;
    }

    if( pos == -1 ) {
        return( FALSE );
    }

    if( !WModifyItemAtPos( menu, pos, entry ) ) {
        return( FALSE );
    }

    if( entry->parent == NULL ) {
        DrawMenuBar( einfo->preview_window );
    }

    return( TRUE );
}

bool WMakeClipDataFromMenuEntry( WMenuEntry *entry, void **data, uint_32 *dsize )
{
    WMenu       menu;
    WMenuEntry  save;
    size_t      size;
    bool        ok;

    ok = (entry != NULL && data != NULL && dsize != NULL);

    if( ok ) {
        memcpy( &save, entry, sizeof( WMenuEntry ) );
        menu.is32bit = entry->is32bit;
        menu.first_entry = entry;
        entry->next = NULL;
        entry->prev = NULL;
        entry->parent = NULL;
        WMakeDataFromMenu( &menu, data, &size );
        *dsize = (uint_32)size;
        ok = (*data != NULL && *dsize != 0);
        if( ok ) {
            ((BYTE *)(*data))[0] = entry->is32bit;
        }
        memcpy( entry, &save, sizeof( WMenuEntry ) );
    }

    return( ok );
}

WMenuEntry *WMakeMenuEntryFromClipData( void *data, uint_32 dsize )
{
    WMenuEntry  *entry;
    size_t      size;
    bool        is32bit;
    bool        ok;

    entry = NULL;

    ok = (data != NULL && dsize != 0);

    if( ok ) {
        is32bit = ((BYTE *)data)[0];
        data = ((BYTE *)data) + 2 * sizeof( WORD );
        dsize -= 2 * sizeof( WORD );
        size = dsize;
        ok = WMakeMenuEntryFromData( &data, &size, NULL, &entry, is32bit );
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
        return( FALSE );
    }

    if( einfo->menu->first_entry == NULL ) {
        return( TRUE );
    }

    return( WResolveEntries( einfo->menu->first_entry, einfo->info->symbol_table ) );
}

bool WResolveEntries( WMenuEntry *entry, WRHashTable *symbol_table )
{
    if( entry == NULL || symbol_table == NULL ) {
        return( FALSE );
    }

    while( entry != NULL ) {
        if( entry->child != NULL ) {
            WResolveEntries( entry->child, symbol_table );
        }
        WResolveEntrySymbol( entry, symbol_table );
        entry = entry->next;
    }

    return( TRUE );
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
            return( TRUE );
        }
        flags = entry->item->Item.Normal.ItemFlags;
        if( flags & MENU_SEPARATOR ) {
            return( TRUE );
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
        return( FALSE );
    }

    if( einfo->menu->first_entry == NULL ) {
        return( TRUE );
    }

    return( WResolveSymIDs( einfo->menu->first_entry, einfo->info->symbol_table ) );
}

bool WResolveSymIDs( WMenuEntry *entry, WRHashTable *symbol_table )
{
    if( entry == NULL || symbol_table == NULL ) {
        return( FALSE );
    }

    while( entry != NULL ) {
        if( entry->child != NULL ) {
            WResolveSymIDs( entry->child, symbol_table );
        }
        WResolveEntrySymIDs( entry, symbol_table );
        entry = entry->next;
    }

    return( TRUE );
}

bool WResolveEntrySymIDs( WMenuEntry *entry, WRHashTable *symbol_table )
{
    MenuFlags           flags;
    WRHashValue         hv;
    bool                ok;

    ok = (entry != NULL && symbol_table != NULL);

    if( ok ) {
        if( entry->item->IsPopup ) {
            return( TRUE );
        }
        flags = entry->item->Item.Normal.ItemFlags;
        if( flags & MENU_SEPARATOR ) {
            return( TRUE );
        }
        ok = WRLookupName( symbol_table, entry->symbol, &hv );
    }

    if( ok ) {
        entry->item->Item.Normal.ItemID = (uint_16)hv;
    }

    return( ok );
}
