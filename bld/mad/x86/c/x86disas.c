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


#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <malloc.h>
#include "x86.h"
#include "x86types.h"
#include "madregs.h"
#include "insdef.h"
#include "formopt.h"
#include "intrface.h"

extern uint_32          InsAddr;

address                 DbgAddr;
static unsigned         Radix;

static dword RegValue( const mad_registers *mr, int idx );

static char             ScratchBuff[40];
static struct {
    address             start;
    unsigned            len;
    unsigned            want;
    unsigned_8          data[0x80];
}                       Cache;

unsigned                DIGENTRY MIDisasmDataSize( void )
{
    return( sizeof( mad_disasm_data ) );
}

unsigned                DIGENTRY MIDisasmNameMax( void )
{
    return( 9 );
}

void DecodeIns( address *a, instruction *ins, int big )
{
    DbgAddr = *a;
    InsAddr = DbgAddr.mach.offset;
    InitCache( DbgAddr, 0x8 );
    DoCode( ins, big );
    a->mach.offset += ins->ins_size;
}

mad_status              GetDisasmPrev( address *a )
{
    instruction ins;
    addr_off    start;
    addr_off    curr_off;
    addr_off    prev;
    unsigned    backup;
    int         big;

    DbgAddr = *a;
    big = BIG_SEG( DbgAddr );
    backup = big ? 0x40 : 0x20;
    curr_off = DbgAddr.mach.offset;
    DbgAddr.mach.offset = (curr_off <= backup) ? 0 : (curr_off - backup);
    InitCache( DbgAddr, curr_off - DbgAddr.mach.offset );
    for( start = DbgAddr.mach.offset; start < curr_off; ++start ) {
        DbgAddr.mach.offset = start;
        for( ;; ) {
            prev = DbgAddr.mach.offset;
            InsAddr = prev;
            DoCode( &ins, big );
            if( ins.ins_size == 0 ) break;      /* invalid address */
            if( DbgAddr.mach.offset == curr_off ) {
                a->mach.offset = prev;
                return( MS_OK );
            }
            if( DbgAddr.mach.offset > curr_off ) break;
            if( DbgAddr.mach.offset < start ) break; /* wrapped around segment */
        }
    }
    /* Couldn't sync instruction stream */
    return( MS_FAIL );
}

mad_status              DIGENTRY MIDisasm( mad_disasm_data *dd, address *a, int adj )
{
    mad_status  ms;

    dd->characteristics = AddrCharacteristics( *a );
    while( adj < 0 ) {
        ms = GetDisasmPrev( a );
        if( ms != MS_OK ) return( ms );
        ++adj;
    }
    while( adj >= 0 ) {
        dd->addr = *a;
        DecodeIns( a, &dd->ins, dd->characteristics & X86AC_BIG );
        --adj;
    }
    *a = DbgAddr;
    dd->buff[0] = '\0';
    return( MS_OK );
}

unsigned                DIGENTRY MIDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, unsigned radix, unsigned max, char *buff )
{
    char        *p;
    char        *e;
    unsigned    format;

    if( radix != Radix ) {
        Radix = radix;
        /* have to reformat */
        dd->buff[0] = '\0';
    }
    if( dd->buff[0] == '\0' ) {
        format = 0;
        if( MADState->disasm_toggles & DT_UPPER ) format |= FORM_REG_UPPER | FORM_NAME_UPPER;
        if( MADState->disasm_toggles & DT_INSIDE ) format |= FORM_INDEX_IN;
        FormatIns( dd->buff, &dd->ins, format );
    }
    p = dd->buff;
    e = p;
    switch( dp ) {
    case MDP_INSTRUCTION:
        for( ;; ) {
            if( *e == ' '  ) break;
            if( *e == '\t' ) break;
            if( *e == '\0' ) break;
            ++e;
        }
        break;
    case MDP_OPERANDS:
        for( ;; ) {
            if( *p == ' '  ) break;
            if( *p == '\t' ) break;
            if( *p == '\0' ) break;
            ++p;
        }
        while( *p == ' ' || *p == '\t' ) ++p;
        e = &p[ strlen( p ) ];
        break;
    case MDP_INSTRUCTION+MDP_OPERANDS:
        e = &p[ strlen( p ) ];
        break;
    }
    if( max > 0 ) {
        --max;
        if( max > (e-p) ) max = (e-p);
        memcpy( buff, p, max );
        buff[max] = '\0';
    }
    return( e - p );
}

