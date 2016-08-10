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
* Description:  MAD interface to x86 disassembler.
*
****************************************************************************/


#include <ctype.h>
#include "walloca.h"
#include "x86.h"
#include "x86types.h"
#include "madregs.h"

#include "clibext.h"


#define OP_1            0

static dis_handle       DH;
address                 DbgAddr;
static dword            RegValue( const mad_registers *mr, int idx );
static char             ScratchBuff[40];
static struct {
    address             start;
    unsigned            len;
    unsigned            want;
    unsigned_8          data[0x80];
} Cache;

unsigned DIGENTRY MIDisasmDataSize( void )
{
    return( sizeof( mad_disasm_data ) );
}

unsigned DIGENTRY MIDisasmNameMax( void )
{
    return( DisInsNameMax( &DH ) );
}

void DoCode( mad_disasm_data *dd, int big )
{
    DisDecodeInit( &DH, &dd->ins );
    dd->addr = DbgAddr;
    dd->ins.flags.u.x86 = DIF_X86_NONE;
    if( big ) {
        dd->ins.flags.u.x86 = DIF_X86_USE32_FLAGS;
    }
    DisDecode( &DH, dd, &dd->ins );
}

void DecodeIns( address *a, mad_disasm_data *dd, int big )
{
    DbgAddr = *a;
    InitCache( DbgAddr, 0x8 );
    DoCode( dd, big );
    a->mach.offset += dd->ins.size;
}

mad_status GetDisasmPrev( address *a )
{
    mad_disasm_data dd;
    addr_off        start;
    addr_off        curr_off;
    addr_off        prev;
    unsigned        backup;
    int             big;

    DbgAddr = *a;
    big = BIG_SEG( DbgAddr );
    backup = ( big ) ? 0x40 : 0x20;
    curr_off = DbgAddr.mach.offset;
    DbgAddr.mach.offset = ( curr_off <= backup ) ? 0 : ( curr_off - backup );
    InitCache( DbgAddr, curr_off - DbgAddr.mach.offset );
    for( start = DbgAddr.mach.offset; start < curr_off; ++start ) {
        DbgAddr.mach.offset = start;
        for( ;; ) {
            prev = DbgAddr.mach.offset;
            DoCode( &dd, big );
            if( dd.ins.size == 0 )
                break;      /* invalid address */
            DbgAddr.mach.offset += dd.ins.size;
            if( DbgAddr.mach.offset == curr_off ) {
                a->mach.offset = prev;
                return( MS_OK );
            }
            if( DbgAddr.mach.offset > curr_off )
                break;
            if( DbgAddr.mach.offset < start ) {
                break; /* wrapped around segment */
            }
        }
    }
    /* Couldn't sync instruction stream */
    return( MS_FAIL );
}

mad_status DIGENTRY MIDisasm( mad_disasm_data *dd, address *a, int adj )
{
    mad_status  ms;

    dd->characteristics = AddrCharacteristics( *a );
    while( adj < 0 ) {
        ms = GetDisasmPrev( a );
        if( ms != MS_OK )
            return( ms );
        ++adj;
    }
    while( adj >= 0 ) {
        DecodeIns( a, dd, dd->characteristics & X86AC_BIG );
        --adj;
    }
    return( MS_OK );
}

size_t DIGENTRY MIDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, mad_radix radix, char *buff, size_t buff_size )
{
    char                nbuff[20];
    char                obuff[256];
    char                *np;
    char                *op;
    size_t              nlen;
    size_t              olen;
    size_t              len;
    dis_format_flags    ff;

    nbuff[0] = '\0';
    obuff[0] = '\0';
    if( dp & MDP_INSTRUCTION ) {
        np = nbuff;
        nlen = sizeof( nbuff );
    } else {
        np = NULL;
        nlen = 0;
    }
    if( dp & MDP_OPERANDS ) {
        op = obuff;
        olen = sizeof( obuff );
    } else {
        op = NULL;
        olen = 0;
    }
    ff = DFF_ASM;
    if( MADState->disasm_state & DT_UPPER )
        ff |= DFF_INS_UP | DFF_REG_UP;
    if( MADState->disasm_state & DT_INSIDE )
        ff |= DFF_ALT_INDEXING;
    dd->radix = radix;
    if( DisFormat( &DH, dd, &dd->ins, ff, np, nlen, op, olen ) != DR_OK ) {
        return( 0 );
    }
    olen = strlen( obuff );
    nlen = strlen( nbuff );
    if( dp == MDP_ALL )
        nbuff[nlen++] = ' ';
    len = nlen + olen;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        if( nlen > buff_size )
            nlen = buff_size;
        memcpy( buff, nbuff, nlen );
        buff += nlen;
        buff_size -= nlen;
        if( olen > buff_size )
            olen = buff_size;
        memcpy( buff, obuff, olen );
        buff[buff_size] = '\0';
    }
    return( len );
}

