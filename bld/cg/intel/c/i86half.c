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


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "regset.h"
#include "model.h"
#include "system.h"
#include "zoiks.h"
#include "makeins.h"

extern  void            ChangeType(instruction*,type_class_def);
extern  name            *IntEquivalent(name*);
extern  void            DupSegOp(instruction*,instruction*,int);
extern  name            *AllocTemp(type_class_def);
extern  opcode_entry    *CodeTable(instruction*);
extern  bool            SameThing(name*,name*);
extern  instruction     *MoveConst(unsigned_32,name*,type_class_def);
extern  constant_defn   *GetFloat(name*,type_class_def);
extern  void            UpdateLive(instruction*,instruction*);
extern  void            DupSegRes(instruction*,instruction*);
extern  void            MoveSegOp(instruction*,instruction*,int);
extern  void            SuffixIns(instruction*,instruction*);
extern  void            HalfType(instruction*);
extern  hw_reg_set      High32Reg(hw_reg_set);
extern  hw_reg_set      High16Reg(hw_reg_set);
extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  name            *TempOffset(name*,type_length,type_class_def);
extern  hw_reg_set      Low32Reg(hw_reg_set);
extern  name            *AllocRegName(hw_reg_set);
extern  hw_reg_set      Low16Reg(hw_reg_set);
extern  name            *AddrConst(name*,int,constant_class);
extern  name            *AllocIntConst(int);
extern  name            *AllocUIntConst(uint);
extern  name            *AllocConst(pointer);
extern  void            ReplIns(instruction*,instruction*);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            DupSeg(instruction*,instruction*);
extern  name            *SegName(name*);
extern  void            DelSeg(instruction*);
extern  name            *ScaleIndex(name*,name*,type_length,type_class_def,type_length,int,i_flags);
extern  name            *AllocIntConst(int);
extern  bool            Overlaps( name *, name * );
extern  name            *LowPart( name *, type_class_def );
extern  name            *HighPart( name *, type_class_def );
extern  void            CnvOpToInt( instruction *, int );

extern    type_class_def        HalfClass[];
extern    type_class_def        Unsigned[];

extern  bool    IndexOverlaps( instruction *ins, int i ) {
/********************************************************/

    if( ins->operands[ i ]->n.class != N_INDEXED ) return( FALSE );
    if( SameThing( ins->operands[ i ]->i.index, ins->result ) ) return( TRUE );
    return( FALSE );
}

#if _TARGET & _TARG_80386
    #define WORD        U4
    #define HALF_WORD   U2
    #define LONG_WORD   U8
    #define LONG_WORD_S I8
    #define HIGH_WORD( x )      ( (x)->c.int_value_2 )
#else
    #define WORD        U2
    #define HALF_WORD   U1
    #define LONG_WORD   U4
    #define LONG_WORD_S I4
    #define HIGH_WORD(x) ( ( ((x)->c.int_value) >> 16 ) & 0xffff )
#endif


