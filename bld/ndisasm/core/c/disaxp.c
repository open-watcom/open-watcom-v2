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
* Description:  Instruction decoding for Alpha AXP architecture.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "distypes.h"
#include "dis.h"

extern long SEX( unsigned long v, unsigned bit );

extern const dis_range          AXPRangeTable[];
extern const int                AXPRangeTablePos[];
extern const unsigned char      AXPMaxInsName;

typedef union {
    unsigned_32 full;
    struct {
        unsigned_32 code        : 26;
        unsigned_32 opcode      : 6;
    }           pal;
    struct {
        unsigned_32 disp        : 21;
        unsigned_32 ra          : 5;
        unsigned_32 opcode      : 6;
    }           branch;
    struct {
        unsigned_32 disp        : 16;
        unsigned_32 rb          : 5;
        unsigned_32 ra          : 5;
        unsigned_32 opcode      : 6;
    }           memory;
    struct {
        unsigned_32 rc          : 5;
        unsigned_32 fnc         : 4;
        unsigned_32 src         : 2;
        unsigned_32 rnd         : 2;
        unsigned_32 trp         : 3;
        unsigned_32 rb          : 5;
        unsigned_32 ra          : 5;
        unsigned_32 opcode      : 6;
    }           fp_operate;
    struct {
        unsigned_32 rc          : 5;
        unsigned_32 func        : 7;
        unsigned_32 lit_flag    : 1;
        unsigned_32             : 3;
        unsigned_32 rb          : 5;
        unsigned_32 ra          : 5;
        unsigned_32 opcode      : 6;
    }           reg_operate;
    struct {
        unsigned_32 rc          : 5;
        unsigned_32 func        : 7;
        unsigned_32 lit_flag    : 1;
        unsigned_32 lit         : 8;
        unsigned_32 ra          : 5;
        unsigned_32 opcode      : 6;
    }           lit_operate;
} axp_ins;

dis_handler_return AXPPal( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = code.pal.code;
    ins->num_ops = 1;
    return( DHR_DONE );
}

dis_handler_return AXPMemory( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.memory.ra + DR_AXP_r0;
    ins->op[1].type = DO_MEMORY_ABS;
    ins->op[1].value = SEX( code.memory.disp, 15 );
    ins->op[1].base = code.memory.rb + DR_AXP_r0;
    ins->num_ops = 2;
    switch( code.memory.opcode & 0x0b ) {
    case 0x00:
        ins->op[1].ref_type = DRT_AXP_FFLOAT;
        break;
    case 0x01:
        ins->op[1].ref_type = DRT_AXP_GFLOAT;
        break;
    case 0x02:
        ins->op[1].ref_type = DRT_AXP_SFLOAT;
        break;
    case 0x03:
        ins->op[1].ref_type = DRT_AXP_TFLOAT;
        break;
    case 0x08:
    case 0x0a:
        ins->op[1].ref_type = DRT_AXP_LWORD;
        break;
    case 0x09:
    case 0x0b:
        ins->op[1].ref_type = DRT_AXP_QWORD;
        break;
    }
    return( DHR_DONE );
}

dis_handler_return AXPFPMemory( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPMemory( h, d, ins );
    ins->op[0].base += DR_AXP_f0 - DR_AXP_r0;
    return( DHR_DONE );
}

dis_handler_return AXPMemoryLA( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPMemory( h, d, ins );
    ins->op[1].type = DO_ABSOLUTE;
    ins->op[1].ref_type = DR_NONE;
    return( DHR_DONE );
}

dis_handler_return AXPMemoryFC( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    switch( ins->type ) {
    case DI_AXP_FETCH:
    case DI_AXP_FETCH_M:
        ins->op[0].type = DO_MEMORY_ABS;
        ins->op[0].base = code.memory.rb + DR_AXP_r0;
        ins->num_ops = 1;
        break;
    case DI_AXP_RPCC:
        ins->op[0].type = DO_REG;
        ins->op[0].base = code.memory.rb + DR_AXP_r0;
        ins->num_ops = 1;
    default:
        break;
    }
    return( DHR_DONE );
}

dis_handler_return AXPJump( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.memory.ra + DR_AXP_r0;
    ins->op[1].type = DO_ABSOLUTE;
    ins->op[1].base = code.memory.rb + DR_AXP_r0;
    ins->op[1].value = 0;
    ins->op[2].type = DO_IMMED;
    ins->op[2].value = code.memory.disp & 0x3fff;
    ins->num_ops = 3;
    return( DHR_DONE );
}

