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


#include "standard.h"
#include "coderep.h"
#include "addrname.h"
#include "model.h"
#include "opcodes.h"
#include "conflict.h"
#include "procdef.h"
#include "pattern.h"
#include "vergen.h"
#include "gen8087.h"
#include "zoiks.h"
#include "cfloat.h"
#include "cgaux.h"
#include "funits.h"
#include "feprotos.h"

extern  block           *HeadBlock;
extern  bool            BlockByBlock;
extern  hw_reg_set      FPRegs[];
extern  name            *FPStatWord;
extern  int             Max87Stk;
extern  bool            Used87;
extern  byte            OptForSize;

extern  void            Opt8087();
extern  bool            DoesSomething(instruction*);
extern  int             NumOperands(instruction*);
extern  name            *AllocRegName(hw_reg_set);
extern  int             Count87Regs(hw_reg_set);
extern  instruction     *MakeBinary(opcode_defs,name*,name*,name*,type_class_def);
extern  name            *AllocIntConst(int);
extern  void            PrefixIns(instruction*,instruction*);
extern  instruction     *MakeMove(name*,name*,type_class_def);
extern  name            *AllocIndex(name*,name*,type_length,type_class_def);
extern  void            ReplIns(instruction*,instruction*);
extern  instruction     *MakeUnary(opcode_defs,name*,name*,type_class_def);
extern  void            SuffixIns(instruction*,instruction*);
extern  void            DoNothing(instruction*);
extern  name            *AllocTemp(type_class_def);
extern  void            AllocALocal(name*);
extern  void            RevCond(instruction*);
extern  void            MoveSegRes(instruction*,instruction*);
extern  void            MoveSegOp(instruction*,instruction*,int);
extern  hw_reg_set      *IdxRegs();
extern  void            FPCalcMax();
extern  void            InitFPStkReq();

extern  opcode_entry    DoNop[];
extern  type_length     TypeClassSize[];

//NYI: probably need more opcode entries for more resolution with func. units
static  opcode_entry    FNOP    = { SETS_CC,  0, G_NO, 0, FU_FOP };


static opcode_entry    RRFBIN[]  = {
                                  { PRESERVE, 0, G_RRFBIN, 0, FU_FADD },
                                  { PRESERVE, 0, G_RRFBIN, 0, FU_FMUL },
                                  { PRESERVE, 0, G_RRFBIN, 0, FU_FDIV },
};
static opcode_entry    RNFBIN[]  = {
                                  { PRESERVE, 0, G_RNFBIN, 0, FU_FADD },
                                  { PRESERVE, 0, G_RNFBIN, 0, FU_FMUL },
                                  { PRESERVE, 0, G_RNFBIN, 0, FU_FDIV },
};
static opcode_entry    RRFBINP[] = {
                                  { PRESERVE, 0, G_RRFBINP, 0, FU_FADD },
                                  { PRESERVE, 0, G_RRFBINP, 0, FU_FMUL },
                                  { PRESERVE, 0, G_RRFBINP, 0, FU_FDIV },
};
static opcode_entry    RNFBINP[] = {
                                  { PRESERVE, 0, G_RNFBINP, 0, FU_FADD },
                                  { PRESERVE, 0, G_RNFBINP, 0, FU_FMUL },
                                  { PRESERVE, 0, G_RNFBINP, 0, FU_FDIV },
};
static opcode_entry    RRFBIND[] = {
                                  { PRESERVE, 0, G_RRFBIND, 0, FU_FADD },
                                  { PRESERVE, 0, G_RRFBIND, 0, FU_FMUL },
                                  { PRESERVE, 0, G_RRFBIND, 0, FU_FDIV },
};
static opcode_entry    RNFBIND[] = {
                                  { PRESERVE, 0, G_RNFBIND, 0, FU_FADD },
                                  { PRESERVE, 0, G_RNFBIND, 0, FU_FMUL },
                                  { PRESERVE, 0, G_RNFBIND, 0, FU_FDIV },
};
static opcode_entry    MRFBIN[]  = {
                                  { PRESERVE, 0, G_MRFBIN, 0, FU_FADD },
                                  { PRESERVE, 0, G_MRFBIN, 0, FU_FMUL },
                                  { PRESERVE, 0, G_MRFBIN, 0, FU_FDIV },
};
static opcode_entry    MNFBIN[]  = {
                                  { PRESERVE, 0, G_MNFBIN, 0, FU_FADD },
                                  { PRESERVE, 0, G_MNFBIN, 0, FU_FMUL },
                                  { PRESERVE, 0, G_MNFBIN, 0, FU_FDIV },
};

