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


#ifndef WACC_INCLUDED
#define WACC_INCLUDED

#include "waccel.h"
#include "winfo.h"
#include "wstat.h"
#include "wtoolbar.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* data types                                                               */
/****************************************************************************/
typedef struct WAccelEntry {
    int   is32bit;
    union {
        AccelTableEntry     entry;
        AccelTableEntry32   entry32;
    };
    struct WAccelEntry *next;
    struct WAccelEntry *prev;
    char  *symbol;
} WAccelEntry;

typedef struct WAccelTable {
    int         is32bit;
    int         num;
    WAccelEntry *first_entry;
} WAccelTable;

typedef struct WGetKeyInfo {
    HWND        text_win;
    uint_16     key;
    Bool        extended;
    Bool        ignore_first_key;
    char        last[4];
} WGetKeyInfo;

typedef struct WAccelEditInfo {
    WAccelHandle        hndl;
    WAccelInfo          *info;
    WAccelTable         *tbl;
    WAccelEntry         *current_entry;
    int                 current_pos;
    HWND                win;
    HWND                edit_dlg;
    WStatBar            *wsb;
    WToolBar            *ribbon;
    Bool                show_ribbon;
    char                *file_name;
    WRFileType          file_type;
    Bool                getting_key;
    Bool                combo_change;
    WGetKeyInfo         key_info;
    int                 last_menu_select;
} WAccelEditInfo;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern WAccelEditInfo   *WAllocAccelEInfo( void );
extern void             WFreeAccelEInfo( WAccelEditInfo * );
extern void             WMakeDataFromAccelTable( WAccelTable *, void **, int * );
extern WAccelTable      *WMakeAccelTableFromInfo( WAccelInfo * );
extern Bool             WInsertAccelTableEntry( WAccelTable *, WAccelEntry *, WAccelEntry * );
extern Bool             WFreeAccelTableEntry( WAccelTable *, WAccelEntry * );
extern void             WFreeAccelTableEntries( WAccelEntry * );
extern Bool             WMakeEntryClipData( WAccelEntry *entry, void **data, uint_32 *dsize );
extern Bool             WMakeEntryFromClipData( WAccelEntry *entry, void *data, uint_32 dsize );
extern Bool             WResolveEntrySymbol( WAccelEntry *entry, WRHashTable *symbol_table );
extern Bool             WResolveAllEntrySymbols( WAccelEditInfo *einfo );
extern Bool             WResolveAllEntrySymIDs( WAccelEditInfo *einfo );
extern Bool             WResolveEntrySymIDs( WAccelEntry *entry, WRHashTable *symbol_table );

#endif
