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
#include "opcodes.h"
#include "pattern.h"
#include "regset.h"
#include "procdef.h"
#include "cfloat.h"
#include "model.h"
#include "vergen.h"
#include "zoiks.h"
#include "score.h"

extern  hw_reg_set      *RegSets[];
extern  proc_def        *CurrProc;
extern  block           *HeadBlock;
extern  byte            OptForSize;

extern  type_class_def  RegClass(hw_reg_set);
extern  opcode_entry    *FindGenEntry(instruction*,bool*);
extern  instruction     *PostExpandIns(instruction*);
extern  void            PrefixIns(instruction*,instruction*);
extern  void            SuffixIns(instruction*,instruction*);
extern  name            *AllocRegName(hw_reg_set);
extern  void            UpdateLive(instruction*,instruction*);
extern  instruction     *MakeMove(name*,name*,type_class_def);
extern  void            DupSeg(instruction*,instruction*);
extern  void            DelSeg(instruction*);
extern  void            DoNothing(instruction*);
extern  void            DeadInstructions(void);
extern  bool            ChangeIns(instruction*,name*,name**,change_type);
extern  void            FreeIns(instruction*);
extern  bool            DoesSomething( instruction* );
extern  name            *HighPart( name *, type_class_def );
extern  name            *LowPart( name *, type_class_def );
extern  void            FreeIns( instruction * );
extern  hw_reg_set      FullReg( hw_reg_set );
extern  bool            ReDefinedBy( instruction *, name * );
extern  void            MoveSegRes( instruction *, instruction * );
extern  void            MoveSegOp( instruction *, instruction *, int );

extern  type_length     TypeClassSize[];

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
        if( ins->operands[0]->n.class != N_CONSTANT ) return( NULL );
        switch( ins->result->n.class ) {
        case N_INDEXED:
            if( ins->result->i.base == NULL ) break;
            /* fall through */
        case N_MEMORY:
        case N_TEMP:
            return( &ins->operands[0] );
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
    }
    for( i = ins->num_operands-1; i >= 0; --i ) {
        switch( ins->operands[i]->n.class ) {
        case N_INDEXED:
        case N_MEMORY:
        case N_TEMP:
            return( &ins->operands[i] );
        }
    }
    if( ins->result != NULL && !_OpIsCondition( ins->head.opcode ) ) {
        switch( ins->result->n.class ) {
        case N_INDEXED:
        case N_MEMORY:
        case N_TEMP:
            return( &ins->result );
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
    hw_reg_set  *start;
    hw_reg_set  *first;
    hw_reg_set  **rover_ptr;

    switch( ins->type_class ) {
    case U1:
    case I1:
        start = RegSets[ RL_BYTE ];
        rover_ptr = &RoverByte;
        break;
    case U2:
    case I2:
        start = RegSets[ RL_WORD ];
        rover_ptr = &RoverWord;
        break;
#if _TARGET & _TARG_80386
    case U4:
    case I4:
        start = RegSets[ RL_DOUBLE ];
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
    first = regs;
    /* NOTE: assumes at least one register in the set */
    for(;;) {
        ++regs;
        if( HW_CEqual( *regs, HW_EMPTY ) ) {
            regs = start;
        }
        if( !HW_Ovlap( *regs, except ) ) break;
        if( regs == first ) return( NULL );
    }
    *rover_ptr = regs;
    return( regs );
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
        if( tbl != NULL && tbl->generate < FIRST_REDUCT ) break;
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

    if( !DoesSomething( ins ) ) return( FALSE );
    op_ptr = Enregister( ins );
    if( op_ptr == NULL ) return( FALSE );
    hw_reg = FindRegister( ins );
    if( hw_reg == NULL ) return( FALSE );
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

#if _TARGET & _TARG_80386
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
        if( ReDefinedBy( ins, ins->operands[ 0 ] ) ) {
            return( FALSE );
        }
    }
    new_h = MakeMove( HighPart( ins->operands[ 0 ], U1 ), HighPart( ins->result, U1 ), U1 );
    new_l = MakeMove( LowPart( ins->operands[ 0 ], U1 ), LowPart( ins->result, U1 ), U1 );

    /* this is cheesy - so that we can recover in case we were unable to
       schedule, we stuff a pointer to the second instruction in one of the
       unused operands of the first move instruction, and the original ins
       in another unused operand.
    */
    new_l->operands[ 1 ] = (void *)new_h;
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

    if( ins->head.next == (instruction *)ins->operands[ 1 ] ) {
        other = ins->head.next;
    } else if( ins->head.prev == (instruction *)ins->operands[ 1 ] ) {
        other = ins->head.prev;
    } else {
        return;
    }
    if( ( other->ins_flags & INS_SPLIT ) == EMPTY ) return;
    if( ins->result->n.class == N_REGISTER && other->result->n.class == N_REGISTER ) {
        full = HW_EMPTY;
        HW_TurnOn( full, other->result->r.reg );
        HW_TurnOn( full, ins->result->r.reg );
        ins->result = AllocRegName( full );
        ins->type_class = U2;
        FreeIns( other );
    }
}

static  bool    FixMem16Moves()
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

    if( OptForSize != 0 ) return( FALSE );
    if( !_CPULevel( CPU_586 ) ) return( FALSE );
    changed = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins=blk->ins.hd.next; ins->head.opcode!=OP_BLOCK; ins=next ) {
            next = ins->head.next;      /* list is shifting underneath us */
            if( ins->head.opcode == OP_MOV && TypeClassSize[ ins->type_class ] == 2 ) {
                switch( ins->operands[ 0 ]->n.class ) {
                case N_MEMORY:
                case N_INDEXED:
                case N_TEMP:
                    if( ins->ins_flags & INS_SPLIT ) {
                        changed |= SplitMem16Move( ins );
                    }
                }
            }
        }
    }
    return( changed );
}

