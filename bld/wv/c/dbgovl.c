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


#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbgmem.h"
#include "dbginfo.h"
#include <string.h>

typedef struct {
    addr_seg    first;
    addr_seg    last;
    signed_16   shift;
    unsigned_16 spacer;         /* to make a power of two */
} section_info;

extern void     RemoteSectTblRead(void *);
extern void     RemoteSectTblWrite(void *);
extern bool     RemoteOvlRetAddr( address *, unsigned );
extern bool     RemoteOvlSectPos( unsigned, mem_block * );
extern unsigned RemoteOvlSectSize();

extern unsigned         OvlSize;
extern machine_state    *DbgRegs;

static void             *TblCache;
static section_info     *OvlRemap;
static unsigned         OvlCount;
static bool             TblCacheValid;


bool InitOvlState()
{
    unsigned    i;
    mem_block   where;

    if( OvlRemap == NULL ) {
        OvlSize = RemoteOvlSectSize();
        OvlCount = OvlSize * 8;
        _Alloc( OvlRemap, OvlCount * sizeof( section_info ) );
        if( OvlRemap == NULL ) return( FALSE );
        _Alloc( TblCache, OvlSize );
        for( i = 0; i < OvlCount; ++i ) {
            if( RemoteOvlSectPos( i + 1, &where ) ) {
                OvlRemap[i].first = where.start.mach.segment;
                OvlRemap[i].last = where.start.mach.segment + (where.len>>4);
            } else {
                OvlRemap[i].first = 0;
                OvlRemap[i].last = 0;
            }
            OvlRemap[i].shift = 0;
        }
    }
    return( TRUE );
}

void FiniOvlState()
{
    _Free( TblCache );
    TblCache = NULL;
    _Free( OvlRemap );
    OvlRemap = NULL;
    OvlSize = OvlCount = 0;
}

void InvalidateTblCache()
{
    TblCacheValid = FALSE;
}

int SectIsLoaded( unsigned sect_id, int sect_map_id )
{
    char    *tbl;

    if( sect_id == 0 ) return( TRUE );
    if( OvlSize == 0 ) return( FALSE );
    if( sect_id > OvlCount ) return( TRUE );
    --sect_id;
    if( sect_map_id == OVL_MAP_CURR ) {
        if( TblCacheValid ) {
            tbl = TblCache;
        } else if( TblCache != NULL ) {
            tbl = TblCache;
            RemoteSectTblRead( tbl );
            TblCacheValid = TRUE;
        } else {
            _AllocA( tbl, OvlSize );
            RemoteSectTblRead( tbl );
        }
    } else { /* map at time of execution */
        tbl = DbgRegs->ovl;
    }
    return( (tbl[ sect_id / 8 ] & (1 << sect_id % 8) ) != 0 );
}

void SectLoad( unsigned sect_id )
{
    char    *tbl;

    if( sect_id == 0 ) return;
    if( OvlSize == 0 ) return;
    if( sect_id > OvlCount ) return;
    if( TblCache != NULL && SectIsLoaded( sect_id, OVL_MAP_CURR ) ) return;
    _AllocA( tbl, OvlSize );
    memset( tbl, 0, OvlSize );
    --sect_id;
    tbl[ sect_id / 8 ] |= 1 << sect_id % 8;
    RemoteSectTblWrite( tbl );
}

void SectTblRead( machine_state *state )
{
    if( state->ovl != NULL ) {
        RemoteSectTblRead( state->ovl );
    }
}

void SetStateOvlSect( machine_state *state, unsigned sect_id )
{
    if( sect_id == 0 ) return;
    RemoteSectTblWrite( state->ovl );
    SectLoad( sect_id );
    RemoteSectTblRead( state->ovl );
}


bool TransOvlRetAddr( address *addr, unsigned ovl_level )
{
    bool    trans;

    if( OvlSize == 0 ) {
        addr->sect_id = 0;
        return( FALSE );
    }
    trans = RemoteOvlRetAddr( addr, ovl_level );
    return( trans );
}

/*
 * RemapSection - the first segment in 'section' has been moved to
 *                'new_seg'. All other segments have moved by the
 *                same amount.
 */

void RemapSection( unsigned section, unsigned new_seg )
{
    --section;
    OvlRemap[section].shift = new_seg - OvlRemap[section].first;
}

void AddrSection( address *addr, unsigned ovl_map_id )
{
    unsigned    i;
    addr_seg    seg;

    addr->indirect = TRUE;
    for( i = 0; i < OvlCount; ++i ) {
        seg = addr->mach.segment - OvlRemap[i].shift;
        if( seg < OvlRemap[i].first ) continue;
        if( seg >= OvlRemap[i].last ) continue;
        if( SectIsLoaded( i + 1, ovl_map_id ) ) {
            addr->sect_id = i + 1;
            addr->mach.segment = seg;
            return;
        }
    }
    addr->sect_id = 0;
}

void AddrFix( address *addr )
{
    if( OvlSize == 0 ) return;
    if( !addr->indirect ) return;
    if( addr->sect_id == 0 ) return;
    addr->mach.segment += OvlRemap[addr->sect_id-1].shift;
    addr->indirect = FALSE;
}

void AddrFloat( address *addr )
{
    if( OvlSize == 0 ) return;
    if( addr->indirect ) return;
    if( addr->sect_id == 0 ) {
        AddrSection( addr, OVL_MAP_CURR );
    } else {
        addr->mach.segment -= OvlRemap[addr->sect_id-1].shift;
        addr->indirect = TRUE;
    }
}