static  opcode_entry    MFLD    = { PRESERVE, 0, G_MFLD, 0, FU_FOP };
static  opcode_entry    RFLD    = { PRESERVE, 0, G_RFLD, 0, FU_FOP };
static  opcode_entry    MFST    = { PRESERVE, 0, G_MFST, 0, FU_FOP };
static  opcode_entry    MFST2   = { PRESERVE, 0, G_MFST, 0, FU_FOP };
static  opcode_entry    RFST    = { PRESERVE, 0, G_RFST, 0, FU_FOP };
static  opcode_entry    FCHS    = { PRESERVE, 0, G_FCHS, 0, FU_FOP };
static  opcode_entry    FMATH   = { PRESERVE, 0, G_FMATH, 0, FU_TRIG };
static  opcode_entry    IFUNC   = { NO_CC,    0, G_IFUNC, 0, FU_TRIG };
static  opcode_entry    FCHOP   = { PRESERVE, 0, G_FCHOP, 0, FU_FOP };
static  opcode_entry    FLDZ    = { PRESERVE, 0, G_FLDZ, 0, FU_FOP };
static  opcode_entry    FLD1    = { PRESERVE, 0, G_FLD1, 0, FU_FOP };
static  opcode_entry    FCOMPP  = { SETS_CC,  0, G_FCOMPP, 0, FU_FOP };
static  opcode_entry    MCOMP   = { SETS_CC,  0, G_MCOMP, 0, FU_FOP };
static  opcode_entry    RCOMP   = { SETS_CC,  0, G_RCOMP, 0, FU_FOP };
static  opcode_entry    MFSTNP  = { PRESERVE, 0, G_MFSTNP, 0, FU_FOP };
static  opcode_entry    RFSTNP  = { PRESERVE, 0, G_RFSTNP, 0, FU_FOP };
static  opcode_entry    FWAIT   = { PRESERVE, 0, G_FWAIT, 0, FU_FOP };//NYI:??
static  opcode_entry    FXCH    = { PRESERVE, 0, G_FXCH, 0, FU_FOP };
static  opcode_entry    RC      = { PRESERVE, 0, G_RC, 0, FU_FOP };
#if _TARGET & _TARG_IAPX86
static  opcode_entry    RR1     = { PRESERVE, 0, G_RR1, 0, FU_FOP };
static  opcode_entry    WORDR1  = { PRESERVE, 0, G_WORDR1, 0, FU_FOP };
#endif

static  name    *ST0;
static  name    *ST1;

static  opcode_entry *GenTab( instruction *ins, opcode_entry *array ) {
/*********************************************************************/

    switch( ins->head.opcode ) {
    case OP_DIV:
        return( array + 2 );
    case OP_MUL:
        return( array + 1 );
    default:
        return( array );
    }
}


extern  bool    FPStackReg( name *reg_name ) {
/********************************************/

    int         reg_num;

    reg_num = FPRegNum( reg_name );
    if( reg_num == -1 ) return( FALSE );
    if( reg_num < Max87Stk ) return( TRUE );
    return( FALSE );
}


extern  int     FPRegNum( name *reg_name ) {
/*******************************************
    given a name, return the 8087 register number (0-7) or -1 if
    it isn't an 8087 register
*/

    hw_reg_set  tmp;

    if( reg_name == NULL ) return( -1 );
    if( reg_name->n.class != N_REGISTER ) return( -1 );
    tmp = reg_name->r.reg;
    HW_COnlyOn( tmp, HW_FLTS );
    if( HW_CEqual( tmp, HW_ST0 ) ) return( 0 );
    if( HW_CEqual( tmp, HW_ST1 ) ) return( 1 );
    if( HW_CEqual( tmp, HW_ST2 ) ) return( 2 );
    if( HW_CEqual( tmp, HW_ST3 ) ) return( 3 );
    if( HW_CEqual( tmp, HW_ST4 ) ) return( 4 );
    if( HW_CEqual( tmp, HW_ST5 ) ) return( 5 );
    if( HW_CEqual( tmp, HW_ST6 ) ) return( 6 );
    if( HW_CEqual( tmp, HW_ST7 ) ) return( 7 );
    return( -1 );
}

extern  name    *ST( int num ) {
/*******************************
    return an N_REGISTER for ST(num)
*/

    return( AllocRegName( FPRegs[  num  ] ) );
}

extern  instruction     *PrefFLDOp( instruction *ins,
                                    operand_types op, name *opnd ) {
/*****************************************************************/

    instruction *new_ins;

    switch( op ) {
    case OP_STK0:
    case OP_STKI:
        new_ins = MakeUnary( OP_MOV, opnd, ST0, FD );
        new_ins->u.gen_table = &RFLD;
        break;
    case OP_MEM:
        new_ins = MakeUnary( OP_MOV, opnd, ST0, FD );
        new_ins->u.gen_table = &MFLD;
        MoveSegOp( ins, new_ins, 0 );
        break;
    case OP_CONS:
        new_ins = MakeUnary( OP_MOV, opnd, ST0, FD );
        if( CFTest( opnd->c.value ) ) {
            new_ins->u.gen_table = &FLD1;
        } else {
            new_ins->u.gen_table = &FLDZ;
        }
        break;
    }
    PrefixIns( ins, new_ins );
    return( new_ins );
}


