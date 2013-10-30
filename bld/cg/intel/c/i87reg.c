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
* Description:  FPU stack (x87) register allocation.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "gen8087.h"
#include "zoiks.h"
#include "data.h"
#include "x87.h"
#include "makeins.h"

extern  hw_reg_set      FPRegs[];
extern  name            *FPStatWord;
extern  int             Max87Stk;

extern  void            SuffixIns(instruction*,instruction*);
extern  name            *AllocIntConst(int);
extern  name            *TempOffset(name*,type_length,type_class_def);
extern  conflict_node   *InMemory(conflict_node*);
extern  conflict_node   *NameConflict(instruction*,name*);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            MoveSegOp(instruction*,instruction*,int);
extern  void            MoveSegRes(instruction*,instruction*);
extern  name            *AllocTemp(type_class_def);
extern  bool            AssignARegister(conflict_node*,hw_reg_set);
extern  conflict_node   *FindConflictNode(name*,block*,instruction*);
extern  void            LiveInfoUpdate(void);
extern  int             NumOperands(instruction *);
extern  bool_maybe      ReDefinedBy(instruction*,name*);
extern  void            UpdateLive(instruction*,instruction*);

/* forward declarations */
static  void            CnvOperand( instruction *ins );
static  void            FPAlloc( void );
static  void            NoStackAcrossCalls( void );
static  void            StackShortLivedTemps( void );
static  void            FSinCos( void );
static  void            CnvResult( instruction *ins );
static  void            FindSinCos( instruction *ins, opcode_defs next_op );
extern  int             Count87Regs( hw_reg_set regs );
static  void            FPConvert( void );

extern  void    FPRegAlloc( void ) {
/*****************************
    Allocate registers comprising the "stack" portion of the 8087.
*/

    if( _FPULevel( FPU_87 ) ) {
        if( _FPULevel( FPU_387 ) ) {
            FSinCos();
        }
        StackShortLivedTemps();
        NoStackAcrossCalls();
        FPAlloc();
    }
}




static byte StackReq8087[LAST_IFUNC-FIRST_IFUNC+1] = {
/********************************************************
    how much stack over and above the parm
    does the operation require? NB: this number + number parms must be <= 4
*/
        2,        /* OP_POW */
        2,        /* OP_P5DIV */
        0,        /* OP_ATAN2 */
        0,        /* OP_FMOD */
        0,        /* OP_NEGATE */
        0,        /* OP_COMPLEMENT */
        1,        /* OP_LOG */
        3,        /* OP_COS */
        3,        /* OP_SIN */
        2,        /* OP_TAN */
        0,        /* OP_SQRT */
        0,        /* OP_FABS */
        2,        /* OP_ACOS */
        2,        /* OP_ASIN */
        1,        /* OP_ATAN */
        2,        /* OP_COSH */
        2,        /* OP_EXP */
        1,        /* OP_LOG10 */
        2,        /* OP_SINH */
        2         /* OP_TANH */
};

static byte StackReq387[LAST_IFUNC-FIRST_IFUNC+1] = {
/********************************************************
    how much stack over and above the parm
    does the operation require? NB: this number + number parms must be <= 4
*/
        2,        /* OP_POW */
        0,        /* OP_ATAN2 */
        0,        /* OP_FMOD */
        0,        /* OP_NEGATE */
        0,        /* OP_COMPLEMENT */
        1,        /* OP_LOG */
        1,        /* OP_COS */
        1,        /* OP_SIN */
        1,        /* OP_TAN */
        0,        /* OP_SQRT */
        0,        /* OP_FABS */
        2,        /* OP_ACOS */
        2,        /* OP_ASIN */
        1,        /* OP_ATAN */
        2,        /* OP_COSH */
        2,        /* OP_EXP */
        1,        /* OP_LOG10 */
        2,        /* OP_SINH */
        2         /* OP_TANH */
};

extern  void    InitFPStkReq( void ) {
/******************************/

    if( _IsTargetModel( I_MATH_INLINE ) ) {
        StackReq387[ OP_SIN-FIRST_IFUNC ] = 0;
        StackReq387[ OP_COS-FIRST_IFUNC ] = 0;
    }
}