unsigned DIGENTRY MIDisasmInsSize( mad_disasm_data *dd )
{
    return( dd->ins.size );
}

mad_status DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
    switch( dd->ins.type ) {
    case DI_X86_bound:
    case DI_X86_int:
    case DI_X86_into:
    case DI_X86_in:
    case DI_X86_in2:
    case DI_X86_ins:
    case DI_X86_lgdt:
    case DI_X86_lidt:
    case DI_X86_lldt:
    case DI_X86_lmsw:
    case DI_X86_out:
    case DI_X86_out2:
    case DI_X86_outs:
        return( MS_FAIL );
    default:
        break;
    }
    return( MS_OK );
}

static mad_disasm_control Adjustment( mad_disasm_data *dd )
{
    switch( dd->ins.op[OP_1].type & DO_MASK ) {
    case DO_IMMED:
    case DO_RELATIVE:
        if( (addr_off)dd->ins.op[OP_1].value < dd->addr.mach.offset )
            return( MDC_TAKEN_BACK );
        return( MDC_TAKEN_FORWARD );
    }
    return( MDC_TAKEN );
}

static mad_disasm_control Cond( mad_disasm_data *dd, int taken )
{
    if( !taken )
        return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
    return( ( MDC_JUMP | MDC_CONDITIONAL ) + Adjustment( dd ) );
}

mad_disasm_control DisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    char            xor;
    unsigned long   val;

    xor = 0;
    switch( dd->ins.type ) {
    case DI_X86_int:
        return( MDC_SYSCALL | MDC_TAKEN );
    case DI_X86_call:
    case DI_X86_call2:
    case DI_X86_call3:
    case DI_X86_call4:
        return( MDC_CALL | MDC_TAKEN );
    case DI_X86_jmp:
    case DI_X86_jmp1:
        return( MDC_JUMP + Adjustment( dd ) );
    case DI_X86_iret:
    case DI_X86_iretd:
        return( MDC_SYSRET | MDC_TAKEN );
    case DI_X86_ret:
    case DI_X86_ret2:
    case DI_X86_retf:
    case DI_X86_retf2:
        return( MDC_RET | MDC_TAKEN );
    case DI_X86_jmp2:
    case DI_X86_jmp3:
    case DI_X86_jmp4:
        return( MDC_JUMP | MDC_TAKEN );
    case DI_X86_bound:
        return( MDC_OPER | MDC_TAKEN_NOT ); /* not supported yet */
    case DI_X86_jno:
    case DI_X86_jno2:
        xor = 1;
    case DI_X86_jo:
    case DI_X86_jo2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & FLG_O ) != 0 ) ^ xor ) );
    case DI_X86_jae:
    case DI_X86_jae2:
        xor = 1;
    case DI_X86_jb:
    case DI_X86_jb2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & FLG_C ) != 0 ) ^ xor ) );
    case DI_X86_jne:
    case DI_X86_jne2:
        xor = 1;
    case DI_X86_je:
    case DI_X86_je2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & FLG_Z ) != 0 ) ^ xor ) );
    case DI_X86_ja:
    case DI_X86_ja2:
        xor = 1;
    case DI_X86_jbe:
    case DI_X86_jbe2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & ( FLG_C | FLG_Z ) ) != 0 ) ^ xor ) );
    case DI_X86_jns:
    case DI_X86_jns2:
        xor = 1;
    case DI_X86_js:
    case DI_X86_js2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & FLG_S ) != 0 ) ^ xor ) );
    case DI_X86_jpo:
    case DI_X86_jpo2:
        xor = 1;
    case DI_X86_jp:
    case DI_X86_jp2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & FLG_P ) != 0 ) ^ xor ) );
    case DI_X86_jge:
    case DI_X86_jge2:
        xor = 1;
    case DI_X86_jl:
    case DI_X86_jl2:
        return( Cond( dd, ( ( mr->x86.cpu.efl & FLG_S ) != 0 ) ^ ( ( mr->x86.cpu.efl & FLG_O ) != 0 ) ^ xor ) );
    case DI_X86_jg:
    case DI_X86_jg2:
        xor = 1;
    case DI_X86_jle:
    case DI_X86_jle2:
        return( Cond( dd, ( ( ( ( mr->x86.cpu.efl & FLG_S ) != 0 ) ^ ( ( mr->x86.cpu.efl & FLG_O ) != 0 ) )
            | ( ( mr->x86.cpu.efl & FLG_Z ) != 0 ) ) ^ xor ) );
    case DI_X86_into:
        return( MDC_SYSCALL | MDC_CONDITIONAL | ( (mr->x86.cpu.efl & FLG_O) != 0 ? MDC_TAKEN : MDC_TAKEN_NOT ) );
    case DI_X86_loopnz:
        val = mr->x86.cpu.ecx;
        if( ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) == 0 )
            val &= 0xffff;
        return( Cond( dd, ( mr->x86.cpu.efl & FLG_Z ) == 0 && val != 1 ) );
    case DI_X86_loopz:
        val = mr->x86.cpu.ecx;
        if( ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) == 0 )
            val &= 0xffff;
        return( Cond( dd, ( mr->x86.cpu.efl & FLG_Z ) != 0 && val != 1 ) );
    case DI_X86_loop:
        val = mr->x86.cpu.ecx;
        if( ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) == 0 )
            val &= 0xffff;
        return( Cond( dd, val != 1 ) );
    case DI_X86_jcxz:
    case DI_X86_jecxz:
        val = mr->x86.cpu.ecx;
        if( ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) == 0 )
            val &= 0xffff;
        return( Cond( dd, val == 0 ) );
    default:
        break;
    }
    return( MDC_OPER | MDC_TAKEN_NOT );
}

