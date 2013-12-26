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
* Description:  RISCify complex instructions into load/store model.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cfloat.h"
#include "zoiks.h"
#include "score.h"
#include "makeins.h"
#include "data.h"
#include "namelist.h"

extern  hw_reg_set      *RegSets[];

extern  type_class_def  RegClass(hw_reg_set);
extern  instruction     *PostExpandIns(instruction*);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            SuffixIns(instruction*,instruction*);
extern  name            *AllocRegName(hw_reg_set);
extern  void            UpdateLive(instruction*,instruction*);
extern  void            DupSeg(instruction*,instruction*);
extern  void            DelSeg(instruction*);
extern  void            DeadInstructions(void);
extern  bool            ChangeIns(instruction*,name*,name**,change_type);
extern  bool            DoesSomething( instruction* );
extern  name            *HighPart( name *, type_class_def );
extern  name            *LowPart( name *, type_class_def );
extern  hw_reg_set      FullReg( hw_reg_set );
extern  bool_maybe      ReDefinedBy( instruction *, name * );
extern  void            MoveSegRes( instruction *, instruction * );
extern  void            MoveSegOp( instruction *, instruction *, int );
extern  int             NumOperands( instruction * );
extern  bool            InsOrderDependant( instruction *, instruction * );

static  bool            PreferSize;

static  name    **Enregister( instruction *ins )
/***********************************************

    Try to find an operand that's useful to enregister. For MOV's this
    is a constant whose result is a N_MEMORY, N_INDEXED, or N_TEMP with a
    base (486 has a 1 cycle stall in those cases). For other instructions,
    a N_MEMORY, N_INDEXED, or N_TEMP as an operand or result.
*/
{
    int         i;

    switch( ins->head.opcode ) {
    case OP_MOV:
        if( ins->operands[0]->n.class != N_CONSTANT )
            return( NULL );
        switch( ins->result->n.class ) {
        case N_INDEXED:
            if( ins->result->i.base == NULL )
                break;
            /* fall through */
        case N_MEMORY:
        case N_TEMP:
            return( &ins->operands[0] );
        default:
            break;
        }
        return( NULL );
    case OP_LA:
    case OP_CAREFUL_LA:
    case OP_CONVERT:
    case OP_ROUND:
    case OP_CALL:
    case OP_CALL_INDIRECT:
    case OP_SELECT:
        return( NULL );
    default:
        break;
    }
    /* only RISCify CMPs, TESTs, and MOVs when optimizing for size */
    if( PreferSize && !_OpIsCondition( ins->head.opcode ) )
        return( FALSE );
    for( i = ins->num_operands; i-- > 0; ) {
        switch( ins->operands[i]->n.class ) {
        case N_INDEXED:
        case N_MEMORY:
        case N_TEMP:
            return( &ins->operands[i] );
        default:
            break;
        }
    }
    if( ins->result != NULL && !_OpIsCondition( ins->head.opcode ) ) {
        switch( ins->result->n.class ) {
        case N_INDEXED:
        case N_MEMORY:
        case N_TEMP:
            return( &ins->result );
        default:
            break;
        }
    }
    return( NULL );
}

static  hw_reg_set      *RoverByte;
static  hw_reg_set      *RoverWord;
static  hw_reg_set      *RoverDouble;

static hw_reg_set       *FindRegister( instruction *ins )
/********************************************************

    Find a register of the appropriate type for the instruction.
    Note we use rover pointers for each of the classes so that the
    scheduler will have a better chance to intermix instructions.
*/
{
    hw_reg_set  except;
    hw_reg_set  *regs;
    hw_reg_set  *curregs;
    hw_reg_set  *start;
    hw_reg_set  *first;
    hw_reg_set  **rover_ptr;

    switch( ins->type_class ) {
    case U1:
    case I1:
        start = RegSets[RL_BYTE];
        rover_ptr = &RoverByte;
        break;
    case U2:
    case I2:
        start = RegSets[RL_WORD];
        rover_ptr = &RoverWord;
        break;
#if _TARGET & _TARG_80386
    case U4:
    case I4:
        start = RegSets[RL_DOUBLE];
        rover_ptr = &RoverDouble;
        break;
#endif
    default:
        return( NULL );
    }
    except = ins->head.live.regs;
    HW_TurnOn( except, ins->zap->reg );
    if( ins->result != NULL && ins->result->n.class == N_REGISTER ) {
        HW_TurnOn( except, ins->result->r.reg );
    }
    regs = *rover_ptr;
    if( regs == NULL ) {
        regs = start;
        *rover_ptr = start;
    }
    /* 2006-04-25 RomanT
       When optimizing for size, disable rover pointers and
       reuse registers to decrease number of push'es in function prologue
    */
    if( PreferSize ) {
        regs = start;
    }
    first = regs;
    /* NOTE: assumes at least one register in the set */
    /* 2006-04-25 RomanT
       Code rewriten because first and best reg in list (_AX) was used last
    */
    for( ;; ) {
        curregs = regs;
        ++regs;
        if( HW_CEqual( *regs, HW_EMPTY ) ) {
            regs = start;
        }
        if( !HW_Ovlap( *curregs, except ) ) {
            *rover_ptr = regs;  /* next run will use next register */
            break;
        }
        if( regs == first )
            return( NULL );
        /* only use _AX when optimizing for size */
        if( PreferSize ) {
            return( NULL );
        }
    }
    return( curregs );
}