static  void    PrefixFLDOp( instruction *ins, operand_types op, int i ) {
/*************************************************************************
    Prefix the floating point instruction "ins" with an FLD instruction
    for one of its operands (ins->operands[i]).  That operand has
    classification "op".
*/

    name        *opnd;
    instruction *new_ins;

    opnd = ins->operands[ i ];
    ins->operands[ i ] = ST0;
    new_ins = PrefFLDOp( ins, op, opnd );
    new_ins->stk_exit = new_ins->stk_entry + 1;
    ins->stk_entry++;
    ins->operands[ i ] = ST0;
}


extern  bool            FPResultNotNeeded( instruction *ins ) {
/*************************************************************/

    return( ins->u.gen_table == &MFST2 );
}


extern  instruction     *SuffFSTPRes( instruction *ins,
                                      name *opnd, result_type res ) {
/*******************************************************************/

    instruction *new_ins;

    new_ins = MakeUnary( OP_MOV, ST0, opnd, FD );
    if( res == RES_STKI ) {
        new_ins->u.gen_table = &RFST;
    } else if( res == RES_MEM_THROWAWAY ) {
        new_ins->u.gen_table = &MFST2; // so we can turn it into fstp st(0) later
    } else {
        new_ins->u.gen_table = &MFST;
    }
    if( ins->head.opcode != OP_BLOCK ) {
        MoveSegRes( ins, new_ins );
    }
    SuffixIns( ins, new_ins );
    return( new_ins );
}


static  instruction     *SuffixFSTPRes( instruction *ins ) {
/***********************************************************
    Suffix the floating point instruction "ins" with an FSTP instruction
    for its result (ins->result).
*/

    instruction *new_ins;
    name        *opnd;

    opnd = ins->result;
    ins->result = ST0;
    new_ins = SuffFSTPRes( ins, opnd, RES_MEM );
    new_ins->stk_entry = ins->stk_exit + 1;
    ins->stk_exit++;
    return( new_ins );
}

extern  instruction     *SuffFXCH( instruction *ins, int i ) {
/************************************************************/

    instruction *new_ins;

    new_ins = MakeUnary( OP_MOV, ST0, ST(i), FD );
    new_ins->u.gen_table = &FXCH;
    SuffixIns( ins, new_ins );
    new_ins->stk_exit = new_ins->stk_entry;
    return( new_ins );
}

extern  instruction     *PrefFXCH( instruction *ins, int i ) {
/*************************************************************
    Prefix an instruction "ins" with an FXCH ST(i)
*/

    instruction *new_ins;

    new_ins = MakeUnary( OP_MOV, ST0, ST(i), FD );
    new_ins->u.gen_table = &FXCH;
    PrefixIns( ins, new_ins );
    new_ins->stk_exit = new_ins->stk_entry;
    return( new_ins );
}


static  void    PrefixChop( instruction *ins ) {
/***********************************************
    Prefix an instruction "ins" with an instruction that will truncate
    ST(0) to the nearest integer.
*/

    instruction *new_ins;

    if( ins->head.opcode == OP_ROUND ) return;
    if( _IsFloating( ins->result->n.name_class ) ) return;
    new_ins = MakeUnary( OP_MOV, ST0, ST0, FD );
    new_ins->u.gen_table = &FCHOP;
    PrefixIns( ins, new_ins );
    new_ins->stk_exit = ins->stk_entry;
}

static  instruction     *ExpUnary( instruction *ins,
                                    operand_type src, result_type res,
                                    opcode_entry *table ) {
/************************************************************************
    Expand a unary instruction "ins" using classifications "src" and "res"
*/

    instruction *unary;

    ins->u.gen_table = table;
    unary = ins;
    if( src != OP_STK0 ) {
        PrefixFLDOp( ins, src, 0 );
    }
    if( res != RES_STK0 ) {
        ins = SuffixFSTPRes( ins );
    }
    unary->operands[ 0 ] = ST0;
    unary->result = ST0;
    return( ins );
}


static  instruction     *ExpCall( instruction *ins ) {
/*****************************************************
    Expand a call instruction.  Its not really an 8087 instruction but
    we must take into account how many parameters it pops off the 8087
    stack and whether it returns a value on the 8087.  If that return
    value is ignored, we have to FSTP ST(0) to get rid of it.
*/

    instruction *new_ins;

    new_ins = ins;
    if( ins->result != NULL
     && ins->result->n.class == N_REGISTER
     && HW_CEqual( ins->result->r.reg, HW_ST0 ) ) {
        if( ins->flags.call_flags & CALL_IGNORES_RETURN ) {
            new_ins = SuffixFSTPRes( ins );
            new_ins->u.gen_table = &RFST;
        }
    }
    return( new_ins );
}

#define _Move( src, dst ) ( dst + src*RES_NONE )

