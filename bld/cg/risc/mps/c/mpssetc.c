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
* Description:  Optimize MIPS conditional set instructions.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cfloat.h"
#include "data.h"
#include "makeins.h"

extern  void            SuffixIns( instruction *, instruction * );
extern  void            RemoveInputEdge( block_edge *);
extern  void            FlipCond( instruction * );
extern  name            *AllocTemp( type_class_def );
extern  name            *AllocS32Const( signed_32 );
extern  instruction     *rSWAPOPS( instruction *ins );
extern  void            RemoveBlock( block * );

/* This code looks for a sequence like the following:
 *
 * if( cond )
 *     result = const;
 * else
 *     result = const -/+ 1;
 *
 * and tries to replace such sequence with a single conditional set
 * instruction, optionally followed by an add of 'const'. Note that on
 * MIPS, we only have slt/slti and sltu/sltiu, which means 'cond' can
 * be just any condition and. Additionally if the condition isn't
 * OP_CMP_LESS, we'll need to tweak the operands.
 */

static  bool    isNiceCondIns( instruction *ins )
/************************************************
* Figure out if an instruction can be converted to OP_SET_LESS
*/
{
    opcode_defs     oc;

    oc = ins->head.opcode;
    if( !_OpIsCondition( oc ) )
        return( FALSE );
    if( oc == OP_CMP_LESS || oc == OP_CMP_GREATER )
        return( TRUE );
    if( (oc == OP_CMP_LESS_EQUAL) && (ins->operands[1]->n.class == N_CONSTANT)
        && (ins->operands[1]->c.const_type == CONS_ABSOLUTE) )
        return( TRUE );
    if( (oc == OP_CMP_GREATER_EQUAL) && (ins->operands[1]->n.class == N_CONSTANT)
        && (ins->operands[1]->c.const_type == CONS_ABSOLUTE) )
        return( TRUE );
    return( FALSE );
}


static  instruction     *SetToConst( block *blk, signed_32 *pcons )
/*****************************************************************/
{
    instruction *ins;
    instruction *next;
    name        *op;

    ins = blk->ins.hd.next;
    while( ins->head.opcode == OP_NOP ) {
        ins = ins->head.next;
    }
    if( ins->head.opcode != OP_MOV )
        return( NULL );
    if( _IsFloating( ins->type_class ) )
        return( NULL );
    next = ins->head.next;
    while( next->head.opcode == OP_NOP ) {
        next = next->head.next;
    }
    if( next->head.opcode != OP_BLOCK )
        return( NULL );
    op = ins->operands[0];
    if( op->n.class != N_CONSTANT || op->c.const_type != CONS_ABSOLUTE )
        return( NULL );
    *pcons = op->c.int_value;
    return( ins );
}


static  bool    FindFlowOut( block *blk )
/***************************************/
{
    signed_32           false_cons;
    signed_32           true_cons;
    instruction         *ins;
    instruction         *ins0;
    instruction         *ins1;
    block               *true;
    block               *false;
    block               *join;
    block_edge          *new_edge;
    bool                reverse;
    name                *u4temp;
    name                *temp;
    name                *result;
    type_class_def      class;
    opcode_defs         oc;

    ins = blk->ins.hd.prev;
    while( !_OpIsCondition( ins->head.opcode ) ) {
        ins = ins->head.prev;
    }
    if( !isNiceCondIns( ins ) )
        return( FALSE );
    if( TypeClassSize[ ins->type_class ] > WORD_SIZE )
        return( FALSE );
    true = blk->edge[ _TrueIndex( ins ) ].destination.u.blk;
    if( true->inputs != 1 )
        return( FALSE );
    if( true->targets != 1 )
        return( FALSE );

    false = blk->edge[ _FalseIndex( ins ) ].destination.u.blk;
    if( false->inputs != 1 )
        return( FALSE );
    if( false->targets != 1 )
        return( FALSE );

    join = false->edge[0].destination.u.blk;
    if( join != true->edge[0].destination.u.blk )
        return( FALSE );
    if( join->inputs != 2 )
        return( FALSE );
    if( join->class & UNKNOWN_DESTINATION )
        return( FALSE );

    ins0 = SetToConst( false, &false_cons );
    if( ins0 == NULL )
        return( FALSE );
    ins1 = SetToConst( true, &true_cons );
    if( ins1 == NULL )
        return( FALSE );
    if( true_cons - false_cons == -1 ) {
        true_cons = false_cons;
        false_cons = true_cons - 1;
        reverse = TRUE;
    } else {
        if( true_cons - false_cons != 1 )
            return( FALSE );
        reverse = FALSE;
    }
    result = ins0->result;
    if( result != ins1->result )
        return( FALSE );
    class = ins0->type_class;
    if( class != ins1->type_class )
        return( FALSE );

    oc = ins->head.opcode;
    if( oc == OP_CMP_GREATER || oc == OP_CMP_GREATER_EQUAL )
        reverse = !reverse;

    /* Replace 'x <= const' with 'x < const + 1' */
    if( oc == OP_CMP_LESS_EQUAL || oc == OP_CMP_GREATER_EQUAL ) {
        signed_32           value;
        name                *op1;

        op1 = ins->operands[1];
        assert( op1->n.class == N_CONSTANT && op1->c.const_type == CONS_ABSOLUTE );
        value = op1->c.int_value;
        if( oc == OP_CMP_LESS_EQUAL )
            value += 1;
        else
            value -= 1;
        ins->operands[1] = AllocS32Const( value );
    }

    /* Can't really reverse condition, must swap operands */
    if( reverse )
        rSWAPOPS( ins );

    u4temp = AllocTemp( U4 );
    temp = AllocTemp( class );

    ins->result = u4temp;
    ins->head.opcode = OP_SET_LESS;

    ins1 = MakeConvert( u4temp, temp, class, U4 );
    SuffixIns( ins, ins1 );
    ins = ins1;

    if( false_cons != 0 ) {
        ins1 = MakeBinary( OP_ADD, temp, AllocS32Const( false_cons ),
                           result, class );
    } else {
        ins1 = MakeMove( temp, result, class );
    }
    SuffixIns( ins, ins1 );

    RemoveInputEdge( join->input_edges );
    RemoveInputEdge( join->input_edges );
    RemoveInputEdge( true->input_edges );
    RemoveInputEdge( false->input_edges );
    true->targets = 0;
    false->targets = 0;
    RemoveBlock( true );
    RemoveBlock( false );

    blk->targets = 1;
    new_edge = &blk->edge[0];
    new_edge->destination.u.blk = join;
    new_edge->next_source = NULL;
    join->input_edges = new_edge;
    join->inputs = 1;
    blk->class &= ~CONDITIONAL;
    blk->class |= JUMP;
    return( TRUE );
}


extern  bool    SetOnCondition( void )
/************************************/
{
    block       *blk;
    bool        change;

    blk = HeadBlock;
    change = FALSE;
    while( blk != NULL ) {
        if( blk->class & CONDITIONAL ) {
            change |= FindFlowOut( blk );
        }
        blk = blk->next_block;
    }
    return( change );
}


extern  reg_set_index   SpecialPossible( instruction *ins )
/*********************************************************/
{
    if( ins->result == NULL )
        return( RL_ );
    if( isNiceCondIns( ins ) )
        return( RL_DWORD );
    return( RL_ );
}
