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
* Description:  Instruction scoreboarding.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "score.h"
#include "makeins.h"
#include "data.h"
#include "x87.h"
#include "rgtbl.h"
#include "expand.h"
#include "namelist.h"
#include "split.h"


extern  int             NumOperands(instruction*);
extern  bool            IndexOkay(instruction*,name*);
extern  bool            CanReplace(instruction*);

opcode_entry    *ResetGenEntry( instruction *ins )
/************************************************/
{
    opcode_entry        *try;
    bool                dummy;

    try = FindGenEntry( ins, &dummy );
#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
    /*
     * Real architectures are orthogonal and don't need to swap
     * op's. BBB
     */
    switch( try->generate ) {
    case R_SWAPOPS:
        rSWAPOPS( ins );
        try = FindGenEntry( ins, &dummy );
        break;
    case R_SWAPCMP:
        rSWAPCMP( ins );
        try = FindGenEntry( ins, &dummy );
        break;
    }
#endif
    return( try );
}


bool    ChangeIns( instruction *ins, name *to, name **op, change_type flags )
/****************************************************************************
    Is it alright to change operand "*op" to a reference to "to" in
    instruction "ins".  We check here that the instruction would still
    be generatable by looking up its generate entry.  We must also check
    that it sets the condition codes the same way as the original
    instruction would.  For example, SUB R1,1 => R1 becoming LA 1[R1] =>
    R1 would be no good if the first form set condition codes but the
    second form did not.
*/
{
    opcode_entry        *try;
    opcode_entry        *table;
    opcode_entry        *gen_table;
    name                *save_result;
    name                *old_op;
    int                 i;
    name                *save_ops[MAX_OPS_PER_INS];
    type_class_def      class;
    bool                ok;

    ok = true;
    table = ins->table;
    gen_table = ins->u.gen_table;
    old_op = *op;
    if( ins->head.opcode == OP_CONVERT ) {
        class = ins->base_type_class;
    } else {
        class = ins->type_class;
    }
    if( to->n.class != N_CONSTANT ) {
        if( TypeClassSize[class] != to->n.size ) return( false );
    }
    for( i = ins->num_operands; i-- > 0; ) {
        save_ops[i] = ins->operands[i];
    }
    save_result = ins->result;
    if( ( flags & CHANGE_ALL ) || to->n.class == N_INDEXED ) {
        for( i = ins->num_operands; i-- > 0; ) {
            if( ins->operands[i] == old_op ) {
                ins->operands[i] = to;
            }
        }
        if( ins->result == old_op ) {
            ins->result = to;
        }
    } else {
        *op = to;
    }
    if( ( flags & CHANGE_GEN ) ) {
        try = ResetGenEntry( ins );
        if( try->generate < G_UNKNOWN &&
            (( (ins->ins_flags & INS_CC_USED) == 0)
                || ((ins->u.gen_table->op_type & MASK_CC) ==
                    (gen_table->op_type & MASK_CC))) ) {
            if( to->n.class != N_INDEXED && old_op->n.class == N_INDEXED ) {
                ins->num_operands = NumOperands( ins );
            }
        } else {
            ok = false;
        }
    }
    if( ( ok == false ) || ( flags & CHANGE_CHECK ) ) {
        // if we failed or we were just checking then
        // restore the original instruction
        for( i = ins->num_operands; i-- > 0; ) {
            ins->operands[i] = save_ops[i];
        }
        ins->result = save_result;
        ins->table = table;
        ins->u.gen_table = gen_table;
    }
    return( ok );
}


