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
* Description:  MIPS instruction decoding.
*
****************************************************************************/


#include <stdlib.h>
#include "walloca.h"
#include "mips.h"
#include "mpstypes.h"
#include "madregs.h"

static dis_handle DH;

mad_status DisasmInit()
{
    bool        swap_bytes;

#ifdef __BIG_ENDIAN__
    swap_bytes = true;
#else
    swap_bytes = false;
#endif
    if( DisInit( DISCPU_mips, &DH, swap_bytes ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    return( MS_OK );
}

void DisasmFini()
{
    DisFini( &DH );
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

size_t DisCliValueString( void *d, dis_dec_ins *ins, unsigned op, char *buff, size_t buff_size )
{
    mad_disasm_data     *dd = d;
    mad_type_info       mti;
    address             val;

    buff[0] = '\0';
    val = dd->addr;
    switch( ins->op[op].type & DO_MASK ) {
    case DO_RELATIVE:
        val.mach.offset += ins->op[op].value;
        //NYI: 64 bit
        MCAddrToString( val, MIPST_N32_PTR, MLK_CODE, buff, buff_size );
        break;
    case DO_ABSOLUTE:
        if( dd->ins.type == DI_MIPS_J || dd->ins.type == DI_MIPS_JAL ) {
            // Handle j/jal as having pointer operand to show target symbol
            val.mach.offset = ins->op[op].value;
            MCAddrToString( val, MIPST_N32_PTR, MLK_CODE, buff, buff_size );
            break;
        }
        // Fall through
    case DO_IMMED:
    case DO_MEMORY_ABS:
        MCTypeInfoForHost( MTK_INTEGER, SIGNTYPE_SIZE( sizeof( ins->op[0].value ) ), &mti );
        MCTypeToString( dd->radix, &mti, &ins->op[op].value, buff, &buff_size );
        break;
    }
    return( strlen( buff ) );
}

unsigned MADIMPENTRY( DisasmDataSize )( void )
{
    return( sizeof( mad_disasm_data ) );
}

unsigned MADIMPENTRY( DisasmNameMax )( void )
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

mad_status MADIMPENTRY( Disasm )( mad_disasm_data *dd, address *a, int adj )
{
    return( DisasmOne( dd, a, adj ) );
}

size_t MADIMPENTRY( DisasmFormat )( mad_disasm_data *dd, mad_disasm_piece dp, mad_radix radix, char *buff, size_t buff_size )
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
    ff = DFF_NONE;
    if( MADState->disasm_state & DT_PSUEDO_OPS ) ff |= DFF_PSEUDO;
    if( MADState->disasm_state & DT_UPPER ) ff |= DFF_INS_UP | DFF_REG_UP;
    if( MADState->reg_state[CPU_REG_SET] & CT_SYMBOLIC_NAMES ) {
        ff |= DFF_SYMBOLIC_REG;
    }
    dd->radix = radix;
    if( DisFormat( &DH, dd, &dd->ins, ff, np, nlen, op, olen ) != DR_OK ) {
        return( 0 );
    }
    olen = strlen( obuff );
    nlen = strlen( nbuff );
    if( dp == MDP_ALL ) nbuff[ nlen++ ] = ' ';
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

unsigned MADIMPENTRY( DisasmInsSize )( mad_disasm_data *dd )
{
    dd = dd;

    return( sizeof( unsigned_32 ) );
}

mad_status MADIMPENTRY( DisasmInsUndoable )( mad_disasm_data *dd )
{
    switch( dd->ins.type ) {
    case DI_MIPS_SYSCALL:
    case DI_MIPS_ERET:
    case DI_MIPS_BREAK:
        return( MS_FAIL );
    }
    return( MS_OK );
}

const unsigned_16   RegTrans[] = {
    #define regpick(id,type,reg_set)    offsetof( mad_registers, mips.id ),
    #define regpicku(u,id,type,reg_set) offsetof( mad_registers, mips.u.id ),
    #include "mpsregs.h"
    #undef regpick
    #undef regpicku
};

// Map left/right accesses to plain word/dword accesses
#define MIPST_WORDL     MIPST_WORD
#define MIPST_WORDR     MIPST_WORD
#define MIPST_DWORDR    MIPST_DWORD
#define MIPST_DWORDL    MIPST_DWORD
#define MIPST_SFLOAT    MIPST_FLOAT
#define MIPST_DFLOAT    MIPST_DOUBLE

const mad_type_handle RefTrans[] = {
    #define refpick( e, n ) MIPST_##e,
    #include "refmips.h"
    #undef refpick
};

static int Cond1( mad_disasm_data *dd, const mad_registers *mr, unsigned condition )
{
    int                 cmp;
    unsigned_32         val;

    val = TRANS_REG( mr, dd->ins.op[0].base )->u._32[I64LO32];
    if( val & 0x80000000 ) {     // check sign bit
        cmp = -1;
    } else if( val != 0 ) {
        cmp = +1;
    } else {
        cmp = 0;
    }

    switch( condition ) {
    case DI_MIPS_BGEZ:
    case DI_MIPS_BGEZL:
    case DI_MIPS_BGEZAL:
    case DI_MIPS_BGEZALL:
        if( cmp >= 0 ) break;
        return( 0 );
    case DI_MIPS_BGTZ:
    case DI_MIPS_BGTZL:
        if( cmp > 0 ) break;
        return( 0 );
    case DI_MIPS_BLEZ:
    case DI_MIPS_BLEZL:
        if( cmp <= 0 ) break;
        return( 0 );
    case DI_MIPS_BLTZ:
    case DI_MIPS_BLTZL:
    case DI_MIPS_BLTZAL:
    case DI_MIPS_BLTZALL:
        if( cmp < 0 ) break;
        return( 0 );
    default:
        break;
    }
    return( 1 );
}

static int Cond2( mad_disasm_data *dd, const mad_registers *mr, unsigned condition )
{
    unsigned_32     sval;
    unsigned_32     tval;

    sval = TRANS_REG( mr, dd->ins.op[0].base )->u._32[I64LO32];
    tval = TRANS_REG( mr, dd->ins.op[1].base )->u._32[I64LO32];
    switch( condition ) {
    case DI_MIPS_BEQ:
    case DI_MIPS_BEQL:
        if( sval == tval ) break;
        return( 0 );
    case DI_MIPS_BNE:
    case DI_MIPS_BNEL:
        if( sval != tval ) break;
        return( 0 );
    default:
        break;
    }
    return( 1 );
}

mad_disasm_control DisasmControl( mad_disasm_data *dd, mad_registers const *mr )
{
    mad_disasm_control  c;
    bool                is_call = false;

    switch( dd->ins.type ) {
    case DI_MIPS_J:
        return( MDC_JUMP | MDC_TAKEN );
    case DI_MIPS_JAL:
    case DI_MIPS_JALR:
        return( MDC_CALL | MDC_TAKEN );
    case DI_MIPS_JR:
        if( dd->ins.op[0].base == DR_MIPS_r31 ) {
            c = MDC_RET;    //  'jr ra'
        } else {
            c = MDC_JUMP;
        }
        return( c | MDC_TAKEN );
    case DI_MIPS_ERET:
        return( MDC_SYSRET | MDC_TAKEN );
    case DI_MIPS_SYSCALL:
        return( MDC_SYSCALL | MDC_TAKEN );
    case DI_MIPS_BEQ:
    case DI_MIPS_BEQL:
    case DI_MIPS_BNE:
    case DI_MIPS_BNEL:
        if( !Cond2( dd, mr, dd->ins.type ) )
            return( MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[2].value < 0
            ? (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_BACK)
            : (MDC_JUMP | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
    case DI_MIPS_BGEZAL:
    case DI_MIPS_BGEZALL:
    case DI_MIPS_BLTZAL:
    case DI_MIPS_BLTZALL:
        is_call = true;
        /* fall through */
    case DI_MIPS_BGEZ:
    case DI_MIPS_BGEZL:
    case DI_MIPS_BGTZ:
    case DI_MIPS_BGTZL:
    case DI_MIPS_BLEZ:
    case DI_MIPS_BLEZL:
    case DI_MIPS_BLTZ:
    case DI_MIPS_BLTZL:
        if( is_call )
            c = MDC_CALL;
        else
            c = MDC_JUMP;
        if( !Cond1( dd, mr, dd->ins.type ) )
            return( c | MDC_CONDITIONAL | MDC_TAKEN_NOT );
        return( dd->ins.op[1].value < 0
            ? (c | MDC_CONDITIONAL | MDC_TAKEN_BACK)
            : (c | MDC_CONDITIONAL | MDC_TAKEN_FORWARD) );
//    case DI_MIPS_SLT:
//        if( !Cond( dd, mr, DI_MIPS_BEQ ) )
//            return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN_NOT );
//        return( MDC_OPER | MDC_CONDITIONAL | MDC_TAKEN );
    case DI_MIPS_TEQ:
    case DI_MIPS_TEQI:
    case DI_MIPS_TGE:
    case DI_MIPS_TGEI:
    case DI_MIPS_TGEU:
    case DI_MIPS_TLT:
    case DI_MIPS_TLTI:
    case DI_MIPS_TLTU:
    case DI_MIPS_TNE:
    case DI_MIPS_TNEI:
        c = MDC_SYSRET | MDC_CONDITIONAL;
//        if( TrapTest( dd, mr ) & dd->ins.op[0].value ) {
//            c |= MDC_TAKEN;
//        }
        return( c );
    default:
        break;
    }
    return( MDC_OPER | MDC_TAKEN );
}

mad_disasm_control MADIMPENTRY( DisasmControl )( mad_disasm_data *dd, mad_registers const *mr )
{
    return( DisasmControl( dd, mr ) );
}

mad_status MADIMPENTRY( DisasmInsNext )( mad_disasm_data *dd, mad_registers const *mr, address *next )
{
    mad_disasm_control  dc;
    addr_off            new;
    unsigned            op;

    memset( next, 0, sizeof( *next ) );
    next->mach.offset = mr->mips.pc.u._32[I64LO32] + sizeof( unsigned_32 );
    dc = DisasmControl( dd, mr );
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_JUMP:
    case MDC_CALL:
    case MDC_RET:
        if( dd->ins.type == DI_MIPS_J || dd->ins.type == DI_MIPS_JAL || dd->ins.type == DI_MIPS_JR )
            op = 0; // Target is the first operand, for other instructions it's the second operand
        else
            op = 1;
        new = dd->ins.op[op].value;
        if( dd->ins.op[op].type == DO_RELATIVE ) {
            new += mr->mips.pc.u._32[I64LO32];
        }
        if( dd->ins.op[op].base != DR_NONE ) {
            new += TRANS_REG( mr, dd->ins.op[op].base )->u._32[I64LO32];
        }
        next->mach.offset = new;
    }
    return( MS_OK );
}

walk_result MADIMPENTRY( DisasmMemRefWalk )( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, mad_registers const *mr, void *d )
{
    address             a;
    unsigned            i;
    walk_result         wr;
    mad_memref_kind     mmk;

    if( dd->ins.type >= DI_MIPS_LB && dd->ins.type <= DI_MIPS_LWC1 ) {
        mmk = MMK_READ;
    } else if( dd->ins.type >= DI_MIPS_SB && dd->ins.type <= DI_MIPS_SWC1 ) {
        mmk = MMK_WRITE;
    } else {
        return( WR_CONTINUE );
    }
    a = dd->addr;
    for( i = 0; i < dd->ins.num_ops; ++i ) {
        if( dd->ins.op[i].type == DO_MEMORY_ABS ) {
            a.mach.offset = dd->ins.op[i].value;
            if( dd->ins.op[i].base != DR_MIPS_r0 ) {
                a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base )->u._32[I64LO32];
            }
            mmk &= (MMK_READ | MMK_WRITE);
            if( dd->ins.op[i].base == DR_MIPS_sp ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, TRANS_REF( dd->ins.op[i].ref_type ), mmk, d );
            return( wr );
        } else if( dd->ins.op[i].extra & PE_XFORM ) {
            a.mach.offset = 0;
            if( dd->ins.op[i].base != DR_MIPS_r0 ) {
                a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base )->u._32[I64LO32];
            }
            a.mach.offset += TRANS_REG( mr, dd->ins.op[i + 1].base )->u._32[I64LO32];
            mmk &= (MMK_READ | MMK_WRITE);
            if( dd->ins.op[i].base == DR_MIPS_sp || dd->ins.op[i + 1].base == DR_MIPS_sp ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, TRANS_REF( dd->ins.op[i].ref_type ), mmk, d );
            return( wr );
        }
    }
    return( WR_CONTINUE );
}

const mad_toggle_strings *MADIMPENTRY( DisasmToggleList )( void )
{
    static const mad_toggle_strings list[] = {
        { MAD_MSTR_MPSEUDOINS, MAD_MSTR_PSEUDOINS, MAD_MSTR_RAWINS },
        { MAD_MSTR_MUPPER, MAD_MSTR_UPPER, MAD_MSTR_LOWER },
        { MAD_MSTR_NIL, MAD_MSTR_NIL, MAD_MSTR_NIL }
    };
    return( list );
}

unsigned MADIMPENTRY( DisasmToggle )( unsigned on, unsigned off )
{
    unsigned    toggle;

    toggle = (on & off);
    MADState->disasm_state ^= toggle;
    MADState->disasm_state |= on & ~toggle;
    MADState->disasm_state &= ~off | toggle;
    return( MADState->disasm_state );
}

mad_status MADIMPENTRY( DisasmInspectAddr )(const char *start, unsigned len, mad_radix radix, mad_registers const *mr, address *a)
{
    char        *buff = __alloca( len * 2 + 1 );
    char        *to;

    mr = mr;
    to = buff;
    for( ; len != 0; --len ) {
        if( *start == '(' )
            *to++ = '+';
        *to++ = *start++;
    }
    *to = '\0';
    return( MCMemExpr( buff, radix, a ) );
}