mad_disasm_control DIGENTRY MIDisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    return( DisasmControl( dd, mr ) );
}

static walk_result FindCallTarget( address a, mad_type_handle th, mad_memref_kind mk, void *d )
{
    th = th; mk = mk;

    *(address *)d = a;
    return( WR_STOP );
}

mad_status DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *next )
{
    mad_disasm_control  dc;

    *next = GetRegIP( mr );
    next->mach.offset += dd->ins.size;
    dc = DisasmControl( dd, mr );
    if( ( dc & MDC_TAKEN_MASK ) == MDC_TAKEN_NOT ) {
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_SYSCALL:
        return( MS_FAIL );
    case MDC_JUMP:
    case MDC_CALL:
        switch( dd->ins.op[OP_1].type & DO_MASK ) {
        case DO_ABSOLUTE:
            next->mach.segment = dd->ins.op[OP_1].extra;
            /* fall through */
        case DO_RELATIVE:
            next->mach.offset = dd->ins.op[OP_1].value;
            break;
        case DO_REG:
            next->mach.offset = RegValue( mr, dd->ins.op[OP_1].base );
            break;
        default:
            /* memory indirect jump/call */
            DoDisasmMemRefWalk( dd, FindCallTarget, mr, &DbgAddr );

            if( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) {
                next->mach.offset = GetDataLong();
                if( dd->ins.op[OP_1].ref_type == DRT_X86_FARPTR48 ) {
                    next->mach.segment = (unsigned_16)GetDataWord();
                }
            } else {
                next->mach.offset = (unsigned_16)GetDataWord();
                if( dd->ins.op[OP_1].ref_type == DRT_X86_FARPTR32 ) {
                    next->mach.segment = (unsigned_16)GetDataWord();
                }
            }
            break;
        }
        break;
    case MDC_SYSRET:
    case MDC_RET:
        DbgAddr = GetRegSP( mr );
        if( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) {
            next->mach.offset = GetDataLong();
        } else {
            next->mach.offset = (unsigned_16)GetDataWord();
        }
        switch( dd->ins.type ) {
        case DI_X86_retf:
        case DI_X86_retf2:
            next->mach.segment = (unsigned_16)GetDataWord();
            break;
        default:
            break;
        }
        break;
    }
    return( MS_OK );
}