static  instruction     *ExpMove( instruction *ins,
                                  operand_type src, result_type dst ) {
/**********************************************************************
    Expand a floating point move "ins" using classifications "src" and "dst".
*/

    switch( _Move( src, dst ) ) {
    case _Move( OP_STK1, RES_STK0 ):
    case _Move( OP_STK1, RES_MEM ):
        _Zoiks( ZOIKS_008 );
        break;
    case _Move( OP_MEM , RES_MEM ):
        PrefixFLDOp( ins, src, 0 );
        PrefixChop( ins );
        DoNothing( ins );
        ins = SuffixFSTPRes( ins );
        break;
    case _Move( OP_STK0, RES_STK0 ):
        DoNothing( ins );
        break;
    case _Move( OP_STK0, RES_MEM  ):
        PrefixChop( ins );
        ins->u.gen_table = &MFST;
        break;
    case _Move( OP_MEM , RES_STK0 ):
        ins->u.gen_table = &MFLD;
        break;
    case _Move( OP_CONS, RES_STK0 ):
        if( CFTest( ins->operands[ 0 ]->c.value ) != 0 ) {
            ins->u.gen_table = &FLD1;
       } else {
            ins->u.gen_table = &FLDZ;
        }
        ins->result = ST0;
        break;
    case _Move( OP_CONS, RES_MEM ):
        PrefixFLDOp( ins, src, 0 );
        DoNothing( ins );
        ins = SuffixFSTPRes( ins );
        break;
    default:
        _Zoiks( ZOIKS_009 );
        break;
    }
    return( ins );
}



static  instruction     *ExpPush( instruction *ins, operand_types op ) {
/***********************************************************************
    expand a PUSH instruction.  On the 386 we generate FSTP 0[esp].  On
    the 8086..286 try MOV BP,SP  FSTP 0[bp].  If thats not possible,
    pick a register from SI,DI,BX and generate for PUSH reg, MOV reg,SP,
    FSTP ss:0[reg], POP reg.
*/

    instruction         *new_ins;
    name                *sp;
    int                 size;
    name                *index;

    sp = AllocRegName( HW_SP );
    size = TypeClassSize[ ins->type_class ];
    new_ins = MakeBinary( OP_SUB, sp, AllocIntConst( size ), sp, WD );
    new_ins->u.gen_table = &RC;
    PrefixIns( ins, new_ins );
    #if _TARGET & _TARG_IAPX86
    {
        instruction         *pop_ins;
        hw_reg_set          avail_index;
        hw_reg_set          *idx;

        HW_CAsgn( avail_index, HW_IDX16 );
        HW_TurnOff( avail_index, ins->head.live.regs );
        idx = IdxRegs();
        for(;;) {
            if( HW_CEqual( *idx, HW_EMPTY ) ) {
                HW_CAsgn( avail_index, HW_BP );
                break;
            }
            if( HW_Subset( avail_index, *idx ) ) {
                HW_CAsgn( avail_index, HW_SS );
                HW_TurnOn( avail_index, *idx );
                break;
            }
            ++idx;
        }
        if( _IsntTargetModel( FLOATING_DS ) && _IsntTargetModel( FLOATING_SS ) ) {
            HW_CTurnOff( avail_index, HW_SS );
        }
        index = AllocRegName( avail_index );
        if( !HW_CEqual( avail_index, HW_BP ) ) {
            new_ins = MakeMove( sp, index, WD );
            new_ins->u.gen_table = &RR1;
            PrefixIns( ins, new_ins );
            index = AllocIndex( index, NULL, 0, ins->type_class );
            new_ins = MakeMove( ins->operands[ 0 ], index,ins->type_class );
            ReplIns( ins, new_ins );
            ins = ExpMove( new_ins, op, RES_MEM );
       } else {
            new_ins = MakeUnary( OP_PUSH, index, NULL, WD );
            new_ins->u.gen_table = &WORDR1;
            PrefixIns( ins, new_ins );
            pop_ins = MakeUnary( OP_POP, NULL, index, WD );
            pop_ins->num_operands = 0;
            pop_ins->u.gen_table = &WORDR1;
            SuffixIns( ins, pop_ins );
            new_ins = MakeMove( sp, index, WD );
            new_ins->u.gen_table = &RR1;
            PrefixIns( ins, new_ins );
            index = AllocIndex( index, NULL, 2, ins->type_class );
            new_ins = MakeMove( ins->operands[ 0 ], index,ins->type_class );
            ReplIns( ins, new_ins );
            ins = ExpMove( new_ins, op, RES_MEM );
            ins = pop_ins;
        }
    }
    #elif _TARGET & _TARG_80386
    {
        index = AllocIndex( sp, NULL, 0, ins->type_class );
        new_ins = MakeMove( ins->operands[ 0 ], index,ins->type_class );
        ReplIns( ins, new_ins );
        ins = ExpMove( new_ins, op, RES_MEM );
    }
    #endif
    return( ins );
}



