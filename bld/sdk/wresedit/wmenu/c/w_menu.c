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


#include <windows.h>
#include <string.h>
#include "wglbl.h"
#include "wmem.h"
#include "wmenu.h"
#include "winfo.h"
#include "wribbon.h"
#include "wstrdup.h"
#include "wcopystr.h"
#include "wresall.h"
#include "wmsg.h"
#include "wmsgfile.h"
#include "wrutil.h"
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
static void     *WInitDataFromMenu      ( WMenuEntry *, void * );
static int      WCalcMenuSize           ( WMenuEntry * );
static int      WMakeMenuEntryFromData  ( void **, int *, WMenuEntry *,
                                          WMenuEntry **, Bool );
static int      WAllocMenuEntryFromData ( void **, int *, WMenuEntry **,
                                          Bool );
static int      WMakeMenuItemFromData   ( void **data, int *size,
                                          MenuItem **new, Bool );
static Bool     WInsertEntryIntoPreview ( WMenuEditInfo *, WMenuEntry * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static  WMenuEntry      *WDummyMenuEntry = NULL;

void WInitDummyMenuEntry( void )
{
    WDummyMenuEntry = (WMenuEntry *) WMemAlloc( sizeof(WMenuEntry) );
    memset( WDummyMenuEntry, 0, sizeof( WMenuEntry ) );
    WDummyMenuEntry->item = ResNewMenuItem();
    WDummyMenuEntry->item->Item.Normal.ItemText =
        WAllocRCString( W_MENUITEM );
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

    einfo = (WMenuEditInfo *) WMemAlloc( sizeof(WMenuEditInfo) );

    if( einfo ) {
        memset( einfo, 0, sizeof(WMenuEditInfo) );
        einfo->current_pos = -1;
    }

    return( einfo );
}

void WFreeMenuEInfo( WMenuEditInfo *einfo )
{
    if( einfo ) {
        if( einfo->menu ) {
            WFreeMenu( einfo->menu );
            einfo->menu = NULL;
        }
        if( einfo->wsb ) {
            WDestroyStatusLine( einfo->wsb );
            einfo->wsb = NULL;
        }
        if( einfo->ribbon ) {
            WDestroyRibbon( einfo );
        }
        if( ( einfo->preview_window != (HWND)NULL ) &&
             IsWindow( einfo->preview_window ) ) {
            DestroyWindow( einfo->preview_window );
            einfo->preview_window = (HWND)NULL;
        }
        if( ( einfo->edit_dlg != (HWND)NULL ) &&
             IsWindow( einfo->edit_dlg ) ) {
            DestroyWindow ( einfo->edit_dlg );
            einfo->edit_dlg = (HWND)NULL;
        }
        if ( ( einfo->win != (HWND)NULL ) && IsWindow ( einfo->win ) ) {
            SetWindowLong( einfo->win, 0, (LONG)0 );
            DestroyWindow ( einfo->win );
            einfo->win = (HWND)NULL;
        }
        if ( einfo->file_name ) {
            WMemFree ( einfo->file_name );
        }
        WMemFree ( einfo );
    }
}

void WMakeDataFromMenu( WMenu *menu, void **data, int *size )
{
    char *tdata;

    if( data && size ) {
        *size = WCalcMenuSize( menu->first_entry ) + 2*sizeof(WORD);
        if( *size ) {
            *data = WMemAlloc( *size );
            if( *data ) {
                tdata = *data;
                memset( tdata, 0, 2*sizeof(WORD) );
                tdata = tdata + 2*sizeof(WORD);
                WInitDataFromMenu( menu->first_entry, tdata );
            }
        } else {
            *data = NULL;
        }
    }
}

int WMakeMenuItemFromData( void **data, int *size, MenuItem **new,
                           Bool is32bit )
{
    MenuItemNormal      *normal;
    char                *text;
    char                *itext;
    int                 msize;
    int                 tlen;
    int                 itlen;

    if( !data || !*data || !size || !*size || !new ) {
        return( FALSE );
    }

    *new = ResNewMenuItem();
    if( *new == NULL ) {
        return( FALSE );
    }

    normal = (MenuItemNormal *)*data;
    (*new)->Item.Normal.ItemFlags = normal->ItemFlags;
    (*new)->IsPopup = ( ( normal->ItemFlags & MENU_POPUP ) != 0 );
    msize = sizeof( MenuFlags );
    if( (*new)->IsPopup ) {
        text = (char *)(*data);
        text += msize;
    } else {
        if( !(normal->ItemFlags & ~MENU_ENDMENU ) && !normal->ItemID ) {
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
        itext = (char *)WMemAlloc( tlen );
        if( itext ) {
            memcpy( itext, text, tlen );
        }
    }

    if( !itext ) {
        *size = 0;
        return( FALSE );
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
        return( FALSE );
    }

    return( TRUE );
}

int WAllocMenuEntryFromData( void **data, int *size, WMenuEntry **entry,
                             Bool is32bit )
{
    int         ok;

    ok = ( data && *data && size && *size && entry );

    if( ok ) {
        *entry = (WMenuEntry *) WMemAlloc ( sizeof(WMenuEntry) );
        ok = ( *entry != NULL );
    }

    if( ok ) {
        memset( *entry, 0, sizeof(WMenuEntry) );
        (*entry)->is32bit = is32bit;
        ok = WMakeMenuItemFromData( data, size, &(*entry)->item, is32bit );
    }

    if( !ok ) {
        if ( *entry ) {
            WFreeMenuEntry ( *entry );
            *entry = NULL;
        }
    }

    return( ok );
}

int WMakeMenuEntryFromData( void **data, int *size, WMenuEntry *parent,
                            WMenuEntry **entry, Bool is32bit )
{
    int         ok;
    WMenuEntry  **current;
    WMenuEntry  *prev;

    if( !entry || !data || !size ) {
        return( FALSE );
    }

    *entry = NULL;
    ok = TRUE;

    if( !*data || !*size ) {
        return( TRUE );
    }

    current = entry;
    prev = NULL;

    while( ok && ( *size > 0 ) ) {
        ok = WAllocMenuEntryFromData( data, size, current, is32bit );
        if( ok ) {
            (*current)->parent = parent;
            (*current)->prev = prev;
            if( (*current)->item->IsPopup ) {
                ok = WMakeMenuEntryFromData ( data, size, *current,
                                              &((*current)->child), is32bit );
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

WMenu *WMakeMenuFromInfo ( WMenuInfo *info )
{
    WMenu       *menu;
    int         ok;
    void        *data;
    int         size;

    menu = NULL;

    ok = ( info != NULL );

    if ( ok ) {
        menu = (WMenu *) WMemAlloc ( sizeof(WMenu) );
        ok = ( menu != NULL );
    }

    if( ok ) {
        menu->first_entry = NULL;
        if( info->data != NULL ) {
            ok = ( ((WORD *)info->data)[0] == 0 );
            if( !ok ) {
                WDisplayErrorMsg( W_NOMENUEX );
            }
        }
    }

    if( ok ) {
        menu->is32bit = info->is32bit;
        if( info->data != NULL ) {
            data = ((char *)info->data) + 2*sizeof(WORD);
            size = info->data_size - 2*sizeof(WORD);
            ok = WMakeMenuEntryFromData( &data, &size, NULL,
                                         &menu->first_entry, info->is32bit );
        }
    }

    if( ok ) {
        info->data      = NULL;
        info->data_size = 0;
    } else {
        if ( menu ) {
            WFreeMenu ( menu );
            menu = NULL;
        }
    }

    return( menu );
}

void *WInitDataFromMenu( WMenuEntry *entry, void *tdata )
{
    uint_16             *word;
    int                 tlen;
    char                *item_text;
    char                *text;

    while( entry ) {
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
            text = text + 2*sizeof( uint_16 );
            item_text = entry->item->Item.Normal.ItemText;
        }

        if( entry->is32bit ) {
            tlen = 0;
            if( item_text ) {
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
            if( item_text ) {
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

int WCalcMenuSize( WMenuEntry *entry )
{
    int         size;
    int         tlen;
    char        *text;

    if( !entry ) {
        return( 0 );
    }

    size = 0;

    while( entry ) {
        if( entry->item->IsPopup ) {
            size += sizeof( MenuFlags ) + 1;
            text = entry->item->Item.Popup.ItemText;
            if( entry->child ) {
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
            if( text ) {
                if( !WRmbcs2unicode( text, NULL, &tlen ) ) {
                    tlen = 0;
                }
            }
            if( tlen == 0 ) {
                tlen = 2;
            }
        } else {
            if( text ) {
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

void WFreeMenu ( WMenu *menu )
{
    if ( menu ) {
        WFreeMenuEntries ( menu->first_entry );
        WMemFree ( menu );
    }
}

void WFreeMenuEntries ( WMenuEntry *entry )
{
    WMenuEntry *e;

    while ( entry ) {
        e = entry;
        entry = entry->next;
        if( e->child ) {
            WFreeMenuEntries ( e->child );
        }
        WFreeMenuEntry ( e );
    }
}

void WFreeMenuEntry ( WMenuEntry *entry )
{
    if( entry ) {
        if( entry->item ) {
            ResFreeMenuItem( entry->item );
        }
        if( entry->symbol ) {
            WMemFree( entry->symbol );
        }
        WMemFree( entry );
    }
}

Bool WRemoveMenuEntry ( WMenu *menu, WMenuEntry *entry )
{
    if( !menu || !entry ) {
        return( FALSE );
    }

    if( menu->first_entry == entry ) {
        menu->first_entry = entry->next;
    } else {
        if( entry->parent && (entry->parent->child == entry) ) {
            entry->parent->child = entry->next;
        }
    }
    if ( entry->next ) {
        entry->next->prev = entry->prev;
    }
    if ( entry->prev ) {
        entry->prev->next = entry->next;
    }

    return( TRUE );
}

Bool WInsertEntryIntoMenu ( WMenuEditInfo *einfo, WMenuEntry *after,
                            WMenuEntry *parent, WMenuEntry *entry,
                            Bool popup )
{
    Bool ok;

    ok = ( einfo && einfo->menu && entry );

    if ( ok ) {
        if ( after ) {
            if( popup ) {
                ok = WInsertEntryIntoMenu( einfo, NULL, after, entry, FALSE );
            } else {
                if ( after->next ) {
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
            if( parent ) {
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

    return ( ok );
}

int WGetMenuEntryDepth ( WMenuEntry *entry )
{
    int depth;

    depth = -1;

    while( entry ) {
        depth++;
        entry = entry->parent;
    }

    return( depth );
}

int WCountMenuChildren( WMenuEntry *entry )
{
    int         count;

    count = 0;

    while( entry ) {
        count += WCountMenuChildren( entry->child );
        count++;
        entry = entry->next;
    }

    return( count );
}

Bool WResetPreviewID( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    Bool        ok;

    ok = TRUE;

    while( ok && entry ) {
        entry->preview_id = einfo->first_preview_id;
        einfo->first_preview_id++;
        if( einfo->first_preview_id == LAST_PREVIEW_ID ) {
            return( FALSE );
        }
        if( entry->child ) {
            ok = WResetPreviewID( einfo, entry->child );
        }
        entry = entry->next;
    }

    return( ok );
}

Bool WResetPreviewIDs( WMenuEditInfo *einfo )
{
    if( einfo == NULL ) {
        return( FALSE );
    }

    einfo->first_preview_id = FIRST_PREVIEW_ID;

    return( WResetPreviewID( einfo, einfo->menu->first_entry ) );
}

Bool WAddItemAtPos( HMENU parent, int pos, WMenuEntry *entry )
{
    MenuFlags   flags;
    Bool        ok;

    ok = ( ( parent != (HMENU)NULL ) && entry );

    if( ok ) {
        flags = entry->item->Item.Normal.ItemFlags;
        flags &= ~MENU_ENDMENU;
        if( flags & MENU_POPUP ) {
            entry->preview_popup = CreatePopupMenu();
            if ( entry->preview_popup == (HMENU)NULL ) {
                return( FALSE );
            }
            ok = InsertMenu( parent, pos, MF_BYPOSITION | flags,
                             (UINT)entry->preview_popup,
                             entry->item->Item.Popup.ItemText );
        } else if( flags & MENU_SEPARATOR ) {
            ok = InsertMenu( parent, pos, MF_BYPOSITION | flags,
                             entry->preview_id, NULL );
        } else {
            ok = InsertMenu( parent, pos, MF_BYPOSITION | flags,
                             entry->preview_id,
                             entry->item->Item.Normal.ItemText );
        }
    }

    return( ok );
}

Bool WModifyItemAtPos( HMENU parent, int pos, WMenuEntry *entry )
{
    MenuFlags   flags;
    Bool        ok;

    ok = ( ( parent != (HMENU)NULL ) && entry );

    if( ok ) {
        flags = entry->item->Item.Normal.ItemFlags;
        flags &= ~MENU_ENDMENU;
        if( flags & MENU_POPUP ) {
            ok = ModifyMenu( parent, pos, MF_BYPOSITION | flags,
                             (UINT)entry->preview_popup,
                             entry->item->Item.Popup.ItemText );
        } else if( flags & MENU_SEPARATOR ) {
            // do nothing
        } else {
            ok = ModifyMenu( parent, pos, MF_BYPOSITION | flags,
                             entry->preview_id,
                             entry->item->Item.Normal.ItemText );
        }
    }

    return( ok );
}

Bool WAddToPreviewMenu( HMENU parent, WMenuEntry *entry )
{
    Bool        ok;
    int         pos;

    ok = ( ( parent != (HMENU)NULL ) && entry );

    pos = 0;
    while( ok && entry ) {
        ok = WAddItemAtPos( parent, pos, entry );
        if( ok && entry->item->IsPopup ) {
            if( entry->child ) {
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

    while( entry ) {
        if( entry->preview_id == id ) {
            return( entry );
        }
        if( entry->child ) {
            found = WFindEntryFromPreviewID( entry->child, id );
            if( found ) {
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

    while( entry ) {
        if( entry->preview_popup == popup ) {
            return( entry );
        }
        if( entry->child ) {
            found = WFindEntryFromPreviewPopup( entry->child, popup );
            if( found ) {
                return( found );
            }
        }
        entry = entry->next;
    }

    return( NULL );
}

Bool WFindEntryLBPos( WMenuEntry *start, WMenuEntry *entry, int *count )
{
    while( start ) {
        *count = *count + 1;
        if( start == entry ) {
            return( TRUE );
        }
        if( start->child ) {
            if ( WFindEntryLBPos( start->child, entry, count ) ) {
                return( TRUE );
            }
        }
        start = start->next;
    }

    return( FALSE );
}

Bool WInsertEntryIntoPreview( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    int         pos;
    HMENU       menu;
    WMenuEntry  *start;

    if( !einfo || !entry ) {
        return( FALSE );
    }

    if( entry->parent ) {
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
    while( start ) {
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

Bool WModifyEntryInPreview( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    int         pos;
    HMENU       menu;
    WMenuEntry  *start;

    if( !einfo || !entry ) {
        return( FALSE );
    }

    if( entry->parent ) {
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
    while( start ) {
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

Bool WMakeClipDataFromMenuEntry( WMenuEntry *entry, void **data, uint_32 *dsize )
{
    WMenu       menu;
    WMenuEntry  save;
    int         size;
    Bool        ok;

    ok = ( entry && data && dsize );

    if( ok ) {
        memcpy( &save, entry, sizeof(WMenuEntry) );
        menu.is32bit = entry->is32bit;
        menu.first_entry = entry;
        entry->next = NULL;
        entry->prev = NULL;
        entry->parent = NULL;
        WMakeDataFromMenu( &menu, data, &size );
        *dsize = size;
        ok = ( *data && *dsize );
        if( ok ) {
            ((BYTE *)(*data))[0] = entry->is32bit;
        }
        memcpy( entry, &save, sizeof(WMenuEntry) );
    }

    return( ok );
}

WMenuEntry *WMakeMenuEntryFromClipData( void *data, uint_32 dsize )
{
    WMenuEntry  *entry;
    int         size;
    Bool        is32bit;
    Bool        ok;

    entry = NULL;

    ok = ( data && dsize );

    if( ok ) {
        is32bit = ((BYTE *)data)[0];
        data = ((BYTE *)data) + 2*sizeof(WORD);
        dsize -= 2*sizeof(WORD);
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

Bool WResolveMenuEntries( WMenuEditInfo *einfo )
{
    if( einfo->menu == NULL ) {
        return( FALSE );
    }

    if( einfo->menu->first_entry == NULL ) {
        return( TRUE );
    }

    return( WResolveEntries( einfo->menu->first_entry,
                             einfo->info->symbol_table ) );
}

Bool WResolveEntries( WMenuEntry *entry, WRHashTable *symbol_table )
{
    if( !entry || !symbol_table ) {
        return( FALSE );
    }

    while( entry ) {
        if( entry->child ) {
            WResolveEntries( entry->child, symbol_table );
        }
        WResolveEntrySymbol( entry, symbol_table );
        entry = entry->next;
    }

    return( TRUE );
}

Bool WResolveEntrySymbol( WMenuEntry *entry, WRHashTable *symbol_table )
{
    uint_16             id;
    MenuFlags           flags;
    WRHashValueList     *vlist;
    Bool                ok;

    vlist = NULL;

    ok = ( entry && symbol_table );

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
        ok = ( vlist && !vlist->next );
    }

    if( ok ) {
        if( entry->symbol ) {
            WMemFree( entry->symbol );
        }
        entry->symbol = WStrDup( vlist->entry->name );
        ok = ( entry->symbol != NULL );
    }

    if( vlist != NULL ) {
        WRValueListFree( vlist );
    }

    return( ok );
}

Bool WResolveMenuSymIDs( WMenuEditInfo *einfo )
{
    if( einfo->menu == NULL ) {
        return( FALSE );
    }

    if( einfo->menu->first_entry == NULL ) {
        return( TRUE );
    }

    return( WResolveSymIDs( einfo->menu->first_entry,
                            einfo->info->symbol_table ) );
}

Bool WResolveSymIDs( WMenuEntry *entry, WRHashTable *symbol_table )
{
    if( !entry || !symbol_table ) {
        return( FALSE );
    }

    while( entry ) {
        if( entry->child ) {
            WResolveSymIDs( entry->child, symbol_table );
        }
        WResolveEntrySymIDs( entry, symbol_table );
        entry = entry->next;
    }

    return( TRUE );
}

Bool WResolveEntrySymIDs( WMenuEntry *entry, WRHashTable *symbol_table )
{
    MenuFlags           flags;
    WRHashValue         hv;
    Bool                ok;

    ok = ( entry && symbol_table );

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