static  instruction     *MakeGeneratable( instruction *ins )
/***********************************************************

    Make sure the instruction is generatable. We might have a couple of
    optmization reductions to work through.
*/
{
    opcode_entry        *tbl;
    instruction         *next;
    instruction         *prev;

    for( ;; ) {
        next = ins->head.next;
        prev = ins->head.prev;
        ins = PostExpandIns( ins );
        if( next != ins->head.next || prev != ins->head.prev ) {
            _Zoiks( ZOIKS_073 );
        }
        tbl = ins->u.gen_table;
        if( tbl != NULL && tbl->generate < FIRST_REDUCT ) {
            break;
        }
    }
    return( ins );
}


static  bool    LoadStoreIns( instruction *ins )
/***********************************************

    RISCify one instruction. See LdStAlloc for details.
*/
{
    name        **op_ptr;
    name        *op;
    hw_reg_set  *hw_reg;
    instruction *new_ins;
    name        *reg;

    if( !DoesSomething( ins ) )
        return( FALSE );
    op_ptr = Enregister( ins );
    if( op_ptr == NULL )
        return( FALSE );
    hw_reg = FindRegister( ins );
    if( hw_reg == NULL )
        return( FALSE );
    reg = AllocRegName( *hw_reg );
    op = *op_ptr;
    if( op == ins->result ) {
        new_ins = MakeMove( reg, op, ins->type_class );
        MoveSegRes( ins, new_ins );
        ins->result = reg;
        SuffixIns( ins, new_ins );
        MakeGeneratable( new_ins );
    }
    if( op_ptr != &ins->result ) {
        new_ins = MakeMove( op, reg, ins->type_class );
        MoveSegOp( ins, new_ins, 0 );
        *op_ptr = reg;
        PrefixIns( ins, new_ins );
        MakeGeneratable( new_ins );
    }
    ins = MakeGeneratable( ins );
    ins->ins_flags |= INS_RISCIFIED;
    return( TRUE );
}

#if _TARGET & _TARG_IAPX86
static  bool    FixMem16Moves( void ) { return( FALSE ); }
static  void    CompressMem16Moves( void ) {}
#else
static  bool    SplitMem16Move( instruction *ins )
/*************************************************
    Return TRUE if we can split the 16-bit move
    instruction given into two one-byte moves.
*/
{
    instruction *new_h;
    instruction *new_l;
    hw_reg_set  reg;

    if( ins->result->n.class == N_REGISTER ) {
        /* make sure we can tear this thing apart */
        reg = ins->result->r.reg;
        HW_COnlyOn( reg, HW_ABCD );
        if( HW_CEqual( reg, HW_EMPTY ) ) {
            return( FALSE );
        }
        if( ReDefinedBy( ins, ins->operands[0] ) ) {
            return( FALSE );
        }
    }
    new_h = MakeMove( HighPart( ins->operands[0], U1 ), HighPart( ins->result, U1 ), U1 );
    new_l = MakeMove( LowPart( ins->operands[0], U1 ), LowPart( ins->result, U1 ), U1 );

    /* this is cheesy - so that we can recover in case we were unable to
       schedule, we stuff a pointer to the second instruction in one of the
       unused operands of the first move instruction, and the original ins
       in another unused operand.
    */
    new_l->operands[1] = (void *)new_h;
    new_l->ins_flags |= INS_SPLIT;
    new_h->ins_flags |= INS_SPLIT;
    SuffixIns( ins, new_l );
    SuffixIns( ins, new_h );
    FreeIns( ins );

    new_h = MakeGeneratable( new_h );
    new_l = MakeGeneratable( new_l );
    return( TRUE );
}

