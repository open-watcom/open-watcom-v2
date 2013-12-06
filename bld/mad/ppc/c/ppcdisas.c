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
* Description:  PowerPC instruction decoding.
*
****************************************************************************/


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "walloca.h"
#include "ppc.h"
#include "ppctypes.h"
#include "madregs.h"

static dis_handle DH;

mad_status DisasmInit()
{
    bool        swap_bytes;

#ifdef __BIG_ENDIAN__
    swap_bytes = FALSE;
#else
    swap_bytes = TRUE;
#endif
    if( DisInit( DISCPU_ppc, &DH, swap_bytes ) != DR_OK ) {
        return( MS_ERR | MS_FAIL );
    }
    return( MS_OK );
}

void DisasmFini()
{
    DisFini( &DH );
}

dis_return DisCliGetData( void *d, unsigned off, unsigned size, void *data )
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
        MCAddrToString( val, PPCT_N32_PTR, MLK_CODE, 40, p );
        break;
    case DO_IMMED:
    case DO_ABSOLUTE:
    case DO_MEMORY_ABS:
        MCTypeInfoForHost( MTK_INTEGER, -(int)sizeof( ins->op[0].value ), &mti );
        max = 40;
        MCTypeToString( dd->radix, &mti, &ins->op[op].value, &max, p );
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
    if( MADState->disasm_state & DT_PSUEDO_OPS ) ff |= DFF_PSEUDO;
    if( MADState->disasm_state & DT_UPPER ) ff |= DFF_INS_UP | DFF_REG_UP;
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
    return( sizeof( unsigned_32 ) );
}

mad_status              DIGENTRY MIDisasmInsUndoable( mad_disasm_data *dd )
{
#if 0 //NYI:
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
#endif
    return( MS_OK );
}

const unsigned_16 RegTrans[] = {
#define regpick( e, n ) offsetof( mad_registers, ppc.e ),
#include "regppc.h"
#undef regpick
};

#define PPCT_BRHWORD    PPCT_HWORD
#define PPCT_BRWORD     PPCT_WORD
#define PPCT_BRDWORD    PPCT_DWORD
#define PPCT_SWORD      MAD_NIL_TYPE_HANDLE
#define PPCT_MWORD      MAD_NIL_TYPE_HANDLE
#define PPCT_SFLOAT     PPCT_FLOAT
#define PPCT_DFLOAT     PPCT_DOUBLE

static const mad_type_handle RefTrans[] = {
#define refpick( e, n ) PPCT_##e,
#include "refppc.h"
#undef refpick
};

static int CTRZero( mad_registers const *mr )
{
    if( mr->ppc.ctr.u._32[I64LO32] != 1 ) return( 0 );
    if( mr->ppc.msr.u._32[I64HI32] & (1UL << MSR_H_sf) ) {
        if( mr->ppc.ctr.u._32[I64HI32] != 0 ) return( 0 );
    }
    return( 1 );
}

static int CRTest( mad_registers const *mr, mad_disasm_data *dd )
{
    if( mr->ppc.cr & (1 << (31 - dd->ins.op[1].value)) ) return( 1 );
    return( 0 );
}

static mad_disasm_control Cond( mad_disasm_data *dd, mad_registers const *mr,
                        addr_off dest )
{
    #define NOT_TAKEN   (MDC_CONDITIONAL | MDC_TAKEN_NOT)
    switch( dd->ins.op[0].value >> 1 ) {
    case 0x0:
        if( CTRZero( mr ) || CRTest( mr, dd ) ) return( NOT_TAKEN );
        break;
    case 0x1:
        if( !CTRZero( mr ) || CRTest( mr, dd ) ) return( NOT_TAKEN );
        break;
    case 0x2:
    case 0x3:
        if( CRTest( mr, dd ) ) return( NOT_TAKEN );
        break;
    case 0x4:
        if( CTRZero( mr ) || !CRTest( mr, dd ) ) return( NOT_TAKEN );
        break;
    case 0x5:
        if( !CTRZero( mr ) || !CRTest( mr, dd ) ) return( NOT_TAKEN );
        break;
    case 0x6:
    case 0x7:
        if( !CRTest( mr, dd ) ) return( NOT_TAKEN );
        break;
    case 0x8:
    case 0xc:
        if( CTRZero( mr ) ) return( NOT_TAKEN );
        break;
    case 0x9:
    case 0xd:
        if( !CTRZero( mr ) ) return( NOT_TAKEN );
        break;
    case 0xa:
    case 0xb:
    case 0xe:
    case 0xf:
        if( dest < dd->addr.mach.offset ) {
            return( MDC_TAKEN_BACK );
        } else {
            return( MDC_TAKEN_FORWARD );
        }
    }
    if( dest < dd->addr.mach.offset ) {
        return( MDC_CONDITIONAL | MDC_TAKEN_BACK );
    } else {
        return( MDC_CONDITIONAL | MDC_TAKEN_FORWARD );
    }
}