static  void    CompressMem16Moves()
/***********************************
*/
{
    block       *blk;
    instruction *ins;

    if( !_CPULevel( CPU_586 ) ) return;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            RestoreMem16Move( ins );
        }
    }
}
#else
static  bool    FixMem16Moves() { return( FALSE ); }
static  void    CompressMem16Moves() {}
#endif



extern  bool    LdStAlloc()
/*******************************

    Look for non-move operations with memory operands and change them
    into RISC style load/store instructions. This helps on the 486 and
    up because of instruction scheduling. Return a boolean saying whether
    anything got twiddled so the register scoreboarder can be run again.
*/
{
    block       *blk;
    instruction *ins;
    instruction *next;
    bool        changed;

    RoverByte = NULL;
    RoverWord = NULL;
    RoverDouble = NULL;
    if( OptForSize > 50 ) return( FALSE );
    if( !_CPULevel( CPU_486 ) ) return( FALSE );

    changed = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins=blk->ins.hd.next; ins->head.opcode!=OP_BLOCK; ins=next ) {
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


static void     CompressIns( instruction *ins )
/**************************************************

    See if a RISCified instruction 'ins' is still in CISCifyable form and
    do it to it.
*/
{
    instruction *next;
    instruction *prev;
    name        **presult;
    name        **popnd;
    name        **preplace;
    name        *replacement;
    int         i;

    if( !(ins->ins_flags & INS_RISCIFIED) ) return;
    switch( ins->head.opcode ) {
    case OP_PUSH:
    case OP_POP:
        /* It's better to use a register for PUSH/POP on a 486 */
        if( _CPULevel( CPU_486 ) ) return;
    }
    next = ins->head.next;
    if( next->head.opcode!=OP_MOV || next->operands[0]->n.class!=N_REGISTER ) {
        next = NULL;
    }
    prev = ins->head.prev;
    if( prev->head.opcode!=OP_MOV || prev->result->n.class!=N_REGISTER ) {
        prev = NULL;
    }
    presult = NULL;
    popnd = NULL;
    if( next != NULL && ins->result == next->operands[0] ) {
        presult = &ins->result;
    }
    if( prev != NULL ) {
        for( i = 0; i < ins->num_operands; ++i ) {
            if( prev->result == ins->operands[i] ) {
                popnd = &ins->operands[i];
            }
        }
    }
    if( presult != NULL ) {
        if( HW_Ovlap( (*presult)->r.reg, next->head.next->head.live.regs ) ) {
            return;
        }
        if( popnd != NULL ) {
            if( *popnd != *presult ) return;
            if( next->result != prev->operands[0] ) return;
        } else {
            for( i = 0; i < ins->num_operands; ++i ) {
                if( ins->operands[i]->n.class != N_REGISTER ) {
                    continue;
                }
                if( HW_Ovlap( ins->operands[i]->r.reg, ins->result->r.reg ) ) {
                    return;
                }
            }
        }
        replacement = next->result;
        preplace = presult;
    } else {
        if( popnd == NULL ) return;
        // make sure that the REG is not used in any operands besides
        // the one which we are thinking of replacing BBB - Dec 4, 1993
        for( i = 0; i < ins->num_operands; i++ ) {
            if( popnd == &ins->operands[ i ] ) continue;
            if( ins->operands[ i ]->n.class != N_REGISTER ) continue;
            if( HW_Ovlap( ins->operands[ i ]->r.reg, (*popnd)->r.reg ) ) {
                return;
            }
        }
        if( HW_Ovlap( (*popnd)->r.reg, ins->head.next->head.live.regs ) ) {
            return;
        }
        replacement = prev->operands[0];
        preplace = popnd;
    }
    if( !ChangeIns( ins, replacement, preplace, CHANGE_GEN | CHANGE_ALL ) ) return;
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


extern  void    LdStCompress()
/**********************************

    Compress any load/store sequences generated by LdStAlloc back
    into memory ops if no optimizations made use of them.
*/
{
    block       *blk;
    instruction *ins;

    if( OptForSize > 50 ) return;
    if( !_CPULevel( CPU_486 ) ) return;

    CompressMem16Moves();
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            CompressIns( ins );
            ins = ins->head.next;
        }
    }
    DeadInstructions();
}