static const unsigned_8 RegIndex[] = {
    offsetof( mad_registers, x86.cpu.eax ),
    offsetof( mad_registers, x86.cpu.ecx ),
    offsetof( mad_registers, x86.cpu.edx ),
    offsetof( mad_registers, x86.cpu.ebx ),
    offsetof( mad_registers, x86.cpu.esp ),
    offsetof( mad_registers, x86.cpu.ebp ),
    offsetof( mad_registers, x86.cpu.esi ),
    offsetof( mad_registers, x86.cpu.edi ),
    offsetof( mad_registers, x86.cpu.es ),
    offsetof( mad_registers, x86.cpu.cs ),
    offsetof( mad_registers, x86.cpu.ss ),
    offsetof( mad_registers, x86.cpu.ds ),
    offsetof( mad_registers, x86.cpu.fs ),
    offsetof( mad_registers, x86.cpu.gs ),
};

static dword RegValue( const mad_registers *mr, int idx )
{
    dword       mask;
    unsigned_8  reg_idx;

    if( idx >= DR_X86_es && idx <= DR_X86_gs ) {
        reg_idx = idx - DR_X86_es + 8;
        mask = 0xffff;
    } else if( idx >= DR_X86_ax && idx <= DR_X86_di ) {
        reg_idx = idx - DR_X86_ax;
        mask = 0xffff;
    } else {
        reg_idx = idx - DR_X86_eax;
        mask = 0xffffffff;
    }
    return( (*(dword *)( (unsigned_8 *)mr + RegIndex[reg_idx] )) & mask );
}

static int GetSegRegOverride( mad_disasm_data *dd, dis_operand *op )
{
    if( dd->ins.flags.u.x86 & DIF_X86_CS ) {
        return( DR_X86_cs );
    } else if( dd->ins.flags.u.x86 & DIF_X86_DS ) {
        return( DR_X86_ds );
    } else if( dd->ins.flags.u.x86 & DIF_X86_ES ) {
        return( DR_X86_es );
    } else if( dd->ins.flags.u.x86 & DIF_X86_FS ) {
        return( DR_X86_fs );
    } else if( dd->ins.flags.u.x86 & DIF_X86_GS ) {
        return( DR_X86_gs );
    } else if( dd->ins.flags.u.x86 & DIF_X86_SS ) {
        return( DR_X86_ss );
    } else {
        switch( op->base ) {
        case DR_X86_sp:
        case DR_X86_esp:
        case DR_X86_bp:
        case DR_X86_ebp:
            return( DR_X86_ss );
        default:
            return( DR_X86_ds );
        }
    }
}

static walk_result MemReference( int opnd, mad_disasm_data *dd, MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    mad_type_handle     th;
    address             addr;
    dis_operand         *op;
    mad_memref_kind     mmk;

    op = &dd->ins.op[opnd];
    switch( op->ref_type ) {
    case DRT_X86_DWORDF:
        th = X86T_FLOAT;
        break;
    case DRT_X86_DWORD:
        th = X86T_DWORD;
        break;
    case DRT_X86_QWORDF:
        th = X86T_DOUBLE;
        break;
    case DRT_X86_QWORD:
        th = X86T_QWORD;
        break;
    case DRT_X86_WORD:
        th = X86T_WORD;
        break;
    case DRT_X86_BYTE:
        th = X86T_BYTE;
        break;
    case DRT_X86_TBYTE:
        th = X86T_EXTENDED;
        break;
    case DRT_X86_FARPTR48:
        th = X86T_F32_PTR;
        break;
    case DRT_X86_FARPTR32:
        th = X86T_F16_PTR;
        break;
    default:
        th = X86T_BYTE;
        break;
    }
    addr.sect_id = 0;
    addr.indirect = 0;
    addr.mach.offset = op->value;
    if( op->base != DR_NONE ) {
        addr.mach.offset += RegValue( mr, op->base );
    }
    if( op->index != DR_NONE ) {
        addr.mach.offset += RegValue( mr, op->index ) * op->scale;
    }
    if( op->type & DO_NO_SEG_OVR ) {
        addr.mach.segment = (addr_seg)RegValue( mr, DR_X86_es );
    } else {
        addr.mach.segment = (addr_seg)RegValue( mr, GetSegRegOverride( dd, op ) );
    }
    if( dd->ins.flags.u.x86 & DIF_X86_ADDR_LONG ) {
        addr.mach.offset &= ~(dword)0;
    } else {
        addr.mach.offset &= ~(word)0;
    }
    mmk = MMK_READ;
    switch( dd->ins.type ) {
    case DI_X86_pop:
    case DI_X86_pop2:
    case DI_X86_pop3d:
    case DI_X86_pop3e:
    case DI_X86_pop3s:
    case DI_X86_pop4f:
    case DI_X86_pop4g:
    case DI_X86_ins:
    case DI_X86_stos:
        mmk = MMK_WRITE;
        break;
    case DI_X86_xchg:
    case DI_X86_xchg2:
    case DI_X86_inc:
    case DI_X86_inc2:
    case DI_X86_dec:
    case DI_X86_dec2:
        mmk |= MMK_WRITE;
        break;
    case DI_X86_cmps:
    case DI_X86_lods:
    case DI_X86_outs:
    case DI_X86_scas:
        break;
    case DI_X86_movzx:
    case DI_X86_movsx:
    case DI_X86_mov:
    case DI_X86_mov2:
    case DI_X86_mov3:
    case DI_X86_mov4:
    case DI_X86_mov5:
    case DI_X86_mov6:
    case DI_X86_mov7:
    case DI_X86_movs:
        if( opnd == OP_1 ) {
            mmk = MMK_WRITE;
        }
        break;
    default:
        if( dd->ins.num_ops >= 2 && opnd == OP_1 ) {
            mmk |= MMK_WRITE;
        }
        break;
    }
    return( wk( addr, th, mmk, d ) );
}

