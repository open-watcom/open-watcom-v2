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


#include <stddef.h>
#include <string.h>
#include "axp.h"
#include "madregs.h"

mad_string              DIGENTRY MICallStackGrowsUp( void )
{
    return( MS_FAIL );
}

const mad_string        *DIGENTRY MICallTypeList( void )
{
    static const mad_string list[] = { MSTR_NIL };

    return( list );
}

mad_status      DIGENTRY MICallBuildFrame( mad_string call, address ret, address rtn, const mad_registers *in, mad_registers *out )
{
    call = call;
    out->axp = in->axp;
    //NYI: 64 bit
    out->axp.r[AR_ra].u64.u._32[0] = ret.mach.offset;
    out->axp.pal.nt.fir.u._32[0] = rtn.mach.offset;
    return( MS_OK );
}

const mad_reg_info      *DIGENTRY MICallReturnReg( mad_string call, address rtn )
{
    return( &RegList[IDX_v0].info );
}

const mad_reg_info      **DIGENTRY MICallParmRegList( mad_string call, address rtn )
{
    static const mad_reg_info *list[] = {
        &RegList[IDX_a0].info, &RegList[IDX_a1].info, &RegList[IDX_a2].info,
        &RegList[IDX_a3].info, &RegList[IDX_a4].info, &RegList[IDX_a5].info,
        NULL };

    return( list );
}

static int GetAnOffset( addr_off in, addr_off *off )
{
    address     a;

    memset( &a, 0, sizeof( a ) );
    a.mach.offset = in;
    return( MCReadMem( a, sizeof( *off ), off ) == sizeof( *off ) );
}

#define NO_OFF  (~(addr_off)0)

unsigned        DIGENTRY MICallUpStackSize( void )
{
    return( sizeof( mad_call_up_data ) );
}

mad_status      DIGENTRY MICallUpStackInit( mad_call_up_data *cud, const mad_registers *mr )
{
    cud->ra = mr->axp.r[AR_ra].u64.u._32[0];
    cud->sp = mr->axp.r[AR_sp].u64.u._32[0];
    cud->fp = mr->axp.r[AR_fp].u64.u._32[0];
    return( MS_OK );
}

mad_status      DIGENTRY MICallUpStackLevel( mad_call_up_data *cud,
                                const address *start,
                                unsigned rtn_characteristics,
                                long return_disp,
                                const mad_registers *in,
                                address *execution,
                                address *frame,
                                address *stack,
                                mad_registers **out )
{
    axp_pdata           pdata;
    mad_disasm_data     dd;
    mad_status          ms;
    address             curr;
    addr_off            prev_ra_off;
    addr_off            prev_sp_off;
    addr_off            prev_fp_off;
    addr_off            frame_size;
    addr_off            frame_start;

    start = start;
    rtn_characteristics = rtn_characteristics;
    in = in;
    *out = NULL;
    if( cud->ra == 0 ) return( MS_FAIL );
    if( cud->sp == 0 ) return( MS_FAIL );
    ms = GetPData( execution->mach.offset, &pdata );
    if( ms != MS_OK ) return( ms );

    frame_size = 0;
    frame_start = cud->sp;
    prev_ra_off = NO_OFF;
    prev_sp_off = NO_OFF;
    prev_fp_off = NO_OFF;
    curr = *execution;
    curr.mach.offset = pdata.beg_addr.u._32[0];
    if( curr.mach.offset == 0 ) return( MS_FAIL );
    for( ;; ) {
        if( curr.mach.offset >= execution->mach.offset ) break;
        if( curr.mach.offset >= pdata.pro_end_addr.u._32[0] ) break;
        ms = DisasmOne( &dd, &curr, 0 );
        if( ms != MS_OK ) return( ms );
        if( curr.mach.offset == (pdata.beg_addr.u._32[0] + sizeof( unsigned_32 )) ) {
            if( dd.ins.type != DI_AXP_LDA ) return( MS_FAIL );
            frame_size = -dd.ins.op[1].value;
        }
        switch( dd.ins.type ) {
        case DI_AXP_STQ:
            switch( TRANS_REG( dd.ins.op[0].base ) ) {
            case AR_ra:
                prev_ra_off = dd.ins.op[1].value;
                break;
            case AR_sp:
                prev_sp_off = dd.ins.op[1].value;
                break;
            case AR_fp:
                prev_fp_off = dd.ins.op[1].value;
                break;
            }
            break;
        case DI_AXP_BIS:
            if( dd.ins.op[0].type == DO_REG
             && dd.ins.op[1].type == DO_REG
             && dd.ins.op[2].type == DO_REG
             && dd.ins.op[0].base == DR_AXP_r31
             && dd.ins.op[1].base == DR_AXP_r30
             && dd.ins.op[2].base == DR_AXP_r15 ) {
                /* variable frame routine, and we've done all the prolog */
                frame_start = cud->fp;
            }
            break;
        }
    }
    if( frame_start == 0 ) return( MS_FAIL );
    if( prev_sp_off != NO_OFF ) {
        if( !GetAnOffset( frame_start + prev_sp_off, &cud->sp ) ) return( MS_FAIL );
    } else {
        cud->sp = frame_start + frame_size;
    }
    if( prev_fp_off != NO_OFF ) {
        if( !GetAnOffset( frame_start + prev_fp_off, &cud->fp ) ) return( MS_FAIL );
    }
    if( prev_ra_off != NO_OFF ) {
        if( !GetAnOffset( frame_start + prev_ra_off, &cud->ra ) ) return( MS_FAIL );
    }
    stack->mach.offset = cud->sp;
    execution->mach.offset = cud->ra;
    if( VariableFrame( execution->mach.offset ) ) {
        frame->mach.offset = cud->fp;
    } else {
        frame->mach.offset = cud->sp;
    }
    return( MS_OK );
}
