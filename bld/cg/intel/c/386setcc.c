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
* Description:  386-specific optimizations using the nifty SETcc instruction.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cfloat.h"
#include "data.h"
#include "i64.h"
#include "makeins.h"

extern  void            SuffixIns(instruction*,instruction*);
extern  void            RemoveInputEdge(block_edge *);
extern  void            FlipCond(instruction*);
extern  name            *AllocTemp(type_class_def);
extern  name            *AllocS32Const(signed_32);
extern  name            *AllocS64Const( unsigned_32 low, unsigned_32 high );
extern  void            RemoveBlock( block * );

static  instruction     *SetToConst( block *blk, signed_64 *pcons ) {
/*******************************************************************/

    instruction *ins;
    instruction *next;
    name        *op;

    ins = blk->ins.hd.next;
    while( ins->head.opcode == OP_NOP ) {
        ins = ins->head.next;
    }
    if( ins->head.opcode != OP_MOV ) return( NULL );
    if( _IsFloating( ins->type_class ) ) return( NULL );
    next = ins->head.next;
    while( next->head.opcode == OP_NOP ) {
        next = next->head.next;
    }
    if( next->head.opcode != OP_BLOCK ) return( NULL );
    op = ins->operands[ 0 ];
    if( op->n.class != N_CONSTANT || op->c.const_type != CONS_ABSOLUTE ) {
        return( NULL );
    }
    U64Set( pcons, op->c.int_value, op->c.int_value_2 );
    return( ins );
}

/* Take advantage of the SETcc instruction in cases such as
 * x = y ? 3 : 4;
 * by adding a constant to the result of SETcc to directly obtain
 * the result of the assignment.
 */
static  bool    FindFlowOut( block *blk ) {
/*****************************************/

    signed_64           false_cons;
    signed_64           true_cons;
    signed_64           one;
    signed_64           neg_one;
    signed_64           diff;
    instruction         *ins;
    instruction         *ins0;
    instruction         *ins1;
//    instruction         *prev;
    block               *true;
    block               *false;
    block               *join;
    block_edge          *new_edge;
    bool                reverse;
    name                *u1temp;
    name                *temp;
    name                *result;
    name                *konst;
    type_class_def      class;

    ins = blk->ins.hd.prev;
    while( !_OpIsCondition( ins->head.opcode ) ) {
        ins = ins->head.prev;
    }
//    prev = ins->head.prev;
    if( TypeClassSize[ ins->type_class ] > WORD_SIZE ) return( FALSE );
    true = blk->edge[ _TrueIndex( ins ) ].destination;
    if( true->inputs != 1 ) return( FALSE );
    if( true->targets != 1 ) return( FALSE );

    false = blk->edge[ _FalseIndex( ins ) ].destination;
    if( false->inputs != 1 ) return( FALSE );
    if( false->targets != 1 ) return( FALSE );

    join = false->edge[ 0 ].destination;
    if( join != true->edge[ 0 ].destination ) return( FALSE );
    if( join->inputs != 2 ) return( FALSE );
    if( join->class & UNKNOWN_DESTINATION ) return( FALSE );

    ins0 = SetToConst( false, &false_cons );
    if( ins0 == NULL ) return( FALSE );
    ins1 = SetToConst( true, &true_cons );
    if( ins1 == NULL ) return( FALSE );

    I32ToI64( 1, &one );
    I32ToI64( -1, &neg_one );
    U64Sub( &true_cons, &false_cons, &diff );
    if( U64Cmp( &diff, &neg_one ) == 0 ) {
        U64IncDec( &false_cons, -1 );
        reverse = TRUE;
    } else {
        if( U64Cmp( &diff, &one ) != 0 ) return( FALSE );
        reverse = FALSE;
    }
    result = ins0->result;
    if( result != ins1->result ) return( FALSE );
    class = ins0->type_class;
    if( class != ins1->type_class ) return( FALSE );

    if( reverse ) FlipCond( ins );

    u1temp = AllocTemp( U1 );
    temp = AllocTemp( class );

    ins->result = u1temp;
    ins1 = MakeConvert( u1temp, temp, class, U1 );
    SuffixIns( ins, ins1 );
    ins = ins1;

    if( I64Test( &false_cons ) != 0 ) {
        konst = AllocS64Const( false_cons.u._32[I64LO32], false_cons.u._32[I64HI32] );
        ins1 = MakeBinary( OP_ADD, temp, konst, result, class );
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
    new_edge = &blk->edge[ 0 ];
    new_edge->destination = join;
    new_edge->next_source = NULL;
    join->input_edges = new_edge;
    join->inputs = 1;
    blk->class &= ~CONDITIONAL;
    blk->class |= JUMP;
    return( TRUE );
}

extern  bool    SetOnCondition( void ) {
/********************************/

    block       *blk;
    bool        change;

    if( !_CPULevel( CPU_386 ) ) return( FALSE );
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

extern  reg_set_index   SpecialPossible( instruction *ins ) {
/***********************************************************/

    if( ins->result == NULL ) return( RL_ );
    if( _OpIsCondition( ins->head.opcode ) ) return( RL_BYTE );
    return( RL_ );
}
