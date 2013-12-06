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

unsigned        DIGENTRY MITraceSize( void )
{
    return( sizeof( mad_trace_data ) );
}

void            DIGENTRY MITraceInit( mad_trace_data *td, const mad_registers *mr )
{
    /* nothing to do */
}

mad_status      DIGENTRY MITraceHaveRecursed( address watch_stack, const mad_registers *mr )
{
    if( mr->axp.r[AR_sp].u64.u._32[0] < watch_stack.mach.offset ) {
        return( MS_OK );
    }
    return( MS_FAIL );
}

mad_trace_how   DIGENTRY MITraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *brk )
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

mad_status      DIGENTRY MITraceSimulate( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    mad_disasm_control  dc;
    axpreg              *reg;
    addr_off            new;

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
        new = dd->ins.op[1].value;
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

void            DIGENTRY MITraceFini( mad_trace_data *td )
{
    /* nothing to do */
}

#define JMP_SHORT       0xc3e00002
#define BRK_POINT       0x00000080

mad_status              DIGENTRY MIUnexpectedBreak( mad_registers *mr, unsigned *maxp, char *buff )
{
    address     a;
    struct {
        unsigned_32     brk;
        unsigned_32     br;
        unsigned_8      name[8];
    }           data;
    char        ch;
    unsigned    max;
    unsigned    len;

    max = *maxp;
    *maxp = 0;
    if( max > 0 ) buff[0] = '\0';
    memset( &a, 0, sizeof( a ) );
    a.mach.offset = mr->axp.pal.nt.fir.u._32[0];
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data ), &data );
    if( data.brk != BRK_POINT ) return( MS_FAIL );
    mr->axp.pal.nt.fir.u._32[0] += sizeof( unsigned_32 );
    if( data.br != JMP_SHORT ) return( MS_OK );
    if( memcmp( data.name, "WVIDEO\0\0", 8 ) != 0 ) return( MS_OK );
    a.mach.offset = mr->axp.r[AR_a0].u64.u._32[0];
    len = 0;
    for( ;; ) {
        if( MCReadMem( a, sizeof( ch ), &ch ) == 0 ) break;
        if( len < max ) buff[len] = ch;
        if( ch == '\0' ) break;
        a.mach.offset++;
        ++len;
    }
    if( max > 0 ) buff[max] = '\0';
    *maxp = len;
    return( MS_OK );
}
