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
* Description:  Alpha AXP instruction decoding.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include "walloca.h"
#include "axp.h"
#include "axptypes.h"
#include "madregs.h"

static dis_handle DH;

mad_status DisasmInit()
{
    if( DisInit( DISCPU_axp, &DH, FALSE ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    return( MS_OK );
}

void DisasmFini()
{
    DisFini( &DH );
}

dis_return DisCliGetData( void *d, unsigned off, unsigned int size, void *data )
{
    mad_disasm_data     *dd = d;
    address             addr;

    addr = dd->addr;
    addr.mach.offset += off;
    if( MCReadMem( addr, size, data ) == 0 ) return( DR_FAIL );
    return( DR_OK );
}

size_t DisCliValueString( void *d, dis_dec_ins *ins, unsigned op, char *buff )
{
    mad_disasm_data     *dd = d;
    char                *p;
    unsigned            max;
    mad_type_info       mti;
    address             val;

    p = buff;
    p[0] = '\0';
    val = dd->addr;
    switch( ins->op[op].type & DO_MASK ) {
    case DO_RELATIVE:
        val.mach.offset += ins->op[op].value;
        //NYI: 64 bit
        MCAddrToString( val, AXPT_N32_PTR, MLK_CODE, p, 40 );
        break;
    case DO_IMMED:
    case DO_ABSOLUTE:
    case DO_MEMORY_ABS:
        MCTypeInfoForHost( MTK_INTEGER, -(int)sizeof( ins->op[0].value ), &mti );
        max = 40;
        MCTypeToString( dd->radix, &mti, &ins->op[op].value, p, &max );
        break;
    }
    return( strlen( buff ) );
}

unsigned                DIGENTRY MIDisasmDataSize( void )
{
    return( sizeof( mad_disasm_data ) );
}

unsigned                DIGENTRY MIDisasmNameMax( void )
{
    return( DisInsNameMax( &DH ) );
}

mad_status DisasmOne( mad_disasm_data *dd, address *a, int adj )
{
    addr_off    new;

    dd->addr = *a;
    new = dd->addr.mach.offset + adj * (int)sizeof( unsigned_32 );
    if( (adj < 0 && new > dd->addr.mach.offset)
     || (adj > 0 && new < dd->addr.mach.offset) ) {
        return( MS_FAIL );
    }
    dd->addr.mach.offset = new;
    DisDecodeInit( &DH, &dd->ins );
    if( DisDecode( &DH, dd, &dd->ins ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    a->mach.offset = dd->addr.mach.offset + sizeof( unsigned_32 );
    return( MS_OK );
}

mad_status              DIGENTRY MIDisasm( mad_disasm_data *dd, address *a, int adj )
{
    return( DisasmOne( dd, a, adj ) );
}

unsigned                DIGENTRY MIDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, unsigned radix, unsigned max, char *buff )
{
    char                nbuff[20];
    char                obuff[256];
    char                *np;
    char                *op;
    unsigned            nlen;
    unsigned            olen;
    unsigned            len;
    dis_format_flags    ff;

    nbuff[0] = '\0';
    obuff[0] = '\0';
    np = (dp & MDP_INSTRUCTION) ? nbuff : NULL;
    op = (dp & MDP_OPERANDS)    ? obuff : NULL;
    ff = DFF_NONE;
    if( MADState->disasm_state & DT_PSEUDO_OPS ) ff |= DFF_PSEUDO;
    if( MADState->disasm_state & DT_UPPER ) ff |= DFF_INS_UP | DFF_REG_UP;
    if( MADState->reg_state[CPU_REG_SET] & CT_SYMBOLIC_NAMES ) {
        ff |= DFF_SYMBOLIC_REG;
    }
    dd->radix = radix;
    if( DisFormat( &DH, dd, &dd->ins, ff, np, op ) != DR_OK ) {
        return( 0 );
    }
    olen = strlen( obuff );
    nlen = strlen( nbuff );
    if( dp == MDP_ALL ) nbuff[ nlen++ ] = ' ';
    len = nlen + olen;
    if( max > 0 ) {
        --max;
        if( max > len ) max = len;
        if( nlen > max ) nlen = max;
        memcpy( buff, nbuff, nlen );
        buff += nlen;
        max -= nlen;
        if( olen > max ) olen = max;
        memcpy( buff, obuff, olen );
        buff[max] = '\0';
    }
    return( len );
}

unsigned                DIGENTRY MIDisasmInsSize( mad_disasm_data *dd )
{
    dd = dd;

    return( sizeof( unsigned_32 ) );
}

mad_status              DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
    switch( dd->ins.type ) {
    case DI_AXP_CALL_PAL:
    case DI_AXP_PAL19:
    case DI_AXP_PAL1B:
    case DI_AXP_PAL1D:
    case DI_AXP_PAL1E:
    case DI_AXP_PAL1F:
    case DI_AXP_LDL_L:
    case DI_AXP_LDQ_L:
    case DI_AXP_STL_C:
    case DI_AXP_STQ_C:
        return( MS_FAIL );
    }
    return( MS_OK );
}

const unsigned_16 RegTrans[] = {
    #define regpick(id,type,reg_set)  offsetof( mad_registers, axp.id ),
    #define regpicku(u,id,type,reg_set)  offsetof( mad_registers, axp.u ),
    #define palpick(pal,id)
    #include "axpregs.h"
    #undef palpick
    #undef regpick
    #undef regpicku
};

const mad_type_handle RefTrans[] = {
    AXPT_BYTE,
    AXPT_WORD,
    AXPT_LWORD,
    AXPT_QWORD,
    AXPT_F_FLOAT,
    AXPT_G_FLOAT,
    AXPT_D_FLOAT,
    AXPT_FLOAT,
    AXPT_DOUBLE,
};

static int Cond( mad_disasm_data *dd, const mad_registers *mr, unsigned condition )
{
    const axpreg    *reg;
    int             cmp;

    reg = TRANS_REG( mr, dd->ins.op[0].base );
    if( dd->ins.op[0].base >= DR_AXP_f0 && dd->ins.op[0].base <= DR_AXP_f31 ) {
        /* floating point */
        if( reg->t.r < 0 ) {
            cmp = -1;
        } else if( reg->t.r > 0 ) {
            cmp = +1;
        } else {
            cmp = 0;
        }
    } else {
        /* integer */
        if( reg->s64.u.sign.v ) {
            cmp = -1;
        } else if( reg->u64.u._32[0] != 0 || reg->u64.u._32[1] != 0 ) {
            cmp = +1;
        } else {
            cmp = 0;
        }
    }
    switch( condition ) {
    case DI_AXP_BLBC:
        if( (reg->u64.u._8[0] & 1) == 0 ) break;
        return( 0 );
    case DI_AXP_BEQ:
        if( cmp == 0 ) break;
        return( 0 );
    case DI_AXP_BLT:
        if( cmp < 0 ) break;
        return( 0 );
    case DI_AXP_BLE:
        if( cmp <= 0 ) break;
        return( 0 );
    case DI_AXP_BLBS:
        if( reg->u64.u._8[0] & 1 ) break;
        return( 0 );
    case DI_AXP_BNE:
        if( cmp != 0 ) break;
        return( 0 );
    case DI_AXP_BGE:
        if( cmp >= 0 ) break;
        return( 0 );
    case DI_AXP_BGT:
        if( cmp > 0 ) break;
        return( 0 );
    }
    return( 1 );
}

mad_disasm_control DisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    switch( dd->ins.type ) {
    case DI_AXP_CALL_PAL:
        return( MDC_SYSCALL | MDC_TAKEN );
    case DI_AXP_JMP:
        return( MDC_JUMP | MDC_TAKEN );
    case DI_AXP_JSR:
    case DI_AXP_JSR_CORTN:
    case DI_AXP_BSR:
        return( MDC_CALL | MDC_TAKEN );
    case DI_AXP_RET:
        return( MDC_RET | MDC_TAKEN );
    case DI_AXP_BR:
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_TAKEN_FORWARD) );
    case DI_AXP_FBEQ:
        if( !Cond( dd, mr, DI_AXP_BEQ ) ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_AXP_FBLE:
        if( !Cond( dd, mr, DI_AXP_BLE ) ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_AXP_FBNE:
        if( !Cond( dd, mr, DI_AXP_BNE ) ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_AXP_FBGE:
        if( !Cond( dd, mr, DI_AXP_BGE ) ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_AXP_FBGT:
        if( !Cond( dd, mr, DI_AXP_BGT ) ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_AXP_BLBC:
    case DI_AXP_BEQ:
    case DI_AXP_BLT:
    case DI_AXP_BLE:
    case DI_AXP_BLBS:
    case DI_AXP_BNE:
    case DI_AXP_BGE:
    case DI_AXP_BGT:
        if( !Cond( dd, mr, dd->ins.type ) ) return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
                        ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
                        : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_AXP_CMOVEQ:
        if( !Cond( dd, mr, DI_AXP_BEQ ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVLBC:
        if( !Cond( dd, mr, DI_AXP_BLBC ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVLBS:
        if( !Cond( dd, mr, DI_AXP_BLBS ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVGE:
        if( !Cond( dd, mr, DI_AXP_BGE ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVGT:
        if( !Cond( dd, mr, DI_AXP_BGT ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVLE:
        if( !Cond( dd, mr, DI_AXP_BLE ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVLT:
        if( !Cond( dd, mr, DI_AXP_BLT ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_CMOVNE:
        if( !Cond( dd, mr, DI_AXP_BNE ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_FCMOVEQ:
        if( !Cond( dd, mr, DI_AXP_BEQ ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_FCMOVGE:
        if( !Cond( dd, mr, DI_AXP_BGE ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_FCMOVGT:
        if( !Cond( dd, mr, DI_AXP_BGT ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_FCMOVLE:
        if( !Cond( dd, mr, DI_AXP_BLE ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_FCMOVLT:
        if( !Cond( dd, mr, DI_AXP_BLT ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_AXP_FCMOVNE:
        if( !Cond( dd, mr, DI_AXP_BNE ) ) return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    }
    return( MDC_OPER | MDC_TAKEN );
}

mad_disasm_control      DIGENTRY MIDisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    return( DisasmControl( dd, mr ) );
}

mad_status      DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *next )
{
    mad_disasm_control  dc;
    addr_off            new;

    memset( next, 0, sizeof( *next ) );
    next->mach.offset = mr->axp.pal.nt.fir.u._32[0] + sizeof( unsigned_32 );
    dc = DisasmControl( dd, mr );
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_JUMP:
    case MDC_CALL:
    case MDC_RET:
        new = dd->ins.op[1].value;
        if( dd->ins.op[1].type == DO_RELATIVE ) {
            new += mr->axp.pal.nt.fir.u._32[0];
        }
        if( dd->ins.op[1].base != DR_NONE ) {
            new += TRANS_REG( mr, dd->ins.op[1].base )->u64.u._32[0];
        }
        next->mach.offset = new;
    }
    return( MS_OK );
}

walk_result             DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    address             a;
    unsigned            i;
    walk_result         wr;
    mad_memref_kind     mmk;

    if( dd->ins.type >= DI_AXP_LDL && dd->ins.type <= DI_AXP_LDT ) {
        mmk = MMK_READ;
    } else if( dd->ins.type >= DI_AXP_STL && dd->ins.type <= DI_AXP_STT ) {
        mmk = MMK_WRITE;
    } else {
        return( WR_CONTINUE );
    }
    a = dd->addr;
    for( i = 0; i < dd->ins.num_ops; ++i ) {
        a.mach.offset = dd->ins.op[i].value;
        switch( dd->ins.op[i].type ) {
        case DO_MEMORY_REL:
            a.mach.offset += dd->addr.mach.offset;
            /* fall through */
        case DO_MEMORY_ABS:
            a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base )->u64.u._32[0];
            mmk &= (MMK_READ|MMK_WRITE);
            if( dd->ins.op[i].base == DR_AXP_sp || dd->ins.op[i].base == DR_AXP_r30 ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, TRANS_REF( dd->ins.op[i].ref_type ), mmk, d );
            if( wr != WR_CONTINUE ) return( wr );
            break;
        }
    }
    return( WR_CONTINUE );
}

const mad_toggle_strings        *DIGENTRY MIDisasmToggleList( void )
{
    static const mad_toggle_strings list[] = {
        { MAD_MSTR_MPSEUDOINS, MAD_MSTR_PSEUDOINS, MAD_MSTR_RAWINS },
        { MAD_MSTR_MUPPER, MAD_MSTR_UPPER, MAD_MSTR_LOWER },
        { MAD_MSTR_NIL, MAD_MSTR_NIL, MAD_MSTR_NIL }
    };
    return( list );
}

unsigned                DIGENTRY MIDisasmToggle( unsigned on, unsigned off )
{
    unsigned    toggle;

    toggle = (on & off);
    MADState->disasm_state ^= toggle;
    MADState->disasm_state |= on & ~toggle;
    MADState->disasm_state &= ~off | toggle;
    return( MADState->disasm_state );
}

mad_status              DIGENTRY MIDisasmInspectAddr( char *from, unsigned len, unsigned radix, const mad_registers *mr, address *a )
{
    char        *buff = __alloca( len * 2 );
    char        *to;

    mr = mr;
    to = buff;
    while( len != 0 ) {
        if( *from == '(' ) *to++ = '+';
        *to++ = *from++;
        --len;
    }
    return( MCMemExpr( buff, to - buff, radix, a ) );
}
