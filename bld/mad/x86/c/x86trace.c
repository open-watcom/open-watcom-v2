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
#include "x86.h"
#include "madregs.h"
#include "insdef.h"

unsigned        DIGENTRY MITraceSize( void )
{
    return( sizeof( mad_trace_data ) );
}

void            DIGENTRY MITraceInit( mad_trace_data *td, const mad_registers *mr )
{
    td->prev_opcode = -1;
    td->prev_pref = 0;
}


mad_status      DIGENTRY MITraceHaveRecursed( address watch_stack, const mad_registers *mr )
{
    if( mr->x86.cpu.ss  == watch_stack.mach.segment
     && mr->x86.cpu.esp <  watch_stack.mach.offset ) {
        /*
           we're down some levels in a recursive call -- want to unwind.
        */
        return( MS_OK );
    }
    return( MS_FAIL );
}

/*
 * BreakRet -- put a break point at the return address of a routine
 */

static void BreakRet( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *mr )
{
    address     sp;
    union {
        addr32_off      off32;
        addr48_off      off48;
    }           off;

    sp = GetRegSP( mr );
    if( dd->characteristics & X86AC_BIG ) {
        MCReadMem( sp, sizeof( off.off48 ), &off.off48 );
        sp.mach.offset += sizeof( off.off48 );
        td->brk.mach.offset = off.off48;
    } else {
        MCReadMem( sp, sizeof( off.off32 ), &off.off32 );
        sp.mach.offset += sizeof( off.off32 );
        td->brk.mach.offset = off.off32;
    }
    switch( td->prev_opcode ) {
    case I_CALL:
    case I_RET:
        td->brk.mach.segment = mr->x86.cpu.cs;
        break;
    case I_CALL_FAR:
    case I_INT:
    case I_INTO:
    case I_IRET:
    case I_IRETD:
    case I_RET_FAR:
    case I_RET_FAR_D:
        MCReadMem( sp, sizeof( td->brk.mach.segment ), &td->brk.mach.segment );
        sp.mach.offset += sizeof( td->brk.mach.segment );
        break;
    }
    MCAddrSection( &td->brk );
}


/*
 * BreakNext -- put a break point at the next instruction
 */

static void BreakNext( mad_trace_data *td, mad_disasm_data *dd )
{
    td->brk = dd->addr;
    td->brk.mach.offset += dd->ins.ins_size;
}

static mad_trace_how DoTraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr )
{
    switch( tk ) {
    case MTK_OUT:
        BreakRet( td, dd, mr );
        return( MTH_BREAK );
    case MTK_INTO:
        return( MTH_STEP );
    case MTK_OVER:
        switch( dd->ins.opcode ) {
        case I_CALL:
        case I_CALL_FAR:
        case I_INT:
        case I_INTO:
            BreakNext( td, dd );
            return( MTH_BREAK );
        case I_MOVS:
        case I_CMPS:
        //case I_SCAS:
        case I_LODS:
        //case I_STOS:
        case I_OUTS:
        //case I_INS:
        case I_MOVSD:
        case I_CMPSD:
        case I_SCASD:
        case I_LODSD:
        case I_STOSD:
        case I_OUTSD:
        case I_INSD:
        case I_MOVSW:
        case I_CMPSW:
        case I_SCASW:
        case I_LODSW:
        case I_STOSW:
        case I_OUTSW:
        case I_INSW:
        case I_MOVSB:
        case I_CMPSB:
        case I_SCASB:
        case I_LODSB:
        case I_STOSB:
        case I_OUTSB:
        case I_INSB:
            if( dd->ins.pref & (PREF_REPE|PREF_REPNE|PREF_FWAIT) ) {
                BreakNext( td, dd );
                return( MTH_BREAK );
            }
            return( MTH_STEP );
        }
        return( MTH_STEP );
    case MTK_NEXT:
        BreakNext( td, dd );
        return( MTH_BREAK );
    }
    return( MTH_STOP );
}


/*
 * CheckSpecial - check for instructions we have to handle specially or
 *                we'll have problems
 */

static mad_trace_how CheckSpecial( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *mr, mad_trace_how th )
{
    if( th != MTH_STEP ) return( th );
    switch( dd->ins.opcode ) {
    case I_INT:
        if( dd->ins.pref & EMU_INTERRUPT ) break;
        /* fall through */
    case I_INTO:
        if( !(dd->characteristics & X86AC_REAL) ) break;
        return( MTH_SIMULATE );
    case I_IRET:
    case I_IRETD:
       BreakRet( td, dd, mr );
       return( MTH_STEPBREAK );
    case I_POP:
    case I_MOV:
        if( dd->ins.op[0].mode != ADDR_REG ) return( MTH_STEP );
        switch( dd->ins.op[0].base ) {
        case ES_REG:
        case DS_REG:
            if( MCSystemConfig()->cpu < X86_386 ) break;
        default:
            return( MTH_STEP );
        }
        break;
    case I_PUSHF:
    case I_PUSHFD:
    case I_POPF:
    case I_POPFD:
        break;
    case I_WAIT:
        if( MCSystemConfig()->fpu != X86_EMU ) return( MTH_STEP );
        break;
    default:
        if( dd->ins.pref & EMU_INTERRUPT ) break;
        if( (dd->ins.pref & FP_INS)
         && ((dd->ins.pref & PREF_FWAIT ) || (MCSystemConfig()->fpu==X86_EMU)) ) break;
        return( MTH_STEP );
    }
    BreakNext( td, dd );
    return( MTH_STEPBREAK );
}

