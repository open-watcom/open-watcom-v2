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
* Description:  Instruction decoding for Sun SPARC architecture.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "dis.h"
#include "distypes.h"
#include "dissparc.h"
#include "sparcenc.h"

#define _SparcReg( x )          ( (x) + DR_SPARC_r0 )
#define _SparcFReg( x )         ( (x) + DR_SPARC_f0 )
#define _SparcCReg( x )         ( (x) + DR_SPARC_c0 )
#define _SparcIns( x )          (x)

dis_handler_return SPARCSetHi( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = DisSEX( code.sethi.imm22, 21 );
    ins->op[1].type = DO_REG;
    ins->op[1].base = _SparcReg( code.sethi.rd );
    ins->num_ops = 2;
    return( DHR_DONE );
}

dis_handler_return SPARCBranch( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    ins->op[0].type = DO_RELATIVE;
    ins->op[0].value = ( DisSEX( code.branch.disp22, 21 ) ) * sizeof( ins->opcode );
    if( code.branch.anul != 0 ) {
        ins->flags.u.sparc |= DIF_SPARC_ANUL;
    }
    ins->num_ops = 1;
    return( DHR_DONE );
}

dis_handler_return SPARCCall( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    ins->op[0].type  = DO_RELATIVE;
    // BartoszP 16.10.2005
    // SPARC Architecture Manual says:
    // CALL saves self address not next instruction into the %o7 register
    //ins->op[0].value = ( DisSEX( code.call.disp, 29 ) + 1) * sizeof( ins->opcode );
    ins->op[0].value = ( DisSEX( code.call.disp, 29 ) ) * sizeof( ins->opcode );
    ins->num_ops     = 1;
    return( DHR_DONE );
}

dis_handler_return SPARCOp3( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    ins->op[ 0 ].type = DO_REG;
    ins->op[ 0 ].base = _SparcReg( code.op3.rs1 );
    ins->op[ 2 ].type = DO_REG;
    ins->op[ 2 ].base = _SparcReg( code.op3.rd );
    if( code.op3.imm != 0 ) {
        ins->op[ 1 ].type = DO_IMMED;
        ins->op[ 1 ].base = DR_NONE;
        ins->op[ 1 ].value = DisSEX( code.op3imm.simm13, 12 );
    } else {
        ins->op[ 1 ].type = DO_REG;
        ins->op[ 1 ].base = _SparcReg( code.op3.rs2 );
        ins->op[ 1 ].value = 0;
    }
    ins->num_ops = 3;
    return( DHR_DONE );
}

static void getOpIndices( sparc_ins code, int *mem, int *reg )
// figure out which operand of an instruction should be
// the memory reference and which the register
{
    // 3rd bit in an opcode for format(3) instructions seems
    // to differentiate between loads/stores
    if( code.op3.opcode_3 & 0x04 ) {
        *mem = 1;
        *reg = 0;
    } else {
        *mem = 0;
        *reg = 1;
    }
}

static void doSparcMem( dis_handle *h, void *d, dis_operand *op, sparc_ins code )
{
    h = h; d = d;

    op->type = DO_MEMORY_ABS;
    op->base = _SparcReg( code.op3.rs1 );
    op->index = DR_NONE;
    op->value = 0;
    if( code.op3.imm != 0 ) {
        op->value = DisSEX( code.op3imm.simm13, 12 );
    } else {
        // NYI: should check for asi and stuff it into op
        op->index = _SparcReg( code.op3.rs2 );
    }
}

static const dis_ref_type integerRefTypes[] = {
    DRT_SPARC_WORD,
    DRT_SPARC_BYTE,
    DRT_SPARC_HALF,
    DRT_SPARC_DWORD
};

dis_handler_return SPARCMem( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;
    int         mem_op;
    int         reg_op;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    getOpIndices( code, &mem_op, &reg_op );
    ins->op[ reg_op ].type = DO_REG;
    ins->op[ reg_op ].base = _SparcReg( code.op3.rd );
    ins->op[ mem_op ].ref_type = integerRefTypes[ code.op3.opcode_3 & 0x03 ];
    doSparcMem( h, d, &ins->op[ mem_op ], code );
    ins->num_ops = 2;
    return( DHR_DONE );
}

static const dis_ref_type floatRefTypes[] = {
    DRT_SPARC_SFLOAT,
    DRT_SPARC_WORD,
    DRT_SPARC_DFLOAT,
    DRT_SPARC_DFLOAT,
};