extern  instruction     *rSPLITOP( instruction *ins ) {
/****************************************************/

    instruction *new_ins;
    instruction *ins2;
    name        *temp;

    if( IndexOverlaps( ins, 0 ) || IndexOverlaps( ins, 1 ) ) {
        temp = AllocTemp( LONG_WORD );
        HalfType( ins );
        new_ins = MakeBinary( ins->head.opcode,
                        LowPart( ins->operands[ 0 ], WORD ),
                        LowPart( ins->operands[ 1 ], WORD ),
                        LowPart( temp,               WORD ),
                        WORD );
        ins2 = MakeBinary( ins->head.opcode,
                        HighPart( ins->operands[ 0 ], WORD ),
                        HighPart( ins->operands[ 1 ], WORD ),
                        HighPart( temp,               WORD ),
                        WORD );
        if( ins->head.opcode == OP_ADD ) {
            ins2->head.opcode = OP_EXT_ADD;
        } else if( ins->head.opcode == OP_SUB ) {
            ins2->head.opcode = OP_EXT_SUB;
        }
        ins2->table = CodeTable( ins2 );
        new_ins->table = ins2->table;
        DupSegOp( ins, new_ins, 0 );
        DupSegOp( ins, ins2, 0 );
        DupSegOp( ins, new_ins, 1 );
        DupSegOp( ins, ins2, 1 );
        ins->operands[ 0 ] = temp;
        ins->operands[ 1 ] = temp;
        PrefixIns( ins, new_ins );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( LowPart( temp, WORD ), LowPart( ins->result, WORD ), WORD );
        DupSegRes( ins, ins2 );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( HighPart( temp, WORD ),
                          HighPart( ins->result, WORD ), WORD );
        DupSegRes( ins, ins2 );
        ReplIns( ins, ins2 );
    } else {
        HalfType( ins );
        new_ins = MakeBinary( ins->head.opcode,
                        LowPart( ins->operands[ 0 ], ins->type_class ),
                        LowPart( ins->operands[ 1 ], ins->type_class ),
                        LowPart( ins->result,        ins->type_class ),
                        ins->type_class );
        DupSeg( ins, new_ins );
        ins->operands[ 0 ] = HighPart( ins->operands[ 0 ], ins->type_class );
        ins->operands[ 1 ] = HighPart( ins->operands[ 1 ], ins->type_class );
        ins->result = HighPart( ins->result, ins->type_class );
        if( ins->head.opcode == OP_ADD ) {
            ins->head.opcode = OP_EXT_ADD;
        } else if( ins->head.opcode == OP_SUB ) {
            ins->head.opcode = OP_EXT_SUB;
        }
/* Assign fake reduce table (from OP_EXT) to new_ins; default reduce table
   can generate INC and DEC which'll not set condition codes
 */
        ins->table = CodeTable( ins );
        new_ins->table = ins->table;

        PrefixIns( ins, new_ins );
    }
    new_ins->ins_flags |= INS_CC_USED;
    return( new_ins );
}


extern  instruction     *rSPLITMOVE( instruction *ins ) {
/******************************************************/

    instruction *new_ins;
    instruction *ins2;
    name        *temp;

    CnvOpToInt( ins, 0 );
    if( IndexOverlaps( ins, 0 ) ) {
        temp = AllocTemp( LONG_WORD );
        new_ins = MakeMove( LowPart( ins->operands[ 0 ], WORD ),
                             LowPart( temp, WORD ), WORD );
        ins2 = MakeMove( HighPart( ins->operands[ 0 ], WORD ),
                             HighPart( temp, WORD ), WORD );
        DupSegOp( ins, new_ins, 0 );
        DupSegOp( ins, ins2, 0 );
        ins->operands[ 0 ] = temp;
        PrefixIns( ins, new_ins );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( LowPart( temp, WORD ), LowPart( ins->result, WORD ), WORD );
        DupSegRes( ins, ins2 );
        PrefixIns( ins, ins2 );
        ins2 = MakeMove( HighPart( temp, WORD ),
                          HighPart( ins->result, WORD ), WORD );
        ReplIns( ins, ins2 );
    } else {
        HalfType( ins );
        new_ins = MakeMove( LowPart( ins->operands[ 0 ], ins->type_class ),
                             LowPart( ins->result, ins->type_class ),
                             ins->type_class );
        DupSeg( ins, new_ins );
        ins->operands[ 0 ] = HighPart( ins->operands[ 0 ], ins->type_class );
        ins->result = HighPart( ins->result, ins->type_class );
        if( new_ins->result->n.class == N_REGISTER
         && ins->operands[ 0 ]->n.class == N_REGISTER
         && HW_Ovlap( new_ins->result->r.reg, ins->operands[ 0 ]->r.reg ) ) {
            SuffixIns( ins, new_ins );
            new_ins = ins;
        } else {
            PrefixIns( ins, new_ins );
        }
    }
    return( new_ins );
}


