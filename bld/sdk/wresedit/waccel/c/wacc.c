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
static void            WInitDataFromAccelTable ( WAccelTable *, void * );
static void            WInitAccelTable         ( WAccelInfo *, WAccelTable *);
static int             WCalcAccelTableSize     ( WAccelTable * );
static int             WCalcNumAccelEntries    ( WAccelInfo * );
static WAccelTable    *WAllocAccelTable        ( int );
static void            WFreeAccelTable         ( WAccelTable * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WAccelEditInfo *WAllocAccelEInfo ( void )
{
    WAccelEditInfo *einfo;

    einfo = (WAccelEditInfo *) WMemAlloc ( sizeof(WAccelEditInfo) );

    if ( einfo ) {
        memset ( einfo, 0, sizeof(WAccelEditInfo) );
        einfo->current_pos = -1;
    }

    return ( einfo );
}

void WFreeAccelEInfo ( WAccelEditInfo *einfo )
{
    if ( einfo ) {
        if ( einfo->tbl ) {
            WFreeAccelTable ( einfo->tbl );
            einfo->tbl = NULL;
        }
        if ( einfo->wsb ) {
            WDestroyStatusLine ( einfo->wsb );
            einfo->wsb = NULL;
        }
        if ( einfo->ribbon ) {
            WDestroyRibbon ( einfo );
        }
        if ( ( einfo->edit_dlg != (HWND)NULL ) &&
             IsWindow ( einfo->edit_dlg ) ) {
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

void WMakeDataFromAccelTable ( WAccelTable *tbl, void **data, int *size )
{
    if ( data && size ) {
        *size = WCalcAccelTableSize ( tbl );
        if ( *size ) {
            *data = WMemAlloc ( *size );
            if ( *data ) {
                WInitDataFromAccelTable ( tbl, *data );
            } else {
                *size = 0;
            }
        } else {
            *data = NULL;
        }
    }
}

WAccelTable *WMakeAccelTableFromInfo ( WAccelInfo *info )
{
    WAccelTable *tbl;
    int          num, ok;

    tbl = NULL;

    ok = ( info != NULL );

    if( ok ) {
        num = WCalcNumAccelEntries( info );
        tbl = WAllocAccelTable( num );
        ok = ( tbl != NULL );
    }

    if( ok ) {
        tbl->is32bit = info->is32bit;
        if( num ) {
            WInitAccelTable( info, tbl );
        }
    }

    if( ok ) {
        info->data      = NULL;
        info->data_size = 0;
    } else {
        if( tbl ) {
            WFreeAccelTable( tbl );
            tbl = NULL;
        }
    }

    return( tbl );
}

void WInitDataFromAccelTable ( WAccelTable *tbl, void *tdata )
{
    WAccelEntry       *entry;
    AccelTableEntry   *data;
    AccelTableEntry32 *data32;
    int                i;

    entry = tbl->first_entry;
    i = -1;

    if ( tbl->is32bit ) {
        data32 = (AccelTableEntry32 *) tdata;
        while ( entry ) {
            i++;
            data32[i] = entry->entry32;
            entry = entry->next;
        }
        data32[i].Flags |= ACCEL_LAST;
    } else {
        data = (AccelTableEntry *) tdata;
        while ( entry ) {
            i++;
            data[i] = entry->entry;
            entry = entry->next;
        }
        data[i].Flags |= ACCEL_LAST;
    }
}

void WInitAccelTable ( WAccelInfo *info, WAccelTable *tbl )
{
    WAccelEntry       *entry;
    AccelTableEntry   *data;
    AccelTableEntry32 *data32;
    int                i;

    entry = tbl->first_entry;
    if ( !entry ) {
        return;
    }

    i = -1;

    if ( info->is32bit ) {
        data32 = (AccelTableEntry32 *) info->data;
        do {
            i++;
            entry->is32bit = TRUE;
            entry->entry32 = data32[i];
            entry->entry32.Flags &= ~ACCEL_LAST;
            entry = entry->next;
        } while ( entry && !( data32[i].Flags & ACCEL_LAST ) );
    } else {
        data = (AccelTableEntry *) info->data;
        do {
            i++;
            entry->is32bit = FALSE;
            entry->entry   = data[i];
            entry->entry.Flags &= ~ACCEL_LAST;
            entry = entry->next;
        } while ( entry && !( data[i].Flags & ACCEL_LAST ) );
    }

    if ( entry ) {
        entry->prev->next = NULL;
        WFreeAccelTableEntries ( entry );
    }

    tbl->num = i + 1;
}

int WCalcAccelTableSize ( WAccelTable *tbl )
{
    int size;

    if ( !tbl ) {
        return ( 0 );
    }

    if ( tbl->is32bit ) {
        size = sizeof (AccelTableEntry32);
    } else {
        size = sizeof (AccelTableEntry);
    }

    size *= tbl->num;

    return ( size );
}

int WCalcNumAccelEntries ( WAccelInfo *info )
{
    int num, size;

    if ( !info ) {
        return ( 0 );
    }

    if ( info->is32bit ) {
        size = sizeof (AccelTableEntry32);
    } else {
        size = sizeof (AccelTableEntry);
    }

    num = info->data_size / size;

    if ( info->data_size % size ) {
        num++;
    }

    return ( num );
}

Bool WInsertAccelTableEntry ( WAccelTable *tbl, WAccelEntry *after,
                              WAccelEntry *entry )
{
    Bool ok;

    ok = ( tbl && entry );

    if ( ok ) {
        if ( after ) {
            if ( after->next ) {
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

    return ( ok );
}

Bool WFreeAccelTableEntry( WAccelTable *tbl, WAccelEntry *entry )
{
    Bool ok;

    ok = ( tbl && entry );

    if( ok ) {
        if( entry->next ) {
            entry->next->prev = entry->prev;
        }
        if( entry->prev ) {
            entry->prev->next = entry->next;
        }
        if( tbl->first_entry == entry ) {
            tbl->first_entry = entry->next;
        }
        if( entry->symbol != NULL ) {
            WMemFree( entry->symbol );
        }
        WMemFree( entry );
        tbl->num--;
    }

    return( ok );
}

WAccelTable *WAllocAccelTable ( int num )
{
    WAccelTable *tbl;
    WAccelEntry *entry;
    int          i;

    tbl = (WAccelTable *) WMemAlloc ( sizeof(WAccelTable) );

    if ( !tbl ) {
        return ( NULL );
    }

    tbl->num     = num;
    tbl->is32bit = FALSE;
    if ( num ) {
        entry = NULL;
        for ( i=0; i<num; i++ ) {
            if ( entry ) {
                entry->next = (WAccelEntry *) WMemAlloc(sizeof(WAccelEntry));
                if ( entry ) {
                    entry->next->symbol = NULL;
                    entry->next->prev = entry;
                    entry = entry->next;
                }
            } else {
                tbl->first_entry = (WAccelEntry *)
                    WMemAlloc ( sizeof(WAccelEntry) );
                tbl->first_entry->symbol = NULL;
                entry = tbl->first_entry;
                entry->prev = NULL;
            }
            if ( !entry ) {
                WFreeAccelTable ( tbl );
                tbl = NULL;
                break;
            }
        }
        if ( entry ) {
            entry->next = NULL;
        }
    } else {
        tbl->first_entry = NULL;
    }

    return ( tbl );
}

void WFreeAccelTable ( WAccelTable *tbl )
{
    if ( tbl ) {
        WFreeAccelTableEntries ( tbl->first_entry );
        WMemFree ( tbl );
    }
}

void WFreeAccelTableEntries ( WAccelEntry *entry )
{
    WAccelEntry *e;

    while ( entry ) {
        e = entry;
        entry = entry->next;
        if( e->symbol != NULL ) {
            WMemFree( e->symbol );
        }
        WMemFree ( e );
    }
}

Bool WMakeEntryClipData( WAccelEntry *entry, void **data, uint_32 *dsize )
{
    Bool        ok;

    ok = ( entry && data && dsize );

    if( ok ) {
        if( entry->is32bit ) {
            *dsize = sizeof(AccelTableEntry32);
        } else {
            *dsize = sizeof(AccelTableEntry);
        }
        *dsize += sizeof( BYTE );
        *data = WMemAlloc( *dsize );
        ok = ( *data != NULL );
    }

    if( ok ) {
        ((BYTE *)(*data))[0] = entry->is32bit;
        memcpy( (BYTE *)(*data) + 1, &entry->entry, *dsize );
    }

    return( ok );
}

Bool WMakeEntryFromClipData( WAccelEntry *entry, void *data, uint_32 dsize )
{
    int         len;
    Bool        ok;

    ok = ( entry && data && dsize );

    if( ok ) {
        memset( entry, 0, sizeof(WAccelEntry) );
        entry->is32bit = ((BYTE *)data)[0];
        len = sizeof(AccelTableEntry);
        if( entry->is32bit ) {
            len = sizeof(AccelTableEntry32);
        }
    }

    if( ok ) {
        memcpy( &entry->entry, (BYTE *)data + 1, len );
    }

    return( ok );
}

Bool WResolveAllEntrySymbols( WAccelEditInfo *einfo )
{
    WAccelEntry *entry;

    if( !einfo || !einfo->tbl ) {
        return( FALSE );
    }

    entry = einfo->tbl->first_entry;
    while( entry ) {
        WResolveEntrySymbol( entry, einfo->info->symbol_table );
        entry = entry->next;
    }

    return( TRUE );
}

Bool WResolveEntrySymbol( WAccelEntry *entry, WRHashTable *symbol_table )
{
    uint_16             id;
    WRHashValueList     *vlist;
    Bool                ok;

    vlist = NULL;

    ok = ( entry && symbol_table );

    if( ok ) {
        if( entry->is32bit ) {
            id = entry->entry32.Id;
        } else {
            id = (uint_16)entry->entry.Id;
        }
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

Bool WResolveEntrySymIDs( WAccelEntry *entry, WRHashTable *symbol_table )
{
    WRHashValue         hv;
    Bool                ok;

    ok = ( entry && symbol_table );

    if( ok ) {
        ok = WRLookupName( symbol_table, entry->symbol, &hv );
    }

    if( ok ) {
        if( entry->is32bit ) {
            entry->entry32.Id = (uint_16)hv;
        } else {
            entry->entry.Id = (uint_16)hv;
        }
    }

    return( ok );
}

Bool WResolveAllEntrySymIDs( WAccelEditInfo *einfo )
{
    WAccelEntry *entry;

    if( !einfo || !einfo->tbl ) {
        return( FALSE );
    }

    entry = einfo->tbl->first_entry;
    while( entry ) {
        WResolveEntrySymIDs( entry, einfo->info->symbol_table );
        entry = entry->next;
    }

    return( TRUE );
}

