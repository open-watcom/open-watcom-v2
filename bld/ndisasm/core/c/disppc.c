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
#include <ctype.h>
#include "distypes.h"
#include "dis.h"

extern long SEX( unsigned long v, unsigned bit );

extern const dis_range          PPCRangeTable[];
extern const unsigned char      PPCMaxInsName;
extern const unsigned short     DisRegisterTable[];

#if DISCPU & DISCPU_ppc

#define MK_SPR(a,b) (((a)<<5)|(b))

typedef union {
    unsigned_32 full;
    struct {
        union {
            struct {
                unsigned_16 Rc          : 1;
                // reference types - only valid if memory = ?101
                unsigned_16 type2       : 2;
                unsigned_16 mem         : 4;
                unsigned_16 type1       : 3;
                // ---
                unsigned_16 OE          : 1;
                unsigned_16 rB          : 5;
            } math;
            struct {
                unsigned_16 Rc          : 1;
                unsigned_16 first       : 5;
                unsigned_16 second      : 5;
                unsigned_16 third       : 5;
            } general;
            unsigned_16 immediate;
            struct {
                unsigned_16 LK          : 1;
                unsigned_16 AA          : 1;
                unsigned_16 BD          : 14;
            } branch;
            struct {
                unsigned_16 zero        : 1;
                unsigned_16 subcode     : 10;
                unsigned_16 morezero    : 1;
                unsigned_16 IMM         : 4;
            } condition;
            struct {
                unsigned_16 Rc          : 1;
                unsigned_16 sh_5        : 1;
                unsigned_16 XO          : 9;
                unsigned_16 sh          : 5;
            } xs_form;
            struct {
                unsigned_16 Rc          : 1;
                unsigned_16 sh_5        : 1;
                unsigned_16 XO          : 3;
                unsigned_16 mb          : 6;
                unsigned_16 sh          : 5;
            } md_form;
            struct {
                unsigned_16 Rc          : 1;
                unsigned_16 XO          : 4;
                unsigned_16 mb          : 6;
                unsigned_16 first       : 5;
            } mds_form;
        } lo;

        union {
            struct {
                unsigned_16 first       : 5;
                unsigned_16 second      : 5;
                // reference types - only valid if memory=1
                unsigned_16 update      : 1;
                unsigned_16 type        : 3;
                unsigned_16 floating    : 1;
                unsigned_16 memory      : 1;
                // ---
            } general;
            struct {
                unsigned_16 zero        : 1;
                unsigned_16 FM          : 8;
                unsigned_16 morezero    : 1;
                unsigned_16 opcode      : 6;
            } math;
            struct {
                unsigned_16 rA          : 5;
                unsigned_16 L           : 1;
                unsigned_16 zero        : 1;
                unsigned_16 crfD        : 3;
                unsigned_16 opcode      : 6;
            } compare;
            struct {
                unsigned_16 zero        : 2;
                unsigned_16 crfS        : 3;
                unsigned_16 morezero    : 2;
                unsigned_16 crfD        : 3;
                unsigned_16 opcode      : 6;
            } condition;
        } hi;
    };
    struct {
        unsigned_32 LK                  : 1;
        unsigned_32 AA                  : 1;
        unsigned_32 LI                  : 24;
        unsigned_32 opcode              : 6;
    } b;
    struct {
        unsigned_32 zero                : 1;
        unsigned_32 subcode             : 10;
        unsigned_32 morezero            : 1;
        unsigned_32 CRM                 : 8;
        unsigned_32 lastzero            : 1;
        unsigned_32 rD                  : 5;
        unsigned_32 opcode              : 6;
    } CRM;
} ppc_ins;
#pragma pack(pop);