static  bool    TryOldIndex( score *sc, instruction *ins, name **opp ) {
/***********************************************************************
    Try to find an 'older' index register to replace the index of
    "*opp", in instruction "ins", given register scoreboard "sc".
*/

    name        *op;
    score       *this_reg;
    score       *curr_reg;
    name        *index;
    name        *reg_name;

    op = *opp;
    if( op->n.class != N_INDEXED ) return( false );
    if( op->i.index->n.class != N_REGISTER ) return( false );
    if( op->i.index->r.reg_index == NO_INDEX ) return( false );
    this_reg = &sc[op->i.index->r.reg_index];
    for( curr_reg = this_reg->next_reg; curr_reg != this_reg; curr_reg = curr_reg->next_reg ) {
        if( curr_reg->generation < this_reg->generation ) {
            reg_name = ScoreList[curr_reg->index]->reg_name;
            // OOPS! what about "op [edx] -> [edx]" which zaps edx...
            // if( opp != &ins->result ||       BBB - Feb 18 - 1994
            if( *opp != ins->result ||
                !HW_Ovlap( reg_name->r.reg, ins->zap->reg ) ) {
                index = ScaleIndex( reg_name,
                                    op->i.base,
                                    op->i.constant,
                                    op->n.name_class,
                                    op->n.size, op->i.scale,
                                    op->i.index_flags );
                if( IndexOkay( ins, index )
                 && ChangeIns( ins, index, opp, CHANGE_NORMAL ) ) return( true );
            }
        }
    }
    return( false );
}

static  bool    TryRegOp( score *sc, instruction *ins, name **opp ) {
/********************************************************************
    See if we can find an equivalent register operand for the operand
    "*opp" in instruction "ins", given that the current state of
    registers is reflected by scoreboard "sc".
*/

    name        *op;
    int         i;
    hw_reg_set  live;
    score       *this_reg;
    score       *curr_reg;
    score_info  info;

    if( CanReplace( ins ) == false ) return( false );
    op = *opp;
    if( op->n.class == N_REGISTER ) {
        live = ins->head.next->head.live.regs;
        this_reg = &sc[op->r.reg_index];
        if( !HW_Ovlap( live, op->r.reg ) ) {
            for( curr_reg = this_reg->next_reg; curr_reg != this_reg; curr_reg = curr_reg->next_reg ) {
                if( HW_Ovlap( live, ScoreList[curr_reg->index]->reg )
                && curr_reg->generation < this_reg->generation
                && ChangeIns( ins, ScoreList[curr_reg->index]->reg_name,
                              opp, CHANGE_GEN ) ) {
                     return( true );
                }
            }
        }
        for( curr_reg = this_reg->next_reg; curr_reg != this_reg; curr_reg = curr_reg->next_reg ) {
            if( curr_reg->generation < this_reg->generation
             && ChangeIns( ins, ScoreList[curr_reg->index]->reg_name,
                           opp, CHANGE_GEN ) ) {
                return( true );
            }
        }
        return( false );
    } else {
        ScoreInfo( &info, op );
        if( info.class == N_CONSTANT ) {
            if( _OpIsCondition( ins->head.opcode ) &&
                info.symbol.p == NULL && info.offset == 0 ) {
                /* don't change cmp x,0 */
                return( false );
            }
            if( _IsFloating( ins->type_class ) ) {
                /* careful -- info->offset is NOT right for FP consts! */
                return( false );
            }
        }
        for( i = ScoreCount; i-- > 0; ) {
            if( ScoreEqual( sc, i, &info )
             && ChangeIns( ins, ScoreList[i]->reg_name, opp, CHANGE_GEN ) )
                return( true );
        }

        /*% couldn't find a register operand, try for an older index*/
        return( TryOldIndex( sc, ins, opp ) );
    }
}


bool    FindRegOpnd( score *sc, instruction *ins )
/*************************************************
    See if we can find an operand of "ins" that could be replaces by a
    register or an 'older' register (one that was defined first).
*/
{
    int         i;
    bool        change;

    if( IsStackReg( ins->result ) ) return( false );
    change = false;
    for( i = NumOperands( ins ); i-- > 0; ) {
        if( TryRegOp( sc, ins, &ins->operands[i] ) ) {
            change = true;
        }
    }
    if( ins->result != NULL ) {
        if( TryOldIndex( sc, ins, &ins->result ) ) {
            change = true;
        }
    }
    return( change );
}