static walk_result TouchesScreenBuff( address a, mad_type_handle th, mad_memref_kind mk, void *d )
{
    const mad_registers *mr = d;

    th = th;
    if( mk & MMK_IMPLICIT ) return( WR_CONTINUE );
    switch( MCSystemConfig()->os ) {
    case OS_DOS:
        if( a.mach.segment < 0xa000 || a.mach.segment >= 0xc000 ) return( WR_CONTINUE );
        break;
    case OS_RATIONAL:
        if( a.mach.segment != mr->x86.cpu.cs && a.mach.segment != mr->x86.cpu.ds ) return( WR_CONTINUE );
        if( a.mach.offset < 0xa0000UL || a.mach.offset >= 0xc0000UL ) return( WR_CONTINUE );
        break;
    case OS_AUTOCAD:
    case OS_PHARLAP:
    case OS_ECLIPSE:
        if( a.mach.segment == mr->x86.cpu.cs
         || a.mach.segment == mr->x86.cpu.ds ) return( WR_CONTINUE );
         break;
    }
    MCNotify( MNT_EXECUTE_TOUCH_SCREEN_BUFF, NULL );
    return( WR_STOP );
}

mad_trace_how   DIGENTRY MITraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *brk )
{
    mad_trace_how       th;

    th = DoTraceOne( td, dd, tk, mr );
    th = CheckSpecial( td, dd, mr, th );
    switch( MCSystemConfig()->os ) {
    case OS_OS2:
    case OS_NW386:
    case OS_QNX:
    case OS_WINDOWS:
    case OS_NT:
        break;
    default:
        DoDisasmMemRefWalk( dd, TouchesScreenBuff, mr, (void *)mr );
        break;
    }
    td->prev_opcode = dd->ins.opcode;
    td->prev_pref   = dd->ins.pref;
    switch( th ) {
    case MTH_BREAK:
        switch( td->prev_opcode ) {
        case I_CALL:
        case I_CALL_FAR:
        case I_INT:
        case I_INTO:
            MCNotify( MNT_EXECUTE_LONG, NULL );
            break;
        }
        /* fall through */
    case MTH_STEPBREAK:
        *brk = td->brk;
        break;
    }
    return( th );
}

mad_status      DIGENTRY MITraceSimulate( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    address     sp;
    word        value;

    td = td;
    switch( dd->ins.opcode ) {
    case I_INTO:
        if( !(in->x86.cpu.efl & FLG_O) ) {
            out->x86 = in->x86;
            out->x86.cpu.eip += dd->ins.ins_size;
            return( MS_OK );
        }
        /* fall through */
    case I_INT:
        /* only in real mode */
        if( !(dd->characteristics & X86AC_REAL) ) break;
        out->x86 = in->x86;
        sp = GetRegSP( out );
        sp.mach.offset -= sizeof( word );
        value = out->x86.cpu.efl;
        MCWriteMem( sp, sizeof( value ), &value );
        out->x86.cpu.efl &= ~FLG_I;
        value = out->x86.cpu.cs;
        MCWriteMem( sp, sizeof( value ), &value );
        value = out->x86.cpu.eip;
        MCWriteMem( sp, sizeof( value ), &value );
        out->x86.cpu.esp = sp.mach.offset;
        return( MS_OK );
    }
    return( MS_UNSUPPORTED );
}

void            DIGENTRY MITraceFini( mad_trace_data *td )
{
    td = td;
}

#define JMP_SHORT        ((unsigned char)0XEB)
#define BRK_POINT        ((unsigned char)0XCC)

mad_status              DIGENTRY MIUnexpectedBreak( mad_registers *mr, unsigned *maxp, char *buff )
{
    address     a;
    union {
        byte            b[9];
        addr32_ptr      a32;
        addr48_ptr      a48;
    }           data;
    unsigned    max;
    unsigned    len;

    max = *maxp;
    *maxp = 0;
    if( max > 0 ) buff[0] = '\0';
    a = GetRegIP( mr );
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data.b ), &data );
    if( data.b[0] != BRK_POINT ) return( MS_FAIL );
    mr->x86.cpu.eip += 1;
    if( data.b[1] != JMP_SHORT ) return( MS_OK );
    if( memcmp( &data.b[3], "WVIDEO", 6 ) != 0 ) return( MS_OK );
    a = GetRegSP( mr );
    MCReadMem( a, sizeof( addr_ptr ), &data );
    if( BIG_SEG( a ) ) {
        a.mach = data.a48;
    } else {
        ConvAddr32ToAddr48( data.a32, a.mach );
    }
    len = 0;
    for( ;; ) {
        if( MCReadMem( a, sizeof( data.b[0] ), &data.b[0] ) == 0 ) break;
        a.mach.offset++;
        if( len < max ) buff[len] = data.b[0];
        if( data.b[0] == '\0' ) break;
        ++len;
    }
    if( max > 0 ) buff[max] = '\0';
    *maxp = len;
    return( MS_OK );
}