dis_handler_return PPCMath( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 3;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[2].type = DO_REG;
    ins->op[2].base = code.lo.math.rB + DR_PPC_r0;

    if( code.lo.math.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    if( code.lo.math.OE ) {
        ins->flags |= DIF_PPC_OE;
    }
    return( DHR_DONE );
}
dis_handler_return PPCMathb( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMath( h, d, ins );

    ins->num_ops = 1;
    ins->op[0] = ins->op[2];

    return( DHR_DONE );
}
dis_handler_return PPCMathd( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMath( h, d, ins );

    ins->num_ops = 1;

    return( DHR_DONE );
}
dis_handler_return PPCMathab( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMath( h, d, ins );

    ins->num_ops = 2;
    ins->op[0] = ins->op[1];
    ins->op[1] = ins->op[2];

    ins->flags &= ~DIF_PPC_OE;
    return( DHR_DONE );
}
dis_handler_return PPCMathda( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMath( h, d, ins );

    ins->num_ops = 2;

    if( ins->type == DI_PPC_clcs ) {
        ins->flags &= ~DIF_PPC_OE;
    }

    return( DHR_DONE );
}
dis_handler_return PPCMathdb( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMath( h, d, ins );

    ins->num_ops = 2;
    ins->op[1] = ins->op[2];

    ins->flags &= ~DIF_PPC_OE;
    return( DHR_DONE );
}
dis_handler_return PPCMath2( dis_handle *h, void *d, dis_dec_ins *ins )
// messy parameter ordering
{
    dis_operand temp;

    PPCMath( h, d, ins );

    temp = ins->op[0];
    ins->op[0] = ins->op[1];
    ins->op[1] = temp;
    ins->flags &= ~DIF_PPC_OE;
    return( DHR_DONE );
}
dis_handler_return PPCMathsa( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMath2( h, d, ins );
    ins->num_ops = 2;
    return( DHR_DONE );
}
dis_handler_return PPCImmediate( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 3;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[2].type = DO_IMMED;
    switch( ins->type ) {
    case DI_PPC_andi_dot:
    case DI_PPC_andis_dot:
    case DI_PPC_ori:
    case DI_PPC_oris:
    case DI_PPC_xori:
    case DI_PPC_xoris: // These take unsigned values.
        ins->op[2].value = code.lo.immediate;
        break;
    default:
        ins->op[2].value = SEX(code.lo.immediate, 15 );
        break;
    }
    return( DHR_DONE );
}
dis_handler_return PPCImmed2( dis_handle *h, void *d, dis_dec_ins *ins )
// messy parameter ordering
{
    dis_operand temp;

    PPCImmediate( h, d, ins );

    temp = ins->op[0];
    ins->op[0] = ins->op[1];
    ins->op[1] = temp;

    return( DHR_DONE );
}
static void PPCDoFloat( dis_dec_ins *ins, const int *order )
// extract float operands.  order is null terminated array of integers as
// follows:  (instruction format as numbered in the book)
// 0              15              31
// +-----+----+----+----+----+----++
// |     | 1  | 2  | 3  | 4  |    ||
// +-----+----+----+----+----+----++
{
    ppc_ins     code;
    int         operand;

    code.full = ins->opcode;

    for( operand=0; operand<4; operand++ ) {
        if( order[operand] == 0 ) break;
        ins->op[operand].type = DO_REG;
        switch( order[operand] ) {
        case 1:
            ins->op[operand].base = code.hi.general.second + DR_PPC_f0;
            break;
        case 2:
            ins->op[operand].base = code.hi.general.first + DR_PPC_f0;
            break;
        case 3:
            ins->op[operand].base = code.lo.general.third + DR_PPC_f0;
            break;
        case 4:
            ins->op[operand].base = code.lo.general.second + DR_PPC_f0;
            break;
        }
    }
    ins->num_ops = operand;
    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
}
dis_handler_return PPCFloat( dis_handle *h, void *d, dis_dec_ins *ins )
{
    // note silly ordering of operands
    const int order[5] = {1, 2, 4, 3, 0};

    PPCDoFloat( ins, order );

    return( DHR_DONE );
}
dis_handler_return PPCFloatdab( dis_handle *h, void *d, dis_dec_ins *ins )
{
    const int order[4] = {1, 2, 3, 0};

    PPCDoFloat( ins, order );

    return( DHR_DONE );
}
dis_handler_return PPCFloatdac( dis_handle *h, void *d, dis_dec_ins *ins )
{
    const int order[4] = {1, 2, 4, 0};

    PPCDoFloat( ins, order );

    return( DHR_DONE );
}
dis_handler_return PPCFloatCmpab( dis_handle *h, void *d, dis_dec_ins *ins )
{
    const int order[4] = {1, 2, 3, 0};
    ppc_ins     code;

    PPCDoFloat( ins, order );

    code.full = ins->opcode;

    ins->op[0].base = code.hi.compare.crfD + DR_PPC_cr0;

    return( DHR_DONE );
}
dis_handler_return PPCFloatdb( dis_handle *h, void *d, dis_dec_ins *ins )
{
    const int order[3] = {1, 3, 0};

    PPCDoFloat( ins, order );

    return( DHR_DONE );
}
dis_handler_return PPCFloato( dis_handle *h, void *d, dis_dec_ins *ins )
// Two obscure float instructions.
{
    ppc_ins     code;

    code.full = ins->opcode;

    PPCFloatdb( h, d, ins );

    switch( ins->type ) {
    case DI_PPC_mffs:
        ins->num_ops = 1;
        break;
    case DI_PPC_mtfsf:
        ins->op[0].type = DO_IMMED;
        ins->op[0].base = DR_NONE;
        ins->op[0].value = code.hi.math.FM;
        break;
    }
    return( DHR_DONE );
}
dis_handler_return PPCMem1( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 2;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_MEMORY_ABS;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[1].value = SEX( code.lo.immediate, 15 );

    if( code.hi.general.floating ) {
        if( code.hi.general.type & 0x2 ) {
            ins->op[1].ref_type = DRT_PPC_SFLOAT;
        } else {
            ins->op[1].ref_type = DRT_PPC_DFLOAT;
        }
    } else {
        switch( code.hi.general.type ) {
        case 0x1:
        case 0x3:
            ins->op[1].ref_type = DRT_PPC_BYTE;
            break;
        case 0x4:
        case 0x5:
        case 0x6:
            ins->op[1].ref_type = DRT_PPC_HWORD;
            break;
        case 0x7:
            ins->op[1].ref_type = DRT_PPC_MWORD;
            break;
        case 0x0:
        case 0x2:
            ins->op[1].ref_type = DRT_PPC_WORD;
            break;
        }
    }
    return( DHR_DONE );
}
dis_handler_return PPCMemD1( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 2;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_MEMORY_ABS;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[1].value = SEX( code.lo.immediate & ~0x3, 15 );
    ins->op[1].ref_type = DRT_PPC_DWORD;
    return( DHR_DONE );
}
dis_handler_return PPCMem2( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 3;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[2].type = DO_REG;
    ins->op[2].base = code.lo.math.rB + DR_PPC_r0;
    ins->op[1].extra |= PE_XFORM;

    switch( code.lo.math.OE ) {
    case 0:
        switch( code.lo.math.type1 & 5 ) {
        case 0:
            ins->op[1].ref_type = DRT_PPC_WORD;
            break;
        case 1:
            ins->op[1].ref_type = DRT_PPC_BYTE;
            break;
        case 4:
            if( code.lo.math.type2 == 1 ) {
                ins->op[1].ref_type = DRT_PPC_SWORD;
                break;
            } // fall through
        case 5:
            ins->op[1].ref_type = DRT_PPC_HWORD;
            break;
        }
        break;
    case 1:
        switch( code.lo.math.type2 ) {
        case 1:
            ins->op[1].ref_type = DRT_PPC_SWORD;
            break;
        case 2:
            if( code.lo.math.type1 & (1<<2) ) {
                ins->op[1].ref_type = DRT_PPC_BRHWORD;
            } else {
                ins->op[1].ref_type = DRT_PPC_BRWORD;
            }
            break;
        case 3:
            if( code.lo.math.type1 & 1 ) {
                ins->op[1].ref_type = DRT_PPC_DFLOAT;
            } else {
                ins->op[1].ref_type = DRT_PPC_SFLOAT;
            }
            break;
        }
        break;
    }
    if( code.lo.math.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }

    return( DHR_DONE );
}
dis_handler_return PPCMemD2( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 3;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[2].type = DO_REG;
    ins->op[2].base = code.lo.math.rB + DR_PPC_r0;
    ins->op[1].extra |= PE_XFORM;
    ins->op[1].ref_type = DRT_PPC_DWORD;
    return( DHR_DONE );
}
dis_handler_return PPCMem3( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 3;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_r0;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;
    ins->op[2].type = DO_IMMED;
    ins->op[2].value = code.lo.general.third;

    ins->op[1].ref_type = DRT_PPC_SWORD;

    return( DHR_DONE );
}
dis_handler_return PPCFloatMem1( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMem1( h, d, ins );
    ins->op[0].base += DR_PPC_f0 - DR_PPC_r0;

    return( DHR_DONE );
}
dis_handler_return PPCFloatMem2( dis_handle *h, void *d, dis_dec_ins *ins )
{
    PPCMem2( h, d, ins );
    ins->op[0].base += DR_PPC_f0 - DR_PPC_r0;

    return( DHR_DONE );
}
dis_handler_return PPCBranch( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;
    int         magic;

    code.full = ins->opcode;

    if( code.lo.branch.LK ) {
        ins->flags |= DIF_PPC_LK;
    }
    magic = 2;
    switch( ins->type ) {
    case DI_PPC_b:
        magic = 0; // fall through
    case DI_PPC_bc:
        magic++; // magic=1 for b, 3 for bc
        if( code.b.AA ) {
            ins->flags |= DIF_PPC_AA;
            ins->op[magic-1].type = DO_ABSOLUTE;
        } else {
            ins->op[magic-1].type = DO_RELATIVE;
        }
        // fall through
    case DI_PPC_bcctr:
    case DI_PPC_bclr:
        ins->num_ops = magic; // magic=2 for bcctr, bclr
        break;
    }
    switch( ins->type ) {
    case DI_PPC_b:
        ins->op[0].value = SEX( code.b.LI, 23 ) << 2;
        break;
    case DI_PPC_bc:
        ins->op[2].value = SEX( code.lo.branch.BD, 13 ) << 2;
        ins->op[2].op_position = 0;
        // fall through
    case DI_PPC_bcctr:
    case DI_PPC_bclr:
        ins->op[0].type = DO_IMMED;
        ins->op[0].value = code.hi.general.second;
        ins->op[0].op_position = 3;
        ins->op[1].type = DO_IMMED;
        ins->op[1].value = code.hi.general.first;
        ins->op[1].op_position = 2;
        break;
    }
    return( DHR_DONE );
}
dis_handler_return PPCCompare( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 4;
    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.compare.crfD + DR_PPC_cr0;
    ins->op[1].type = DO_IMMED;
    ins->op[1].value = code.hi.compare.L;
    ins->op[2].type = DO_REG;
    ins->op[2].base = code.hi.compare.rA + DR_PPC_r0;

    switch( ins->type ) {
    case DI_PPC_cmp:
    case DI_PPC_cmpl:
        ins->op[3].type = DO_REG;
        ins->op[3].base = code.lo.math.rB + DR_PPC_r0;
        break;
    case DI_PPC_cmpi:
        ins->op[3].type = DO_IMMED;
        ins->op[3].value = SEX( code.lo.immediate, 15 );
        break;
    case DI_PPC_cmpli:
        ins->op[3].type = DO_IMMED;
        ins->op[3].value = code.lo.immediate;
    }
    return( DHR_DONE );
}
dis_handler_return PPCCondition( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.general.second + DR_PPC_crb0;

    switch( ins->type ) {
    case DI_PPC_mtfsb0:
    case DI_PPC_mtfsb1:
        ins->num_ops = 1;
        break;
    default:
        ins->num_ops = 3;
        ins->op[1].type = DO_REG;
        ins->op[1].base = code.hi.general.first + DR_PPC_crb0;
        ins->op[2].type = DO_REG;
        ins->op[2].base = code.lo.math.rB + DR_PPC_crb0;
        break;
    }

    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    return( DHR_DONE );
}
dis_handler_return PPCConditionField( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->op[0].type = DO_REG;
    ins->op[0].base = code.hi.condition.crfD + DR_PPC_cr0;

    switch( ins->type ) {
    case DI_PPC_mcrf:
    case DI_PPC_mcrfs:
        ins->num_ops = 2;
        ins->op[1].type = DO_REG;
        ins->op[1].base = code.hi.condition.crfS + DR_PPC_cr0;
        break;
    case DI_PPC_mcrxr:
        ins->num_ops = 1;
        break;
    case DI_PPC_mtfsfi:
        ins->num_ops = 2;
        ins->op[1].type = DO_IMMED;
        ins->op[1].value = code.lo.condition.IMM;
        break;
    }

    return( DHR_DONE );
}
dis_handler_return PPCSpecial( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;
    int         magic;

    code.full = ins->opcode;

    ins->num_ops = 2;
    switch( ins->type ) {
    case DI_PPC_mfspr:
    case DI_PPC_mfsr:
        magic = 0;
        break;
    case DI_PPC_mtspr:
    case DI_PPC_mtsr:
    case DI_PPC_mtcrf:
        magic = 1;
        break;
    }
    ins->op[magic].type = DO_REG;
    ins->op[magic].base = code.hi.general.second + DR_PPC_r0;
    magic = 1-magic;
    ins->op[magic].type = DO_IMMED;
    switch( ins->type ) {
    case DI_PPC_mfspr:
    case DI_PPC_mtspr:
        ins->op[magic].value = MK_SPR( code.lo.general.third, code.hi.general.first );
        break;
    case DI_PPC_mfsr:
    case DI_PPC_mtsr:
        ins->op[magic].value = code.hi.general.first;
        break;
    case DI_PPC_mtcrf:
        ins->op[magic].value = code.CRM.CRM;
        break;
    }

    return( DHR_DONE );
}
dis_handler_return PPCShiftImmed( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    PPCImmed2( h, d, ins );
    code.full = ins->opcode;

    ins->op[2].value = code.lo.general.third;

    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    return( DHR_DONE );
}
dis_handler_return PPCShiftImmedD( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    PPCImmed2( h, d, ins );
    code.full = ins->opcode;

    ins->op[2].value = code.lo.xs_form.sh | (code.lo.xs_form.sh_5 << 5);

    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    return( DHR_DONE );
}
dis_handler_return PPCRotate( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    PPCMath2( h, d, ins );
    code.full = ins->opcode;

    ins->num_ops = 5;

    switch( ins->type ) {
    case DI_PPC_rlwimi:
    case DI_PPC_rlwinm:
        ins->op[2].type = DO_IMMED;
        ins->op[2].base = DR_NONE;
        ins->op[2].value = code.lo.general.third;
        break;
    }

    ins->op[3].type = DO_IMMED;
    ins->op[3].value = code.lo.general.second;
    ins->op[4].type = DO_IMMED;
    ins->op[4].value = code.lo.general.first;

    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    return( DHR_DONE );
}
dis_handler_return PPCRotateD( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    PPCMath2( h, d, ins );
    code.full = ins->opcode;

    ins->num_ops = 4;

    ins->op[3].type = DO_IMMED;
    ins->op[3].value = code.lo.mds_form.mb;

    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    return( DHR_DONE );
}
dis_handler_return PPCRotateImmD( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    PPCMath2( h, d, ins );
    code.full = ins->opcode;

    ins->num_ops = 4;

    ins->op[2].type = DO_IMMED;
    ins->op[2].base = DR_NONE;
    ins->op[2].value = code.lo.md_form.sh | (code.lo.md_form.sh_5 << 5);

    ins->op[3].type = DO_IMMED;
    ins->op[3].value = code.lo.mds_form.mb;

    if( code.lo.general.Rc ) {
        ins->flags |= DIF_PPC_RC;
    }
    return( DHR_DONE );
}
dis_handler_return PPCTrap( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ppc_ins     code;

    code.full = ins->opcode;

    ins->num_ops = 3;
    ins->op[0].type = DO_IMMED;
    ins->op[0].value = code.hi.general.second;
    ins->op[0].op_position = 3;
    ins->op[1].type = DO_REG;
    ins->op[1].base = code.hi.general.first + DR_PPC_r0;

    switch( ins->type ) {
    case DI_PPC_tw:
    case DI_PPC_td:
        ins->op[2].type = DO_REG;
        ins->op[2].base = code.lo.math.rB + DR_PPC_r0;
        break;
    case DI_PPC_twi:
    case DI_PPC_tdi:
        ins->op[2].type = DO_IMMED;
        ins->op[2].value = SEX( code.lo.immediate, 15 );
        break;
    }

    return( DHR_DONE );
}
dis_handler_return PPCNull( dis_handle *h, void *d, dis_dec_ins *ins )
{
    ins->num_ops = 0;
    return( DHR_DONE );
}

static unsigned PPCInsHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    const char *new;
    const char *more;

    if( !(flags & DFF_PSEUDO) ) return( 0 );
    new = NULL;
    more = NULL;
    switch( ins->type ) {
    // Comparison statements.
    case DI_PPC_cmpi:
        if( ins->op[1].value == 0 ) {
            ins->num_ops = 3;
            ins->op[1] = ins->op[2];
            ins->op[2] = ins->op[3];
            new = "cmpwi";
        }
        break;
    case DI_PPC_cmp:
        if( ins->op[1].value == 0 ) {
            ins->num_ops = 3;
            ins->op[1] = ins->op[2];
            ins->op[2] = ins->op[3];
            new = "cmpw";
        }
        break;
    case DI_PPC_cmpli:
        if( ins->op[1].value == 0 ) {
            ins->num_ops = 3;
            ins->op[1] = ins->op[2];
            ins->op[2] = ins->op[3];
            new = "cmplwi";
        }
        break;
    case DI_PPC_cmpl:
        if( ins->op[1].value == 0 ) {
            ins->num_ops = 3;
            ins->op[1] = ins->op[2];
            ins->op[2] = ins->op[3];
            new = "cmplw";
        }
        break;
    // Trap statements.
    //NYI: handle trap double extended stuff
    case DI_PPC_twi:
        ins->num_ops = 2;
        switch( ins->op[0].value ) {
        case 1:  new = "twlgti"; break;
        case 2:  new = "twllti"; break;
        case 4:  new = "tweqi";  break;
        case 5:  new = "twlgei"; break;
        case 6:  new = "twllei"; break;
        case 8:  new = "twgti";  break;
        case 12: new = "twgei";  break;
        case 16: new = "twlti";  break;
        case 20: new = "twlei";  break;
        case 24: new = "twnei";  break;
        default:
            ins->num_ops = 3;
        }
        if( ins->num_ops == 2 ) {
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
        }
        break;
    case DI_PPC_tw:
        ins->num_ops = 2;
        switch( ins->op[0].value ) {
        case 1:  new = "twlgt"; break;
        case 2:  new = "twllt"; break;
        case 4:  new = "tweq";  break;
        case 5:  new = "twlge";  break;
        case 6:  new = "twlle"; break;
        case 8:  new = "twgt";  break;
        case 12: new = "twge";  break;
        case 16: new = "twlt";  break;
        case 20: new = "twle";  break;
        case 24: new = "twne";  break;
        case 31: new = "trap";
            ins->num_ops = 0;
            break;
        default:
            ins->num_ops = 3;
        }
        if( ins->num_ops == 2 ) {
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
        }
        break;
    case DI_PPC_addi:
    // load 16-bit immediate value.
        if( ins->op[1].base == DR_PPC_r0 ) {
            new = "li";
            ins->num_ops = 2;
            ins->op[1] = ins->op[2];
            break;
        }
    // adding negative value.
        if( ins->op[2].value < 0 ) {
            new = "subi";
            ins->op[2].value = -ins->op[2].value;
        }
        break;
    // load 16-bit shifted immediate value.
    case DI_PPC_addis:
        if( ins->op[1].base == DR_PPC_r0 ) {
            new = "lis";
            ins->num_ops = 2;
            ins->op[1] = ins->op[2];
            break;
        }
        break;
    // Move register and compliment.
    case DI_PPC_or:
        if( ins->op[1].base == ins->op[2].base ) {
            new = "mr";
            ins->num_ops = 2;
        }
        break;
    case DI_PPC_nor:
        if( ins->op[1].base == ins->op[2].base ) {
            new = "not";
            ins->num_ops = 2;
        }
        break;
    // nop.
    case DI_PPC_ori:
        if( ins->opcode == 0x60000000) { /* ori r0, r0, 0 */
            new = "nop"; /* not no-op! */
            ins->num_ops = 0;
        }
        break;
    // SPR statements
    case DI_PPC_mtspr:
        ins->num_ops = 1;
    case DI_PPC_mfspr:
        ins->num_ops--; // ins->num_ops = 0 for mtspr, 1 for mfspr.
        switch( ins->op[ins->num_ops].value ) {
        case 1:
            more = "xer";
            break;
        case 8:
            more = "lr";
            break;
        case 9:
            more = "ctr";
            break;
        case 18:
            more = "dsisr";
            break;
        case 19:
            more = "dar";
            break;
        case 22:
            more = "dec";
            break;
        case 25:
            more = "sdr1";
            break;
        case 26:
            more = "srr0";
            break;
        case 27:
            more = "srr1";
            break;
        case 272:
        case 273:
        case 274:
        case 275:
            more = "sprg";
            ins->op[ins->num_ops].value -= 272;
            ins->num_ops += 3; // To be fixed below
            break;
        case 282:
            more = "ear";
            break;
        case 287:
            if( ins->type == DI_PPC_mfspr ) {
                more = "pvr";
            } else {
                ins->num_ops = 2;
            }
            break;
        case 528:
        case 530:
        case 532:
        case 534:
            more = "ibatu";
            ins->op[ins->num_ops].value = (ins->op[ins->num_ops].value - 528)/2;
            ins->num_ops += 3;
            break;
        case 529:
        case 531:
        case 533:
        case 535:
            more = "ibatl";
            ins->op[ins->num_ops].value = (ins->op[ins->num_ops].value - 529)/2;
            ins->num_ops += 3;
            break;
        default:
            ins->num_ops = 2;
        }
        switch( ins->num_ops ) {
        case 0:
            ins->op[0] = ins->op[1];
            ins->num_ops = 1;
            new = "mt";
            break;
        case 3:
            new = "mt";
            ins->num_ops = 2;
            break;
        case 4:
            ins->num_ops = 2;
        case 1:
            new = "mf";
            break;
        }
        break;
    // Branch statements
    case DI_PPC_bc:
        if( (ins->op[0].value & 0x14) == 0x14 ) {
            new = "b";
            ins->op[0] = ins->op[2];
            ins->num_ops = 1;
            break;
        }
    case DI_PPC_bclr:
        if( (ins->op[0].value & 0x14) == 0x14 ) {
            new = "blr";
            ins->num_ops = 0;
            break;
        }
        ins->num_ops = 2;
        switch( ins->op[0].value >> 1 ) {
        case 0:
            new = "bdnzf";
            break;
        case 1:
            new = "bdzf";
            break;
        case 4:
            new = "bdnzt";
            break;
        case 5:
            new = "bdzt";
            break;
        case 8:
            new = "bdnz";
            ins->num_ops = 1;
            ins->op[0] = ins->op[2];
            break;
        case 9:
            new = "bdz";
            ins->num_ops = 1;
            ins->op[0] = ins->op[2];
            break;
        default:
            ins->num_ops = 3;
        }
        if( ins->num_ops == 2 ) {
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
            ins->op[0].base = (ins->op[0].value / 4) + DR_PPC_cr0;
            ins->op[0].value %= 4;
        }
        if( ins->num_ops <= 2 ) {
            if( ins->type == DI_PPC_bclr ) {
                ins->num_ops--;
                more = "lr";
            }
            break;
        }
        // else, fall through
    case DI_PPC_bcctr:
        if( (ins->op[0].value & 0x14) == 0x14 ) {
            new = "bctr";
            ins->num_ops = 0;
            break;
        }
        if( (ins->op[0].value & 12) == 12 ) {
            ins->num_ops = 2;
            switch( ins->op[1].value % 4 ) {
            case 0:
                new = "blt";
                break;
            case 1:
                new = "bgt";
                break;
            case 2:
                new = "beq";
                break;
            case 3:
                new = "bso";
                break;
            }
        } else if( (ins->op[0].value & 4) == 4 ) {
            ins->num_ops = 2;
            switch( ins->op[1].value % 4 ) {
            case 0:
                new = "bge";
                break;
            case 1:
                new = "ble";
                break;
            case 2:
                new = "bne";
                break;
            case 3:
                new = "bns";
                break;
            }
        }
        if( ins->num_ops == 2 ) {
            ins->op[0] = ins->op[1];
            ins->op[1] = ins->op[2];
            ins->op[0].type = DO_REG;
            ins->op[0].base = (ins->op[0].value / 4) + DR_PPC_cr0;
            ins->op[0].value = 0;
            if( ins->op[0].base == DR_PPC_cr0 ) {
                ins->num_ops = 1;
                ins->op[0] = ins->op[1];
            }
            switch( ins->type ) {
            case DI_PPC_bclr:
                ins->num_ops--;
                more = "lr";
                break;
            case DI_PPC_bcctr:
                ins->num_ops--;
                more = "ctr";
                break;
            }
        }
        break;
    }
    if( name != NULL && new != NULL ) {
        strcpy( name, new );
        if( more != NULL ) {
            strcat( name, more );
        }
        return( strlen( name ) );
    }
    return 0;
}