unsigned                DIGENTRY MIDisasmInsSize( mad_disasm_data *dd )
{
    return( dd->ins.ins_size );
}

mad_status              DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
    switch( dd->ins.opcode ) {
    case I_BOUND:
    case I_INT:
    case I_INTO:
    case I_IN:
    case I_INSB:
    case I_INSW:
    case I_INSD:
    case I_LGDT:
    case I_LIDT:
    case I_LLDT:
    case I_LMSW:
    case I_OUT:
    case I_OUTSB:
    case I_OUTSW:
    case I_OUTSD:
        return( MS_FAIL );
    }
    return( MS_OK );
}

static unsigned Adjustment( mad_disasm_data *dd )
{
    switch( dd->ins.op[0].mode ) {
    case ADDR_CONST:
    case ADDR_LABEL:
        if( dd->ins.op[0].disp < dd->addr.mach.offset ) return( MDC_TAKEN_BACK );
        return( MDC_TAKEN_FORWARD );
    }
    return( MDC_TAKEN );
}

static mad_disasm_control Cond( mad_disasm_data *dd, int taken )
{
    if( !taken ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
    return( (MDC_JUMP | MDC_CONDITIONAL) + Adjustment( dd ) );
}

static mad_disasm_control DisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    char            xor;
    unsigned long   val;

    xor = 0;
    switch( dd->ins.opcode ) {
    case I_INT:
        return( MDC_SYSCALL | MDC_TAKEN );
    case I_CALL:
    case I_CALL_FAR:
        return( MDC_CALL | MDC_TAKEN );
    case I_JMP:
        return( MDC_JUMP + Adjustment( dd ) );
    case I_IRET:
    case I_IRETD:
        return( MDC_SYSRET | MDC_TAKEN );
    case I_RET:
    case I_RET_FAR:
    case I_RET_FAR_D:
        return( MDC_RET | MDC_TAKEN );
    case I_JMP_FAR:
        return( MDC_JUMP | MDC_TAKEN );
    case I_BOUND:
        return( MDC_OPER | MDC_TAKEN_NOT ); /* not supported yet */
    case I_JNO:
        xor = 1;
    case I_JO:
        return Cond( dd, ((mr->x86.cpu.efl & FLG_O) != 0) ^ xor );
    case I_JAE:
        xor = 1;
    case I_JB:
        return Cond( dd, ((mr->x86.cpu.efl & FLG_C) != 0) ^ xor );
    case I_JNE:
        xor = 1;
    case I_JE:
        return Cond( dd, ((mr->x86.cpu.efl & FLG_Z) != 0) ^ xor );
    case I_JA:
        xor = 1;
    case I_JBE:
        return Cond( dd, ((mr->x86.cpu.efl & (FLG_C|FLG_Z)) != 0) ^ xor );
    case I_JNS:
        xor = 1;
    case I_JS:
        return Cond( dd, ((mr->x86.cpu.efl & FLG_S) != 0) ^ xor );
    case I_JPO:
        xor = 1;
    case I_JP:
        return Cond( dd, ((mr->x86.cpu.efl & FLG_P) != 0) ^ xor );
    case I_JGE:
        xor = 1;
    case I_JL:
        return Cond( dd, ((mr->x86.cpu.efl & FLG_S)!=0) ^ ((mr->x86.cpu.efl & FLG_O)!=0)
                ^ xor );
    case I_JG:
        xor = 1;
    case I_JLE:
        return Cond( dd, ((((mr->x86.cpu.efl & FLG_S)!=0) ^ ((mr->x86.cpu.efl & FLG_O)!=0))
                | ((mr->x86.cpu.efl & FLG_Z) != 0))
                ^ xor );
    case I_INTO:
        return( (mr->x86.cpu.efl & FLG_O) != 0 ? (MDC_SYSCALL|MDC_CONDITIONAL|MDC_TAKEN) : (MDC_SYSCALL|MDC_CONDITIONAL|MDC_TAKEN_NOT) );
    case I_LOOPNE:
        val= mr->x86.cpu.ecx;
        if( !(dd->ins.pref & OPND_LONG) ) val &= 0xffff;
        return Cond( dd, (mr->x86.cpu.efl & FLG_Z) == 0 && val != 1 );
    case I_LOOPE:
        val= mr->x86.cpu.ecx;
        if( !(dd->ins.pref & OPND_LONG) ) val &= 0xffff;
        return Cond( dd, (mr->x86.cpu.efl & FLG_Z) != 0 && val != 1 );
    case I_LOOP:
        val= mr->x86.cpu.ecx;
        if( !(dd->ins.pref & OPND_LONG) ) val &= 0xffff;
        return Cond( dd, val != 1 );
    case I_JCXZ:
        val= mr->x86.cpu.ecx;
        if( !(dd->ins.pref & OPND_LONG) ) val &= 0xffff;
        return Cond( dd, val == 0 );
    }
    return( MDC_OPER | MDC_TAKEN_NOT );
}