#define TRANS_REG( mr, r ) (*(unsigned_64 *)((unsigned_8*)(mr) + RegTrans[r - DR_PPC_FIRST]))

static unsigned TrapTest( mad_disasm_data *dd, mad_registers const *mr )
{
    unsigned_64 a;
    unsigned_64 b;
    unsigned    bits;

    switch( dd->ins.type ) {
    case DI_PPC_twi:
    case DI_PPC_tdi:
       b.u._32[I64LO32] = dd->ins.op[2].value;
       if( dd->ins.op[2].value < 0 ) {
           b.u._32[I64HI32] = -1;
       } else {
           b.u._32[I64HI32] = 0;
       }
       break;
    default:
        b = TRANS_REG( mr, dd->ins.op[2].base );
        break;
    }
    a = TRANS_REG( mr, dd->ins.op[1].base );
    bits = 0;
    switch( dd->ins.type ) {
    case DI_PPC_td:
    case DI_PPC_tdi:
        if( a.u._32[I64HI32] == b.u._32[I64HI32] ) {
            if( a.u._32[I64LO32] == b.u._32[I64LO32] ) {
                bits |= 0x04;
            } else {
                if( a.u._32[I64LO32] < b.u._32[I64LO32] ) {
                    bits |= 0x02;
                    if( a.u.sign.v ) {
                        bits |= 0x08;
                    } else {
                        bits |= 0x10;
                    }
                } else {
                    bits |= 0x01;
                    if( a.u.sign.v ) {
                        bits |= 0x10;
                    } else {
                        bits |= 0x08;
                    }
                }
            }
        } else if( a.u._32[I64HI32] < b.u._32[I64HI32] ) {
            bits |= 0x02;
            if( a.u.sign.v ) {
                bits |= 0x08;
            } else {
                bits |= 0x10;
            }
        } else {
            bits |= 0x01;
            if( a.u.sign.v ) {
                bits |= 0x10;
            } else {
                bits |= 0x08;
            }
        }
        break;
    default:
        if( a.u._32[I64LO32] < b.u._32[I64LO32] ) bits |= 0x02;
        if( a.u._32[I64LO32] > b.u._32[I64LO32] ) bits |= 0x01;
        if( (signed_32)a.u._32[I64LO32] < (signed_32)b.u._32[I64LO32] ) bits |= 0x10;
        if( (signed_32)a.u._32[I64LO32] > (signed_32)b.u._32[I64LO32] ) bits |= 0x08;
        if( bits == 0 ) bits |= 0x04;
        break;
    }
    return( bits );
}

