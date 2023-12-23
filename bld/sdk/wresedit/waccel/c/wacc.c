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
#include "waccel.h"
#include "winfo.h"
#include "wribbon.h"
#include "wstrdup.h"
#include "wacc.h"

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

WAccelEditInfo *WAllocAccelEditInfo( void )
{
    WAccelEditInfo *einfo;

    einfo = (WAccelEditInfo *)WRMemAlloc( sizeof( WAccelEditInfo ) );

    if( einfo != NULL ) {
        memset( einfo, 0, sizeof( WAccelEditInfo ) );
        einfo->current_pos = LB_ERR;
    }

    return( einfo );
}

void WFreeAccelTableEntries( WAccelEntry *entry )
{
    WAccelEntry *next;

    for( ; entry != NULL; entry = next ) {
        next = entry->next;
        if( entry->symbol != NULL ) {
            WRMemFree( entry->symbol );
        }
        WRMemFree( entry );
    }
}

static void WFreeAccelTable( WAccelTable *tbl )
{
    if( tbl != NULL ) {
        WFreeAccelTableEntries( tbl->first_entry );
        WRMemFree( tbl );
    }
}

void WFreeAccelEditInfo( WAccelEditInfo *einfo )
{
    if( einfo != NULL ) {
        if( einfo->tbl != NULL ) {
            WFreeAccelTable( einfo->tbl );
            einfo->tbl = NULL;
        }
        if( einfo->wsb != NULL ) {
            WDestroyStatusLine( einfo->wsb );
            einfo->wsb = NULL;
        }
        if( einfo->ribbon != NULL ) {
            WDestroyRibbon( einfo );
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

static void WInitDataFromAccelTable( WAccelTable *tbl, char *data )
{
    WAccelEntry         *entry;
    char                *last;

    last = NULL;
    if( tbl->is32bit ) {
        for( entry = tbl->first_entry; entry != NULL; entry = entry->next ) {
            last = data;
            data = ResWriteDataAccelTableEntry32( data, &entry->u.entry32 );
        }
        SetAccelTableLastEntry32( last );
    } else {
        for( entry = tbl->first_entry; entry != NULL; entry = entry->next ) {
            last = data;
            data = ResWriteDataAccelTableEntry( data, &entry->u.entry );
        }
        SetAccelTableLastEntry( last );
    }
}

static size_t WCalcAccelTableSize( WAccelTable *tbl )
{
    size_t size;

    if( tbl == NULL ) {
        return( 0 );
    }

    if( tbl->is32bit ) {
        size = RES_SIZE_AccelTableEntry32;
    } else {
        size = RES_SIZE_AccelTableEntry;
    }

    size *= tbl->num;

    return( size );
}

void WMakeDataFromAccelTable( WAccelTable *tbl, char **pdata, size_t *dsize )
{
    size_t  size;
    char    *data;

    if( pdata != NULL && dsize != NULL ) {
        size = WCalcAccelTableSize( tbl );
        if( size != 0 ) {
            data = WRMemAlloc( size );
            if( data != NULL ) {
                WInitDataFromAccelTable( tbl, data );
            } else {
                size = 0;
            }
        } else {
            data = NULL;
        }
        *pdata = data;
        *dsize = size;
    }
}

static WAccelTable *WAllocAccelTable( size_t num )
{
    WAccelTable *tbl;
    WAccelEntry *prev;
    WAccelEntry *entry;
    size_t      i;

    tbl = (WAccelTable *)WRMemAlloc( sizeof( WAccelTable ) );
    if( tbl == NULL ) {
        return( NULL );
    }

    tbl->num = num;
    tbl->is32bit = false;
    tbl->first_entry = NULL;
    if( num != 0 ) {
        prev = NULL;
        for( i = 0; i < num; i++ ) {
            entry = (WAccelEntry *)WRMemAlloc( sizeof( WAccelEntry ) );
            if( prev == NULL ) {
                tbl->first_entry = entry;
            } else {
                prev->next = entry;
            }
            if( entry == NULL ) {
                WFreeAccelTable( tbl );
                return( NULL );
            }
            entry->prev = prev;
            entry->symbol = NULL;
            prev = entry;
        }
        entry->next = NULL;
    }

    return( tbl );
}

static size_t WCalcNumAccelEntries( WAccelInfo *info )
{
    size_t  num;
    size_t  size;

    if( info == NULL ) {
        return( 0 );
    }

    if( info->is32bit ) {
        size = RES_SIZE_AccelTableEntry32;
    } else {
        size = RES_SIZE_AccelTableEntry;
    }

    num = info->data_size / size;

    if( info->data_size % size != 0 ) {
        num++;
    }

    return( num );
}

static void WInitAccelTable( WAccelInfo *info, WAccelTable *tbl )
{
    WAccelEntry         *entry;
    const char          *data;
    int                 i;
    bool                last;

    entry = tbl->first_entry;
    if( entry == NULL ) {
        return;
    }

    data = info->data;
    i = 0;
    last = false;
    if( info->is32bit ) {
        for( ; entry != NULL && !last; entry = entry->next ) {
            entry->is32bit = true;
            last = IsAccelTableLastEntry32( data );
            data = ResReadDataAccelTableEntry32( data, &entry->u.entry32 );
            i++;
        }
    } else {
        for( ; entry != NULL && !last; entry = entry->next ) {
            entry->is32bit = false;
            last = IsAccelTableLastEntry( data );
            data = ResReadDataAccelTableEntry( data, &entry->u.entry );
            i++;
        }
    }
    if( entry != NULL ) {
        entry->prev->next = NULL;
        WFreeAccelTableEntries( entry );
    }

    tbl->num = i;
}

WAccelTable *WMakeAccelTableFromInfo( WAccelInfo *info )
{
    WAccelTable *tbl;
    size_t      num;
    bool        ok;

    tbl = NULL;
    num = 0;

    ok = (info != NULL);

    if( ok ) {
        num = WCalcNumAccelEntries( info );
        tbl = WAllocAccelTable( num );
        ok = (tbl != NULL);
    }

    if( ok ) {
        tbl->is32bit = info->is32bit;
        if( num != 0 ) {
            WInitAccelTable( info, tbl );
        }
    }

    if( ok ) {
        info->data = NULL;
        info->data_size = 0;
    } else {
        if( tbl != NULL ) {
            WFreeAccelTable( tbl );
            tbl = NULL;
        }
    }

    return( tbl );
}

bool WInsertAccelTableEntry( WAccelTable *tbl, WAccelEntry *after, WAccelEntry *entry )
{
    bool ok;

    ok = (tbl != NULL && entry != NULL);

    if( ok ) {
        if( after != NULL ) {
            if( after->next != NULL ) {
                after->next->prev = entry;
            }
            entry->next = after->next;
            entry->prev = after;
            after->next = entry;
        } else {
            entry->next = tbl->first_entry;
            entry->prev = NULL;
            tbl->first_entry = entry;
        }
        tbl->num++;
    }

    return( ok );
}

bool WFreeAccelTableEntry( WAccelTable *tbl, WAccelEntry *entry )
{
    bool ok;

    ok = (tbl != NULL && entry != NULL);

    if( ok ) {
        if( entry->next != NULL ) {
            entry->next->prev = entry->prev;
        }
        if( entry->prev != NULL ) {
            entry->prev->next = entry->next;
        }
        if( tbl->first_entry == entry ) {
            tbl->first_entry = entry->next;
        }
        if( entry->symbol != NULL ) {
            WRMemFree( entry->symbol );
        }
        WRMemFree( entry );
        tbl->num--;
    }

    return( ok );
}

bool WMakeClipDataFromAccelEntry( WAccelEntry *entry, char **pdata, size_t *dsize )
{
    bool        ok;
    char        *data;
    size_t      size;

    ok = (entry != NULL && pdata != NULL && dsize != NULL);

    if( ok ) {
        if( entry->is32bit ) {
            size = 1 + sizeof( AccelTableEntry32 );
        } else {
            size = 1 + sizeof( AccelTableEntry );
        }
        *dsize = size;
        *pdata = data = WRMemAlloc( size );
        ok = (data != NULL);
    }

    if( ok ) {
        data[0] = entry->is32bit;
        memcpy( data + 1, &entry->u.entry, size - 1 );
    }

    return( ok );
}

bool WMakeAccelEntryFromClipData( WAccelEntry *entry, const char *data, size_t dsize )
{
    size_t      len;
    bool        ok;

    len = 0;
    ok = (entry != NULL && data != NULL && dsize != 0);

    if( ok ) {
        memset( entry, 0, sizeof( WAccelEntry ) );
        entry->is32bit = data[0];
        if( entry->is32bit ) {
            len = sizeof( AccelTableEntry32 );
        } else {
            len = sizeof( AccelTableEntry );
        }
    }

    if( ok ) {
        memcpy( &entry->u.entry, data + 1, len );
    }

    return( ok );
}

bool WResolveAllEntrySymbols( WAccelEditInfo *einfo )
{
    WAccelEntry *entry;

    if( einfo == NULL || einfo->tbl == NULL ) {
        return( false );
    }

    for( entry = einfo->tbl->first_entry; entry != NULL; entry = entry->next ) {
        WResolveEntrySymbol( entry, einfo->info->symbol_table );
    }

    return( true );
}

bool WResolveEntrySymbol( WAccelEntry *entry, WRHashTable *symbol_table )
{
    uint_16             id;
    WRHashValueList     *vlist;
    bool                ok;

    vlist = NULL;

    ok = (entry != NULL && symbol_table != NULL);

    if( ok ) {
        if( entry->is32bit ) {
            id = entry->u.entry32.Id;
        } else {
            id = (uint_16)entry->u.entry.Id;
        }
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

bool WResolveEntrySymIDs( WAccelEntry *entry, WRHashTable *symbol_table )
{
    WRHashValue         hv;
    bool                ok;

    hv = 0;
    ok = (entry != NULL && symbol_table != NULL);

    if( ok ) {
        ok = WRLookupName( symbol_table, entry->symbol, &hv );
    }

    if( ok ) {
        if( entry->is32bit ) {
            entry->u.entry32.Id = (uint_16)hv;
        } else {
            entry->u.entry.Id = (uint_16)hv;
        }
    }

    return( ok );
}

bool WResolveAllEntrySymIDs( WAccelEditInfo *einfo )
{
    WAccelEntry *entry;

    if( einfo == NULL || einfo->tbl == NULL ) {
        return( false );
    }

    for( entry = einfo->tbl->first_entry; entry != NULL; entry = entry->next ) {
        WResolveEntrySymIDs( entry, einfo->info->symbol_table );
    }

    return( true );
}