dis_handler_return AXPBranch( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.branch.ra + DR_AXP_r0;
    ins->op[1].type = DO_RELATIVE;
    ins->op[1].value = (SEX( code.branch.disp, 20 ) + 1) * sizeof( unsigned_32 );
    ins->num_ops = 2;
    return( DHR_DONE );
}

dis_handler_return AXPFPBranch( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPBranch( h, d, ins );
    ins->op[0].base += DR_AXP_f0 - DR_AXP_r0;
    return( DHR_DONE );
}

dis_handler_return AXPOperate( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.reg_operate.ra + DR_AXP_r0;
    if( code.reg_operate.lit_flag ) {
        ins->op[1].type = DO_IMMED;
        ins->op[1].value = code.lit_operate.lit;
    } else {
        ins->op[1].type = DO_REG;
        ins->op[1].base = code.reg_operate.rb + DR_AXP_r0;
    }
    ins->op[2].type = DO_REG;
    ins->op[2].base = code.reg_operate.rc + DR_AXP_r0;
    ins->num_ops = 3;
    return( DHR_DONE );
}

dis_handler_return AXPOperateV( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPOperate( h, d, ins );
    if( ins->opcode & (1 << 11) ) {
        ins->flags.u.axp |= DIF_AXP_V;
    }
    return( DHR_DONE );
}

dis_handler_return AXPFPOperate( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.fp_operate.ra + DR_AXP_f0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.fp_operate.rb + DR_AXP_f0;
    ins->op[2].type = DO_REG;
    ins->op[2].base = code.fp_operate.rc + DR_AXP_f0;
    ins->num_ops = 3;
    return( DHR_DONE );
}

dis_handler_return AXPFPConvert( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.fp_operate.rb + DR_AXP_f0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.fp_operate.rc + DR_AXP_f0;
    if( ins->opcode & (1 << 13) ) {
        ins->flags.u.axp |= DIF_AXP_V;
    }
    if( ins->opcode & (1 << 14) ) {
        ins->flags.u.axp |= DIF_AXP_S;
    }
    ins->num_ops = 2;
    return( DHR_DONE );
}

static dis_handler_return SetIEEEFlags( dis_dec_ins *ins )
{
    axp_ins     code;

    code.full = ins->opcode;
    switch( code.fp_operate.trp ) {
    case 0x2:
    case 0x3:
    case 0x6:
        return( DHR_INVALID );
    }
    if( code.fp_operate.trp & 0x1 ) {
        if( code.fp_operate.fnc == 0xf ) {
            ins->flags.u.axp |= DIF_AXP_V;
        } else {
            ins->flags.u.axp |= DIF_AXP_U;
        }
    }
    if( code.fp_operate.trp & 0x2 ) {
        ins->flags.u.axp |= DIF_AXP_I;
    }
    if( code.fp_operate.trp & 0x4 ) {
        ins->flags.u.axp |= DIF_AXP_S;
    }
    switch( code.fp_operate.rnd ) {
    case 0x0:
        ins->flags.u.axp |= DIF_AXP_C;
        break;
    case 0x1:
        ins->flags.u.axp |= DIF_AXP_M;
        break;
    case 0x2:
        break;
    case 0x3:
        ins->flags.u.axp |= DIF_AXP_D;
        break;
    }
    return( DHR_DONE );
}

dis_handler_return AXPIEEEOperate( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPFPOperate( h, d, ins );
    return( SetIEEEFlags( ins ) );
}

dis_handler_return AXPIEEECompare( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPIEEEOperate( h, d, ins );
    //NYI illegal opcodes
    return( DHR_DONE );
}

dis_handler_return AXPIEEEConvert( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->num_ops = 2;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.fp_operate.rb + DR_AXP_f0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.fp_operate.rc + DR_AXP_f0;
    return( SetIEEEFlags( ins ) );
}

dis_handler_return AXPVAXOperate( dis_handle *h, void *d, dis_dec_ins *ins )
{
    AXPFPOperate( h, d, ins );
    if( !(ins->opcode & (1UL << (7+5))) ) ins->flags.u.axp |= DIF_AXP_C;
    if(   ins->opcode & (1UL << (8+5))  ) ins->flags.u.axp |= DIF_AXP_U;
    if(   ins->opcode & (1UL << (10+5)) ) ins->flags.u.axp |= DIF_AXP_S;
    return( DHR_DONE );
}