mad_disasm_control      DIGENTRY MIDisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    return( DisasmControl( dd, mr ) );
}

static walk_result FindCallTarget( address a, mad_type_handle th, mad_memref_kind mk, void *d )
{
    *(address *)d = a;
    return( WR_STOP );
}

mad_status      DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *next )
{
    mad_disasm_control  dc;

    *next = GetRegIP( mr );
    next->mach.offset += dd->ins.ins_size;
    dc = DisasmControl( dd, mr );
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_SYSCALL:
        return( MS_FAIL );
    case MDC_JUMP:
    case MDC_CALL:
        switch( dd->ins.op[0].mode ) {
        case ADDR_SEG_OFFSET:
            next->mach.segment = dd->ins.op[1].disp;
            /* fall through */
        case ADDR_LABEL:
            next->mach.offset = dd->ins.op[0].disp;
            break;
        case ADDR_REG:
            next->mach.offset = RegValue( mr, dd->ins.op[0].base );
            break;
        default:
            /* memory indirect jump/call */
            DoDisasmMemRefWalk( dd, FindCallTarget, mr, &DbgAddr );

            if( dd->ins.pref & OPND_LONG ) {
                next->mach.offset = GetDataLong();
                if( dd->ins.modifier == MOD_PWORD ) {
                    next->mach.segment = (unsigned_16)GetDataWord();
                }
            } else {
                next->mach.offset = (unsigned_16)GetDataWord();
                if( dd->ins.modifier == MOD_LINT ) {
                    next->mach.segment = (unsigned_16)GetDataWord();
                }
            }
            break;
        }
        break;
    case MDC_SYSRET:
    case MDC_RET:
        DbgAddr = GetRegSP( mr );
        if( dd->ins.pref & OPND_LONG ) {
            next->mach.offset = GetDataLong();
        } else {
            next->mach.offset = (unsigned_16)GetDataWord();
        }
        switch( dd->ins.opcode ) {
        case I_RET_FAR:
        case I_RET_FAR_D:
            next->mach.segment = (unsigned_16)GetDataWord();
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
    dword       *reg;
    dword       mask;

    if( idx >= FIRST_SEG_REG ) {
        idx -= FIRST_SEG_REG - 8;
        mask = 0xffff;
    } else if( idx >= FIRST_WORD_REG && idx <= LAST_WORD_REG ) {
        idx -= FIRST_WORD_REG;
        mask = 0xffff;
    } else {
        idx -= FIRST_DWORD_REG;
        mask = 0xffffffff;
    }
    reg = (dword *)((unsigned_8 *)mr + RegIndex[ idx ]);
    return( *reg & mask );
}

walk_result DoDisasmMemRefWalk( mad_disasm_data *dd, MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    walk_result         wr;
    mad_type_handle     th;
    operand             *op;
    address             addr;
    mad_memref_kind     mmk;

    th = (mad_type_handle)-1;
    switch( dd->ins.opcode ) {
    case I_RET:
        th = (dd->ins.pref & OPND_LONG) ? X86T_N32_PTR : X86T_N16_PTR;
        break;
    case I_RET_FAR:
    case I_RET_FAR_D:
        th = (dd->ins.pref & OPND_LONG) ? X86T_F32_PTR : X86T_F16_PTR;
        break;
    case I_IRET:
    case I_IRETD:
        th = (dd->ins.pref & OPND_LONG) ? X86T_IRET32 : X86T_IRET16;
        break;
    case I_POP:
    case I_POPF:
    case I_POPFD:
    case I_LEAVE:
        th = (dd->ins.pref & OPND_LONG) ? X86T_DWORD : X86T_WORD;
        break;
    case I_POPA:
    case I_POPAD:
        th = (dd->ins.pref & OPND_LONG) ? X86T_POPAD : X86T_POPA;
        break;
    }
    if( th != (mad_type_handle)-1 ) {
        wr = wk( GetRegSP( mr ), th, MMK_VOLATILE|MMK_IMPLICIT|MMK_READ, d );
        if( wr != WR_CONTINUE ) return( wr );
    }
    if( dd->ins.mem_ref_op == NULL_OP ) return( WR_CONTINUE );
    op = &dd->ins.op[dd->ins.mem_ref_op];
    if( dd->ins.modifier == MOD_NONE ) {
        switch( dd->ins.mem_ref_size ) {
        case 1:
            dd->ins.modifier = MOD_BYTE;
            break;
        case 2:
            dd->ins.modifier = MOD_WORD;
            break;
        case 4:
            dd->ins.modifier = MOD_LINT;
            break;
        default:
            /* for lack of anything better */
            dd->ins.modifier = MOD_BYTE;
            break;
        }
    }
    switch( dd->ins.modifier ) {
    case MOD_SREAL:
        th = X86T_FLOAT;
        break;
    case MOD_LINT:
        th = X86T_DWORD;
        break;
    case MOD_LREAL:
        th = X86T_DOUBLE;
        break;
    case MOD_SINT:
        th = X86T_WORD;
        break;
    case MOD_BYTE:
        th = X86T_BYTE;
        break;
    case MOD_WORD:
        th = X86T_WORD;
        break;
    case MOD_TREAL:
        th = X86T_EXTENDED;
        break;
    case MOD_PWORD:
        th = X86T_F32_PTR;
        break;
    }
    addr.sect_id = 0;
    addr.indirect = 0;
    addr.mach.offset = op->disp;
    switch( op->mode ) {
    case ADDR_BASE:
    case ADDR_BASE_INDEX:
        addr.mach.offset += RegValue( mr, op->base );
        break;
    }
    switch( op->mode ) {
    case ADDR_INDEX:
    case ADDR_BASE_INDEX:
        addr.mach.offset += RegValue( mr, op->index ) * op->scale;
        break;
    case ADDR_ES_DI:
    case ADDR_ES_EDI:
        addr.mach.offset = mr->x86.cpu.edi;
        break;
    case ADDR_DS_SI_ES_DI:
    case ADDR_DS_ESI_ES_EDI:
        addr.mach.segment = RegValue( mr, ES_REG );
        addr.mach.offset = mr->x86.cpu.edi;
        if( dd->ins.pref & ADDR_LONG ) {
            addr.mach.offset &= ~(dword)0;
        } else {
            addr.mach.offset &= ~(word)0;
        }
        mmk = MMK_WRITE;
        switch( dd->ins.opcode ) {
        case I_CMPSB:
        case I_CMPSW:
        case I_CMPSD:
            mmk = MMK_READ;
            break;
        }
        wr = wk( addr, th, mmk, d );
        if( wr != WR_CONTINUE ) return( wr );
        addr.mach.offset = mr->x86.cpu.esi;
        break;
    }
    if( dd->ins.pref & ADDR_LONG ) {
        addr.mach.offset &= ~(dword)0;
    } else {
        addr.mach.offset &= ~(word)0;
    }
    addr.mach.segment = RegValue( mr, dd->ins.seg_used );
    mmk = MMK_READ;
    switch( dd->ins.opcode ) {
    case I_POP:
        mmk = MMK_WRITE;
        break;
    case I_XCHG:
    case I_INC:
    case I_DEC:
        mmk |= MMK_WRITE;
        break;
    case I_CMPSB:
    case I_CMPSW:
    case I_CMPSD:
        /* nothing to do */
        break;
    case I_MOVZX:
    case I_MOVSX:
    case I_MOV:
        if( dd->ins.mem_ref_op == OP_1 ) {
            mmk = MMK_WRITE;
        }
        break;
    default:
        if( dd->ins.num_oper >= 2 && dd->ins.mem_ref_op == OP_1 ) {
            mmk |= MMK_WRITE;
        }
        break;
    }
    return( wk( addr, th, mmk, d ) );
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

walk_result             DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    struct memref_glue  glue;

    glue.wk = wk;
    glue.d  = d;
    return( DoDisasmMemRefWalk( dd, MemRefGlue, mr, &glue ) );
}

const mad_toggle_strings        *DIGENTRY MIDisasmToggleList( void )
{
    static const mad_toggle_strings list[] = {
        { MSTR_MUPPER, MSTR_UPPER, MSTR_LOWER },
        { MSTR_MINSIDE, MSTR_INSIDE, MSTR_OUTSIDE },
        { MSTR_NIL, MSTR_NIL, MSTR_NIL }
    };
    return( list );
}

unsigned                DIGENTRY MIDisasmToggle( unsigned on, unsigned off )
{
    unsigned    toggle;

    toggle = (on & off);
    MADState->disasm_toggles ^= toggle;
    MADState->disasm_toggles |= on & ~toggle;
    MADState->disasm_toggles &= ~off | toggle;
    return( MADState->disasm_toggles );
}


static int RegAt( char *from, char *reg )
{
    unsigned    len;

    ++from;
    switch( *from ) {
    case 'e':
    case 'E':
        ++from;
    }
    len = strlen( reg );
    if( strnicmp( from, reg, len ) != 0 ) return( 0 );
    if( isalnum( from[len] ) || from[len] == '_' ) return( 0 );
    return( 1 );
}

static char *StrCopy( const char *s, char *d )
{
    for( ;; ) {
        *d = *s;
        if( *d == '\0' ) break;
        ++s;
        ++d;
    }
    return( d );
}

mad_status              DIGENTRY MIDisasmInspectAddr( char *from, unsigned len, unsigned radix, const mad_registers *mr, address *a )
{
    char        *buff = __alloca( len * 2 );
    char        *to;
    int         parens;

    mr = mr;
    to = buff;
    parens = 0;
    while( len != 0 ) {
        switch( *from ) {
        case '[':
            *to++ = '+';
            *to++ = '(';
            if( RegAt( from, "bp" ) || RegAt( from, "sp" ) ) {
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
            *to++ = *from;
        }
        ++from;
        --len;
    }
    while( --parens >= 0 ) {
        memmove( buff+1, buff, to - buff + 1 );
        buff[0] = '(';
        ++to;
        *to++ = ')';
    }
    return( MCMemExpr( buff, to - buff, radix, a ) );
}

/*
 * CnvRadix -- convert an unsigned number of a given radix to a string
 */

char *CnvRadix( unsigned long value, unsigned radix, char base,
                char *buff, int len )
{
    char        internal[ 33 ];
    char        *ptr;
    unsigned    dig;

    ptr = &internal[ 32 ];
    for( ; len > 0 || value != 0; value /= radix ) {
        dig = value % radix;
        *ptr = (dig <= 9) ? dig + '0' : dig - 10 + base;
        --ptr;
        --len;
    }
    len = &internal[32] - ptr;
    memcpy( buff, ptr + 1, len );
    buff[ len ] = '\0';
    return( buff + len );
}

/****************** DISASSEMBLER INTERFACE **************************************/


/*
 * CnvUAsm -- convert from integer to unsigned parsable string for disassembler
 */

static int CnvUAsm( char *ptr, unsigned long value, int min_digits )
{
    char        *p;
    unsigned    len;

    len = MCRadixPrefix( Radix, ~0, ptr );
    p = CnvRadix( value, Radix, 'A', &ptr[len], min_digits );
    if( !isdigit( *ptr ) && len != 0 ) {
        memmove( ptr + 1, ptr, p - ptr + 1 );
        *ptr = '0';
        ++p;
    }
    return( p - ptr );
}

/*
 * CnvSAsm -- convert to signed parsable string for disassembler
 */

static void CnvSAsm( char *ptr, long value )
{
    char        *str;
    char        buff[12];
    unsigned    len;

    len = BIG_SEG( DbgAddr ) ? 8 : 4;
    if( value < 0 ) {
        len = CnvUAsm( buff, -value, len );
    } else {
        len = CnvUAsm( buff, value, len );
    }
    str = buff;
    while( *str == '0' ) {
        ++str;
    }
    if( !isdigit( *str ) ) {
        --str;
    }
    memcpy( ptr + 1, str, len + (&buff[1] - str) );
    if( value < 0 ) {
        *ptr = '-';
    } else {
        *ptr = '+';
    }
}


/*
 * JmpLabel -- process a label
 */

#pragma off(unreferenced);
char *JmpLabel( unsigned long addr, addr_off off )
#pragma on(unreferenced);
{
    address             memaddr;
    mad_type_handle     th;
    char                *p;

    memaddr = DbgAddr;
    memaddr.mach.offset = addr;
    th = BIG_SEG( memaddr ) ? X86T_N32_PTR : X86T_N16_PTR;
    #define PREFIX_STR "CS:"
    #define PREFIX_LEN (sizeof(PREFIX_STR)-1)
    p = &ScratchBuff[ PREFIX_LEN ];
    if( MCAddrToString( memaddr, th, MLK_CODE, sizeof( ScratchBuff ) - 1 - PREFIX_LEN, p ) != MS_OK ) {
        p -= PREFIX_LEN;
        memcpy( p, PREFIX_STR, PREFIX_LEN );
    }
    return( p );
}


/*
 * ToStr -- convert integer to hex string
 */

#pragma off(unreferenced);
char *ToStr( unsigned long value, uint_16 length, addr_off addr )
#pragma on(unreferenced);
{

    switch( length ) {
    case 2:
        value &= 0xFF;
        break;
    case 4:
        value &= 0xFFFF;
        break;
    }
    CnvUAsm( ScratchBuff, value, length );
    return( ScratchBuff );
}


/*
 * ToIndex -- convert to index
 */

#pragma off(unreferenced);
char *ToIndex( unsigned long value, unsigned long addr )
#pragma on(unreferenced);
{

    CnvSAsm( ScratchBuff, value );
    return( ScratchBuff );
}


/*
 * ToBrStr -- convert to branch string
 */

#pragma off(unreferenced);
char *ToBrStr( unsigned long value, addr_off addr )
#pragma on(unreferenced);
{
    int  len;

    ScratchBuff[ 0 ] = '[';
    if( BIG_SEG( DbgAddr ) ) {
        len = CnvUAsm( &ScratchBuff[ 1 ], value, 8 );
    } else {
        len = CnvUAsm( &ScratchBuff[ 1 ], value & 0xffff, 4 );
    }
    ScratchBuff[ len + 1 ] = ']';
    ScratchBuff[ len + 2 ] = '\0';
    return( ScratchBuff );
}


/*
 * ToSegStr -- convert to segment string
 */

#pragma off(unreferenced);
char *ToSegStr( addr_off value, addr_seg seg, addr_off addr )
#pragma on(unreferenced);
{
    address             memaddr;
    mad_type_handle     th;

    memaddr.mach.segment = seg;
    memaddr.mach.offset  = value;
    MCAddrSection( &memaddr );
    th = BIG_SEG( memaddr ) ? X86T_F32_PTR : X86T_F16_PTR;
    MCAddrToString( memaddr, th, MLK_MEMORY, sizeof( ScratchBuff ) - 1, ScratchBuff );
    return( ScratchBuff );
}


/*
 *    Memory access routines used by disassembler
 *    ===========================================
 *
 *    GetDataByte   ; get byte and advance ptr
 *    GetNextByte   ; get byte
 *    GetDataWord   ; get word and advance ptr
 *    GetNextWord   ; get word
 *    GetOffset     ; get current offset in segment
 *    EndOfSegment  ; at end of segment ?
 */

void InitCache( address start, unsigned len )
{
    Cache.start = start;
    if( len > sizeof( Cache.data ) ) len = sizeof( Cache.data );
    Cache.want = len;
    Cache.len = MCReadMem( start, len, Cache.data );
}

static void ReadMem( address a, unsigned s, void *d )
{

    if( a.sect_id == Cache.start.sect_id
     && a.mach.segment == Cache.start.mach.segment
     && a.mach.offset >= Cache.start.mach.offset
     && (a.mach.offset+s) < (Cache.start.mach.offset+Cache.len) ) {
        memcpy( d, &Cache.data[a.mach.offset-Cache.start.mach.offset], s );
        return;
    }
#if 0
    InitCache( a, Cache.want );
    if( a.sect_id == Cache.start.sect_id
     && a.mach.segment == Cache.start.mach.segment
     && a.mach.offset >= Cache.start.mach.offset
     && (a.mach.offset+s) < (Cache.start.mach.offset+Cache.len) ) {
        memcpy( d, &Cache.data[a.mach.offset-Cache.start.mach.offset], s );
        return;
    }
#endif
    MCReadMem( a, s, d );
}

int_16 GetNextByte()
{
    signed_8    d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    return( d );
}


int_16 GetDataByte()
{
    signed_8    d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    DbgAddr.mach.offset += sizeof( d );
    return( d );
}


int_16 GetNextWord()
{
    signed_16   d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    return( d );
}


int_16 GetDataWord()
{
    signed_16   d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    DbgAddr.mach.offset += sizeof( d );
    return( d );
}


long GetNextLong()
{
    signed_32   d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    return( d );
}


long GetDataLong()
{
    signed_32   d;

    ReadMem( DbgAddr, sizeof( d ), &d );
    DbgAddr.mach.offset += sizeof( d );
    return( d );
}


addr_off GetOffset()
{
    return( DbgAddr.mach.offset );
}

char EndOfSegment()
{
    char    ch;

    return( MCReadMem( DbgAddr, 1, &ch ) == 0 );
}


/*
 * Stub routines for Weitek support
 */

char *GetWtkInsName( unsigned ins )
{
    ins = ins;
    return( "" );
}

void DoWtk(void)
{
}

int IsWtk()
{
    return( 0 );
}
