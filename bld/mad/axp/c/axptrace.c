/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include "axp.h"
#include "madregs.h"
#include "brkptcpu.h"


unsigned MADIMPENTRY( TraceSize )( void )
{
    return( sizeof( mad_trace_data ) );
}

void MADIMPENTRY( TraceInit )( mad_trace_data *td, const mad_registers *mr )
{
    /* unused parameters */ (void)td; (void)mr;

    /* nothing to do */
}

mad_status MADIMPENTRY( TraceHaveRecursed )( address watch_stack, const mad_registers *mr )
{
    if( mr->axp.u30.sp.u64.u._32[0] < watch_stack.mach.offset ) {
        return( MS_OK );
    }
    return( MS_FAIL );
}

mad_trace_how MADIMPENTRY( TraceOne )( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *brk )
{
    mad_disasm_control  dc;
    addr_off            ra;

    dc = DisasmControl( dd, mr );
    ra = td->ra;
    td->ra = mr->axp.pal.nt.fir.u._32[0] + sizeof( unsigned_32 );
    switch( tk ) {
    case MTRK_OUT:
        memset( brk, 0, sizeof( *brk ) );
        brk->mach.offset = ra;
        return( MTRH_BREAK );
    case MTRK_INTO:
        switch( dc & MDC_TYPE_MASK ) {
        case MDC_JUMP:
        case MDC_CALL:
        case MDC_RET:
            return( MTRH_SIMULATE );
        }
        return( MTRH_STEP );
    case MTRK_OVER:
        switch( dc & MDC_TYPE_MASK ) {
        case MDC_JUMP:
        case MDC_RET:
            return( MTRH_SIMULATE );
        case MDC_OPER:
            return( MTRH_STEP );
        }
        break;
    }
    /* break next */
    memset( brk, 0, sizeof( *brk ) );
    brk->mach.offset = td->ra;
    return( MTRH_BREAK );
}

mad_status MADIMPENTRY( TraceSimulate )( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    mad_disasm_control  dc;
    axpreg              *reg;
    addr_off            new;

    /* unused parameters */ (void)td;

    dc = DisasmControl( dd, in );
    out->axp = in->axp;
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        out->axp.pal.nt.fir.u._32[0] += sizeof( unsigned_32 );
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_JUMP:
    case MDC_CALL:
    case MDC_RET:
        new = dd->ins.op[1].value.s._32[I64LO32];
        if( dd->ins.op[1].type == DO_RELATIVE ) {
            new += out->axp.pal.nt.fir.u._32[0];
        }
        if( dd->ins.op[1].base != DR_NONE ) {
            new += TRANS_REG( out, dd->ins.op[1].base )->u64.u._32[0];
        }
        if( !(dc & MDC_CONDITIONAL) ) {
            if( dd->ins.op[0].base != DR_AXP_r31 ) {
                reg = TRANS_REG( out, dd->ins.op[0].base );
                reg->u64 = out->axp.pal.nt.fir;
                reg->u64.u._32[0] += sizeof( unsigned_32 );
            }
        }
        out->axp.pal.nt.fir.u._32[0] = new;
        return( MS_OK );
    }
    return( MS_UNSUPPORTED );
}

void MADIMPENTRY( TraceFini )( mad_trace_data *td )
{
    /* unused parameters */ (void)td;

    /* nothing to do */
}

#define JMP_SHORT       0xc3e00002

mad_status MADIMPENTRY( UnexpectedBreak )( mad_registers *mr, char *buff, size_t *buff_size_p )
{
    address     a;
    struct {
        unsigned_32     brk;
        unsigned_32     br;
        unsigned_8      name[8];
    }           data;
    char        ch;
    size_t      buff_size;
    size_t      len;

    buff_size = *buff_size_p;
    *buff_size_p = 0;
    if( buff_size > 0 )
        buff[0] = '\0';
    memset( &a, 0, sizeof( a ) );
    a.mach.offset = mr->axp.pal.nt.fir.u._32[0];
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data ), &data );
    if( data.brk != BRKPOINT )
        return( MS_FAIL );
    mr->axp.pal.nt.fir.u._32[0] += sizeof( unsigned_32 );
    if( data.br != JMP_SHORT )
        return( MS_OK );
    if( memcmp( data.name, "WVIDEO\0\0", 8 ) != 0 )
        return( MS_OK );
    a.mach.offset = mr->axp.u16.a0.u64.u._32[0];
    len = 0;
    while( MCReadMem( a, sizeof( ch ), &ch ) != 0 ) {
        if( len + 1 < buff_size )
            buff[len] = ch;
        if( ch == '\0' )
            break;
        a.mach.offset++;
        ++len;
    }
    *buff_size_p = len;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        buff[buff_size] = '\0';
    }
    return( MS_OK );
}