static  void    RestoreMem16Move( instruction *ins )
/***************************************************
    Put a 16-bit move back together in case we were
    unable to schedule the one-byte moves into being
    faster. See comment about cheesiness above.
*/
{
    instruction *other;
    hw_reg_set  full;

    if( ins->head.next == (instruction *)ins->operands[1] ) {
        other = ins->head.next;
    } else if( ins->head.prev == (instruction *)ins->operands[1] ) {
        other = ins->head.prev;
    } else {
        return;
    }
    if( ( other->ins_flags & INS_SPLIT ) == EMPTY )
        return;
    if( ins->result->n.class == N_REGISTER && other->result->n.class == N_REGISTER ) {
        full = HW_EMPTY;
        HW_TurnOn( full, other->result->r.reg );
        HW_TurnOn( full, ins->result->r.reg );
        ins->result = AllocRegName( full );
        ins->type_class = U2;
        FreeIns( other );
    }
}

static  bool    FixMem16Moves( void )
/******************************
    Look for 16-bit mem moves and turn them into two 1-byte
    mem moves, so as to escape the dreaded size override. If
    we are unable to schedule them, we will crunch them back
    up in LdStCompress. We want to do this as a separate pass
    so that we can fix moves generated by LdStAlloc.
*/
{
    block       *blk;
    instruction *ins;
    bool        changed;
    instruction *next;

    if( OptForSize != 0 )
        return( FALSE );
    if( !_CPULevel( CPU_586 ) )
        return( FALSE );
    changed = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
            next = ins->head.next;      /* list is shifting underneath us */
            if( ins->head.opcode == OP_MOV && TypeClassSize[ins->type_class] == 2 ) {
                switch( ins->operands[0]->n.class ) {
                case N_MEMORY:
                case N_INDEXED:
                case N_TEMP:
                    if( ins->ins_flags & INS_SPLIT ) {
                        changed |= SplitMem16Move( ins );
                    }
                default:
                    break;
                }
            }
        }
    }
    return( changed );
}

static  void    CompressMem16Moves( void )
/***********************************
*/
{
    block       *blk;
    instruction *ins;

    if( !_CPULevel( CPU_586 ) )
        return;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            RestoreMem16Move( ins );
        }
    }
}
#endif



extern  bool    LdStAlloc( void )
/*******************************

    Look for non-move operations with memory operands and change them
    into RISC style load/store instructions. This helps on the 486 and
    up because of instruction scheduling. Return a boolean saying whether
    anything got twiddled so the register scoreboarder can be run again.

    This routine also required for optimization of assignment
    of one copy of same constant to multiple memory locations using
    temporary register.
*/
{
    block       *blk;
    instruction *ins;
    instruction *next;
    bool        changed;

    RoverByte = NULL;
    RoverWord = NULL;
    RoverDouble = NULL;

    /* 2006-04-25 RomanT:
       Run RISCifier for all modes, but sometimes (depending on CPU and
       optimization) prefer shorter non-RISC version of instructions.
    */
    PreferSize = FALSE;
    if( OptForSize > 50 ) {
        PreferSize = TRUE;
    }
    if( !_CPULevel( CPU_486 ) ) {
        PreferSize = TRUE;
    }

#if 0  /* You can optionally disable riscifer when optimizing for size */
    if (PreferSize) return( FALSE );
#endif

    changed = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
            next = ins->head.next;      /* list is shifting underneath us */
            if( LoadStoreIns( ins ) ) {
                UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
                changed = TRUE;
            }
        }
    }
    changed |= FixMem16Moves();
    return( changed );
}

static bool CanCompressOperand( instruction *ins, name **popnd )
/**************************************************************/
{
    int         i;
    hw_reg_set  reg;
    name        *op;

    reg = (*popnd)->r.reg;
    if( HW_Ovlap( reg, ins->head.next->head.live.regs ) ) {
        return( FALSE );
    }
    // make sure that the REG is not used in any operands besides
    // the one which we are thinking of replacing BBB - Dec 4, 1993
    for( i = 0; i < ins->num_operands; i++ ) {
        if( &ins->operands[i] != popnd ) {
            op = ins->operands[i];
            if( op->n.class == N_REGISTER && HW_Ovlap( op->r.reg, reg ) ) {
                return( FALSE );
            }
        }
    }
    // make sure that the REG is not used in result
    op = ins->result;
    if( op != NULL ) {
        switch( op->n.class ) {
        case N_REGISTER:
            if( HW_Ovlap( op->r.reg, reg ) )
                return( FALSE );
            break;
        case N_INDEXED:
            if( HW_Ovlap( op->i.index->r.reg, reg ) )
                return( FALSE );
            break;
        default:
            break;
        }
    }
    return( TRUE );
}

static bool     CanCompressResult( instruction *ins,
                                   name *prev_op0, instruction *next,
                                   name **popnd )
