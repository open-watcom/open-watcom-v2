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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "zoiks.h"
#include "model.h"
#include "makeins.h"
#include "namelist.h"
#include "split.h"
#include "insutil.h"
#include "confldef.h"
#include "liveinfo.h"
#include "rscsplit.h"
#include "optimize.h"
#include "optab.h"
#include "_rscsplt.h"


instruction      *rCONSTLOAD( instruction *ins )
/**********************************************/
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
    c = ins->operands[0]->c.lo.int_value;
    high = ( c >> 16 ) & 0xffff;
    low = c & 0xffff;
    high_part = AllocAddrConst( NULL, high, CONS_HIGH_ADDR, type_class );
    temp = AllocTemp( type_class );
    first_ins = MakeMove( high_part, temp, type_class );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_OR, temp, AllocS32Const( low ), ins->result, type_class );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
}

instruction     *rMOD2DIV( instruction *ins )
/*******************************************/
{
    instruction         *first_ins;
    instruction         *new_ins;

    first_ins = MakeBinary( OP_DIV, ins->operands[0], ins->operands[1], ins->result, ins->type_class );
    PrefixIns( ins, first_ins );
    new_ins = MakeBinary( OP_MUL, ins->operands[1], ins->result, ins->result, ins->type_class );
    PrefixIns( ins, new_ins );
    new_ins = MakeBinary( OP_SUB, ins->operands[0], ins->result, ins->result, ins->type_class );
    ReplIns( ins, new_ins );
    UpdateLive( first_ins, new_ins );
    return( first_ins );
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
/****************************************************
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
/*************************************************
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