static  instruction     *ExpandFPIns( instruction *ins, operand_type op1,
                                      operand_type op2, result_type res ) {
/********************************************************
    Using the operand/result classifications "op1", "op2", "res", turn
    "ins" into a real live 8087 instruction by deciding what to generate
    for it (setting gen_table).  Note that it may turn into multiple
    8087 instructions.
*/

    if( !_Is87Ins( ins ) ) return( ins );
    if( op2 != OP_NONE ) {
        switch( ins->head.opcode ) {
        case OP_P5DIV:
        case OP_POW:
        case OP_ATAN2:
        case OP_FMOD:
            ExpBinFunc( ins, op1, op2 );
            if( res != RES_STK0 ) {
                ins = SuffixFSTPRes( ins );
            }
            break;
        default:
            switch( res ) {
            case RES_MEM:
                ExpBinary( ins, op1, op2 );
                ins = SuffixFSTPRes( ins );
                break;
            case RES_STK0:
                ExpBinary( ins, op1, op2 );
                break;
            case RES_NONE:
                ExpCompare( ins, op1, op2 );
                break;
            }
            break;
        }
   } else {
        switch( ins->head.opcode ) {
        case OP_PUSH:
            ins = ExpPush( ins, op1 );
            break;
        case OP_NEGATE:
            ins = ExpUnary( ins, op1, res, &FCHS );
            break;
        case OP_FABS:
            ins = ExpUnary( ins, op1, res, &FMATH );
            break;
        case OP_COS:
        case OP_SIN:
        case OP_TAN:
        case OP_SQRT:
        case OP_ATAN:
            if( _FPULevel( FPU_387 ) && _IsTargetModel( I_MATH_INLINE ) &&
                _IsntTargetModel( P5_DIVIDE_CHECK ) ) {
                ins = ExpUnary( ins, op1, res, &FMATH );
            } else {
                ins = ExpUnary( ins, op1, res, &IFUNC );
            }
            break;
        case OP_LOG:
        case OP_LOG10:
        case OP_EXP:
            if( _FPULevel( FPU_387 ) && _IsTargetModel( I_MATH_INLINE ) &&
                _IsntTargetModel( P5_DIVIDE_CHECK ) && OptForSize <= 50 ) {
                ins = ExpUnary( ins, op1, res, &FMATH );
            } else {
                ins = ExpUnary( ins, op1, res, &IFUNC );
            }
            break;
        case OP_ACOS:
        case OP_ASIN:
        case OP_COSH:
        case OP_SINH:
        case OP_TANH:
            ins = ExpUnary( ins, op1, res, &IFUNC );
            break;
        case OP_MOV:
        case OP_CONVERT:
        case OP_ROUND:
            if( ins->result->n.class == N_REGISTER
             && HW_CEqual( ins->result->r.reg, HW_EMPTY ) ) {
                ins->result = ST0;
                ins->u.gen_table = &RFST;
            } else {
                ins = ExpMove( ins, op1, res );
            }
            break;
        }
    }
    return( ins );
}


static  instruction     *DoExpand( instruction *ins ) {
/*************************************************
    Expand one instruction "ins" The bulk of this routine is
    spent classifying the operands and result of the instruction, and if
    they are an 8087 register, adjusting them to the real register
    number to be used in the instruction.
*/


    int                 i;
    int                 reg_num;
    operand_type        op1_type;
    operand_type        op2_type;
    result_type         res_type;

    i = NumOperands( ins );
    op1_type = OP_NONE;
    while( --i >= 0 ) {
        op2_type = op1_type;
        reg_num = FPRegNum( ins->operands[ i ] );
        if( reg_num != -1 ) {
            if( reg_num == 0 ) {
                op1_type = OP_STK0;
           } else {
                op1_type = ins->stk_entry - reg_num;
                ins->operands[ i ] = ST( ins->stk_entry - reg_num );
            }
       } else if( ins->operands[ i ]->n.class == N_CONSTANT ) {
            op1_type = OP_CONS;
       } else {
            op1_type = OP_MEM;
        }
    }
    if( _OpIsCondition( ins->head.opcode ) ) {
        res_type = RES_NONE;
   } else {
        reg_num = FPRegNum( ins->result );
        res_type = RES_NONE;
        if( reg_num != -1 ) {
            res_type = RES_STK0;
            ins->result = ST0;
        } else {
            res_type = RES_MEM;
        }
    }
    return( ExpandFPIns( ins, op1_type, op2_type, res_type ) );
}

static  void    Expand() {
/************************
    Run through the instruction stream expanding each
    instruction.  All 8087 instructions will have
        gen_table->generate == G_UNKNOWN.

*/

    block       *blk;
    instruction *ins;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            if( DoesSomething( ins ) ) {
                if( _OpIsCall( ins->head.opcode ) ) {
                    ins = ExpCall( ins );
                } else if( ins->u.gen_table->generate == G_UNKNOWN ) {
                    ins = DoExpand( ins );
                }
            }
           ins = ins->head.next;
        }
        blk = blk->next_block;
    }
}




