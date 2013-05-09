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
* Description:  Interface with Overlay Manager
*
****************************************************************************/


#include <string.h>
#include <i86.h>
#include "trpimp.h"
#include "ovldbg.h"
#include "doscomm.h"

extern unsigned_8           RunProg( trap_cpu_regs *, trap_cpu_regs * );
extern ovl_dbg_hook_func    OvlTrap;

extern trap_cpu_regs        TaskRegs;

static trap_elen            OvlStateSize;
static ovl_dbg_req_func     *OvlRequest;
static ovl_dbg_req_func     NoOvlsHdlr;

static int far NoOvlsHdlr( int req, void far *data )
/**************************************************/
{
    return( 0 );
}

void NullOvlHdlr( void )
/**********************/
{
    OvlRequest = &NoOvlsHdlr;
}

bool CheckOvl( addr32_ptr start )
/*******************************/
{
    struct ovl_header   far *hdr;

    hdr = MK_FP( start.segment, start.offset );
    if( hdr->signature == OVL_SIGNATURE ) {
        hdr->hook = &OvlTrap;
        OvlRequest = MK_FP( start.segment, hdr->handler_offset );
        RunProg( &TaskRegs, &TaskRegs ); /* init overlay manager */
        return( TRUE );
    }
    return( FALSE );
}

trap_elen ReqOvl_state_size( void )
/********************************/
{
    ovl_state_size_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->size = OvlRequest( OVLDBG_GET_STATE_SIZE, 0 );
    OvlStateSize = ret->size;
    return( sizeof( *ret ) );
}

trap_elen ReqOvl_read_state( void )
/********************************/
{
    OvlRequest( OVLDBG_GET_OVERLAY_STATE, GetOutPtr( 0 ) );
    return( OvlStateSize );
}

trap_elen ReqOvl_write_state( void )
/*********************************/
{
    SetUsrTask(); /* overlay manager needs access to its file table */
    OvlRequest( OVLDBG_SET_OVERLAY_STATE, GetInPtr( sizeof( ovl_write_state_req ) ) );
    SetDbgTask();
    return( 0 );
}

trap_elen ReqOvl_trans_vect_addr( void )
/*************************************/
{
    ovl_trans_vect_addr_req     *acc;
    ovl_trans_vect_addr_ret     *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( OvlRequest( OVLDBG_TRANSLATE_VECTOR_ADDR, &acc->ovl_addr ) ) {
        ret->ovl_addr = acc->ovl_addr;
    } else {
        ret->ovl_addr.sect_id = 0;
    }
    return( sizeof( *ret ) );
}

trap_elen ReqOvl_trans_ret_addr( void )
/************************************/
{
    ovl_trans_ret_addr_req      *acc;
    ovl_trans_ret_addr_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( OvlRequest( OVLDBG_TRANSLATE_RETURN_ADDR, &acc->ovl_addr ) ) {
        ret->ovl_addr = acc->ovl_addr;
    } else {
        ret->ovl_addr.sect_id = 0;
    }
    return( sizeof( *ret ) );
}

trap_elen ReqOvl_get_remap_entry( void )
/*************************************/
{
    ovl_get_remap_entry_req     *acc;
    ovl_get_remap_entry_ret     *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->remapped = OvlRequest( OVLDBG_GET_MOVED_SECTION, &acc->ovl_addr );
    ret->ovl_addr = acc->ovl_addr;
    return( sizeof( *ret ) );
}

trap_elen ReqOvl_get_data( void )
/******************************/
{
    ovl_get_data_req    *acc;
    ovl_get_data_ret    *ret;
    ovl_address         addr;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    addr.sect_id = acc->sect_id;
    if( !OvlRequest( OVLDBG_GET_SECTION_DATA, &addr ) ) {
        addr.mach.segment = 0;
    }
    ret->segment = addr.mach.segment;
    ret->size = addr.sect_id * 16UL;
    return( sizeof( *ret ) );
}