extern  int     FPStkReq( instruction *ins ) {
/********************************************/

    if( !_OpIsIFunc( ins->head.opcode ) ) return( 0 );
    if( _FPULevel( FPU_387 ) ) {
        return( StackReq387[ ins->head.opcode - FIRST_IFUNC ] );
    } else {
        return( StackReq8087[ ins->head.opcode - FIRST_IFUNC ] );
    }
}


static  bool    MathOpsBlowStack( conflict_node *conf, int stk_level ) {
/*****************************************************************
    See if putting "conf" on the stack would blow the 8087 stack in the
    face of stack requirements for any complicated math operations in
    between.
*/

    instruction *ins;
    instruction *last;

    ins = conf->ins_range.first;
    last = conf->ins_range.last;
    if( ins == last ) return( TRUE );
    for( ins = ins->head.next; ins != last; ins = ins->head.next ) {
        if( FPStkReq( ins ) + stk_level + NumOperands( ins ) >= Max87Stk ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static  bool    AssignFPResult( block *blk, instruction *ins, int *stk_level ) {
/*******************************************************************************
    Try to assign a "stack" register to the result of instruction
    "ins"., updating "*stk_level" if we did.  These register given out
    will be one of ST(1) ..  ST(Max87Stk) as described in FPAlloc.
    Notice that the result must be a temporary with the "CAN_STACK"
    attribute.  This corresponds to a temporary that is defined once
    then used in a fashion compatible with a stack.  The CAN_STACK
    attribute is determined in TREE.C, and also in StackShortLivedTemps.

*/

    name                *op;
    conflict_node       *conf;
    bool                need_live_update;

    op = ins->result;
    if( op == NULL ) return( FALSE );
    if( op->n.class == N_REGISTER ) {
        if( HW_COvlap( op->r.reg, HW_FLTS ) ) ++*stk_level;
        return( FALSE );
    }
    if( op->n.class != N_TEMP ) return( FALSE );
    if( ( op->t.temp_flags & CAN_STACK ) == 0 ) return( FALSE );
    if( op->v.usage & (USE_ADDRESS | USE_IN_ANOTHER_BLOCK) ) return( FALSE );
    if( !_IsFloating( op->n.name_class ) ) return( FALSE );
    if( *stk_level < 0 ) return( FALSE );
    if( *stk_level >= (Max87Stk-1) ) return( FALSE );
    conf = FindConflictNode( op, blk, ins );
    if( conf == NULL ) return( FALSE );
    if( MathOpsBlowStack( conf, *stk_level ) ) return( FALSE );
    ++*stk_level;
    need_live_update = AssignARegister( conf, FPRegs[ *stk_level ] );
    return( need_live_update );
}


static  void    AssignFPOps( instruction *ins, int *stk_level ) {
/****************************************************************
    This checks for operands which are "stack" registers and for each
    one that is used, it bumps down "*stk_level" since we know that the
    use of the operand will "pop" the 8087 stack.
*/

    int                 old_level;
    name                *op;
    int                 i;

    /* Now check operands ... bump down stack level for each*/
    /* top of stack operand that will be popped (may be more than one)*/
    if( _OpIsCall( ins->head.opcode ) ) {
        *stk_level -= Count87Regs( ins->operands[ CALL_OP_USED ]->r.reg );
    } else {
        old_level = *stk_level;
        for( i = ins->num_operands; i-- > 0; ) {
            op = ins->operands[i];
            if( op->n.class == N_REGISTER && ( HW_COvlap( op->r.reg, HW_FLTS ) ) ) {
                 --*stk_level;
            }
        }
        if( ins->num_operands < 2 ) return;
        if( ins->operands[0] != ins->operands[1] ) return;
        if( old_level == *stk_level ) return;
        ++*stk_level;
    }
}


static  void    SetStackLevel( instruction *ins, int *stk_level ) {
/******************************************************************
    Returns TRUE if a call ignores a return value in ST(0)
*/


    if( !_OpIsCall( ins->head.opcode ) ) return;
    if( !HW_CEqual( ins->result->r.reg, HW_ST0 ) ) return;
    if( !( ins->flags.call_flags & CALL_IGNORES_RETURN ) ) return;
    --*stk_level;
}


static  void    FPAlloc( void ) {
/**************************
   Pre allocate 8087 registers to temporarys that lend themselves
   to a stack architecture.  ST(0) is reserved as the floating top
   of stack pointer.  Notice that by not "assigning" ST(0), we
   give FPExpand one register on the 8087 stack to play with if it
   needs to FLD a memory location or a constant or something.  We
   also set stk_entry, stk_exit to indicate the affect that each
   instruction has on the 8087 stack.  stk_extra indicates how much
   stack over and above stk_entry gets used.


*/
    block               *blk;
    instruction         *ins;
    int                 stk_level;
    int                 sequence;
    bool                need_live_update;
    name                *name;
    instruction         *new_ins;

    HW_CTurnOn( CurrProc->state.unalterable, HW_FLTS );
    FPStatWord = NULL;
    stk_level = 0;
    need_live_update = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        ins = blk->ins.hd.next;
        if( ins->head.opcode == OP_BLOCK ) continue;
        ins->stk_entry = stk_level;
        SetStackLevel( ins, &stk_level );
        sequence = 0;
        for( ;; ) {
            /*
             * if we have an ins which looks like "cnv FS FD t1 -> t1" we need to
             * split it up so that we can give it a register, since rDOCVT is so lame
             */
            for( ;; ) { // not really a loop - forgive me
                if( ins->head.opcode != OP_CONVERT ) break;
                if( !( ( ins->type_class == FS && ins->base_type_class == FD ) ||
                      ( ins->type_class == FD && ins->base_type_class == FS ) ) ) break;
                if( ins->operands[0]->n.class == N_REGISTER ) break;
                if( ins->operands[0]->n.class == N_TEMP && ( ( ins->operands[0]->t.temp_flags & CAN_STACK ) != EMPTY ) ) break;
                name = AllocTemp( ins->base_type_class );
                name->t.temp_flags |= CAN_STACK;
                new_ins = MakeMove( ins->operands[ 0 ], name, ins->base_type_class );
                ins->operands[ 0 ] = name;
                MoveSegOp( ins, new_ins, 0 );
                PrefixIns( ins, new_ins );
                ins = new_ins;
                break;
            }
            /* check the result ... if it's top of stack, bump up stack level*/
            if( AssignFPResult( blk, ins, &stk_level ) ) {
                need_live_update = TRUE;
            }
            ins->sequence = sequence;
            ins->stk_exit = stk_level;
            // ins->s.stk_extra = FPStkReq( ins ); // BBB - Mar 22, 1994
            if(  _FPULevel( FPU_586 ) &&
                stk_level == 0 ) ++sequence; // NYI - overflow?
            ins = ins->head.next;
            if( ins->head.opcode == OP_BLOCK ) break;
            ins->stk_entry = stk_level;
            SetStackLevel( ins, &stk_level );
            AssignFPOps( ins, &stk_level );
        }
        if( ( blk->class & RETURN ) && stk_level == 1 ) {
            stk_level = 0;
        }
        if( stk_level != 0 ) {
            _Zoiks( ZOIKS_074 );
        }
    }
    if( need_live_update ) {
        LiveInfoUpdate();
    }
    FPConvert();
}


static  void    FPConvert( void ) {
/****************************
    Make sure all operands of _IsFloating() instructions are a type that
    may be used in an FLD or FST instruction.

*/


    block       *blk;
    instruction *ins;
    instruction *next;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
            next = ins->head.next;
            if( ins->head.opcode == OP_CONVERT ) {
                if( _Is87Ins( ins ) ) {
                    CnvOperand( ins );
                    CnvResult( ins );
                }
            }
        }
    }
}