#define _OPS( op1, op2 ) ( op1 + op2*OP_NONE )

static  void    ExpBinFunc( instruction *ins,
                            operand_type op1, operand_type op2 ) {
/****************************************************************
    Expand a floating point binary math instructon "ins", like pow,
    atan2, etc using classifications "op1" and "op2".
*/
    switch( _OPS( op1, op2 ) ) {
    case _OPS( OP_STK1, OP_STK1 ):
    case _OPS( OP_MEM , OP_STK1 ):
    case _OPS( OP_CONS, OP_STK1 ):
    case _OPS( OP_STK1, OP_MEM  ):
    case _OPS( OP_STK1, OP_CONS ):
    case _OPS( OP_CONS, OP_CONS ):
        _Zoiks( ZOIKS_010 );
        break;
    case _OPS( OP_STK0, OP_MEM  ):
    case _OPS( OP_STK0, OP_CONS ):
        PrefixFLDOp( ins, op2, 1 );
        PrefFXCH( ins, 1 );
        break;
    case _OPS( OP_STK1, OP_STK0 ):
        PrefFXCH( ins, 1 );
        break;
    case _OPS( OP_STK0, OP_STK0 ):
    case _OPS( OP_MEM , OP_STK0 ):
    case _OPS( OP_CONS, OP_STK0 ):
        PrefixFLDOp( ins, op1, 0 );
        break;
    case _OPS( OP_STK0, OP_STK1 ):
        break;
    case _OPS( OP_MEM , OP_MEM  ):
    case _OPS( OP_CONS, OP_MEM  ):
    case _OPS( OP_MEM , OP_CONS ):
        PrefixFLDOp( ins, op2, 1 );
        PrefixFLDOp( ins, op1, 0 );
        break;
    default:
        _Zoiks( ZOIKS_011 );
        break;
    }
    ins->u.gen_table = &IFUNC;
}


static  void    ExchangeOps( instruction *ins ) {
/***********************************************/

    name        *op1;
    name        *op2;

    op1 = ins->operands[ 0 ];
    op2 = ins->operands[ 1 ];
    ins->operands[ 0 ] = op2;
    ins->operands[ 1 ] = op1;
}


static  void    ExpBinary( instruction *ins,
                           operand_type op1, operand_type op2 ) {
/****************************************************************
    Expand a floating point binary instructon "ins" using
    classifications "op1" and "op2".
*/

  /* expand a binary floating point instruction. op1 is the implied locn.*/

    switch( _OPS( op1, op2 ) ) {
    case _OPS( OP_STK1, OP_STK1 ):
    case _OPS( OP_MEM , OP_STK1 ):
    case _OPS( OP_CONS, OP_STK1 ):
    case _OPS( OP_STK1, OP_MEM  ):
    case _OPS( OP_STK1, OP_CONS ):
    case _OPS( OP_CONS, OP_CONS ):
/*   case 0,6,7,8,9,11,16,21,24:*/
        _Zoiks( ZOIKS_010 );
        break;
    case _OPS( OP_STK0, OP_STK0 ):
        ins->u.gen_table = GenTab( ins, RNFBIN );
        break;
    case _OPS( OP_STK1, OP_STK0 ):
        ins->u.gen_table = GenTab( ins, RNFBINP );
        break;
    case _OPS( OP_MEM , OP_STK0 ):
        ins->u.gen_table = GenTab( ins, MRFBIN );
        break;
    case _OPS( OP_CONS, OP_STK0 ):
        PrefixFLDOp( ins, op1, 0 );
        ins->u.gen_table = GenTab( ins, RRFBINP );
        ins->operands[ 0 ] = ST1;
        break;
    case _OPS( OP_STK0, OP_STK1 ):
        ins->u.gen_table = GenTab( ins, RRFBINP );
        ExchangeOps( ins );
        break;
    case _OPS( OP_STK0, OP_MEM  ):
        ins->u.gen_table = GenTab( ins, MNFBIN );
        ExchangeOps( ins );
        break;
    case _OPS( OP_MEM , OP_MEM  ):
    case _OPS( OP_CONS, OP_MEM  ):
        PrefixFLDOp( ins, op1, 0 );
        ins->u.gen_table = GenTab( ins, MNFBIN );
        ExchangeOps( ins );
        break;
    case _OPS( OP_STK0, OP_CONS ):
        PrefixFLDOp( ins, op2, 1 );
        ins->u.gen_table = GenTab( ins, RNFBINP );
        ins->operands[ 0 ] = ST1;
        ins->operands[ 1 ] = ST0;
        break;
    case _OPS( OP_MEM , OP_CONS ):
        PrefixFLDOp( ins, op2, 1 );
        ins->u.gen_table = GenTab( ins, MRFBIN );
        break;
    default:
        _Zoiks( ZOIKS_011 );
        break;
    }
}