dis_handler_return AXPVAXConvert( dis_handle *h, void *d, dis_dec_ins *ins )
{
    axp_ins     code;

    h = h; d = d;
    code.full = ins->opcode;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.fp_operate.rb + DR_AXP_f0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.fp_operate.rc + DR_AXP_f0;
    if( !(ins->opcode & (1UL << (7+5))) ) ins->flags.u.axp |= DIF_AXP_C;
    if(   ins->opcode & (1UL << (8+5))  ) ins->flags.u.axp |= DIF_AXP_V;
    if(   ins->opcode & (1UL << (10+5)) ) ins->flags.u.axp |= DIF_AXP_S;
    return( DHR_DONE );
}

static size_t AXPInsHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    const char  *new;

    h = h; d = d;
    if( !(flags & DFF_PSEUDO) ) 
        return( 0 );
    new = NULL;
    switch( ins->type ) {
    case DI_AXP_ADDL:
        if( ins->op[0].base == DR_AXP_r31 ) {
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
            new = "sextl";
            ins->num_ops = 2;
        } else if( ins->op[1].base == DR_AXP_r31 ) {
            ins->op[1] = ins->op[2];
            new = "sextl";
            ins->num_ops = 2;
        }
        break;
    case DI_AXP_BIS:
        if( ins->op[0].base == DR_AXP_r31 ) {
            if( ins->op[1].base != DR_AXP_r31 ) {
                new  = "mov";
                ins->num_ops = 2;
                ins->op[0] = ins->op[1];
                ins->op[1] = ins->op[2];
            } else if( ins->op[2].base == DR_AXP_r31 ) {
                new = "nop";
                ins->num_ops = 0;
            } else {
                new = "clr";
                ins->op[0] = ins->op[2];
                ins->num_ops = 1;
            }
        } else if( ins->op[0].base == ins->op[1].base ) {
            new  = "mov";
            ins->num_ops = 2;
            ins->op[1] = ins->op[2];
        }
        break;
    case DI_AXP_BR:
        if( ins->op[0].base == DR_AXP_r31 ) {
            ins->op[0] = ins->op[1];
            ins->num_ops = 1;
        }
        break;
    case DI_AXP_CPYS:
        if( ins->op[0].base == DR_AXP_f31 ) {
            if( ins->op[1].base != DR_AXP_f31 ) {
                new  = "fabs";
                ins->num_ops = 2;
                ins->op[0] = ins->op[1];
                ins->op[1] = ins->op[2];
            } else if( ins->op[2].base == DR_AXP_f31 ) {
                new = "fnop";
                ins->num_ops = 0;
            } else {
                new = "fclr";
                ins->op[0] = ins->op[2];
                ins->num_ops = 1;
            }
        } else if( ins->op[0].base == ins->op[1].base ) {
            new  = "fmov";
            ins->num_ops = 2;
            ins->op[1] = ins->op[2];
        }
        break;
    case DI_AXP_CPYSN:
        if( ins->op[0].base == ins->op[1].base ) {
            new = "fneg";
            ins->op[1] = ins->op[2];
            ins->num_ops = 2;
        }
        break;
    case DI_AXP_LDA:
        if( ins->op[1].base == DR_AXP_r31 ) {
            unsigned long       val;

            new = "mov";
            val = ins->op[1].value;
            ins->op[1] = ins->op[0];
            ins->op[0].value = val;
            ins->op[0].type = DO_IMMED;
        }
        break;
    case DI_AXP_MF_FPCR:
    case DI_AXP_MT_FPCR:
        if( ins->op[0].base == ins->op[1].base &&
            ins->op[0].base == ins->op[2].base ) {
            ins->num_ops = 1;
        }
        break;
    case DI_AXP_ORNOT:
        if( ins->op[0].base == DR_AXP_r31 ) {
            new = "not";
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
            ins->num_ops = 2;
        }
        break;
    case DI_AXP_RET:
        if( ins->op[0].base == DR_AXP_r31 ) {
            if( !( flags & DFF_ASM ) ) {
                ins->op[0] = ins->op[1];
                ins->num_ops = 1;
            }
        }
        break;
    case DI_AXP_SUBL:
        if( ins->op[0].base == DR_AXP_r31 ) {
            new = "negl";
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
            ins->num_ops = 2;
        }
        break;
    case DI_AXP_SUBQ:
        if( ins->op[0].value == 31 ) {
            new = "negq";
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
            ins->num_ops = 2;
        }
        break;
    case DI_AXP_SUBS:
        if( ins->op[0].base == DR_AXP_f31 ) {
            if( (ins->flags.u.axp == DIF_AXP_NONE) ||
                (ins->flags.u.axp == (DIF_AXP_S | DIF_AXP_U) ) ||
                (ins->flags.u.axp == (DIF_AXP_S | DIF_AXP_U | DIF_AXP_I ) ) ) {
                new = "negs";
                ins->op[0] = ins->op[1];
                ins->op[1] = ins->op[2];
                ins->num_ops = 2;
            }
        }
        break;
    case DI_AXP_SUBT:
        if( ins->op[0].base == DR_AXP_f31 ) {
            if( (ins->flags.u.axp == DIF_AXP_NONE) ||
                (ins->flags.u.axp == (DIF_AXP_S | DIF_AXP_U) ) ||
                (ins->flags.u.axp == (DIF_AXP_S | DIF_AXP_U | DIF_AXP_I ) ) ) {
                new = "negt";
                ins->op[0] = ins->op[1];
                ins->op[1] = ins->op[2];
                ins->num_ops = 2;
            }
        }
        break;
    default:
        break;
    }
    if( name != NULL && new != NULL ) {
        strcpy( name, new );
        return( strlen( name ) );
    }
    return( 0 );
}