static  void    ToMemory( instruction *ins, name *t ) {
/******************************************************
    Force "t" into memory (no register may be allocated for it) since we
    can't use 8086 registers in 8087 instructions.
*/

    conflict_node       *conf;

    if( t->n.class == N_TEMP || t->n.class == N_MEMORY ) {
        conf = NameConflict( ins, t );
        if( conf != NULL ) {
            InMemory( conf );
        } else {
            t->v.usage |= NEEDS_MEMORY | USE_MEMORY;
        }
    }
}


static  void    CnvOperand( instruction *ins ) {
/***********************************************
    Turn op1 (operands[0]) of instruction "ins" into a variable of a
    type that can be used directly by an 8087 instruction like FLD or
    FILD.
*/

    name                *t;
    instruction         *new_ins;
    type_class_def      class;

    class = ins->base_type_class;
    switch( class ) {
    case U1:
    case I1:
        t = AllocTemp( I2 );
        new_ins = MakeConvert( ins->operands[ 0 ], t, I2, class );
        ins->base_type_class = I2;
        ins->operands[ 0 ] = t;
        MoveSegOp( ins, new_ins, 0 );
        PrefixIns( ins, new_ins );
        InMemory( NameConflict( ins, t ) );
        break;
    case U2:
        t = AllocTemp( I4 );
        new_ins = MakeConvert( ins->operands[ 0 ], t, I4, class );
        ins->base_type_class = I4;
        ins->operands[ 0 ] = t;
        MoveSegOp( ins, new_ins, 0 );
        PrefixIns( ins, new_ins );
        InMemory( NameConflict( ins, t ) );
        break;
    case I2:
    case I4:
    case I8:
    case U8:
        if( ins->operands[ 0 ]->n.class == N_TEMP &&
                ( ins->operands[ 0 ]->v.usage & HAS_MEMORY ) == 0 ) {
            t = AllocTemp( class );
            new_ins = MakeMove( ins->operands[ 0 ], t, class );
            ins->operands[ 0 ] = t;
            MoveSegOp( ins, new_ins, 0 );
            PrefixIns( ins, new_ins );
            InMemory( NameConflict( ins, t ) );
        } else {
            ToMemory( ins, ins->operands[ 0 ] );
        }
        break;
    case U4:
        t = AllocTemp( U8 );
        new_ins = MakeConvert( ins->operands[ 0 ], t, U8, U4 );
        ins->operands[ 0 ] = t;
        MoveSegOp( ins, new_ins, 0 );
        PrefixIns( ins, new_ins );
        InMemory( NameConflict( ins, t ) );
        break;
    default:
        break;
    }
}