static  void    RevFPCond( instruction *ins ) {
/**********************************************
    Reverse the sense of a floating point comarison (condition).  For
    example, if we're trying to generate OP_COMPARE_GREATER  X, ST(0),
    we would use FCOM X, and then flip the the comparison to
    OP_COMPARE_LESS.  Since condition code scoreboarding has already
    been done, we have to also go to any subsequent basic blocks and
    flip any comparisons that rely on this comparison to set condition
    codes as well.
*/

    instruction         *other;
    block               *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        blk->class &= ~BLOCK_VISITED;
        blk = blk->next_block;
    }
    other = ins;
    while( other->head.opcode != OP_BLOCK ) {
        other = other->head.next;
    }
    RevOtherCond( _BLOCK( other ), ins );
    RevCond( ins );
}



static  void    RevOtherCond( block *blk, instruction *ins ) {
/*************************************************************
    Run through block "blk" and its successors, continuing until we hit
    an instruction that changes the condition codes, or uses a previous
    set of condition codes.  When we hit and instruction that uses
    previous codes, reverse it since we changed the compare instruction
    up in RevFPCond.
*/

    int         i;
    block       *target;

    blk->class |= BLOCK_VISITED;
    for( ;; ) {
        ins = ins->head.next;
        if( ins->head.opcode == OP_BLOCK ) {
            i = blk->targets;
            while( --i >= 0 ) {
                target = blk->edge[ i ].destination;
                if( ( target->class & BLOCK_VISITED ) == EMPTY ) {
                    RevOtherCond( target, (instruction *)&target->ins );
                }
            }
            break;
        }
        if( ( ins->u.gen_table->op_type & MASK_CC ) != PRESERVE ) break;
        if( _OpIsCondition( ins->head.opcode )
          && ins->table == &DoNop ) { /* used cond codes of original ins */
            RevCond( ins );
            ins->table = &FNOP;
            ins->u.gen_table = &FNOP;
        }
    }
}


extern  void    ExpCompare ( instruction *ins,
                             operand_type op1, operand_type op2 ) {
/******************************************************************
    Expand a floating point comparison using classifications "op1" and
    "op2".
*/


    if( !_CPULevel( CPU_386 ) ) {
        if( FPStatWord == NULL && ( !_CPULevel(CPU_286) || _IsEmulation() ) ) {
            FPStatWord = AllocTemp( U2 );
            FPStatWord->v.usage |= VAR_VOLATILE+USE_ADDRESS; /* so that it really gets allocd */
            AllocALocal( FPStatWord );
        }
    }
    switch( _OPS( op1, op2 ) ) {
    case _OPS( OP_STK1, OP_STK1 ):
    case _OPS( OP_MEM , OP_STK1 ):
    case _OPS( OP_CONS, OP_STK1 ):
    case _OPS( OP_STK1, OP_MEM  ):
    case _OPS( OP_STK1, OP_CONS ):
    case _OPS( OP_CONS, OP_CONS ):
/*   case 0,6,7,8,9,11,16,21,24:*/
        _Zoiks( ZOIKS_012 );
        break;
    case _OPS( OP_STK0, OP_STK0 ):
        ins->u.gen_table = &RCOMP;
        break;
    case _OPS( OP_STK1, OP_STK0 ):
        ins->u.gen_table = &FCOMPP;
        RevFPCond( ins );
        break;
    case _OPS( OP_MEM , OP_STK0 ):
        ins->u.gen_table = &MCOMP;
        RevFPCond( ins );
        break;
    case _OPS( OP_CONS, OP_STK0 ):
        PrefixFLDOp( ins, op1, 0 );
        ins->u.gen_table = &FCOMPP;
        break;
    case _OPS( OP_STK0, OP_STK1 ):
        ins->u.gen_table = &FCOMPP;
        break;
    case _OPS( OP_STK0, OP_MEM  ):
        ins->u.gen_table = &MCOMP;
        ExchangeOps( ins );
        break;
    case _OPS( OP_MEM , OP_MEM  ):
    case _OPS( OP_CONS, OP_MEM  ):
        PrefixFLDOp( ins, op1, 0 );
        ins->u.gen_table = &MCOMP;
        ExchangeOps( ins );
        break;
    case _OPS( OP_STK0, OP_CONS ):
        PrefixFLDOp( ins, op2, 1 );
        ins->u.gen_table = &FCOMPP;
        RevFPCond( ins );
        break;
    case _OPS( OP_MEM , OP_CONS ):
        PrefixFLDOp( ins, op2, 1 );
        ins->u.gen_table = &MCOMP;
        RevFPCond( ins );
        break;
    default:
        _Zoiks( ZOIKS_013 );
        break;
    }
}


extern  void    NoPopRBin( instruction *ins ) {
/*********************************************
    Turn a binary floating point instruction into its equivalent version
    that does not pop the stack. For example FMULP ST(1),ST  -> FMUL ST(1),ST
*/

    if( ins->u.gen_table->generate == G_RRFBINP ) {
        ins->u.gen_table = GenTab( ins, RRFBIND );
        ins->stk_exit++;
    } else if( ins->u.gen_table->generate == G_RNFBINP ) {
        ins->u.gen_table = GenTab( ins, RNFBIND );
        ins->stk_exit++;
    }
}