dis_handler_return SPARCFPop2( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    if( code.op3opf.opcode_3 == 0x35 ) {
        // fcmp
        ins->op[ 0 ].type = DO_REG;
        ins->op[ 0 ].base = _SparcFReg( code.op3opf.rs1 );
        ins->op[ 1 ].type = DO_REG;
        ins->op[ 1 ].base = _SparcFReg( code.op3opf.rs2 );
    } else {
        ins->op[ 0 ].type = DO_REG;
        ins->op[ 0 ].base = _SparcFReg( code.op3opf.rs2 );
        ins->op[ 1 ].type = DO_REG;
        ins->op[ 1 ].base = _SparcFReg( code.op3opf.rd );
    }
    ins->num_ops = 2;
    return( DHR_DONE );
}

dis_handler_return SPARCFPop3( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;

    h = h; d = d;

    code.full = _SparcIns( ins->opcode );
    ins->op[ 0 ].type = DO_REG;
    ins->op[ 0 ].base = _SparcFReg( code.op3opf.rs1 );
    ins->op[ 1 ].type = DO_REG;
    ins->op[ 1 ].base = _SparcFReg( code.op3opf.rs2 );
    ins->op[ 2 ].type = DO_REG;
    ins->op[ 2 ].base = _SparcFReg( code.op3opf.rd );
    ins->num_ops = 3;
    return( DHR_DONE );
}


dis_handler_return SPARCMemF( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;
    int         mem_op;
    int         reg_op;

    code.full = _SparcIns( ins->opcode );
    getOpIndices( code, &mem_op, &reg_op );
    ins->op[ reg_op ].type = DO_REG;
    ins->op[ reg_op ].base = _SparcFReg( code.op3.rd );
    ins->op[ mem_op ].ref_type = floatRefTypes[ code.op3.opcode_3 & 0x03 ];
    doSparcMem( h, d, &ins->op[ mem_op ], code );
    if( code.op3.opcode_3 == 0x21 || code.op3.opcode_3 == 0x25 ) {
        // special case for ldfsr/stfsr instructions
        ins->op[ reg_op ].base = DR_SPARC_fsr;
    }
    if( code.op3.opcode_3 == 0x26 ) {
        // another special case - stdfq instruction
        ins->op[ reg_op ].base = DR_SPARC_fq;
    }
    ins->num_ops = 2;
    return( DHR_DONE );
}

static const dis_ref_type coproRefTypes[] = {
    DRT_SPARC_WORD,
    DRT_SPARC_WORD,
    DRT_SPARC_DWORD,
    DRT_SPARC_DWORD,
};

dis_handler_return SPARCMemC( dis_handle *h, void *d, dis_dec_ins *ins )
{
    sparc_ins   code;
    int         mem_op;
    int         reg_op;

    code.full = _SparcIns( ins->opcode );
    getOpIndices( code, &mem_op, &reg_op );
    ins->op[ reg_op ].type = DO_REG;
    ins->op[ reg_op ].base = _SparcCReg( code.op3.rd );
    ins->op[ mem_op ].ref_type = coproRefTypes[ code.op3.opcode_3 & 0x03 ];
    doSparcMem( h, d, &ins->op[ mem_op ], code );
    if( code.op3.opcode_3 == 0x31 || code.op3.opcode_3 == 0x35 ) {
        // special case for ldcsr/stcsr instructions
        ins->op[ reg_op ].base = DR_SPARC_csr;
    }
    if( code.op3.opcode_3 == 0x36 ) {
        // another special case - stdcq instruction
        ins->op[ reg_op ].base = DR_SPARC_cq;
    }
    ins->num_ops = 2;
    return( DHR_DONE );
}