static size_t AXPFlagHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    char        *p;

    h = h; d = d; flags = flags;
    p = name;
    if( ins->flags.u.axp != DIF_AXP_NONE ) {
        *p++ = '/';
        if( ins->flags.u.axp & DIF_AXP_C ) *p++ = 'c';
        if( ins->flags.u.axp & DIF_AXP_D ) *p++ = 'd';
        if( ins->flags.u.axp & DIF_AXP_I ) *p++ = 'i';
        if( ins->flags.u.axp & DIF_AXP_M ) *p++ = 'm';
        if( ins->flags.u.axp & DIF_AXP_S ) *p++ = 's';
        if( ins->flags.u.axp & DIF_AXP_U ) *p++ = 'u';
        if( ins->flags.u.axp & DIF_AXP_V ) *p++ = 'v';
        *p = '\0';
    }
    return( p - name );
}

static size_t AXPOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff, unsigned buff_len )
{
    dis_operand *op;

    h = h; d = d; op_buff = op_buff; buff_len = buff_len;
    if( flags & DFF_SYMBOLIC_REG ) {
        op = &ins->op[op_num];
        if( op->base >= DR_AXP_r0 && op->base <= DR_AXP_r31 ) {
            op->base += DR_AXP_v0 - DR_AXP_r0;
        }
        if( op->index >= DR_AXP_r0 && op->index <= DR_AXP_r31 ) {
            op->index += DR_AXP_v0 - DR_AXP_r0;
        }
    }
    if( flags & DFF_ASM ) {
        op = &ins->op[op_num];
        if( op->base >= DR_AXP_f0 && op->base <= DR_AXP_zero ) {
            op->base += DR_AXP_af0 - DR_AXP_f0;
        }
        if( op->index >= DR_AXP_f0 && op->base <= DR_AXP_zero ) {
            op->index += DR_AXP_af0 - DR_AXP_f0;
        }
    }
    return( 0 );
}

static dis_handler_return AXPDecodeTableCheck( int page, dis_dec_ins *ins )
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

static void AXPPreprocHook( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ByteSwap( h, d, ins );
}

static size_t AXPPostOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff, unsigned buff_len )
{
    // Nothing to do
    h = h; d = d; ins = ins; flags = flags; op_num = op_num; op_buff = op_buff; buff_len = buff_len;
    return( 0 );
}

const dis_cpu_data AXPData = {
    AXPRangeTable, AXPRangeTablePos, AXPPreprocHook, AXPDecodeTableCheck, AXPInsHook, AXPFlagHook, AXPOpHook, AXPPostOpHook, &AXPMaxInsName, 4
};