extern  void    NoPopBin( instruction *ins ) {
/*********************************************
    Turn a binary floating point instruction into its equivalent version
    that does not pop the stack. For example FMULP ST(1),ST  -> FMUL X
*/

    if( ins->u.gen_table->generate == G_RRFBINP ) {
        if( ins->operands[0]->n.class == N_REGISTER ) {
            ins->u.gen_table = GenTab( ins, RRFBIN );
        } else {
            ins->u.gen_table = GenTab( ins, MRFBIN );
        }
        ins->stk_exit++;
    } else if( ins->u.gen_table->generate == G_RNFBINP ) {
        if( ins->operands[0]->n.class == N_REGISTER ) {
            ins->u.gen_table = GenTab( ins, RNFBIN );
        } else {
            ins->u.gen_table = GenTab( ins, MNFBIN );
        }
        ins->stk_exit++;
    }
}


extern  void    ToPopBin( instruction *ins ) {
/*********************************************
    Turn a binary floating point instruction into its equivalent version
    that pops the stack. For example FSUB ST,ST(1) -> FSUBRP ST(1),ST
*/

    if( ins->u.gen_table->generate == G_RRFBIN ) {
        ins->u.gen_table = GenTab( ins, RNFBINP );
        ins->stk_exit--;
    } else if( ins->u.gen_table->generate == G_RNFBIN ) {
        ins->u.gen_table = GenTab( ins, RRFBINP );
        ins->stk_exit--;
    }
}


extern  void    ReverseFPGen( instruction *ins ) {
/*********************************************
    Turn a binary floating point instruction into its reverse instruction
    For example FSUB ST(1)  -> FSUBR ST(1)
*/

    switch( ins->u.gen_table->generate ) {
    case G_RRFBIN:
        ins->u.gen_table = GenTab( ins, RNFBIN );
        break;
    case G_RNFBIN:
        ins->u.gen_table = GenTab( ins, RRFBIN );
        break;
    case G_RRFBINP:
        ins->u.gen_table = GenTab( ins, RNFBINP );
        break;
    case G_RNFBINP:
        ins->u.gen_table = GenTab( ins, RRFBINP );
        break;
    case G_MRFBIN:
        ins->u.gen_table = GenTab( ins, MNFBIN );
        break;
    case G_MNFBIN:
        ins->u.gen_table = GenTab( ins, MRFBIN );
        break;
    }
}


extern  void    ToRFld( instruction *ins ) {
/**********************************************
    Turn an FLD x into an FLD ST(i)
*/

    ins->u.gen_table = &RFLD;
}


extern  void    ToRFstp( instruction *ins ) {
/**********************************************
    Turn an FLD x into an FLD ST(i)
*/

    ins->u.gen_table = &RFST;
}


extern  void    NoPop( instruction *ins ) {
/******************************************
    Turn an FSTP instruction into FST.
*/

    if( ins->u.gen_table->generate == G_MFST ) {
        ins->u.gen_table = &MFSTNP;
        ins->stk_exit++;
    } else if( ins->u.gen_table->generate == G_RFST ) {
        ins->u.gen_table = &RFSTNP;
        ins->stk_exit++;
    }
}


extern  void    NoMemBin( instruction *ins ) {
/*********************************************
    FADD x  ==>  FADD ST(0) for example
*/
    if( ins->u.gen_table->generate == G_MRFBIN ) {
        ins->u.gen_table = GenTab( ins, RRFBIN );
    } else if( ins->u.gen_table->generate == G_MNFBIN ) {
        ins->u.gen_table = GenTab( ins, RNFBIN );
    }
}


extern  instruction     *MakeWait() {
/************************************
    Create an "instruction" that will generate an FWAIT.
*/

    instruction *new_ins;

    new_ins = MakeUnary( OP_MOV, ST0, ST0, FD );
    new_ins->u.gen_table = &FWAIT;
    return( new_ins );
}

extern  void    InitFP() {
/*************************
    Initialize.
*/

    Max87Stk = (int)FEAuxInfo( NULL, STACK_SIZE_8087 );
    if( Max87Stk > 8 ) Max87Stk = 8;
    if( Max87Stk < 4 ) Max87Stk = 4;
    Used87 = FALSE;
    InitFPStkReq();
}

extern  void    FPExpand() {
/**************************
    Expand the 8087 instructions.  The instructions so far
    have been assigned registers, with ST(1) ..  ST(Max87Stk) being the
    "stack" portion of the 8087 with the register number indicating the
    depth of the stack at the time the instruction is executed.
    ST(0) is special and always means the current top of stack

*/

    if( _FPULevel( FPU_87 ) ) {
        ST0 = ST( 0 );
        ST1 = ST( 1 );
        Expand();
//      FPCalcMax();
    }
}

