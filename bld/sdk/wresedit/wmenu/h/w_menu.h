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


#ifndef W_MENU_INCLUDED
#define W_MENU_INCLUDED

#include "wmenu.h"
#include "winfo.h"
#include "ldstr.h"
#include "wstat.h"
#include "wtoolbar.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define FIRST_PREVIEW_ID        0x0001
#define LAST_PREVIEW_ID         0xffff
#define WGETMENUITEMTEXT( item ) \
    ((item)->IsPopup ? (item)->Item.Popup.ItemText : (item)->Item.Normal.ItemText)
#define WGETMENUITEMFLAGS( item ) \
    ((item)->Item.Normal.ItemFlags)
#define WSETMENUITEMFLAGS( item, flags ) \
    ((item)->Item.Normal.ItemFlags = (flags))
#define WGETMENUITEMID( item ) \
    ((item)->IsPopup ? -1 : (item)->Item.Normal.ItemID)

/****************************************************************************/
/* data types                                                               */
/****************************************************************************/
typedef struct WMenuEntry {
    bool                is32bit;
    WORD                preview_id;
    HMENU               preview_popup;
    MenuItem            *item;
    char                *symbol;
    struct WMenuEntry   *next;
    struct WMenuEntry   *prev;
    struct WMenuEntry   *parent;
    struct WMenuEntry   *child;
} WMenuEntry;

typedef struct WMenu {
    bool        is32bit;
    WMenuEntry  *first_entry;
} WMenu;

typedef struct WMenuEditInfo {
    WMenuHandle hndl;
    WMenuInfo   *info;
    WMenu       *menu;
    WMenuEntry  *current_entry;
    int         current_pos;
    HWND        win;
    HWND        edit_dlg;
    HWND        preview_window;
    WStatBar    *wsb;
    WToolBar    *ribbon;
    bool        show_ribbon;
    char        *file_name;
    WRFileType  file_type;
    bool        insert_before;
    bool        insert_subitems;
    bool        combo_change;
    WORD        first_preview_id;
} WMenuEditInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern void             WInitDummyMenuEntry( void );
extern void             WFiniDummyMenuEntry( void );
extern WMenuEditInfo    *WAllocMenuEInfo( void );
extern void             WFreeMenuEInfo( WMenuEditInfo * );
extern void             WMakeDataFromMenu( WMenu *, void **, size_t * );
extern void             WFreeMenu( WMenu *menu );
extern void             WFreeMenuEntries( WMenuEntry *entry );
extern void             WFreeMenuEntry( WMenuEntry *entry );
extern bool             WRemoveMenuEntry( WMenu *, WMenuEntry * );
extern WMenu            *WMakeMenuFromInfo( WMenuInfo * );
extern int              WGetMenuEntryDepth( WMenuEntry *entry );
extern bool             WInsertEntryIntoMenu( WMenuEditInfo *einfo, WMenuEntry *after, WMenuEntry *parent, WMenuEntry *entry, bool popup );
extern int              WCountMenuChildren( WMenuEntry *entry );
extern bool             WResetPreviewIDs( WMenuEditInfo *einfo );
extern HMENU            WCreatePreviewMenu( WMenuEditInfo *einfo );
extern WMenuEntry       *WFindEntryFromPreviewID( WMenuEntry *, WORD );
extern WMenuEntry       *WFindEntryFromPreviewPopup( WMenuEntry *, HMENU );
extern bool             WFindEntryLBPos( WMenuEntry *, WMenuEntry *, box_pos * );
extern bool             WModifyEntryInPreview( WMenuEditInfo *, WMenuEntry * );

extern bool             WMakeClipDataFromMenuEntry( WMenuEntry *entry, void **data, uint_32 *dsize );
extern WMenuEntry       *WMakeMenuEntryFromClipData( void *data, uint_32 dsize );

extern bool WResolveEntrySymbol( WMenuEntry *entry, WRHashTable *symbol_table );
extern bool WResolveMenuEntries( WMenuEditInfo *einfo );
extern bool WResolveEntries( WMenuEntry *entry, WRHashTable *symbol_table );

extern bool WResolveMenuSymIDs( WMenuEditInfo *einfo );
extern bool WResolveSymIDs( WMenuEntry *entry, WRHashTable *symbol_table );
extern bool WResolveEntrySymIDs( WMenuEntry *entry, WRHashTable *symbol_table );

#endif
