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
#include "ppc.h"
#include "madregs.h"

unsigned        DIGENTRY MITraceSize( void )
{
    return( sizeof( mad_trace_data ) );
}

void            DIGENTRY MITraceInit( mad_trace_data *td, mad_registers *mr )
{
    /* nothing to do */
}

mad_status      DIGENTRY MITraceHaveRecursed( address watch_stack, mad_registers *mr )
{
    if( mr->ppc.sp.u._32[0] < watch_stack.mach.offset ) {
        return( MS_OK );
    }
    return( MS_FAIL );
}

mad_trace_how   DIGENTRY MITraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, mad_registers *mr, address *brk )
{
    mad_disasm_control  dc;
    addr_off            ra;

    dc = DisasmControl( dd, mr );
    ra = td->ra;
    td->ra = mr->ppc.iar.u._32[0] + sizeof( unsigned_32 );
    switch( tk ) {
    case MTK_OUT:
        memset( brk, 0, sizeof( *brk ) );
        brk->mach.offset = ra;
        return( MTH_BREAK );
    case MTK_INTO:
        return( MTH_STEP );
    case MTK_OVER:
        switch( dc & MDC_TYPE_MASK ) {
        case MDC_OPER:
        case MDC_RET:
            return( MTH_STEP );
        }
        break;
    }
    /* break next */
    memset( brk, 0, sizeof( *brk ) );
    brk->mach.offset = td->ra;
    return( MTH_BREAK );
}

mad_status      DIGENTRY MITraceSimulate( mad_trace_data *td, mad_disasm_data *dd, mad_registers *in, mad_registers *out )
{
    return( MS_UNSUPPORTED );
}

void            DIGENTRY MITraceFini( mad_trace_data *td )
{
    /* nothing to do */
}

#define JMP_SHORT       0x4800000c
#define BRK_POINT       0x00000000

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
    a.mach.offset = mr->ppc.iar.u._32[0];
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data ), &data );
    if( data.brk != BRK_POINT ) return( MS_FAIL );
    mr->ppc.iar.u._32[0] += sizeof( unsigned_32 );
    if( data.br != JMP_SHORT ) return( MS_OK );
    if( memcmp( data.name, "WVIDEO\0\0", 8 ) != 0 ) return( MS_OK );
    a.mach.offset = mr->ppc.r3.u._32[0];
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