static size_t SPARCInsHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    const char  *new_op_name;

    h = h; d = d;

    if( !(flags & DFF_PSEUDO) )
        return( 0 );
    new_op_name = NULL;
    switch( ins->type ) {
    case DI_SPARC_sethi:
        if( ins->op[ 1 ].base == DR_SPARC_r0 &&
            ins->op[ 0 ].value == 0 ) {
            new_op_name = "nop";
            ins->num_ops = 0;
        }
        break;
    case DI_SPARC_jmpl:
        if( ins->op[ 0 ].base == DR_SPARC_r31 &&
            ins->op[ 0 ].value == 8 &&
            ins->op[ 1 ].base == DR_SPARC_r0 ) {
            // jmpl %i7+8, %g0 -> ret
            new_op_name = "ret";
            ins->num_ops = 0;
        } else if( ins->op[ 0 ].base == DR_SPARC_r15 &&
            ins->op[ 0 ].value == 8 &&
            ins->op[ 1 ].base == DR_SPARC_r0 ) {
            // jmpl %o7+8, %g0 -> retl
            new_op_name = "retl";
            ins->num_ops = 0;
        }
        break;
    case DI_SPARC_subcc:
        if( ins->op[ 2 ].base == DR_SPARC_r0 ) {
            new_op_name = "cmp";
            ins->num_ops = 2;
        }
        break;
    case DI_SPARC_or:
        if( ins->op[ 0 ].base == DR_SPARC_r0 ) {
            new_op_name = "mov";
            ins->num_ops = 2;
            ins->op[ 0 ] = ins->op[ 1 ];
            ins->op[ 1 ] = ins->op[ 2 ];
            if( ins->op[ 0 ].base == DR_SPARC_r0
                 || ( ins->op[ 0 ].type == DO_IMMED
                     && ins->op[ 0 ].value == 0 )  ) {
                new_op_name = "clr";
                ins->num_ops = 1;
                ins->op[ 0 ] = ins->op[ 1 ];
            }
        } else {
            if( ins->op[ 1 ].base == DR_SPARC_r0
                 || ( ins->op[ 1 ].type == DO_IMMED
                     && ins->op[ 1 ].value == 0 )  ) {
                new_op_name = "mov";
                ins->num_ops = 2;
                ins->op[ 1 ] = ins->op[ 2 ];
            }
        }
        break;
    case DI_SPARC_orcc:
        if( ins->op[ 0 ].base == DR_SPARC_r0 &&
            ins->op[ 2 ].base == DR_SPARC_r0 ) {
            new_op_name = "tst";
            ins->num_ops = 1;
            ins->op[ 0 ] = ins->op[ 1 ];
        }
        break;
    case DI_SPARC_restore:
    case DI_SPARC_save:
        if( ins->op[ 0 ].base == DR_SPARC_r0 &&
            ins->op[ 1 ].base == DR_SPARC_r0 &&
            ins->op[ 2 ].base == DR_SPARC_r0 ) {
            ins->num_ops = 0;
        }
        break;
    default:
        break;
    }
    if( name != NULL && new_op_name != NULL ) {
        strcpy( name, new_op_name );
        return( strlen( name ) );
    }
    return( 0 );
}

static size_t SPARCFlagHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    h = h; d = d; ins = ins; flags = flags; name = name;

    return( 0 );
}

static dis_register sparcTranslate( dis_register reg ) {

    if( reg >= DR_SPARC_r0 && reg <= DR_SPARC_r31 ) {
        reg += DR_SPARC_g0 - DR_SPARC_r0;
        switch( reg ) {
        case DR_SPARC_i6:
            reg = DR_SPARC_fp;
            break;
        case DR_SPARC_o6:
            reg = DR_SPARC_sp;
            break;
        default:
            break;
        }
    }
    return( reg );
}

static size_t SPARCOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff, size_t buff_len )
{
    dis_operand *op;

    h = h; d = d; op_buff = op_buff; buff_len = buff_len;

    ins->op[op_num].ref_type = DRT_SPARC_WORD;
    if( flags & DFF_SYMBOLIC_REG ) {
        op = &ins->op[op_num];
        if( op->base >= DR_SPARC_r0 && op->base <= DR_SPARC_r31 ) {
            op->base = sparcTranslate( op->base );
        }
        if( op->index >= DR_SPARC_r0 && op->index <= DR_SPARC_r31 ) {
            op->index = sparcTranslate( op->index );
        }
    }
    return( 0 );
}

static dis_handler_return SPARCDecodeTableCheck( int page, dis_dec_ins *ins )
{
    page = page; ins = ins;

    return( DHR_DONE );
}

static void ByteSwap( dis_handle *h, void *d, dis_dec_ins *ins )
{
    h = h; d = d;

    if( h->need_bswap ) {
        SWAP_32( ins->opcode );
    }
}

static void SPARCPreprocHook( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ByteSwap( h, d, ins );
}

static size_t SPARCPostOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff, size_t buff_len )
{
    // Nothing to do
    h = h; d = d; ins = ins; flags = flags; op_num = op_num; op_buff = op_buff; buff_len = buff_len;
    return( 0 );
}

const dis_cpu_data SPARCData = {
    SPARCRangeTable, SPARCRangeTablePos, SPARCPreprocHook, SPARCDecodeTableCheck, SPARCInsHook, SPARCFlagHook, SPARCOpHook, SPARCPostOpHook, &SPARCMaxInsName, 4
};
