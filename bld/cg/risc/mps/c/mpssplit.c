/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MIPS specific instruction reductions.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "makeins.h"
#include "data.h"
#include "rtrtn.h"
#include "namelist.h"
#include "rgtbl.h"
#include "split.h"
#include "insutil.h"
#include "liveinfo.h"
#include "opctable.h"
#include "rscsplit.h"
#include "optimize.h"
#include "optab.h"
#include "_split.h"
#include "_rscsplt.h"


instruction *rMOVEXX_8( instruction *ins )
/****************************************/
{
    name        *bit_mask;
    name        *temp;
    name        *temp_2;
    name        *src;
    name        *dst;
    type_length curr;           // which quad word we are currently on
    type_length size;           // size of the structure
    type_length quads;          // number of full quad-word writes to use
    type_length rem;            // number of bytes to write after last quad
    instruction *new_ins;
    instruction *first_ins;
    instruction *last_ins;

    /*
     * Bust up a MOVXX into a series of 8-byte moves - we are guaranteed
     * that both the source and dest are 8-byte aligned.
     */
    assert( ins->operands[0]->n.class == N_TEMP || ins->operands[0]->n.class == N_INDEXED );
    temp = AllocTemp( U8 );
    size = ins->operands[0]->n.size;
    first_ins = NULL;
    last_ins = NULL;
    curr = 0;
    quads = size / 8;
    rem = size % 8;
    if( rem ) {
        if( ins->result->n.class == N_TEMP ) {
            if( ( ins->result->t.temp_flags & ALIAS ) == EMPTY ) {
                // we have a write to a struct on the stack which is a master
                // since we don't 'pack' anything into the empty space after
                // this struct, we can safely overwrite it and not bother
                // doing the non-destructive last quad-word write
                quads += 1;
                rem = 0;
            }
        }
    }
    while( quads ) {
        src = OffsetMem( ins->operands[0], curr, U8 );
        dst = OffsetMem( ins->result, curr, U8 );
        curr += 8;
        quads -= 1;
        new_ins = MakeMove( src, temp, U8 );
        PrefixIns( ins, new_ins );
        if( first_ins == NULL ) {
            first_ins = new_ins;
        }
        new_ins = MakeMove( temp, dst, U8 );
        if( quads == 0 && rem == 0 ) {
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        } else {
            PrefixIns( ins, new_ins );
        }
    }
    if( rem != 0 ) {
        if( rem == 4 ) {
            src = OffsetMem( ins->operands[0], curr, U4 );
            dst = OffsetMem( ins->result, curr, U4 );
            temp_2 = AllocTemp( U4 );
            new_ins = MakeMove( src, temp_2, U4 );
            PrefixIns( ins, new_ins );
            if( first_ins == NULL ) {
                first_ins = new_ins;
            }
            new_ins = MakeMove( temp_2, dst, U4 );
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        } else {
            src = OffsetMem( ins->operands[0], curr, U8 );
            dst = OffsetMem( ins->result, curr, U8 );
            temp_2 = AllocTemp( U8 );
            bit_mask = AllocS32Const( ( 1 << rem ) - 1 );
            new_ins = MakeMove( src, temp, U8 );
            PrefixIns( ins, new_ins );
            if( first_ins == NULL ) {
                first_ins = new_ins;
            }
            new_ins = MakeMove( dst, temp_2, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeBinary( OP_ZAP_NOT, temp, bit_mask, temp, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeBinary( OP_ZAP, temp_2, bit_mask, temp_2, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeBinary( OP_OR, temp_2, temp, temp, U8 );
            PrefixIns( ins, new_ins );
            new_ins = MakeMove( temp, dst, U8 );
            ReplIns( ins, new_ins );
            last_ins = new_ins;
        }

    }
    UpdateLive( first_ins, last_ins );
    return( first_ins );
}


instruction *rCONSTLOAD( instruction *ins )
/*****************************************/
{
    unsigned_32         low;
    unsigned_32         high;
    unsigned_32         c;
    name                *high_part;
    name                *temp;
    instruction         *first_ins;
    instruction         *new_ins;
    type_class_def      type_class;

    assert( ins->operands[0]->n.class == N_CONSTANT );
    assert( ins->operands[0]->c.const_type == CONS_ABSOLUTE );

    type_class = ins->type_class;
    c = ins->operands[0]->c.lo.uint_value;
    high = ( c >> 16 ) & 0xffff;
    low  = c & 0xffff;
    high_part = AllocAddrConst( NULL, high, CONS_HIGH_ADDR, type_class );
    temp = AllocTemp( type_class );
    first_ins = MakeMove( high_part, temp, type_class );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_OR, temp, AllocS32Const( low ), ins->result, type_class );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}


instruction *rLOAD_4U( instruction *ins )
/***************************************/
{
    instruction         *first_ins;
    instruction         *new_ins;
    name                *mem_1;
    name                *mem_2;
    name                *temp;

    mem_1 = OffsetMem( ins->operands[0], 3, U4 );
    mem_2 = OffsetMem( ins->operands[0], 0, U4 );
    temp = AllocTemp( U4 );
    first_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_1, temp, U4 );
    PrefixIns( ins, first_ins );
    new_ins = MakeUnary( OP_LOAD_UNALIGNED, mem_2, temp, U4 );
    PrefixIns( ins, new_ins );
    new_ins = MakeMove( temp, ins->result, U4 );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}


static instruction *CheapCall( instruction *ins, rt_class rtindex, name *p1, name *p2 )
/*************************************************************************************/
{
    instruction         *call;
    hw_reg_set          reg;
    name                *reg_name;
    label_handle        lbl;

    reg = HW_EMPTY;
    lbl = RTLabel( rtindex );
    call = NewIns( 3 );
    call->head.opcode = OP_CALL;
    call->type_class = WD;
    call->operands[CALL_OP_USED] = p1;
    call->operands[CALL_OP_USED2] = p2;
    call->operands[CALL_OP_ADDR] = AllocMemory( lbl, 0, CG_LBL, WD );
    call->result = NULL;
    call->num_operands = 2;     /* special case for OP_CALL*/
    HW_TurnOn( reg, ReturnAddrReg() );
    HW_TurnOn( reg, ScratchReg() );
    // TODO: these regs are most likely wrong for MIPS
    HW_CTurnOn( reg, HW_R1 );   // know this is only other reg modified!
    HW_CTurnOn( reg, HW_R2 );   // and this one two!
    HW_CTurnOn( reg, HW_R3 );   // and this one three!
    reg_name = AllocRegName( reg );
    call->zap = &reg_name->r;
    PrefixIns( ins, call );
    return( call );
}


static void CopyStack( instruction *ins, name *alloc_size, type_length arg_size )
/*******************************************************************************/
{
    instruction         *new_ins;
    name                *p1;
    name                *p2;

    p1 = AllocRegName( HW_D1 );
    new_ins = MakeMove( alloc_size, p1, WD );
    PrefixIns( ins, new_ins );
    p2 = AllocRegName( HW_D2 );
    new_ins = MakeMove( AllocS32Const( arg_size ), p2, WD );
    PrefixIns( ins, new_ins );
    CheapCall( ins, RT_STK_COPY, p1, p2 );
}


instruction *rALLOCA( instruction *ins )
/**************************************/
{
    name                *sreg;
    name                *amount;
    name                *real_amount;
    name                *temp;
    unsigned_32         value;
    instruction         *first;
    instruction         *last;
    type_class_def      type_class;
    bool                check;

    sreg = AllocRegName( StackReg() );
    amount = ins->operands[0];
    temp = AllocTemp( ins->type_class );
    type_class = WD;
    check = true;
    CurrProc->targ.base_is_fp = true;
    if( amount->n.class == N_CONSTANT && amount->c.const_type == CONS_ABSOLUTE ) {
        value = amount->c.lo.uint_value;
        value = _RoundUp( value, STACK_ALIGNMENT );
        real_amount = AllocS32Const( value );
        first = MakeBinary( OP_SUB, sreg, AllocS32Const( value ), temp, type_class );
        PrefixIns( ins, first );
        if( value <= ( _TARGET_PAGE_SIZE - 7 ) ) {
            check = false;
        }
    } else {
        real_amount = AllocTemp( ins->type_class );
        first = MakeBinary( OP_ADD, amount, AllocS32Const( STACK_ALIGNMENT - 1 ), temp, type_class );
        PrefixIns( ins, first );
        last = MakeBinary( OP_AND, temp, AllocU32Const( ~(STACK_ALIGNMENT - 1) ), real_amount, type_class );
        PrefixIns( ins, last );
        last = MakeBinary( OP_SUB, sreg, real_amount, temp, type_class );
        PrefixIns( ins, last );
    }
    last = MakeMove( temp, sreg, WD );
    PrefixIns( ins, last );
    if( check ) {
        CheapCall( ins, RT_STK_CRAWL, AllocRegName( HW_EMPTY ), AllocRegName( HW_EMPTY ) );
    }
    if( MaxStack != 0 ) {
        if( _IsModel( MICROSOFT_COMPATIBLE ) ) {
            // in order to support doing alloca's in parm lists, we copy
            // the parm cache area down now
            CopyStack( ins, real_amount, MaxStack );

        }
        last = MakeBinary( OP_ADD, temp, AllocS32Const( MaxStack ), ins->result, type_class );
    } else {
        last = MakeMove( temp, ins->result, type_class );
    }
    ReplIns( ins, last );
    UpdateLive( first, last );
    return( first );
}


/* MIPS is a little weird - it only has 'set' instruction for
 * 'less than' (reg/reg and reg/imm variants). Conditional branch
 * instructions can't take an immediate operand (except $zero of course)
 * and there isn't a full set of reg/reg conditional branches. So we
 * have to carefully reduce the instructions here...
 * NB: This is a clone of rSIMPCMP from rscsplit.c
 */
instruction      *rM_SIMPCMP( instruction *ins )
/**********************************************/
{
    instruction         *new;
    opcode_defs         opcode;
    bool                reverse;

    reverse = false;
    opcode = 0;
    assert( ins->result == NULL );
    switch( ins->head.opcode ) {
    case OP_CMP_NOT_EQUAL:
        reverse = true;
        /* fall through */
    case OP_CMP_EQUAL:
        opcode = OP_SET_EQUAL;
        break;
    case OP_CMP_GREATER:
        reverse = true;
        /* fall through */
    case OP_CMP_LESS_EQUAL:
        opcode = OP_SET_LESS_EQUAL;
        /* Special reduction: use OP_SET_LESS but increment constant */
        if( (ins->operands[1]->n.class == N_CONSTANT)
            && (ins->operands[1]->c.const_type == CONS_ABSOLUTE) ) {
            signed_32           value;

            opcode = OP_SET_LESS;
            // TODO: we may be leaking memory here by losing track of the
            // original constant operand
            value = ins->operands[1]->c.lo.int_value;
            ins->operands[1] = AllocS32Const( value + 1 );
        }
        if( ins->operands[1]->n.class == N_REGISTER ) {
            // Swap operands and reverse condition - we can do slt/sltu but
            // nothing else
            return( rSWAPCMP( ins ) );
        }
        break;
    case OP_CMP_GREATER_EQUAL:
        reverse = true;
        /* fall through */
    case OP_CMP_LESS:
        opcode = OP_SET_LESS;
        break;
    }
    ins->result = AllocTemp( ins->type_class );
    switch( ins->type_class ) {
    case I4:
    case U4:
        ins->table = OpcodeTable( BIN4 );
        break;
    case I8:
    case U8:
        ins->table = OpcodeTable( BIN8 );
        break;
    case FS:
        ins->table = OpcodeTable( FBINS );
        break;
    case FD:
        ins->table = OpcodeTable( FBIND );
        break;
    case FL:
        ins->table = OpcodeTable( FBINL );
        break;
    default:
        _Zoiks( ZOIKS_096 );
    }
    ins->head.opcode = opcode;
    opcode = OP_CMP_NOT_EQUAL;
    if( reverse ) {
        opcode = OP_CMP_EQUAL;
    }
    new = MakeCondition( opcode, ins->result, AllocS32Const( 0 ), _TrueIndex( ins ), _FalseIndex( ins ), ins->type_class );
    SuffixIns( ins, new );
    return( new );
}

/* Note: This could be used for 128-bit types implemented on top of
 * 64-bit regs or anything along those lines.
 */
#define LONG_WORD           U8
#define HIGH_WORD( x )      ((x)->c.hi.uint_value)

/* NB: The following routines are clones of their Intel counterparts
 * with all segment related junk stripped off.
 */


static  void  CnvOpToInt( instruction * ins, opcnt op )
/*****************************************************/
{
    name        *name1;

    switch( ins->type_class ) {
#if 0
    case FS:
        name1 = ins->operands[op];
        if( name1->n.class == N_CONSTANT ) {
            ins->operands[op] = IntEquivalent( name1 );
        }
        break;
#endif
    // this is for the I8 stuff - can't tell what to do in
    // HighPart and LowPart if we don't get rid on constant
    // here
    case FD:
        name1 = ins->operands[op];
        if( name1->n.class == N_CONSTANT ) {
            ins->operands[op] = Int64Equivalent( name1 );
        }
        break;
    default:
        break;
    }
}


static bool IndexOverlaps( instruction *ins, opcnt i )
/****************************************************/
{
    if( ins->operands[i]->n.class != N_INDEXED )
        return( false );
    if( SameThing( ins->operands[i]->i.index, ins->result ) )
        return( true );
    return( false );
}

instruction     *rSPLITOP( instruction *ins )
/********************************************
* Split a multi-word operation instruction.
*/
{
    instruction *new_ins;
    instruction *ins2;
    name        *temp;

    if( IndexOverlaps( ins, 0 ) || IndexOverlaps( ins, 1 ) ) {
        temp = AllocTemp( LONG_WORD );
        HalfType( ins );
        new_ins = MakeBinary( ins->head.opcode,
                        LowPart( ins->operands[0], WD ),
                        LowPart( ins->operands[1], WD ),
                        LowPart( temp,             WD ),
                        WD );
        ins2 = MakeBinary( ins->head.opcode,
                        HighPart( ins->operands[0], WD ),
                        HighPart( ins->operands[1], WD ),
                        HighPart( temp,             WD ),
                        WD );
        if( ins->head.opcode == OP_ADD ) {
            ins2->head.opcode = OP_EXT_ADD;
        } else if( ins->head.opcode == OP_SUB ) {
            ins2->head.opcode = OP_EXT_SUB;
        }
        ins2->table = CodeTable( ins2 );
        new_ins->table = ins2->table;
        ins->operands[0] = temp;
        ins->operands[1] = temp;
        PrefixIns( ins, new_ins );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( LowPart( temp, WD ), LowPart( ins->result, WD ), WD );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( HighPart( temp, WD ),
                          HighPart( ins->result, WD ), WD );
        ReplIns( ins, ins2 );
    } else {
        HalfType( ins );
        new_ins = MakeBinary( ins->head.opcode,
                        LowPart( ins->operands[0], ins->type_class ),
                        LowPart( ins->operands[1], ins->type_class ),
                        LowPart( ins->result,      ins->type_class ),
                        ins->type_class );
        ins->operands[0] = HighPart( ins->operands[0], ins->type_class );
        ins->operands[1] = HighPart( ins->operands[1], ins->type_class );
        ins->result = HighPart( ins->result, ins->type_class );
        if( ins->head.opcode == OP_ADD ) {
            ins->head.opcode = OP_EXT_ADD;
        } else if( ins->head.opcode == OP_SUB ) {
            ins->head.opcode = OP_EXT_SUB;
        }
        ins->table = CodeTable( ins );
        new_ins->table = ins->table;

        PrefixIns( ins, new_ins );
    }
    new_ins->ins_flags |= INS_CC_USED;
    return( new_ins );
}


instruction *rSPLITMOVE( instruction *ins )
/******************************************
* Split a multi-word move instruction.
*/
{
    instruction     *new_ins;
    instruction     *ins2;
    name            *temp;

    CnvOpToInt( ins, 0 );
    if( IndexOverlaps( ins, 0 ) ) {
        temp = AllocTemp( LONG_WORD );
        new_ins = MakeMove( LowPart( ins->operands[0], WD ),
                             LowPart( temp, WD ), WD );
        ins2 = MakeMove( HighPart( ins->operands[0], WD ),
                             HighPart( temp, WD ), WD );
        ins->operands[0] = temp;
        PrefixIns( ins, new_ins );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( LowPart( temp, WD ), LowPart( ins->result, WD ), WD );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( HighPart( temp, WD ),
                          HighPart( ins->result, WD ), WD );
        ReplIns( ins, ins2 );
    } else {
        HalfType( ins );
        new_ins = MakeMove( LowPart( ins->operands[0], ins->type_class ),
                             LowPart( ins->result, ins->type_class ),
                             ins->type_class );
        ins->operands[0] = HighPart( ins->operands[0], ins->type_class );
        ins->result = HighPart( ins->result, ins->type_class );
        if( new_ins->result->n.class == N_REGISTER
         && ins->operands[0]->n.class == N_REGISTER
         && HW_Ovlap( new_ins->result->r.reg, ins->operands[0]->r.reg ) ) {
            SuffixIns( ins, new_ins );
            new_ins = ins;
        } else {
            PrefixIns( ins, new_ins );
        }
    }
    return( new_ins );
}


instruction     *rSPLITNEG( instruction *ins )
/*********************************************
* Split a multi-word negate instruction.
*/
{
    name            *hi_res;
    name            *lo_res;
    name            *hi_src;
    name            *lo_src;
    instruction     *hi_ins;
    instruction     *lo_ins;
    instruction     *subtract;

    HalfType( ins );
    hi_res = HighPart( ins->result, ins->type_class );
    hi_src = HighPart( ins->operands[0], ins->type_class );
    lo_res = LowPart( ins->result, ins->type_class );
    lo_src = LowPart( ins->operands[0], ins->type_class );
    hi_ins = MakeUnary( OP_NEGATE, hi_src, hi_res, ins->type_class );
    lo_ins = MakeUnary( OP_NEGATE, lo_src, lo_res, ins->type_class );
    lo_ins->ins_flags |= INS_CC_USED;
    subtract = MakeBinary( OP_EXT_SUB, hi_res, AllocIntConst( 0 ), hi_res,
                            ins->type_class );
    PrefixIns( ins, hi_ins );
    ins->operands[0] = ins->result;
    ins->operands[1] = AllocIntConst( 0 );
    PrefixIns( ins, lo_ins );
    ReplIns( ins, subtract );
    UpdateLive( hi_ins, subtract );
    return( hi_ins );
}


instruction      *rSPLITUNARY( instruction *ins )
/*******************************************************
* Split a multi-word unary operation. Only valid for ops
* which can be split into two independent operations on
* constituent types (e.g. bitwise complement).
*/
{
    instruction         *new_ins;
    name                *high_res;
    name                *low_res;

    CnvOpToInt( ins, 0 );
    HalfType( ins );
    if( ins->result == NULL ) {
        high_res = NULL;
        low_res = NULL;
    } else {
        high_res = HighPart( ins->result, ins->type_class );
        low_res  = LowPart( ins->result, ins->type_class );
    }
    new_ins = MakeUnary( ins->head.opcode,
                         LowPart( ins->operands[0], ins->type_class ),
                         low_res, ins->type_class );
    ins->operands[0] = HighPart( ins->operands[0],ins->type_class );
    ins->result = high_res;
    if( ins->head.opcode == OP_PUSH ) {
        SuffixIns( ins, new_ins );
        new_ins = ins;
    } else {
        PrefixIns( ins, new_ins );
    }
    return( new_ins );
}


instruction     *rSPLITCMP( instruction *ins )
/*********************************************
* Split a multi-word comparison instruction
*/
{
    name                *left;
    name                *right;
    instruction         *low = NULL;
    instruction         *high = NULL;
    instruction         *not_equal = NULL;
    type_class_def      high_type_class;
    type_class_def      low_type_class;
    byte                true_idx;
    byte                false_idx;

    high_type_class = HalfClass[ins->type_class];
    low_type_class  = Unsigned[high_type_class];
    left = ins->operands[0];
    right = ins->operands[1];
    true_idx = _TrueIndex( ins );
    false_idx = _FalseIndex( ins );
    switch( ins->head.opcode ) {
    case OP_BIT_TEST_TRUE:
        high = MakeCondition( ins->head.opcode,
                        HighPart( left, high_type_class ),
                        HighPart( right, high_type_class ),
                        true_idx, NO_JUMP,
                        WD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_type_class ),
                        LowPart( right, low_type_class ),
                        true_idx, false_idx,
                        WD );
        not_equal = NULL;
        break;
    case OP_BIT_TEST_FALSE:
        high = MakeCondition( OP_BIT_TEST_TRUE,
                        HighPart( left, high_type_class ),
                        HighPart( right, high_type_class ),
                        false_idx, NO_JUMP,
                        WD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_type_class ),
                        LowPart( right, low_type_class ),
                        true_idx, false_idx,
                        WD );
        not_equal = NULL;
        break;
    case OP_CMP_EQUAL:
        high = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_type_class ),
                        HighPart( right, high_type_class ),
                        false_idx, NO_JUMP,
                        WD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_type_class ),
                        LowPart( right, low_type_class ),
                        true_idx, false_idx,
                        WD );
        not_equal = NULL;
        break;
    case OP_CMP_NOT_EQUAL:
        high = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_type_class ),
                        HighPart( right, high_type_class ),
                        true_idx, NO_JUMP,
                        WD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_type_class ),
                        LowPart( right, low_type_class ),
                        true_idx, false_idx,
                        WD );
        not_equal = NULL;
        break;
    case OP_CMP_LESS:
    case OP_CMP_LESS_EQUAL:
        not_equal = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_type_class ),
                        HighPart( right, high_type_class ),
                        false_idx, NO_JUMP,
                        high_type_class );
        if( high_type_class == WD
         && right->n.class == N_CONSTANT
         && right->c.const_type == CONS_ABSOLUTE
         && HIGH_WORD( right ) == 0 ) {
            high = NULL;
        } else {
            high = MakeCondition( OP_CMP_LESS,
                        not_equal->operands[0], not_equal->operands[1],
                        true_idx, NO_JUMP,
                        high_type_class );
        }
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_type_class ),
                        LowPart( right, low_type_class ),
                        true_idx, false_idx,
                        low_type_class );
        break;
    case OP_CMP_GREATER_EQUAL:
    case OP_CMP_GREATER:
        not_equal = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_type_class ),
                        HighPart( right, high_type_class ),
                        false_idx, NO_JUMP,
                        high_type_class );
        if( high_type_class == WD
         && right->n.class == N_CONSTANT
         && right->c.const_type == CONS_ABSOLUTE
         && HIGH_WORD( right ) == 0 ) {
            _SetBlockIndex( not_equal, true_idx, NO_JUMP );
            high = NULL;
        } else {
            high = MakeCondition( OP_CMP_GREATER,
                        not_equal->operands[0], not_equal->operands[1],
                        true_idx, NO_JUMP,
                        high_type_class );
        }
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_type_class ),
                        LowPart( right, low_type_class ),
                        true_idx, false_idx,
                        low_type_class );
        break;
    default:
        break;
    }
    if( high != NULL ) {
        PrefixIns( ins, high );
    } else {
        high = not_equal;              /* for return value */
    }
    if( not_equal != NULL ) {
        PrefixIns( ins, not_equal );
    }
    ReplIns( ins, low );
    return( high );
}