/**********************************************************************/
{
    int         i;
    hw_reg_set  reg;
    name        *op;

    reg = ins->result->r.reg;
    if( HW_Ovlap( reg, next->head.next->head.live.regs ) ) {
        return( FALSE );
    }
    if( popnd != NULL ) {
        if( *popnd != ins->result ) {
            return( FALSE );
        }
        if( next->result != prev_op0 ) {
            return( FALSE );
        }
    } else {
        for( i = 0; i < ins->num_operands; ++i ) {
            op = ins->operands[i];
            if( op->n.class == N_REGISTER && HW_Ovlap( op->r.reg, reg ) ) {
                return( FALSE );
            }
        }
    }
    return( TRUE );
}

static void     CompressIns( instruction *ins )
/**************************************************

    See if a RISCified instruction 'ins' is still in CISCifyable form and
    do it to it.
*/
{
    instruction *next;
    instruction *prev;
    name        *prev_op0;
    name        **presult;
    name        **popnd;
    name        **preplace;
    name        *replacement;
    int         i;
    int         num_operands;

    if( !(ins->ins_flags & INS_RISCIFIED) )
        return;
    switch( ins->head.opcode ) {
    case OP_PUSH:
    case OP_POP:
        /* If size preferable then push must be compacted */
        if( PreferSize )
            break;
        /* It's better to use a register for PUSH/POP on a 486 */
        if( _CPULevel( CPU_486 ) ) {
            return;
        }
    default:
        break;
    }
    next = ins->head.next;
    if( next->head.opcode != OP_MOV || next->operands[0]->n.class != N_REGISTER ) {
        next = NULL;
    }
    prev = ins->head.prev;
    prev_op0 = prev->operands[0];
    if( prev->head.opcode != OP_MOV || prev->result->n.class != N_REGISTER ) {
        /* 2006-10-14 RomanT
         * Special case: "MOV REG, 0" usually reduced to "XOR REG, REG",
         * changing opcode and confusing deriscifier. XOR is shorter then
         * non-optimized 16- or 32-bit MOV, but worse for 8-bit moves
         * (all kinds) and 16-bit moves to temp var (via ebp/esp)
         * (same size, two commands, extra register occupied).
         *
         * Handling these XOR's everywhere is boring. It must be rewriten
         * in some other way. May be we shall instroduce G_SMARTMOV which must
         * be resolved to XOR/AND 0/OR -1/XOR+INC only during generation
         * of machine code.
         */
        if ( prev->head.opcode == OP_XOR            &&
             prev_op0 == prev->operands[1]          &&
             ( TypeClassSize[prev->type_class] == 1
#if _TARGET & _TARG_IAPX86  /* Does not work right on 386 - temps becomes 32-bit much later. Todo. */
               || ( TypeClassSize[prev->type_class] == 2 && ins->result && ins->result->n.class == N_TEMP )
#endif
             )
            ) {
            prev_op0 = AllocIntConst( 0 );  /* fake "MOV RESULT, 0" */
        } else {
            prev = NULL;
        }
    }

    presult = NULL;
    popnd = NULL;
    if( next != NULL && ins->result == next->operands[0] ) {
        presult = &ins->result;
    }
    if( prev != NULL ) {
        // 2005-04-05 RomanT
        // Do not use ins->num_operands here, otherwise we'll falsely trigger
        // compression for segment operand of instruction which we shouldn't.
        // (bug #442)
        num_operands = NumOperands( ins );
        for( i = 0; i < num_operands; ++i ) {
            if( prev->result == ins->operands[i] ) {
                popnd = &ins->operands[i];
            }
        }
    }
    // 2006-05-19 RomanT
    // Even if compression of result failed, we must try to compress operands
    if( presult != NULL && CanCompressResult( ins, prev_op0, next, popnd ) ) {
        replacement = next->result;
        preplace = presult;
    } else if( popnd != NULL && CanCompressOperand( ins, popnd ) ) {
        replacement = prev_op0;
        preplace = popnd;
        presult = NULL;     // Forget about result (don't free ins below!)
    } else {
        return;
    }
    if( !ChangeIns( ins, replacement, preplace, CHANGE_GEN | CHANGE_ALL ) )
        return;
    if( presult != NULL ) {
        DupSeg( next, ins );
        FreeIns( next );
    }
    if( popnd != NULL ) {
        DupSeg( prev, ins );
        FreeIns( prev );
    }
    UpdateLive( ins->head.prev, ins );
}


extern  void    LdStCompress( void )
/**********************************

    Compress any load/store sequences generated by LdStAlloc back
    into memory ops if no optimizations made use of them.
*/
{
    block       *blk;
    instruction *ins;

    /* Note: LdStAlloc() must be called first to set PreferSize variable */

    CompressMem16Moves();
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            CompressIns( ins );
        }
    }
    DeadInstructions();
}
