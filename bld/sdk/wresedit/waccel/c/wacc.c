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
static void         WInitDataFromAccelTable( WAccelTable *, void * );
static void         WInitAccelTable( WAccelInfo *, WAccelTable * );
static size_t       WCalcAccelTableSize( WAccelTable * );
static size_t       WCalcNumAccelEntries( WAccelInfo * );
static WAccelTable  *WAllocAccelTable( size_t );
static void         WFreeAccelTable( WAccelTable * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WAccelEditInfo *WAllocAccelEInfo( void )
{
    WAccelEditInfo *einfo;

    einfo = (WAccelEditInfo *)WRMemAlloc( sizeof( WAccelEditInfo ) );

    if( einfo != NULL ) {
        memset( einfo, 0, sizeof( WAccelEditInfo ) );
        einfo->current_pos = -1;
    }

    return( einfo );
}

void WFreeAccelEInfo( WAccelEditInfo *einfo )
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

void WMakeDataFromAccelTable( WAccelTable *tbl, void **pdata, size_t *psize )
{
    size_t  size;
    void    *data;

    if( pdata != NULL && psize != NULL ) {
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
        *psize = size;
    }
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

void WInitDataFromAccelTable( WAccelTable *tbl, void *tdata )
{
    WAccelEntry         *entry;
    AccelTableEntry     *data;
    AccelTableEntry32   *data32;
    int                 i;

    entry = tbl->first_entry;
    i = -1;

    if( tbl->is32bit ) {
        data32 = (AccelTableEntry32 *)tdata;
        while( entry != NULL ) {
            i++;
            data32[i] = entry->u.entry32;
            entry = entry->next;
        }
        data32[i].Flags |= ACCEL_LAST;
    } else {
        data = (AccelTableEntry *)tdata;
        while( entry != NULL ) {
            i++;
            data[i] = entry->u.entry;
            entry = entry->next;
        }
        data[i].Flags |= ACCEL_LAST;
    }
}

void WInitAccelTable( WAccelInfo *info, WAccelTable *tbl )
{
    WAccelEntry         *entry;
    AccelTableEntry     *data;
    AccelTableEntry32   *data32;
    int                 i;

    entry = tbl->first_entry;
    if( entry == NULL ) {
        return;
    }

    i = -1;

    if( info->is32bit ) {
        data32 = (AccelTableEntry32 *)info->data;
        do {
            i++;
            entry->is32bit = true;
            entry->u.entry32 = data32[i];
            entry->u.entry32.Flags &= ~ACCEL_LAST;
            entry = entry->next;
        } while( entry != NULL && !(data32[i].Flags & ACCEL_LAST) );
    } else {
        data = (AccelTableEntry *)info->data;
        do {
            i++;
            entry->is32bit = false;
            entry->u.entry = data[i];
            entry->u.entry.Flags &= ~ACCEL_LAST;
            entry = entry->next;
        } while( entry != NULL && !(data[i].Flags & ACCEL_LAST) );
    }

    if( entry != NULL ) {
        entry->prev->next = NULL;
        WFreeAccelTableEntries( entry );
    }

    tbl->num = i + 1;
}

size_t WCalcAccelTableSize( WAccelTable *tbl )
{
    size_t size;

    if( tbl == NULL ) {
        return( 0 );
    }

    if( tbl->is32bit ) {
        size = sizeof( AccelTableEntry32 );
    } else {
        size = sizeof( AccelTableEntry );
    }

    size *= tbl->num;

    return( size );
}

size_t WCalcNumAccelEntries( WAccelInfo *info )
{
    size_t  num, size;

    if( info == NULL ) {
        return( 0 );
    }

    if( info->is32bit ) {
        size = sizeof( AccelTableEntry32 );
    } else {
        size = sizeof( AccelTableEntry );
    }

    num = info->data_size / size;

    if( info->data_size % size != 0 ) {
        num++;
    }

    return( num );
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

WAccelTable *WAllocAccelTable( size_t num )
{
    WAccelTable *tbl;
    WAccelEntry *entry;
    size_t      i;

    tbl = (WAccelTable *)WRMemAlloc( sizeof( WAccelTable ) );

    if( tbl == NULL ) {
        return( NULL );
    }

    tbl->num = num;
    tbl->is32bit = false;
    if( num != 0 ) {
        entry = NULL;
        for( i = 0; i < num; i++ ) {
            if( entry != NULL ) {
                entry->next = (WAccelEntry *)WRMemAlloc( sizeof( WAccelEntry ) );
                if( entry != NULL ) {
                    entry->next->symbol = NULL;
                    entry->next->prev = entry;
                    entry = entry->next;
                }
            } else {
                tbl->first_entry = (WAccelEntry *)WRMemAlloc( sizeof( WAccelEntry ) );
                tbl->first_entry->symbol = NULL;
                entry = tbl->first_entry;
                entry->prev = NULL;
            }
            if( entry == NULL ) {
                WFreeAccelTable( tbl );
                tbl = NULL;
                break;
            }
        }
        if( entry != NULL ) {
            entry->next = NULL;
        }
    } else {
        tbl->first_entry = NULL;
    }

    return( tbl );
}

void WFreeAccelTable( WAccelTable *tbl )
{
    if( tbl != NULL ) {
        WFreeAccelTableEntries( tbl->first_entry );
        WRMemFree( tbl );
    }
}

void WFreeAccelTableEntries( WAccelEntry *entry )
{
    WAccelEntry *e;

    while( entry != NULL ) {
        e = entry;
        entry = entry->next;
        if( e->symbol != NULL ) {
            WRMemFree( e->symbol );
        }
        WRMemFree( e );
    }
}

bool WMakeEntryClipData( WAccelEntry *entry, void **data, uint_32 *dsize )
{
    bool        ok;

    ok = (entry != NULL && data != NULL && dsize != NULL);

    if( ok ) {
        if( entry->is32bit ) {
            *dsize = sizeof( AccelTableEntry32 );
        } else {
            *dsize = sizeof( AccelTableEntry );
        }
        *dsize += sizeof( BYTE );
        *data = WRMemAlloc( *dsize );
        ok = (*data != NULL);
    }

    if( ok ) {
        ((BYTE *)(*data))[0] = entry->is32bit;
        memcpy( (BYTE *)(*data) + 1, &entry->u.entry, *dsize );
    }

    return( ok );
}

bool WMakeEntryFromClipData( WAccelEntry *entry, void *data, uint_32 dsize )
{
    size_t      len;
    bool        ok;

    len = 0;
    ok = (entry != NULL && data != NULL && dsize != 0);

    if( ok ) {
        memset( entry, 0, sizeof( WAccelEntry ) );
        entry->is32bit = ((BYTE *)data)[0];
        len = sizeof( AccelTableEntry );
        if( entry->is32bit ) {
            len = sizeof( AccelTableEntry32 );
        }
    }

    if( ok ) {
        memcpy( &entry->u.entry, (BYTE *)data + 1, len );
    }

    return( ok );
}

bool WResolveAllEntrySymbols( WAccelEditInfo *einfo )
{
    WAccelEntry *entry;

    if( einfo == NULL || einfo->tbl == NULL ) {
        return( FALSE );
    }

    entry = einfo->tbl->first_entry;
    while( entry != NULL ) {
        WResolveEntrySymbol( entry, einfo->info->symbol_table );
        entry = entry->next;
    }

    return( TRUE );
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
        return( FALSE );
    }

    entry = einfo->tbl->first_entry;
    while( entry != NULL ) {
        WResolveEntrySymIDs( entry, einfo->info->symbol_table );
        entry = entry->next;
    }

    return( TRUE );
}