static unsigned PPCFlagHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, char *name )
{
    char *p;

    p = name;
    if( ins->flags & DIF_PPC_OE ) *p++ = 'o';
    if( ins->flags & DIF_PPC_RC ) *p++ = '.';
    if( ins->flags & DIF_PPC_LK ) *p++ = 'l';
    if( ins->flags & DIF_PPC_AA ) *p++ = 'a';
    *p='\0';
    return( p-name );
}

static const char ConditionField[4][3] = {
    "lt", "gt", "eq", "so"
};

static unsigned PPCOpHook( dis_handle *h, void *d, dis_dec_ins *ins,
        dis_format_flags flags, unsigned op_num, char *op_buff )
{
    char        *p;
    int         val;
    char        ch;
    const char  *src;
    dis_operand *op;

    p = op_buff;
    switch( ins->op[op_num].type ) {
    // somewhat pointless to print out much more than this for
    // these cases.
    case DO_IMMED:
        switch( ins->type ) {
        case DI_PPC_cmp:
        case DI_PPC_cmpl:
        case DI_PPC_cmpi:
        case DI_PPC_cmpli:
            if( op_num == 1 ) {
                // The 1-bit L parameter.
                *p++ = ins->op[op_num].value + '0';
                *p='\0';
            }
            break;
        case DI_PPC_bc:
        case DI_PPC_bcctr:
        case DI_PPC_bclr:
            if( (flags & DFF_PSEUDO) && op_num == 0 ) {
                // pretty-printed BI parameter
                if( ins->op[op_num].base != DR_PPC_cr0 ) {
                    p = DisAddReg( ins->op[op_num].base, p, flags );
                    *p++ = '+';
                }
                src = ConditionField[ins->op[op_num].value % 4];
                for( ;; ) {
                    ch = *src;
                    *p = ch;
                    if( ch == '\0' ) break;
                    ++src;
                    ++p;
                }
                break;
            }
            // else, fall through
            // The 5-bit BO and BI parameters.
            //NYI: have to use client to get numeric prefix right
            *p++ = '0';
            *p++ = 'x';
            val = ins->op[op_num].value;
            *p++ = (val >> 4) + '0';
            if( (val & 0xf) > 9 ) {
                *p++ = (val & 0xf) + 'a' - 0xa;
            } else {
                *p++ = (val & 0xf) + '0';
            }
            *p='\0';
            break;
        }
    }
    if( flags & DFF_AXP_SYMBOLIC_REG ) {
        op = &ins->op[op_num];
        switch( op->base ) {
        case DR_PPC_r1:
            op->base = DR_PPC_sp;
            break;
        case DR_PPC_r2:
            op->base = DR_PPC_rtoc;
            break;
        }
    }
    return( p-op_buff );
}

const dis_cpu_data PPCData = {
    PPCRangeTable, PPCInsHook, PPCFlagHook, PPCOpHook, &PPCMaxInsName, 4
};
#else

const dis_cpu_data PPCData;
#endif