#if _TARGET & _TARG_IAPX86
extern  instruction     *rMAKEU2( instruction *ins ) {
/***************************************************/

    instruction *new_ins;
    instruction *ins2;
    name        *temp = NULL;

    if( IndexOverlaps( ins, 0 ) || IndexOverlaps( ins, 1 ) ) {
        ChangeType( ins, WORD );
        if( ins->result != NULL ) {
            new_ins = MakeMove( HighPart( ins->operands[ 0 ], WORD ),
                                 AllocTemp( WORD ), WORD );
            temp = HighPart( ins->result, WORD );
            ins->result = LowPart( ins->result, WORD );
            DupSegOp( ins, new_ins, 0 );
            PrefixIns( ins, new_ins );
        } else {
            new_ins = ins;
        }
        ins->operands[ 0 ] = LowPart( ins->operands[ 0 ], WORD );
        if( ins->operands[ 1 ]->n.name_class == LONG_WORD
         || ins->operands[ 1 ]->n.name_class == LONG_WORD_S ) {
            ins->operands[ 1 ] = LowPart( ins->operands[ 1 ], WORD );
        }
        if( ins->result != NULL ) {
            ins2 = MakeMove( new_ins->result, temp, WORD );
            DupSegRes( ins, ins2 );
            SuffixIns( ins, ins2 );
        }
    } else {
        ChangeType( ins, WORD );
        if( ins->result != NULL ) {
            new_ins = MakeMove( HighPart( ins->operands[ 0 ], WORD ),
                                 HighPart( ins->result, WORD ), WORD );
            ins->result = LowPart( ins->result, WORD );
            DupSegOp( ins, new_ins, 0 );
            DupSegRes( ins, new_ins );
            PrefixIns( ins, new_ins );
        } else {
            new_ins = ins;
        }
        ins->operands[ 0 ] = LowPart( ins->operands[ 0 ], WORD );
        ins->operands[ 1 ] = LowPart( ins->operands[ 1 ], WORD );
    }
    return( new_ins );
}
#endif

extern  instruction     *rSPLITNEG( instruction *ins ) {
/******************************************************/

    name        *hi_res;
    name        *lo_res;
    name        *hi_src;
    name        *lo_src;
    instruction *hi_ins;
    instruction *lo_ins;
    instruction *subtract;

    HalfType( ins );
    hi_res = HighPart( ins->result, ins->type_class );
    hi_src = HighPart( ins->operands[ 0 ], ins->type_class );
    lo_res = LowPart( ins->result, ins->type_class );
    lo_src = LowPart( ins->operands[ 0 ], ins->type_class );
    hi_ins = MakeUnary( OP_NEGATE, hi_src, hi_res, ins->type_class );
    lo_ins = MakeUnary( OP_NEGATE, lo_src, lo_res, ins->type_class );
    lo_ins->ins_flags |= INS_CC_USED;
    subtract = MakeBinary( OP_EXT_SUB, hi_res, AllocIntConst( 0 ), hi_res,
                            ins->type_class );
    DupSegRes( ins, subtract );
    DupSeg( ins, hi_ins );
    DupSeg( ins, lo_ins );
    PrefixIns( ins, hi_ins );
    ins->operands[ 0 ] = ins->result;
    ins->operands[ 1 ] = AllocIntConst( 0 );
    PrefixIns( ins, lo_ins );
    ReplIns( ins, subtract );
    UpdateLive( hi_ins, subtract );
    return( hi_ins );
}

