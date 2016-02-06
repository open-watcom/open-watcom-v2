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
* Description:  Access to trap file overlay extensions.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "trpovl.h"
#include "trapglbl.h"
#include "dbgovl.h"
#include "trpld.h"
#include "removl.h"


static trap_shandle     SuppOvlId = 0;

#define SUPP_OVL_SERVICE( in, request )         \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppOvlId;    \
        in.req                  = request;


bool InitOvlSupp( void )
{
    SuppOvlId = GetSuppId( OVERLAY_SUPP_NAME );
    return( SuppOvlId != 0 );
}

trap_elen RemoteOvlSectSize( void )
{
    ovl_state_size_req  acc;
    ovl_state_size_ret  ret;

    if( SuppOvlId == 0 )
        return( 0 );
    SUPP_OVL_SERVICE( acc, REQ_OVL_STATE_SIZE );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.size );
}

bool RemoteOvlSectPos( unsigned sect, mem_block *where )
{
    ovl_get_data_req    acc;
    ovl_get_data_ret    ret;

    if( SuppOvlId == 0 ) return( false );
    SUPP_OVL_SERVICE( acc, REQ_OVL_GET_DATA );
    acc.sect_id = sect;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.segment == 0 ) return( false );
    where->len = ret.size;
    where->start.mach.segment = ret.segment;
    where->start.mach.offset = 0;
    where->start.sect_id = sect;
    where->start.indirect = 0;
    return( true );
}

static void CheckRemapping( void )
{
    ovl_get_remap_entry_req     acc;
    ovl_get_remap_entry_ret     ret;

    SUPP_OVL_SERVICE( acc, REQ_OVL_GET_REMAP_ENTRY );
    acc.ovl_addr.sect_id = 0;
    for( ;; ) {
        TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
        if( !ret.remapped ) break;
        RemapSection( ret.ovl_addr.sect_id, ret.ovl_addr.mach.segment );
        acc.ovl_addr = ret.ovl_addr;
    }
}

void RemoteSectTblRead( byte *ovl )
{
    ovl_read_state_req  acc;

    SUPP_OVL_SERVICE( acc, REQ_OVL_READ_STATE );
    TrapSimpAccess( sizeof( acc ), &acc, OvlSize, ovl );
    CheckRemapping();
}

void RemoteSectTblWrite( const byte *ovl )
{
    in_mx_entry         in[2];
    ovl_write_state_req acc;

    SUPP_OVL_SERVICE( acc, REQ_OVL_WRITE_STATE );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = ovl;
    in[1].len = OvlSize;
    TrapAccess( 2, in, 0, NULL );
    CheckRemapping();
    InvalidateTblCache();
}

bool RemoteOvlTransAddr( address *addr )
{
    ovl_trans_vect_addr_req     acc;
    ovl_trans_vect_addr_ret     ret;
    address                     real;

    if( SuppOvlId == 0 ) return( false );
    SUPP_OVL_SERVICE( acc, REQ_OVL_TRANS_VECT_ADDR );
    real = *addr;
    AddrFix( &real );
    ConvAddr48ToAddr32( real.mach, acc.ovl_addr.mach );
    acc.ovl_addr.sect_id = addr->sect_id;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.ovl_addr.sect_id == 0 ) {
        /* not translated */
        return( false );
    }
    addr->sect_id = ret.ovl_addr.sect_id;
    addr->indirect = false;
    ConvAddr32ToAddr48( ret.ovl_addr.mach, addr->mach );
    AddrFloat( addr );
    return( true );
}

bool RemoteOvlRetAddr( address *addr, unsigned ovl_level )
{
    ovl_trans_vect_addr_req     acc;
    ovl_trans_vect_addr_ret     ret;
    address                     real;

    if( SuppOvlId == 0 ) return( false );
    SUPP_OVL_SERVICE( acc, REQ_OVL_TRANS_RET_ADDR );
    real = *addr;
    AddrFix( &real );
    ConvAddr48ToAddr32( real.mach, acc.ovl_addr.mach );
    /* hide overlay level in section id */
    acc.ovl_addr.sect_id = ovl_level;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    if( ret.ovl_addr.sect_id == 0 ) {
        /* not translated */
        return( false );
    }
    addr->sect_id = ret.ovl_addr.sect_id;
    addr->indirect = false;
    ConvAddr32ToAddr48( ret.ovl_addr.mach, addr->mach );
    AddrFloat( addr );
    return( true );
}