mad_disasm_control DisasmControl( mad_disasm_data *dd, mad_registers const *mr )
{
    mad_disasm_control  c;
    addr_off            v;

    switch( dd->ins.type ) {
    case DI_PPC_b:
        if( dd->ins.flags.u.ppc & DIF_PPC_LK ) {
            c = MDC_CALL;
        } else {
            c = MDC_JUMP;
        }
        v = dd->ins.op[0].value;
        if( dd->ins.op[0].type == DO_RELATIVE ) {
            v += dd->addr.mach.offset;
        }
        if( v < dd->addr.mach.offset ) {
            return( c | MDC_TAKEN_BACK );
        } else {
            return( c | MDC_TAKEN_FORWARD );
        }
    case DI_PPC_bc:
        if( dd->ins.flags.u.ppc & DIF_PPC_LK ) {
            c = MDC_CALL;
        } else {
            c = MDC_JUMP;
        }
        v = dd->ins.op[2].value;
        if( dd->ins.op[2].type == DO_RELATIVE ) {
            v += dd->addr.mach.offset;
        }
        return( c | Cond( dd, mr, v ) );
    case DI_PPC_bcctr:
        if( dd->ins.flags.u.ppc & DIF_PPC_LK ) {
            c = MDC_CALL;
        } else {
            c = MDC_JUMP;
        }
        return( c | Cond( dd, mr, mr->ppc.ctr.u._32[I64LO32] ) );
    case DI_PPC_bclr:
        if( dd->ins.flags.u.ppc & DIF_PPC_LK ) {
            c = MDC_CALL;
        } else {
            c = MDC_RET;
        }
        return( c | Cond( dd, mr, mr->ppc.lr.u._32[I64LO32] ) );
    case DI_PPC_rfi:
        return( MDC_SYSRET | MDC_TAKEN );
    case DI_PPC_sc:
        return( MDC_SYSCALL | MDC_TAKEN );
    case DI_PPC_td:
    case DI_PPC_tdi:
    case DI_PPC_tw:
    case DI_PPC_twi:
        c = MDC_SYSRET | MDC_CONDITIONAL;
        if( TrapTest( dd, mr ) & dd->ins.op[0].value ) {
            c |= MDC_TAKEN;
        }
        return( c );
    default:
        break;
    }
    return( MDC_OPER | MDC_TAKEN );
}

mad_disasm_control      DIGENTRY MIDisasmControl( mad_disasm_data *dd, mad_registers const *mr )
{
    return( DisasmControl( dd, mr ) );
}

mad_status      DIGENTRY MIDisasmInsNext( mad_disasm_data *dd, mad_registers const *mr, address *next )
{
    mad_disasm_control  dc;

    memset( next, 0, sizeof( *next ) );
    next->mach.offset = mr->ppc.iar.u._32[I64LO32] + sizeof( unsigned_32 );
    dc = DisasmControl( dd, mr );
    if( (dc & MDC_TAKEN_MASK) == MDC_TAKEN_NOT ) {
        return( MS_OK );
    }
    switch( dc & MDC_TYPE_MASK ) {
    case MDC_JUMP:
    case MDC_CALL:
    case MDC_RET:
        //NYI:
        return( MS_UNSUPPORTED );
    }
    return( MS_OK );
}

walk_result             DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, mad_registers const *mr, void *d )
{
    address             a;
    unsigned            i;
    walk_result         wr;
    mad_memref_kind     mmk;

    if( dd->ins.type >= DI_PPC_lbz && dd->ins.type <= DI_PPC_lwzx ) {
        mmk = MMK_READ;
    } else if( dd->ins.type >= DI_PPC_stb && dd->ins.type <= DI_PPC_stwx ) {
        mmk = MMK_WRITE;
    } else {
        return( WR_CONTINUE );
    }
    a = dd->addr;
    for( i = 0; i < dd->ins.num_ops; ++i ) {
        if( dd->ins.op[i].type == DO_MEMORY_ABS ) {
            a.mach.offset = dd->ins.op[i].value;
            if( dd->ins.op[i].base != DR_PPC_r0 ) {
                a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base ).u._32[I64LO32];
            }
            mmk &= (MMK_READ|MMK_WRITE);
            if( dd->ins.op[i].base == DR_PPC_r1 ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, RefTrans[dd->ins.op[i].ref_type-DRT_PPC_FIRST], mmk, d );
            return( wr );
        } else if( dd->ins.op[i].extra & PE_XFORM ) {
            a.mach.offset = 0;
            if( dd->ins.op[i].base != DR_PPC_r0 ) {
                a.mach.offset += TRANS_REG( mr, dd->ins.op[i].base ).u._32[I64LO32];
            }
            a.mach.offset += TRANS_REG( mr, dd->ins.op[i+1].base ).u._32[I64LO32];
            mmk &= (MMK_READ|MMK_WRITE);
            if( dd->ins.op[i].base == DR_PPC_r1 || dd->ins.op[i+1].base == DR_PPC_r1 ) {
                mmk |= MMK_VOLATILE;
            }
            wr = wk( a, RefTrans[dd->ins.op[i].ref_type-DRT_PPC_FIRST], mmk, d );
            return( wr );
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

mad_status              DIGENTRY MIDisasmInspectAddr( char *from, unsigned len, unsigned radix, mad_registers const *mr, address *a )
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
