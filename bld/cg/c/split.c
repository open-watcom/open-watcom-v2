/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Architecture independent instruction splitting (reductions).
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "regset.h"
#include "conflict.h"
#include "cfloat.h"
#include "makeins.h"
#include "namelist.h"
#include "regalloc.h"
#include "rgtbl.h"
#include "split.h"
#include "insutil.h"
#include "optab.h"
#include "inssegs.h"


extern  conflict_node   *NameConflict(instruction*,name*);
extern  void            CheckCC(instruction*,instruction*);
extern  void            MarkPossible(instruction*,name*,reg_set_index);
extern  void            RevCond(instruction*);

type_class_def  HalfClass[] = {
    0,                  /* U1 */
    0,                  /* I1 */
    U1,                 /* U2 */
    I1,                 /* I2 */
    U2,                 /* U4 */
    I2,                 /* I4 */
    U4,                 /* U8 */
    I4,                 /* I8 */
    U2,                 /* CP */
    U2,                 /* PT */
    I2,                 /* FS */
    I4,                 /* FD */
    U4,                 /* FL */
    U4                  /* XX */
};

type_class_def  DoubleClass[] = {
/*******************************/

    U2,                 /* U1*/
    I2,                 /* I1*/
    U4,                 /* U2*/
    I4,                 /* I2*/
    U8,                 /* U4*/
    I8,                 /* I4*/
    XX,                 /* U8*/
    XX,                 /* I8*/
    0,                  /* CP*/
    0,                  /* PT*/
    0,                  /* FS*/
    0,                  /* FD*/
    XX                  /* XX*/
};

type_class_def  Unsigned[] = {
/****************************/

    U1,                 /* U1*/
    U1,                 /* I1*/
    U2,                 /* U2*/
    U2,                 /* I2*/
    U4,                 /* U4*/
    U4,                 /* I4*/
    U8,                 /* U8*/
    U8,                 /* U8*/
#if _TARGET & _TARG_370
    U4,                 /* CP*/
    U4,                 /* PT*/
#else
    CP,                 /* CP*/
    PT,                 /* PT*/
#endif
    FS,                 /* FS*/
    FD,                 /* FD*/
    FL,                 /* FL*/
    XX                  /* XX*/
};

type_class_def  Signed[] = {
/**************************/

    I1,                 /* U1*/
    I1,                 /* I1*/
    I2,                 /* U2*/
    I2,                 /* I2*/
    I4,                 /* U4*/
    I4,                 /* I4*/
    I8,                 /* U8*/
    I8,                 /* I8*/
#if _TARGET & _TARG_370
    I4,                 /* CP*/
    I4,                 /* PT*/
#else
    CP,                 /* CP*/
    PT,                 /* PT*/
#endif
    FS,                 /* FS*/
    FD,                 /* FD*/
    FL,                 /* FL*/
    XX                  /* XX*/
};

static  reg_set_index   ResultPossible( instruction *ins )
/********************************************************/
{
    return( RegList[ins->u.gen_table->reg_set].result );
}


static  reg_set_index   Op2Possible( instruction *ins )
/*****************************************************/
{
    return( RegList[ins->u.gen_table->reg_set].right );
}

static  reg_set_index   Op1Possible( instruction *ins )
/*****************************************************/
{
    return( RegList[ins->u.gen_table->reg_set].left );
}

static  reg_set_index   ResPossible( instruction *ins )
/*****************************************************/
{
    return( RegList[ins->u.gen_table->reg_set].result );
}

extern  hw_reg_set      Op1Reg( instruction *ins )
/************************************************/
{
    hw_reg_set  *list;

    list = RegSets[Op1Possible( ins )];
    return( *list );
}

extern  hw_reg_set      ResultReg( instruction *ins )
/***************************************************/
{
    hw_reg_set  *list;

    list = RegSets[ResultPossible( ins )];
    return( *list );
}

extern  hw_reg_set      ZapReg( instruction *ins )
/************************************************/
{
    hw_reg_set  *list;

    list = RegSets[RegList[ins->u.gen_table->reg_set].zap];
    return( *list );
}

extern  instruction     *MoveConst( unsigned_32 value,
                                    name *result, type_class_def class )
/**********************************************************************/
{
    return( MakeMove( AllocConst( CFCnvU32F( value ) ), result, class ) );
}


extern  instruction     *Reduce( instruction *ins )
/*************************************************/
{
    ins->head.state = INS_NEEDS_WORK;
    return( ReduceTab[ins->u.gen_table->generate - FIRST_REDUCT]( ins ) );
}


