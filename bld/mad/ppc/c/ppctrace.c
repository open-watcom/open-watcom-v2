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
* Description:  PowerPC instruction tracing support.
*
****************************************************************************/


#include "ppc.h"
#include "madregs.h"

unsigned DIGENTRY MADImpTraceSize( void )
{
    return( sizeof( mad_trace_data ) );
}

void DIGENTRY MADImpTraceInit( mad_trace_data *td, mad_registers const *mr )
{
    td = td; mr = mr;

    /* nothing to do */
}

mad_status DIGENTRY MADImpTraceHaveRecursed( address watch_stack, mad_registers const *mr )
{
    if( mr->ppc.u1.sp.u._32[I64LO32] < watch_stack.mach.offset ) {
        return( MS_OK );
    }
    return( MS_FAIL );
}

mad_trace_how DIGENTRY MADImpTraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, mad_registers const *mr, address *brk )
{
    mad_disasm_control  dc;
    addr_off            ra;

    dc = DisasmControl( dd, mr );
    ra = td->ra;
    td->ra = mr->ppc.iar.u._32[I64LO32] + sizeof( unsigned_32 );
    switch( tk ) {
    case MTRK_OUT:
        // NYI: this doesn't always work - lr need not contain useful value!
        memset( brk, 0, sizeof( *brk ) );
        brk->mach.offset = ra;
        return( MTRH_BREAK );
    case MTRK_INTO:
        return( MTRH_STEP );
    case MTRK_OVER:
        switch( dc & MDC_TYPE_MASK ) {
        case MDC_CALL:
             /* Handle special case of call to the next instruction, which is used
              * to get the GOT pointer in position independent code.
              */
             if( dd->ins.op[0].value == sizeof( unsigned_32 ) )
                 return( MTRH_STEP );
             break;
        case MDC_OPER:
        case MDC_RET:
        case MDC_JUMP:
            return( MTRH_STEP );
        }
        break;
    }
    /* break next */
    memset( brk, 0, sizeof( *brk ) );
    brk->mach.offset = td->ra;
    return( MTRH_BREAK );
}

mad_status DIGENTRY MADImpTraceSimulate( mad_trace_data *td, mad_disasm_data *dd, mad_registers const *in, mad_registers *out )
{
    td = td; dd = dd; in = in; out = out;

    return( MS_UNSUPPORTED );
}

void DIGENTRY MADImpTraceFini( mad_trace_data *td )
{
    td = td;

    /* nothing to do */
}

#define JMP_SHORT       0x4800000c      // 'b' (to 3rd next instruction)
#define BRK_POINT       0x7fe00008      // 'trap'

mad_status DIGENTRY MADImpUnexpectedBreak( mad_registers *mr, char *buff, size_t *buff_size_p )
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
    a.mach.offset = mr->ppc.iar.u._32[I64LO32];
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data ), &data );
    if( data.brk != BRK_POINT )
        return( MS_FAIL );
    mr->ppc.iar.u._32[I64LO32] += sizeof( unsigned_32 );
    if( data.br != JMP_SHORT )
        return( MS_OK );
    if( memcmp( data.name, "WVIDEO\0\0", 8 ) != 0 )
        return( MS_OK );
    a.mach.offset = mr->ppc.u3.r3.u._32[I64LO32];
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