extern  instruction     *rSPLITCMP( instruction *ins ) {
/*********************************************************/

    name                *left;
    name                *right;
    instruction         *low = NULL;
    instruction         *high = NULL;
    instruction         *not_equal = NULL;
    type_class_def      high_class;
    type_class_def      low_class;
    byte                true_idx;
    byte                false_idx;

    high_class = HalfClass[  ins->type_class  ];
    low_class  = Unsigned[  high_class  ];
    left = ins->operands[ 0 ];
    right = ins->operands[ 1 ];
    true_idx = _TrueIndex( ins );
    false_idx = _FalseIndex( ins );
    switch( ins->head.opcode ) {
    case OP_BIT_TEST_TRUE:
        high = MakeCondition( ins->head.opcode,
                        HighPart( left, high_class ),
                        HighPart( right, high_class ),
                        true_idx, NO_JUMP,
                        WORD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_class ),
                        LowPart( right, low_class ),
                        true_idx, false_idx,
                        WORD );
        not_equal = NULL;
        break;
    case OP_BIT_TEST_FALSE:
        high = MakeCondition( OP_BIT_TEST_TRUE,
                        HighPart( left, high_class ),
                        HighPart( right, high_class ),
                        false_idx, NO_JUMP,
                        WORD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_class ),
                        LowPart( right, low_class ),
                        true_idx, false_idx,
                        WORD );
        not_equal = NULL;
        break;
    case OP_CMP_EQUAL:
        high = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_class ),
                        HighPart( right, high_class ),
                        false_idx, NO_JUMP,
                        WORD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_class ),
                        LowPart( right, low_class ),
                        true_idx, false_idx,
                        WORD );
        not_equal = NULL;
        break;
    case OP_CMP_NOT_EQUAL:
        high = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_class ),
                        HighPart( right, high_class ),
                        true_idx, NO_JUMP,
                        WORD );
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_class ),
                        LowPart( right, low_class ),
                        true_idx, false_idx,
                        WORD );
        not_equal = NULL;
        break;
    case OP_CMP_LESS:
    case OP_CMP_LESS_EQUAL:
        not_equal = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_class ),
                        HighPart( right, high_class ),
                        false_idx, NO_JUMP,
                        high_class );
        if( high_class == WORD
         && right->n.class == N_CONSTANT
         && right->c.const_type == CONS_ABSOLUTE
         && HIGH_WORD( right ) == 0 ) {
            high = NULL;
        } else {
            high = MakeCondition( OP_CMP_LESS,
                        not_equal->operands[ 0 ], not_equal->operands[ 1 ],
                        true_idx, NO_JUMP,
                        high_class );
        }
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_class ),
                        LowPart( right, low_class ),
                        true_idx, false_idx,
                        low_class );
        break;
    case OP_CMP_GREATER_EQUAL:
    case OP_CMP_GREATER:
        not_equal = MakeCondition( OP_CMP_NOT_EQUAL,
                        HighPart( left, high_class ),
                        HighPart( right, high_class ),
                        false_idx, NO_JUMP,
                        high_class );
        if( high_class == WORD
         && right->n.class == N_CONSTANT
         && right->c.const_type == CONS_ABSOLUTE
         && HIGH_WORD( right ) == 0 ) {
            _SetBlockIndex( not_equal, true_idx, NO_JUMP );
            high = NULL;
        } else {
            high = MakeCondition( OP_CMP_GREATER,
                        not_equal->operands[ 0 ], not_equal->operands[ 1 ],
                        true_idx, NO_JUMP,
                        high_class );
        }
        low = MakeCondition( ins->head.opcode,
                        LowPart( left, low_class ),
                        LowPart( right, low_class ),
                        true_idx, false_idx,
                        low_class );
        break;
    default:
        break;
    }
    if( high != NULL ) {
/*
 * 2005-04-06 RomanT (bug #407)
 * I removed calls do DoNothing(), it seems ok, extra jumps are perfectly
 * optimized out in other places of compiler. Calling DoNothing() on chain
 * of conditions to reuse existing CC flags is ugly and causes unpredictable
 * logical faults in other places.
 */
        DupSeg( ins, high );
        PrefixIns( ins, high );
    } else {
        high = not_equal;              /* for return value*/
    }
    if( not_equal != NULL ) {
        DupSeg( ins, not_equal );
        PrefixIns( ins, not_equal );
    }
    DupSeg( ins, low );
    ReplIns( ins, low );
    return( high );
}