static  void    CnvResult( instruction *ins ) {
/**********************************************
    Convert the result of instruction "ins" into a variable of a type
    that can be directly used by an instruction like FSTP or FISTP.
*/

    name                *t;
    instruction         *new_ins;
    type_class_def      class;

    class = ins->type_class;
    switch( class ) {
    case U1:
    case I1:
        t = AllocTemp( I2 );
        t->v.usage |= NEEDS_MEMORY | USE_MEMORY;
        new_ins = MakeConvert( t, ins->result, class, I2 );
        ins->result = t;
        MoveSegRes( ins, new_ins );
        SuffixIns( ins, new_ins );
        break;
    case U2:
        t = AllocTemp( I4 );
        t->v.usage |= NEEDS_MEMORY | USE_MEMORY;
        new_ins = MakeConvert( t, ins->result, class, I4 );
        ins->result = t;
        MoveSegRes( ins, new_ins );
        SuffixIns( ins, new_ins );
        break;
    case I2:
    case I4:
    case I8:
    case U8:
        ToMemory( ins, ins->result );
        break;
    case U4:
        t = AllocTemp( XX );
        t->n.size = 8;
        t->v.usage |= NEEDS_MEMORY | USE_MEMORY;
        new_ins = MakeMove( TempOffset(t,0,U4), ins->result, U4);
        ins->result = t;
        MoveSegRes( ins, new_ins );
        SuffixIns( ins, new_ins );
        break;
    default:
        break;
    }
}


extern  int     Count87Regs( hw_reg_set regs ) {
/***********************************************
    Count the number of 8087 registers named in hw_reg_set "regs".
*/

    int         count;
    int         i;

    i = 0;
    count = 0;
    for(;;) {
        if( HW_Ovlap( FPRegs[ i ], regs ) ) {
            ++count;
        }
        if( i == 7 ) break;
        ++i;
    }
    return( count );
}


