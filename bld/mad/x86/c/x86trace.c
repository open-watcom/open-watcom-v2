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
* Description:  Execution tracing support for x86.
*
****************************************************************************/


#include "x86.h"
#include "madregs.h"

unsigned MADIMPENTRY( TraceSize )( void )
{
    return( sizeof( mad_trace_data ) );
}

void MADIMPENTRY( TraceInit )( mad_trace_data *td, const mad_registers *mr )
{
    mr = mr;

    td->prev_ins_type = -1;
    td->prev_ins_flags.u.x86 = DIF_X86_NONE;
}


mad_status MADIMPENTRY( TraceHaveRecursed )( address watch_stack, const mad_registers *mr )
{
    if( mr->x86.cpu.ss == watch_stack.mach.segment && mr->x86.cpu.esp < watch_stack.mach.offset ) {
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
    switch( td->prev_ins_type ) {
    case DI_X86_call:
    case DI_X86_call2:
    case DI_X86_ret:
    case DI_X86_ret2:
        td->brk.mach.segment = mr->x86.cpu.cs;
        break;
    case DI_X86_call3:
    case DI_X86_call4:
    case DI_X86_int:
    case DI_X86_into:
    case DI_X86_iret:
    case DI_X86_iretd:
    case DI_X86_retf:
    case DI_X86_retf2:
        MCReadMem( sp, sizeof( td->brk.mach.segment ), &td->brk.mach.segment );
        sp.mach.offset += sizeof( td->brk.mach.segment );
        break;
    default:
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
    td->brk.mach.offset += dd->ins.size;
}

static mad_trace_how DoTraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr )
{
    switch( tk ) {
    case MTRK_OUT:
        switch( dd->ins.type ) {
        case DI_X86_ret:
        case DI_X86_ret2:
        case DI_X86_iret:
        case DI_X86_iretd:
        case DI_X86_retf:
        case DI_X86_retf2:
            BreakRet( td, dd, mr );
            return( MTRH_BREAK );
        default:
            break;
        }
        return( MTRH_STEP );
    case MTRK_INTO:
        return( MTRH_STEP );
    case MTRK_OVER:
        switch( dd->ins.type ) {
        case DI_X86_call:
            /* Handle special case of a call to the next instruction, which is
             * used under Linux to get the GOT pointer when compiled for
             * 386 processors.
             */
            if( dd->ins.op[0].value == dd->ins.size )
                return( MTRH_STEP );
            /* Fall through for normal handling */
        case DI_X86_call2:
        case DI_X86_call3:
        case DI_X86_call4:
        case DI_X86_int:
        case DI_X86_into:
            BreakNext( td, dd );
            return( MTRH_BREAK );
        case DI_X86_movs:
        case DI_X86_cmps:
        case DI_X86_scas:
        case DI_X86_lods:
        case DI_X86_stos:
        case DI_X86_outs:
        case DI_X86_ins:
            if( dd->ins.flags.u.x86 & (DIF_X86_REPE|DIF_X86_REPNE|DIF_X86_FWAIT) ) {
                BreakNext( td, dd );
                return( MTRH_BREAK );
            }
            return( MTRH_STEP );
        default:
            break;
        }
        return( MTRH_STEP );
    case MTRK_NEXT:
        BreakNext( td, dd );
        return( MTRH_BREAK );
    }
    return( MTRH_STOP );
}


/*
 * CheckSpecial - check for instructions we have to handle specially or
 *                we'll have problems
 */

static mad_trace_how CheckSpecial( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *mr, mad_trace_how th )
{
    if( th != MTRH_STEP ) return( th );
    switch( dd->ins.type ) {
    case DI_X86_int:
        if( dd->ins.flags.u.x86 & DIF_X86_EMU_INT )
            break;
        /* fall through */
    case DI_X86_into:
        if( (dd->characteristics & X86AC_REAL) == 0 )
            break;
        return( MTRH_SIMULATE );
    case DI_X86_iret:
    case DI_X86_iretd:
       BreakRet( td, dd, mr );
       return( MTRH_STEPBREAK );
    case DI_X86_pop:
    case DI_X86_pop2:
    case DI_X86_pop3d:
    case DI_X86_pop3e:
    case DI_X86_pop3s:
    case DI_X86_pop4f:
    case DI_X86_pop4g:
    case DI_X86_mov:
    case DI_X86_mov2:
    case DI_X86_mov3:
    case DI_X86_mov4:
    case DI_X86_mov5:
    case DI_X86_mov6:
    case DI_X86_mov7:
        if( dd->ins.op[0].type != DO_REG )
            return( MTRH_STEP );
        switch( dd->ins.op[0].base ) {
        case DR_X86_es:
        case DR_X86_ds:
            if( ( MCSystemConfig()->cpu & X86_CPU_MASK ) < X86_386 )
                break;
            /* fall through */
        default:
            return( MTRH_STEP );
        }
        break;
    case DI_X86_pushf:
    case DI_X86_pushfd:
    case DI_X86_popf:
    case DI_X86_popfd:
        break;
    case DI_X86_fwait:
        if( MCSystemConfig()->fpu != X86_EMU )
            return( MTRH_STEP );
        break;
    default:
        if( dd->ins.flags.u.x86 & DIF_X86_EMU_INT )
            break;
        if( ( dd->ins.flags.u.x86 & DIF_X86_FP_INS )
            && ( ( dd->ins.flags.u.x86 & DIF_X86_FWAIT ) || ( MCSystemConfig()->fpu == X86_EMU ) ) )
            break;
        return( MTRH_STEP );
    }
    BreakNext( td, dd );
    return( MTRH_STEPBREAK );
}

static walk_result TouchesScreenBuff( address a, mad_type_handle th, mad_memref_kind mk, void *d )
{
    const mad_registers *mr = d;

    th = th;
    if( mk & MMK_IMPLICIT )
        return( WR_CONTINUE );
    switch( MCSystemConfig()->os ) {
    case MAD_OS_DOS:
        if( a.mach.segment < 0xa000 || a.mach.segment >= 0xc000 )
            return( WR_CONTINUE );
        break;
    case MAD_OS_RATIONAL:
        if( a.mach.segment != mr->x86.cpu.cs && a.mach.segment != mr->x86.cpu.ds )
            return( WR_CONTINUE );
        if( a.mach.offset < 0xa0000UL || a.mach.offset >= 0xc0000UL )
            return( WR_CONTINUE );
        break;
    case MAD_OS_AUTOCAD:
    case MAD_OS_PHARLAP:
    case MAD_OS_ECLIPSE:
        if( a.mach.segment == mr->x86.cpu.cs
            || a.mach.segment == mr->x86.cpu.ds )
            return( WR_CONTINUE );
        break;
    }
    MCNotify( MNT_EXECUTE_TOUCH_SCREEN_BUFF, NULL );
    return( WR_STOP );
}

mad_trace_how MADIMPENTRY( TraceOne )( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *brk )
{
    mad_trace_how       th;

    th = DoTraceOne( td, dd, tk, mr );
    th = CheckSpecial( td, dd, mr, th );
    switch( MCSystemConfig()->os ) {
    case MAD_OS_OS2:
    case MAD_OS_NW386:
    case MAD_OS_QNX:
    case MAD_OS_WINDOWS:
    case MAD_OS_NT:
        break;
    default:
        DoDisasmMemRefWalk( dd, TouchesScreenBuff, mr, (void *)mr );
        break;
    }
    td->prev_ins_type = dd->ins.type;
    td->prev_ins_flags.u.x86 = dd->ins.flags.u.x86;
    switch( th ) {
    case MTRH_BREAK:
        switch( td->prev_ins_type ) {
        case DI_X86_call:
        case DI_X86_call2:
        case DI_X86_call3:
        case DI_X86_call4:
        case DI_X86_int:
        case DI_X86_into:
            MCNotify( MNT_EXECUTE_LONG, NULL );
            break;
        default:
            break;
        }
        /* fall through */
    case MTRH_STEPBREAK:
        *brk = td->brk;
        break;
    }
    return( th );
}

mad_status MADIMPENTRY( TraceSimulate )( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    address     sp;
    word        value;

    td = td;
    switch( dd->ins.type ) {
    case DI_X86_into:
        if( !( in->x86.cpu.efl & FLG_O ) ) {
            out->x86 = in->x86;
            out->x86.cpu.eip += dd->ins.size;
            return( MS_OK );
        }
        /* fall through */
    case DI_X86_int:
        /* only in real mode */
        if( (dd->characteristics & X86AC_REAL) == 0 )
            break;
        out->x86 = in->x86;
        sp = GetRegSP( out );
        sp.mach.offset -= sizeof( word );
        value = (word)out->x86.cpu.efl;
        MCWriteMem( sp, sizeof( value ), &value );
        out->x86.cpu.efl &= ~FLG_I;
        value = out->x86.cpu.cs;
        MCWriteMem( sp, sizeof( value ), &value );
        value = (word)out->x86.cpu.eip;
        MCWriteMem( sp, sizeof( value ), &value );
        out->x86.cpu.esp = sp.mach.offset;
        return( MS_OK );
    default:
        break;
    }
    return( MS_UNSUPPORTED );
}

void MADIMPENTRY( TraceFini )( mad_trace_data *td )
{
    td = td;
}

#define JMP_SHORT        ((unsigned char)0XEB)
#define BRK_POINT        ((unsigned char)0XCC)

mad_status MADIMPENTRY( UnexpectedBreak )( mad_registers *mr, char *buff, size_t *buff_size_p )
{
    address     a;
    union {
        byte            b[9];
        addr32_ptr      a32;
        addr48_ptr      a48;
    }           data;
    size_t      buff_size;
    size_t      len;

    buff_size = *buff_size_p;
    *buff_size_p = 0;
    if( buff_size > 0 )
        buff[0] = '\0';
    a = GetRegIP( mr );
    memset( &data, 0, sizeof( data ) );
    MCReadMem( a, sizeof( data.b ), data.b );
    if( data.b[0] != BRK_POINT )
        return( MS_FAIL );
    mr->x86.cpu.eip += 1;
    if( data.b[1] != JMP_SHORT )
        return( MS_OK );
    if( memcmp( data.b + 3, "WVIDEO", 6 ) != 0 )
        return( MS_OK );
    a = GetRegSP( mr );
    MCReadMem( a, sizeof( addr_ptr ), &data );
    if( BIG_SEG( a ) ) {
        a.mach = data.a48;
    } else {
        ConvAddr32ToAddr48( data.a32, a.mach );
    }
    len = 0;
    while( MCReadMem( a, sizeof( data.b[0] ), data.b ) != 0 ) {
        a.mach.offset++;
        if( len + 1 < buff_size )
            buff[len] = data.b[0];
        if( data.b[0] == '\0' )
            break;
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
