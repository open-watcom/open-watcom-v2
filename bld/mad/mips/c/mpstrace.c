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
* Description:  MIPS instruction tracing support.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#include "mips.h"
#include "madregs.h"


/* Implementation Notes:
 *
 * MIPS architecture does not support single stepping (just like Alpha AXP).
 * That means we need to use breakpoints only and simulate control transfer
 * instructions. MITraceOne() never returns MTRH_STEP because we don't want
 * to have to deal with inability to step inside trap files (in part because
 * PTRACE_SINGLESTEP doesn't work at all on Linux).
 *
 * Tracing on MIPS has an extra twist compared to Alpha. For instructions
 * with a delay slot, breakpoint cannot be placed into the delay slot (which
 * normally contains a NOP instruction). The breakpoint fires before the
 * instruction is executed. This isn't an issue for branch instructions
 * (since we simulate them anyway) but it's a problem when stepping over calls.
 */

unsigned DIGENTRY MITraceSize( void )
{
    return( sizeof( mad_trace_data ) );
}

void DIGENTRY MITraceInit( mad_trace_data *td, mad_registers const *mr )
{
    /* nothing to do */
}

mad_status DIGENTRY MITraceHaveRecursed( address watch_stack, mad_registers const *mr )
{
    if( mr->mips.u29.sp.u._32[I64LO32] < watch_stack.mach.offset ) {
        return( MS_OK );
    }
    return( MS_FAIL );
}

mad_trace_how DIGENTRY MITraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, mad_registers const *mr, address *brk )
{
    mad_disasm_control  dc;
    addr_off            ra;
    mad_trace_how       how = MTRH_BREAK;

    dc = DisasmControl( dd, mr );
    ra = td->ra;
    td->ra = mr->mips.pc.u._32[I64LO32] + sizeof( unsigned_32 );
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
        how = MTRH_STEPBREAK;
        break;
    case MTRK_OVER:
        switch( dc & MDC_TYPE_MASK ) {
        case MDC_JUMP:
        case MDC_RET:
            return( MTRH_SIMULATE );
        case MDC_CALL:
            td->ra += sizeof( unsigned_32 );    // set bp after delay slot
        }
        how = MTRH_STEPBREAK;
        break;
    }
    /* break next */
    memset( brk, 0, sizeof( *brk ) );
    brk->mach.offset = td->ra;
    return( how );
}

/* This had better be consistent with DisasmControl() in mipsdisas.c */
mad_status DIGENTRY MITraceSimulate( mad_trace_data *td, mad_disasm_data *dd, mad_registers const *in, mad_registers *out )
{
    mad_disasm_control  dc;
    unsigned_64         *reg;
    addr_off            new;
    int                 base;

    dc = DisasmControl( dd, in );
    out->mips = in->mips;
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        out->mips.pc.u._32[I64LO32] += sizeof( unsigned_32 );
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_JUMP:
    case MDC_CALL:
        if( dd->ins.op[0].type == DO_ABSOLUTE ) {
            // takes care of 'j' and 'jal'
            new = dd->ins.op[0].value;
        } else if( dd->ins.num_ops == 3 && dd->ins.op[2].type == DO_RELATIVE ) {
            // takes care of all 'b' instructions with three operands
            new  = out->mips.pc.u._32[I64LO32];
            new += dd->ins.op[2].value;
        } else if( dd->ins.num_ops == 2 && dd->ins.op[1].type == DO_RELATIVE ) {
            // takes care of all 'b' instructions with two operands
            new  = out->mips.pc.u._32[I64LO32];
            new += dd->ins.op[1].value;
        } else {
            // takes care of 'jalr' and 'jr'
            new = TRANS_REG( out, dd->ins.op[dd->ins.num_ops - 1].base )->u._32[I64LO32];
        }

        if( dd->ins.flags.u.mips & DIF_MIPS_LINK ) {
            // note that linking conditonal jumps unconditionally update r31!
            if( dd->ins.type == DI_MIPS_JALR )
                base = dd->ins.op[0].base;  // 'jalr' may update any register
            else
                base = DR_MIPS_r31;         // everything else updates r31
            if( base != DR_MIPS_r0 ) {
                reg = TRANS_REG( out, base );
                *reg = out->mips.pc;
                reg->u._32[I64LO32] += sizeof( unsigned_32 ) * 2;
            }
        }
        out->mips.pc.u._32[I64LO32] = new;
        return( MS_OK );
    case MDC_RET:
        // this is a 'jr ra'
        new = TRANS_REG( out, dd->ins.op[0].base )->u._32[I64LO32];
        out->mips.pc.u._32[I64LO32] = new;
        return( MS_OK );
    }
    return( MS_UNSUPPORTED );
}

void DIGENTRY MITraceFini( mad_trace_data *td )
{
    /* nothing to do */
}

#define JMP_SHORT       0x10000003      // 'beq $zero,$zero' (3 instructions forward)
#define BRK_POINT       0x0000000D      // 'break' (with code of 0)


mad_status DIGENTRY MIUnexpectedBreak( mad_registers *mr, unsigned *maxp, char *buff )
{
    address     a;
    struct {
        unsigned_32     brk;
        unsigned_32     beq;
        unsigned_32     nop;
        unsigned_8      name[8];
    }           data;
    char        ch;
    unsigned    max;
    unsigned    len;

    max = *maxp;
    *maxp = 0;
    if( max > 0 ) buff[0] = '\0';
    memset( &a, 0, sizeof( a ) );
    a.mach.offset = mr->mips.pc.u._32[I64LO32];
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data ), &data );
    if( data.brk != BRK_POINT ) return( MS_FAIL );
    mr->mips.pc.u._32[I64LO32] += sizeof( unsigned_32 );
    if( data.beq != JMP_SHORT ) return( MS_OK );
    if( memcmp( data.name, "WVIDEO\0\0", 8 ) != 0 ) return( MS_OK );
    a.mach.offset = mr->mips.u4.a0.u._32[I64LO32];
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