extern  void    HalfType( instruction *ins )
/******************************************/
{
    ins->type_class = HalfClass[ins->type_class];
    ins->table = NULL;
    ins->head.state = INS_NEEDS_WORK;
}


extern  void    ChangeType( instruction *ins, type_class_def class )
/******************************************************************/
{
    ins->type_class = class;
    ins->table = NULL;
    ins->head.state = INS_NEEDS_WORK;
}


static  void    ForceToMemory( name *name )
/*****************************************/
{
    while( name->v.conflict != NULL ) {
        InMemory( name->v.conflict );
    }
}


extern  instruction     *rMOVOP1TEMP( instruction *ins )
/******************************************************/
{
    instruction         *new_ins;
    type_class_def      class;
    name                *name;

    class = _OpClass( ins );
    name = AllocTemp( class );
    new_ins = MakeMove( ins->operands[0], name, class );
    ins->operands[0] = name;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rMOVOP2TEMP( instruction *ins )
/******************************************************/
{
    instruction         *new_ins;
    name                *name1;

    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( ins->operands[1], name1, ins->type_class );
    ins->operands[1] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rOP1REG( instruction *ins )
/**************************************************/
{
    instruction         *new_ins;
    name                *name1;
    type_class_def      class;

    class = _OpClass( ins );
    name1 = AllocTemp( class );
    new_ins = MakeMove( ins->operands[0], name1, class );
    ins->operands[0] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    MarkPossible( ins, name1, Op1Possible( ins ) );
    ins->u.gen_table = NULL;
    GiveRegister( NameConflict( ins, name1 ), true );
    return( new_ins );
}

extern instruction      *rOP2REG( instruction *ins )
/**************************************************/
{
    instruction         *new_ins;
    name                *name1;
    type_class_def      class;

    class = _OpClass( ins );
    name1 = AllocTemp( class );
    new_ins = MakeMove( ins->operands[1], name1, class );
    ins->operands[1] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    MarkPossible( ins, name1, Op1Possible( ins ) );
    ins->u.gen_table = NULL;
    GiveRegister( NameConflict( ins, name1 ), true );
    return( new_ins );
}


/* 370 */
extern instruction      *rMOVRESREG( instruction *ins )
/*****************************************************/
{
    instruction         *new_ins;
    name                *name1;

    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( name1, ins->result, ins->type_class );
    ins->result = name1;
    MoveSegRes( ins, new_ins );
    SuffixIns( ins, new_ins );
    MarkPossible( ins, name1, ResPossible( ins ) );
    ins->u.gen_table = NULL;
    GiveRegister( NameConflict( ins, name1 ), true );
    return( ins );
}

extern instruction      *rMOVRESTEMP( instruction *ins )
/******************************************************/
{
    instruction         *new_ins;
    name                *name1;

    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( name1, ins->result, ins->type_class );
    ins->result = name1;
    MoveSegRes( ins, new_ins );
    SuffixIns( ins, new_ins );
    MarkPossible( ins, name1, ResPossible( ins ) );
    ins->u.gen_table = NULL;
    return( ins );
}


extern instruction      *rRESREG( instruction *ins )
/**************************************************/
{
    instruction         *new_ins;
    name                *name1;

    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( name1, ins->result, ins->type_class );
    ins->result = name1;
    MoveSegRes( ins, new_ins );
    SuffixIns( ins, new_ins );
    MarkPossible( ins, name1, ResultPossible( ins ) );
    ins->u.gen_table = NULL;
    GiveRegister( NameConflict( ins, name1 ), true );
    new_ins = ins;
    return( new_ins );
}


extern instruction      *rOP1RESREG( instruction *ins )
/*****************************************************/
{
    instruction         *new_ins;
    instruction         *ins2;
    name                *name1;
    name                *name2;

    name1 = AllocRegName( Op1Reg( ins ) );
    new_ins = MakeMove( ins->operands[0], name1, _OpClass( ins ) );
    ins->operands[0] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    name2 = AllocRegName( ResultReg( ins ) );
    ins2 = MakeMove( name2, ins->result, ins->type_class );
    ins->result = name2;
    MoveSegRes( ins, ins2 );
    SuffixIns( ins, ins2 );
    return( new_ins );
}

/* 370 */
static  bool    CanUseOp1( instruction *ins, name *op1 )
/******************************************************/
{
    name        *name2;

    if( op1->n.class != N_REGISTER ) return( false );
    if( HW_Ovlap( op1->r.reg, ins->head.next->head.live.regs ) ) return( false );
    if( ins->result->n.class == N_INDEXED ) {
        name2 = ins->result->i.index;
        if( name2->n.class == N_REGISTER ) {
            if( HW_Ovlap( name2->r.reg, op1->r.reg ) ) return( false );
        }
    }
    return( true );
}

/* 370 */
extern instruction      *rUSEREGISTER( instruction *ins )
/*******************************************************/
{
    instruction         *new_ins;
    instruction         *ins2;
    name                *name1;

    name1 = ins->operands[0];
    if( CanUseOp1( ins, name1 ) ) {
        new_ins = MakeMove( name1, ins->result, ins->type_class );
        ins->result = name1;
        MoveSegRes( ins, new_ins );
        SuffixIns( ins, new_ins );
        new_ins = ins;
    } else {
        name1 = AllocTemp( ins->type_class );
        new_ins = MakeMove( ins->operands[0], name1, ins->type_class );
        CheckCC( ins, new_ins );
        ins->operands[0] = name1;
        MoveSegOp( ins, new_ins, 0 );
        PrefixIns( ins, new_ins );
        ins2 = MakeMove( name1, ins->result, ins->type_class );
        ins->result = name1;
        MoveSegRes( ins, ins2 );
        SuffixIns( ins, ins2 );
        MarkPossible( ins, name1, ResultPossible( ins ) );
        ins->u.gen_table = NULL;
        GiveRegister( NameConflict( ins, name1 ), true );
    }
    return( new_ins );
}


extern instruction      *rCHANGESHIFT( instruction *ins )
/*******************************************************/
{
    signed_32   shift_count;

    shift_count = ins->operands[1]->c.lo.int_value;
    assert( shift_count >= 0 );
    ins->operands[1] = AllocS32Const( shift_count & ( ( WORD_SIZE * 8 ) - 1 ) );
    return( ins );
}


extern instruction      *rFIXSHIFT( instruction *ins )
/****************************************************/
{
    instruction         *new_ins;
#ifndef NDEBUG
    signed_32           shift_count;
#endif

    /* Fix up shift instructions that try to shift by too large amounts. This
     * can happen when optimizer merges adjacent shift instructions. Arithmetic
     * rights shifts must never exceed (REGISTER_BITS - 1), logical shifts can
     * be replaced with loads of zero constant.
     */
#ifndef NDEBUG
    assert( ins->operands[1]->n.class == N_CONSTANT );
    shift_count = ins->operands[1]->c.lo.int_value;
    assert( shift_count >= REG_SIZE * 8 );
#endif
    if( ins->head.opcode == OP_RSHIFT && Signed[ins->type_class] == ins->type_class ) {
        ins->operands[1] = AllocS32Const( REG_SIZE * 8 - 1 );
        return( ins );
    } else {
        new_ins = MoveConst( 0, ins->result, ins->type_class );
        DupSeg( ins, new_ins );
        ReplIns( ins, new_ins );
        return( new_ins );
    }
}


extern instruction      *rCLRHI_BW( instruction *ins )
/****************************************************/
{
    instruction         *new_ins;
    instruction         *ins2;
    name                *name1;
    type_class_def      class;

    class = HalfClass[ins->type_class];
    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( ins->operands[0], LowPart( name1, class ), class );
    ins->operands[0] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    ins2 = MoveConst( 0, HighPart( name1, class ), class );
    PrefixIns( ins, ins2 );
    ins->head.opcode = OP_MOV;
    ins->table = NULL;
    ins->u.gen_table = NULL;
    return( new_ins );
}


extern instruction      *rCLRHI_R( instruction *ins )
/***************************************************/
{
    instruction         *new_ins;
    instruction         *and_ins;
    type_class_def      class;
    hw_reg_set          high;
    name                *res;
    signed_32           value;
    type_class_def      half_class;
    name                *op;

    half_class = HalfClass[ins->type_class];
    op = ins->operands[0];
    class = ins->base_type_class;
    res = ins->result;
    high = HighReg( res->r.reg );
    if( op->n.class == N_INDEXED
        && ( ( op->i.index->n.class == N_REGISTER && HW_Ovlap( op->i.index->r.reg, res->r.reg ) ) /* (1) */
            || ( CheckIndecies( ins, res->r.reg, HW_EMPTY, NULL ) == MB_MAYBE ) /* 2 */
           )
      ) {
        /* (1) would have gen'd movzd  eax,[eax]
         * (2) avoid incorrect reduction of "cnv [foo] => BX" to
         *      xor bx,bx / mov bx, [foo] / mov bl, [bx]
         *     (zoiks register allocator)
         */
        new_ins = NULL;
    } else if( !HW_CEqual( high, HW_EMPTY ) && half_class == class ) {
        if( op->n.class == N_REGISTER && HW_Equal( op->r.reg, high ) ) {        // BBB - may 19, 1994
            /* look out for movzd ax,ah */
            new_ins = NULL;
        } else {
            new_ins = MoveConst( 0, HighPart( res, class ), class );
        }
    } else if( op->n.class == N_REGISTER
            && HW_Ovlap( op->r.reg, res->r.reg ) ) {
        /* would have gen'd movzd eax,ah */
        new_ins = NULL;
    } else {
        new_ins = MoveConst( 0, res, ins->type_class );
    }
    ins->result = LowPart( res, class );
    if( new_ins != NULL ) {
        PrefixIns( ins, new_ins );
    } else {
        value = 0;
        switch( class ) {
        case U1:
        case I1:
            value = 0xff;
            break;
        case U2:
        case I2:
            value = 0xffff;
            break;
        }
        high = res->r.reg;
        HW_TurnOff( high, ins->result->r.reg );
        new_ins = MakeNop();
        new_ins->zap = (register_name *) AllocRegName( high );
        SuffixIns( ins, new_ins ); // don't cause high part to be live on entry

        and_ins = MakeBinary( OP_AND, res, AllocS32Const( value ), res, ins->type_class );
        SuffixIns( new_ins, and_ins );
        new_ins = ins;
    }
    ins->type_class = class;
    ins->head.opcode = OP_MOV;
    ins->table = NULL;
    ins->u.gen_table = NULL;
    return( new_ins );
}


extern instruction      *rCONVERT_LOW( instruction *ins )
/*******************************************************/
/* change convert 4 byte==>1 byte to 2 byte ==> 1 byte */
{
    ins->operands[0] = LowPart( ins->operands[0], HalfClass[ins->operands[0]->n.name_class] );
    ins->base_type_class = HalfClass[ins->base_type_class];
    ins->table = NULL;
    return( ins );
}


extern instruction      *rCYPHIGH( instruction *ins )
/***************************************************/
{
    HalfType( ins );
    ins->operands[0] = HighPart( ins->operands[0], ins->type_class );
    ins->operands[1] = HighPart( ins->operands[1], ins->type_class );
    if( ins->result != NULL && ins->head.opcode < FIRST_CONDITION ) {
        ins->result = HighPart( ins->result, ins->type_class );
    }
    return( ins );
}


extern instruction      *rCYPLOW( instruction *ins )
/**************************************************/
{
    HalfType( ins );
    ins->operands[0] = LowPart( ins->operands[0], ins->type_class );
    ins->operands[1] = LowPart( ins->operands[1], ins->type_class );
    if( ins->result != NULL && ins->head.opcode < FIRST_CONDITION ) {
        ins->result = LowPart( ins->result, ins->type_class );
    }
    return( ins );
}


extern instruction      *rDOUBLEHALF( instruction *ins )
/******************************************************/
{
    instruction         *new_ins;
    name                *name1;

    name1 = ins->operands[1];
    name1 = AllocIntConst( name1->c.lo.int_value >> 1 );
    ins->operands[1] = name1;
    new_ins = MakeBinary( ins->head.opcode, ins->operands[0], name1,
                            ins->result, ins->type_class );
    new_ins->table = ins->table;
    DupSeg( ins, new_ins );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rOP1MEM( instruction *ins )
/**************************************************/
{
    ForceToMemory( ins->operands[0] );
    return( ins );
}


extern instruction      *rOP2MEM( instruction *ins )
/**************************************************/
{
    ForceToMemory( ins->operands[1] );
    return( ins );
}


extern instruction      *rFORCERESMEM( instruction *ins )
/*******************************************************/
{
    ForceToMemory( ins->result );
    return( ins );
}


extern instruction      *rMAKEMOVE( instruction *ins )
/****************************************************/
{
    instruction         *new_ins;

    new_ins = MakeMove( ins->operands[0], ins->result, ins->type_class );
    DupSeg( ins, new_ins );
    ReplIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rMAKEXORRR( instruction *ins )
/*****************************************************/
{
    instruction         *new_ins;

    new_ins = MakeBinary( OP_XOR, ins->result, ins->result, ins->result,
                                    ins->type_class );
    ReplIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rADDRR( instruction *ins )
/*************************************************/
{
    ins->head.opcode = OP_ADD;
    ins->operands[1] = ins->operands[0];
    ins->table = NULL;
    return( ins );
}


extern instruction      *rMOVOP2( instruction *ins )
/**************************************************/
{
    instruction         *new_ins;

    new_ins = MakeMove( ins->operands[1], ins->result, ins->type_class );
    MoveSegOp( ins, new_ins, 0 );
    DupSegRes( ins, new_ins );
    ReplIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rMOVOP1RES( instruction *ins )
/*****************************************************/
{
    instruction         *new_ins;

    new_ins = MakeMove( ins->operands[0], ins->result, ins->type_class );
    ins->operands[0] = ins->result;
    MoveSegOp( ins, new_ins, 0 );
    DupSegRes( ins, new_ins );
    PrefixIns( ins, new_ins );
    return( new_ins );
}


/* 386 */
extern instruction      *rMOVOP2RES( instruction *ins )
/*****************************************************/
{
    instruction         *new_ins;

    new_ins = MakeMove( ins->operands[1], ins->result, ins->type_class );
    ins->operands[1] = ins->result;
    MoveSegOp( ins, new_ins, 1 );
    DupSegRes( ins, new_ins );
    PrefixIns( ins, new_ins );
    return( new_ins );
}

/* 370 */
extern instruction      *rSWAPOPS( instruction *ins )
/***************************************************/
{
    name                *name1;

    name1 = ins->operands[0];
    ins->operands[0] = ins->operands[1];
    ins->operands[1] = name1;
    return( ins );
}


/* 370 */
extern instruction      *rSWAPCMP( instruction *ins )
/***************************************************/
{
    RevCond( ins );
    return( rSWAPOPS( ins ) );
}


extern instruction      *rMOVEINDEX( instruction *ins )
/*****************************************************/
{
    instruction         *new_ins;

    new_ins = MakeMove( ins->operands[0]->i.index,
                         ins->result, ins->type_class );
    ReplIns( ins, new_ins );
    return( new_ins );
}

/* 370 */
extern instruction      *rLOADOP2( instruction *ins )
/***************************************************/
{
    instruction         *new_ins;
    name                *name1;

    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( ins->operands[1], name1, ins->type_class );
    CheckCC( ins, new_ins );
    ins->operands[1] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    MarkPossible( ins, name1, Op2Possible( ins ) );
    ins->u.gen_table = NULL;
    GiveRegister( NameConflict( ins, name1 ), true );
    return( new_ins );
}


extern instruction      *rNEGADD( instruction *ins )
/**************************************************/
{
    instruction         *new_ins;

    new_ins = MakeUnary( OP_NEGATE, ins->operands[1],
                          AllocTemp( U4 ), U4 );
    MoveSegOp( ins, new_ins, 0 );
    ins->operands[1] = new_ins->result;
    ins->head.opcode = OP_ADD;
    ins->table = NULL;
    PrefixIns( ins, new_ins );
    return( new_ins );
}


static  void    NegOp2( instruction *ins )
/****************************************/
{
    ins->operands[1] = AllocConst( CFCnvI32F( -ins->operands[1]->c.lo.int_value ) );
}


extern instruction      *rMAKEADD( instruction *ins )
/***************************************************/
{
    ins->head.opcode = OP_ADD;
    NegOp2( ins );
    ins->table = NULL;
    return( ins );
}


extern instruction      *rMAKENEG( instruction *ins )
/***************************************************/
{
    instruction         *new_ins;

    new_ins = MakeUnary( OP_NEGATE, ins->operands[1], ins->result,
                          ins->type_class );
    DupSeg( ins, new_ins );
    ReplIns( ins, new_ins );
    return( new_ins );
}


extern instruction      *rMAKESUB( instruction *ins )
/***************************************************/
{
    ins->head.opcode = OP_SUB;
    NegOp2( ins );
    ins->table = NULL;
    return( ins );
}


extern instruction      *rCMPTRUE( instruction *ins )
/***************************************************/
{
    DoNothing( ins );
    _SetBlockIndex( ins, _TrueIndex( ins ), _TrueIndex( ins ) );
    return( ins );
}


extern instruction      *rCMPFALSE( instruction *ins )
/****************************************************/
{
    DoNothing( ins );
    _SetBlockIndex( ins, _FalseIndex( ins ), _FalseIndex( ins ) );
    return( ins );
}


extern instruction      *rOP1RESTEMP( instruction *ins )
/******************************************************/
{
    instruction         *new_ins;
    instruction         *ins2;
    name                *name1;

    name1 = AllocTemp( ins->type_class );
    new_ins = MakeMove( ins->operands[0], name1, ins->type_class );
    ins->operands[0] = name1;
    MoveSegOp( ins, new_ins, 0 );
    PrefixIns( ins, new_ins );
    ins2 = MakeMove( name1, ins->result, ins->type_class );
    ins->result = name1;
    MoveSegRes( ins, ins2 );
    SuffixIns( ins, ins2 );
    return( new_ins );
}