extern  bool    FPStackIns( instruction *ins ) {
/**********************************************/

    if( !_FPULevel( FPU_87 ) ) return( FALSE );
    if( _OpIsCall( ins->head.opcode ) ) return( TRUE );
    if( _Is87Ins( ins ) ) return( TRUE );
    return( FALSE );
}


extern  bool    FPSideEffect( instruction *ins ) {
/*************************************************
    Return TRUE if instruction "ins" is an instruction that has a side
    effect, namely pushes or pops the 8087 stack.

*/

    int         i;
    bool        has_fp_reg;

    if( !_FPULevel( FPU_87 ) ) return( FALSE );
    /* calls require a clean stack */
    if( _OpIsCall( ins->head.opcode ) ) return( TRUE );
    if( !_Is87Ins( ins ) ) return( FALSE );
    has_fp_reg = FALSE;
    for( i = ins->num_operands; i-- > 0; ) {
        if( ins->operands[i]->n.class == N_REGISTER ) {
            if( HW_COvlap( ins->operands[i]->r.reg, HW_FLTS ) ) {
                has_fp_reg = TRUE;
            }
        }
    }
    if( ins->result != NULL ) {
        if( ins->result->n.class == N_REGISTER ) {
            if( HW_COvlap( ins->result->r.reg, HW_FLTS ) ) {
                has_fp_reg = TRUE;
            }
        }
    }
    if( has_fp_reg ) {
        if( ins->ins_flags & INS_PARAMETER ) return( TRUE );
        if( ins->stk_entry != ins->stk_exit ) return( TRUE );
    }
    return( FALSE );
}


static  bool    CanStack( name *name ) {
/***************************************
    Return true if "name" is a candidate for an 8087 "stack" location.
*/

    instruction         *first;
    instruction         *last;
    conflict_node       *conf;

    if( ( name->v.usage & USE_IN_ANOTHER_BLOCK ) ) return( FALSE );
    for( conf = name->v.conflict; conf != NULL; conf = conf->next_for_name ) {
        if( conf->start_block == NULL ) return( FALSE );
        first = conf->ins_range.first;
        last = conf->ins_range.last;
        if( first == NULL ) return( FALSE );
        if( last == NULL ) return( FALSE );
        if( first == last ) return( FALSE );
        for( first = first->head.next; first != last; first = first->head.next ) {
            if( first->head.opcode == OP_CALL ) return( FALSE );
            if( first->head.opcode == OP_CALL_INDIRECT ) return( FALSE );
        }
    }
    return( TRUE );
}


static  void    StackShortLivedTemps( void ) {
/***************************************
    Most temporaries are marked as CAN_STACK when they are back end
    generated temps which are used to hold an intermediate result from
    an expression tree.  Some user or front end temps also fall into the
    CAN_STACK category if they are defined once then used.  This catches
    temps that are defined in one instruction and used in the next one.

*/

    conflict_node       *conf;
    instruction         *ins1;
    instruction         *ins2;
    name                *temp;

    for( conf = ConfList; conf != NULL; conf = conf->next_conflict ) {
        ins1 = conf->ins_range.first;
        ins2 = conf->ins_range.last;
        if( ins1 == NULL ) continue;
        if( ins2 == NULL ) continue;
        if( !_Is87Ins( ins1 ) ) continue;
        if( !_Is87Ins( ins2 ) ) continue;
        if( ins1->ins_flags & INS_PARAMETER ) continue;
        if( ins2->ins_flags & INS_PARAMETER ) continue;
        if( ins1->head.next != ins2 ) continue;
        temp = conf->name;
        if( temp->n.class != N_TEMP ) continue;
        if( temp->t.temp_flags & CROSSES_BLOCKS ) continue;
        if( temp->t.alias != temp ) continue;
        FPSetStack( temp );
    }
}


static  void    CheckForStack( name *temp )
/*************************************
    used by NoStackAcrossCalls
*/
{
    if( temp->n.class != N_TEMP ) return;
    if( CanStack( temp ) ) return;
    temp->t.temp_flags &= ~CAN_STACK;
}