void    ScoreMakeEqual( score *sc, name *op1, name *op2 )
/********************************************************
    Make 'op1' and 'op2' equivalent in scoreboarder information
        - one of them must be a register
*/
{
    int         op2_index;
    name        *tmp;
    score_info  info;

    if( op1->n.class == N_REGISTER ) {
        tmp = op1;
        op1 = op2;
        op2 = tmp;
    }
    if( op2->n.class == N_REGISTER ) {
        op2_index = op2->r.reg_index;
        if( op1->n.class == N_REGISTER ) {
            RegAdd( sc, op2_index, op1->r.reg_index );
        } else {
            ScoreInfo( &info, op1 );
            /* NB: reg can never have the value x[reg]*/
            if( info.index_reg == NO_INDEX
                || !HW_Ovlap( op2->r.reg, ScoreList[info.index_reg]->reg ) ) {
                ScoreAssign( sc, op2_index, &info );
            }
        }
    }
}

bool    ScoreMove( score *sc, instruction *ins )
/***********************************************
    Update "sc" to reflect the affect of an OP_MOV instruction "ins" on
    the registers and memory locations.
*/
{
    name        *src;
    name        *dst;
    int         src_index;
    int         dst_index;
    score_info  info;

    src = ins->operands[0];
    dst = ins->result;
    src_index = 0;
    if( src->n.class == N_REGISTER ) {
        src_index = src->r.reg_index;
    }
    if( dst->n.class == N_REGISTER ) {
        dst_index = dst->r.reg_index;
        if( src->n.class == N_REGISTER ) {
            if( RegsEqual( sc,  dst_index, src_index ) ) {
                FreeIns( ins );
                return( true );
            } else {
                RegKill( sc, dst->r.reg );
                RegAdd( sc, dst_index, src_index );
            }
        } else {
            ScoreInfo( &info, src );
            if( ScoreEqual( sc, dst->r.reg_index, &info ) ) {
                FreeIns( ins );
                return( true );
            } else {
                RegKill( sc, dst->r.reg );
                /* NB: reg can never have the value x[reg]*/
               if( info.index_reg == NO_INDEX
                || !HW_Ovlap( dst->r.reg, ScoreList[info.index_reg]->reg ) ) {
                    if( !FPIsConvert( ins ) ) {
                        ScoreAssign( sc, dst_index, &info );
                    }
                }
            }
        }
    } else {
        ScoreInfo( &info, dst );
        if( src->n.class == N_REGISTER ) {       /* and dst is not a register*/
            if( ScoreEqual( sc, src->r.reg_index, &info ) ) {
                FreeIns( ins );
                return( true );
            } else {
                ScoreKillInfo( sc, dst, &info,
                                  src->r.reg );
                if( !FPIsConvert( ins ) ) {
                    ScoreAssign( sc, src_index, &info );
                }
            }
        } else {
            ScoreKillInfo( sc, dst, &info, HW_EMPTY );
        }
    }
    return( false );
}


bool    ScoreLA( score *sc, instruction *ins )
/*********************************************
    Update "sc" to reflect the affect of an OP_MOV instruction "ins" on
    the registers and memory locations.
*/
{
    name        *src;
    name        *dst;
    int         dst_index;
    score_info  info;

    src = ins->operands[0];
    dst = ins->result;
    if( dst->n.class == N_REGISTER ) {
        dst_index = dst->r.reg_index;
        if( !ScoreLAInfo( &info, src ) ) {
            RegKill( sc, dst->r.reg );
        } else if( ScoreEqual( sc, dst->r.reg_index, &info ) ) {
            FreeIns( ins );
            return( true );
        } else {
            RegKill( sc, dst->r.reg );
            ScoreAssign( sc, dst_index, &info );
        }
    } else {
        ScoreInfo( &info, dst );
        ScoreKillInfo( sc, dst, &info, HW_EMPTY );
    }
    return( false );
}


void    ScZeroCheck( score *sc, instruction *ins )
/*************************************************
    Check if instruction "ins" ends up with the result being Zero.  For
    example SUB R1,R1 => R1, results in R1 becoming 0.
*/
{
    int i;

    if( ins->head.opcode != OP_XOR
     && ins->head.opcode != OP_MOD
     && ins->head.opcode != OP_SUB ) return;
    if( ins->operands[0] != ins->operands[1] ) return;
    if( ins->operands[0] != ins->result ) return;
    if( ins->operands[0]->n.class != N_REGISTER ) return;
    i = ins->result->r.reg_index;
    ScoreAssign( sc, i, ScZero );
}