walk_result DoDisasmMemRefWalk( mad_disasm_data *dd, MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    walk_result         wr;
    mad_type_handle     th;
    int                 i;

    th = MAD_NIL_TYPE_HANDLE;
    switch( dd->ins.type ) {
    case DI_X86_ret:
    case DI_X86_ret2:
        th = ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_N32_PTR : X86T_N16_PTR;
        break;
    case DI_X86_retf:
    case DI_X86_retf2:
        th = ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_F32_PTR : X86T_F16_PTR;
        break;
    case DI_X86_iret:
    case DI_X86_iretd:
        th = ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_IRET32 : X86T_IRET16;
        break;
    case DI_X86_pop:
    case DI_X86_pop2:
    case DI_X86_pop3d:
    case DI_X86_pop3e:
    case DI_X86_pop3s:
    case DI_X86_pop4f:
    case DI_X86_pop4g:
    case DI_X86_popf:
    case DI_X86_popfd:
    case DI_X86_leave:
        th = ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_DWORD : X86T_WORD;
        break;
    case DI_X86_popa:
    case DI_X86_popad:
        th = ( dd->ins.flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_POPAD : X86T_POPA;
        break;
    default:
        break;
    }
    if( th != MAD_NIL_TYPE_HANDLE ) {
        wr = wk( GetRegSP( mr ), th, MMK_VOLATILE | MMK_IMPLICIT | MMK_READ, d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    for( i = 0; i < dd->ins.num_ops; i++ ) {
        if( ( dd->ins.op[i].type & ( DO_MASK | DO_HIDDEN ) ) == DO_MEMORY_ABS ) {
            wr = MemReference( i, dd, wk, mr, d );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
    }
    return( WR_CONTINUE );
}

struct memref_glue {
    MI_MEMREF_WALKER    *wk;
    void                *d;
};

static walk_result MemRefGlue( address a, mad_type_handle th, mad_memref_kind mk, void *d )
{
    struct memref_glue  *gd = d;

    return( gd->wk( a, th, mk, gd->d ) );
}

walk_result DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    struct memref_glue  glue;

    glue.wk = wk;
    glue.d = d;
    return( DoDisasmMemRefWalk( dd, MemRefGlue, mr, &glue ) );
}

const mad_toggle_strings *DIGENTRY MIDisasmToggleList( void )
{
    static const mad_toggle_strings list[] = {
        { MAD_MSTR_MUPPER, MAD_MSTR_UPPER, MAD_MSTR_LOWER },
        { MAD_MSTR_MINSIDE, MAD_MSTR_INSIDE, MAD_MSTR_OUTSIDE },
        { MAD_MSTR_NIL, MAD_MSTR_NIL, MAD_MSTR_NIL }
    };
    return( list );
}

unsigned DIGENTRY MIDisasmToggle( unsigned on, unsigned off )
{
    unsigned    toggle;

    toggle = ( on & off );
    MADState->disasm_state ^= toggle;
    MADState->disasm_state |= on & ~toggle;
    MADState->disasm_state &= ~off | toggle;
    return( MADState->disasm_state );
}


static int RegAt( const char *from, const char *reg, unsigned len )
{
    switch( *from ) {
    case 'e':
    case 'E':
        ++from;
        break;
    }
    if( strnicmp( from, reg, len ) != 0 )
        return( 0 );
    if( isalnum( from[len] ) || from[len] == '_' )
        return( 0 );
    return( 1 );
}

static char *StrCopy( const char *s, char *d )
{
    while( ( *d = *s ) != '\0' ) {
        ++s;
        ++d;
    }
    return( d );
}

mad_status DIGENTRY MIDisasmInspectAddr( const char *start, unsigned len, mad_radix radix, const mad_registers *mr, address *a )
{
    char        *buff = __alloca( len * 2 + 1 );
    char        *to;
    unsigned    parens;
    char        c;

    mr = mr;
    to = buff;
    parens = 0;
    for( ; len > 0; --len ) {
        switch( c = *start++ ) {
        case '[':
            *to++ = '+';
            *to++ = '(';
            if( RegAt( start, "bp", 2 ) || RegAt( start, "sp", 2 ) ) {
                to = StrCopy( "ss", to );
            } else {
                to = StrCopy( "ds", to );
            }
            to = StrCopy( ":(", to );
            break;
        case ']':
            to = StrCopy( "))", to );
            break;
        case ':':
            to = StrCopy( "):(", to );
            ++parens;
            break;
        default:
            *to++ = c;
        }
    }
    for( ; parens > 0; --parens ) {
        memmove( buff + 1, buff, to - buff + 1 );
        buff[0] = '(';
        ++to;
        *to++ = ')';
    }
    *to = '\0';
    return( MCMemExpr( buff, radix, a ) );
}

/*
 * CnvRadix -- convert an unsigned number of a given radix to a string
 */

char *CnvRadix( unsigned long value, mad_radix radix, char base, char *buff, size_t len )
{
    char        internal[33];
    char        *ptr;
    unsigned    dig;

    ptr = &internal[32];
    for( ; len > 0 || value != 0; value /= radix ) {
        dig = value % radix;
        *ptr = ( dig <= 9 ) ? (char)dig + '0' : (char)( dig - 10 ) + base;
        --ptr;
        --len;
    }
    len = &internal[32] - ptr;
    memcpy( buff, ptr + 1, len );
    buff[len] = '\0';
    return( buff + len );
}

/****************** DISASSEMBLER INTERFACE **************************************/

/*
 * JmpLabel -- process a label
 */

char *JmpLabel( unsigned long addr, addr_off off )
{
    address             memaddr;
    mad_type_handle     th;
    char                *p;

    off = off;

    memaddr = DbgAddr;
    memaddr.mach.offset = addr;
    th = ( BIG_SEG( memaddr ) ) ? X86T_N32_PTR : X86T_N16_PTR;
#define PREFIX_STR "CS:"
#define PREFIX_LEN (sizeof( PREFIX_STR ) - 1)
    p = &ScratchBuff[PREFIX_LEN];
    if( MCAddrToString( memaddr, th, MLK_CODE, p, sizeof( ScratchBuff ) - 1 - PREFIX_LEN ) != MS_OK ) {
        p -= PREFIX_LEN;
        memcpy( p, PREFIX_STR, PREFIX_LEN );
    }
    return( p );
}

/*
 * ToSegStr -- convert to segment string
 */

char *ToSegStr( addr_off value, addr_seg seg, addr_off addr )
{
    address             memaddr;
    mad_type_handle     th;

    addr = addr;

    memaddr.mach.segment = seg;
    memaddr.mach.offset = value;
    MCAddrSection( &memaddr );
    th = BIG_SEG( memaddr ) ? X86T_F32_PTR : X86T_F16_PTR;
    MCAddrToString( memaddr, th, MLK_MEMORY, ScratchBuff, sizeof( ScratchBuff ) - 1 );
    return( ScratchBuff );
}


static void ReadMem( address a, size_t s, void *d )
{

    if( a.sect_id == Cache.start.sect_id
      && a.mach.segment == Cache.start.mach.segment
      && a.mach.offset >= Cache.start.mach.offset
      && ( a.mach.offset + s ) < ( Cache.start.mach.offset + Cache.len ) ) {
        memcpy( d, &Cache.data[a.mach.offset - Cache.start.mach.offset], s );
        return;
    }
#if 0
    InitCache( a, Cache.want );
    if( a.sect_id == Cache.start.sect_id
      && a.mach.segment == Cache.start.mach.segment
      && a.mach.offset >= Cache.start.mach.offset
      && ( a.mach.offset + s ) < ( Cache.start.mach.offset + Cache.len ) ) {
        memcpy( d, &Cache.data[a.mach.offset - Cache.start.mach.offset], s );
        return;
    }
#endif
    MCReadMem( a, s, d );
}

signed_16 GetDataWord( void )
{
    signed_16   d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    DbgAddr.mach.offset += sizeof( d );
    return( d );
}


signed_32 GetDataLong( void )
{
    signed_32   d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    DbgAddr.mach.offset += sizeof( d );
    return( d );
}

void InitCache( address start, unsigned len )
{
    Cache.start = start;
    if( len > sizeof( Cache.data ) )
        len = sizeof( Cache.data );
    Cache.want = len;
    Cache.len = MCReadMem( start, len, Cache.data );
}

dis_return DisCliGetData( void *d, unsigned off, size_t size, void *data )
{
    mad_disasm_data     *dd = d;
    address             addr;

    addr = dd->addr;
    addr.mach.offset += off;
    if( MCReadMem( addr, size, data ) == 0 )
        return( DR_FAIL );
    return( DR_OK );
}

static int GetValueByteSize( unsigned long value )
{
    int                 size;

    for( size = 4; size > 1; size-- ) {
        if( value & 0xFF000000 )
            break;
        value <<= 8;
    }
    return( size );
}

size_t DisCliValueString( void *d, dis_dec_ins *ins, unsigned opnd, char *buff, size_t buff_size )
{
    mad_disasm_data     *dd = d;
    mad_type_info       mti;
    address             val;
    dis_operand         *op;
    int                 size;

    op = &ins->op[opnd];
    buff[0] = '\0';
    val = dd->addr;
    switch( op->type & DO_MASK ) {
    case DO_IMMED:
        switch( op->ref_type ) {
        case DRT_X86_BYTE:
            size = 1;
            break;
        case DRT_X86_WORD:
            size = 2;
            break;
        case DRT_X86_DWORD:
        case DRT_X86_DWORDF:
            size = 4;
            break;
        default:
            size = ( ins->flags.u.x86 & DIF_X86_OPND_LONG ) ? 4 : 2;
        }
        MCTypeInfoForHost( MTK_INTEGER, size, &mti );
        MCTypeToString( dd->radix, &mti, &op->value, buff, &buff_size );
        break;
    case DO_RELATIVE:
        val.mach.offset += op->value;
        MCAddrToString( val, ( ins->flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_N32_PTR : X86T_N16_PTR, MLK_CODE, buff, buff_size );
        break;
    case DO_ABSOLUTE:
        if( op->type & DO_EXTRA ) {
            val.mach.offset = op->value;
            val.mach.segment = op->extra;
            MCAddrToString( val, ( ins->flags.u.x86 & DIF_X86_OPND_LONG ) ? X86T_F32_PTR : X86T_F16_PTR, MLK_CODE, buff, buff_size );
            break;
        }
        /* fall through for LEA instruction */
    case DO_MEMORY_ABS:
    case DO_MEMORY_REL:
        if( op->base == DR_NONE && op->index == DR_NONE ) {
            // direct memory address
            size = ( ins->flags.u.x86 & DIF_X86_ADDR_LONG ) ? 4 : 2;
            MCTypeInfoForHost( MTK_INTEGER, size, &mti );
            MCTypeToString( dd->radix, &mti, &op->value, buff, &buff_size );
        } else if( op->value == 0 ) {
            // don't output zero disp in indirect memory address
        } else {
            char *p = buff;
            // indirect memory address with displacement
            if( op->value < 0 ) {
                *( p++ ) = '-';
                --buff_size;
                op->value = -op->value;
            }
            size = GetValueByteSize( op->value );
            MCTypeInfoForHost( MTK_INTEGER, size, &mti );
            MCTypeToString( dd->radix, &mti, &op->value, p, &buff_size );
        }
        break;
    }
    return( strlen( buff ) );
}

mad_status DisasmInit( void )
{
    if( DisInit( DISCPU_x86, &DH, false ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    return( MS_OK );
}

void DisasmFini( void )
{
    DisFini( &DH );
}