static  void    NoStackAcrossCalls( void ) {
/*************************************
    Since a call requires the stack of the 8087 to be empty, we can't
    hold a value in a stack register across a call instrution.  This
    routine turns off the CAN_STACK attribute in temps which live across
    a call instruction. Also, we cannot stack the operands of POW, ATAN2, etc.
    Run the block list, rather than the temp list for speed when we're
    BlockByBlock.
*/

    block       *blk;
    instruction *ins;
    int         i;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            for( i = ins->num_operands; i-- > 0; ) {
                CheckForStack( ins->operands[i] );
            }
            if( ins->result != NULL ) {
                CheckForStack( ins->result );
            }
        }
    }
}


extern  type_class_def  FPInsClass( instruction *ins ) {
/*******************************************************
    Return FD if the instruction will use the 8087.
*/

    if( !_FPULevel( FPU_87 ) ) return( XX );
    if( !_Is87Ins( ins ) ) return( XX );
    return( FD );
}


extern  void    FPSetStack( name *name ) {
/*****************************************
    Turn on the CAN_STACK attribute in "name" if its ok to do so.
*/

    if( name->n.class != N_TEMP ) return;
    if( name->v.usage & USE_IN_ANOTHER_BLOCK ) return;
    if( !_IsFloating( name->n.name_class ) ) return;
    name->t.temp_flags |= CAN_STACK;
}


extern  bool    FPIsStack( name *name ) {
/****************************************
    return TRUE if "name" is a stackable temp.
*/

    if( name->n.class != N_TEMP ) return( FALSE );
    if( ( name->t.temp_flags & CAN_STACK ) == EMPTY ) return( FALSE );
    return( TRUE );
}


extern  bool    FPStackOp( name *name ) {
/****************************************
    return TRUE if "name" is a stackable temp.
*/

    if( !_FPULevel( FPU_87 ) ) return( FALSE );
    return( FPIsStack( name ) );
}


extern  void    FPNotStack( name *name ) {
/*****************************************
    Turn off the CAN_STACK attribute of "name".  This is done whenever
    an instruction is moved during an optimization, since it could cause
    the value to be not at top of stack at the reference.
*/

    if( name->n.class == N_TEMP ) {
        name->t.temp_flags &= ~CAN_STACK;
    }
}


extern  bool    FPIsConvert( instruction *ins ) {
/************************************************
    return TRUE if "ins" is a converstion that could be handled by the 8087.
*/

    type_class_def      op_class;
    type_class_def      res_class;

    if( !_FPULevel( FPU_87 ) ) return( FALSE );
    if( ins->operands[ 0 ]->n.class == N_CONSTANT ) return( FALSE );
    op_class = ins->operands[ 0 ]->n.name_class;
    res_class = ins->result->n.name_class;
    if( op_class == res_class ) return( FALSE );
    if( _Is87Ins( ins ) ) return( TRUE );
    return( FALSE );
}

static  void    FSinCos( void ) {
/*************************/

    block       *blk;
    instruction *ins;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            if( ins->head.opcode == OP_SIN ) {
                FindSinCos( ins, OP_COS );
            } else if( ins->head.opcode == OP_COS ) {
                FindSinCos( ins, OP_SIN );
            }
        }
    }
}

static  void   FindSinCos( instruction *ins, opcode_defs next_op ) {
/*****************************************************************/

    instruction *next;
    instruction *new_ins;
    name        *temp;

    next = ins->head.next;
    for( ;; ) {
        if( next->head.opcode == OP_BLOCK ) return;
        if( ReDefinedBy( next, ins->operands[ 0 ] ) ) return;
        if( next->head.opcode == next_op ) {
            if( next->operands[ 0 ] == ins->operands[ 0 ] &&
                next->type_class == ins->type_class ) break;
        }
        next = next->head.next;
    }
    temp = AllocTemp( ins->type_class );
    new_ins = MakeUnary( next_op, ins->operands[0], temp, ins->type_class );
    next->head.opcode = OP_MOV;
    next->operands[ 0 ] = temp;
    SuffixIns( ins, new_ins );
    UpdateLive( ins, next );
}
