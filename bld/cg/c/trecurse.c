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
* Description:  Tail recursion elimination.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "dump.h"
#include "data.h"
#include "makeins.h"

extern  bool            SideEffect(instruction*);
extern  pointer         SafeRecurse(pointer(*)(),pointer);
extern  name            *AllocTemp(type_class_def );
extern  void            PrefixIns(instruction*,instruction*);
extern  void            SuffixIns(instruction *,instruction *);
extern  void            ReplIns(instruction *,instruction *);
extern  void            RemoveInputEdge( block_edge * );
extern  void            PointEdge( block_edge *, block * );

static  name            *ReturnValue;

#define _TR_LINK( x )   x->u2.parm_list
#define _PROC_LINK( x ) x->parms_list


static instruction *FindPreviousIns( instruction *curr )
/*******************************************************
    Return the instruction which would be executed immediately prior
    to the given instruction, ignoring block boundaries and other
    such trivia. Uses the MARKED flag to determine which, of all
    possible input sources, to select.
*/
{
    instruction *prev;
    block       *blk;
    block_edge  *edge;

    prev = curr->head.prev;
    if( prev->head.opcode == OP_BLOCK ) {
        blk = _BLOCK( prev );
        edge = blk->input_edges;
        while( edge != NULL ) {
            if( edge->source->class & BLOCK_MARKED ) break;
            edge = edge->next_source;
        }
        prev = edge->source->ins.hd.prev;
    }
    // note - prev->head.opcode guaranteed not to be OP_BLOCK
    // because of silly OP_NOP appended to every instruction ring
    return( prev );
}


static bool SafeOp( name *op, bool write )
/*****************************************
    Return TRUE if the given operand is not something which
    would indicate an instruction with a possibly useful effect.
    Basically, we are allowed to ignore instructions which simply
    shuffle temporaries and constants around, as well as stuff
    which reads registers. Writing a register is bad news though.
*/
{
    if( op == NULL ) return( TRUE );
    switch( op->n.class ) {
    case N_TEMP:
    case N_CONSTANT:
        return( TRUE );
    case N_REGISTER:
        return( !write );
    default:
        return( FALSE );
    }
}

static bool CheckReturn( instruction *ins )
/******************************************
    Check to see if the given instruction, which writes to
    ReturnValue, receives it's value, unmodified, from the
    return value of the recursive call. To do this, backtrack
    through the blocks (always choosing the MARKED one from
    the list of input sources) keeping track of the source of the
    value we are writing into the return register. If, when
    we reach the call (as we must), this value is the return
    value itself, we can declare this instruction safe.
*/
{
    name        *value;

    // since we have already descended past all these instructions on
    // the way down we know the first call we will hit will be the one
    // we are thinking of eliminating
    value = ReturnValue;
    while( ins->head.opcode != OP_CALL ) {
        if( ins->result == value ) {
            if( ins->head.opcode != OP_MOV ) return( FALSE );
            value = ins->operands[ 0 ];
        }
        // following will slide by block boundaries with greatest of ease
        ins = FindPreviousIns( ins );
    }
    return( value == ReturnValue );
}


static void DoOneParm( instruction *parm_ins, instruction *decl_ins, instruction *callins )
/********************************************************************
    Replace the parm_ins with a convert and a move designed to set up
    the parms into the appropriate temps so we can simply jump to the
    start of our function.
*/
{
    instruction         *first;
    instruction         *second;
    name                *tmp;
    name                *src;
    type_class_def      src_class;
    type_class_def      dst_class;

    src = parm_ins->operands[ 0 ];
    src_class = src->n.name_class;
    dst_class = decl_ins->result->n.name_class;
    tmp = AllocTemp( dst_class );
    first = MakeConvert( src, tmp, dst_class, src_class );
    second = MakeMove( tmp, decl_ins->result, dst_class );
    ReplIns( parm_ins, first );
    PrefixIns( callins, second );
}

static void DoTrans( block *blk, instruction *call_ins )
/*******************************************************
    Transform a recursive call into a jump to the
    start of our function.
*/
{
    instruction *ins;
    instruction *next;
    instruction *decl_ins;
    block       *target;
    int         i;
    block_edge  *edge;

    decl_ins = _PROC_LINK( CurrProc );
    for( ins = _TR_LINK( call_ins ); ins != NULL; ins = next ) {
        next = _TR_LINK( ins );
        DoOneParm( ins, decl_ins, call_ins );
        decl_ins = _TR_LINK( decl_ins );
    }

    // delete all instructions in this block from call instruction on
    for( ins = call_ins; ins->head.opcode != OP_BLOCK; ins = next ) {
        next = ins->head.next;
        FreeIns( ins );
    }

    // make blk jump to the block after the prologue
    target = HeadBlock->edge[ 0 ].destination.u.blk;
    blk->class = ( blk->class | JUMP ) & ~( CONDITIONAL | RETURN | SELECT );

    // remove blk from the input lists of any blocks which it might have
    // previously gone to
    for( i = 0; i < blk->targets; i++ ) {
        edge = &blk->edge[ i ];
        RemoveInputEdge( edge );
    }

    blk->targets = 0;
    PointEdge( &blk->edge[ 0 ], target );
}

static bool SafePath( instruction *ins )
/***************************************
    Return TRUE if the path from the given ins (not OP_BLOCK)
    to the end of the block is safe to ignore for purposes of
    eliminating a call higher up the tree.
*/
{
    int         i;

    for( ; ins->head.opcode != OP_BLOCK ; ins = ins->head.next ) {
        if( SideEffect( ins ) ) return( FALSE );
        if( _OpIsCall( ins->head.opcode ) ) return( FALSE );
        for( i = 0; i < ins->num_operands; i++ ) {
            if( !SafeOp( ins->operands[ i ], FALSE ) ) return( FALSE );
        }
        if( ins->result == ReturnValue ) {
            // check to see if this value we are writing into the
            // return register is the value returned from the
            // recursive call we are eliminating - if so continue
            if( CheckReturn( ins ) ) continue;
        }
        if( !SafeOp( ins->result, TRUE ) ) return( FALSE );
    }
    return( TRUE );
}

static pointer SafeBlock( block *blk )
/*********************************************************
    Return blk if the given block, and all reachable blocks,
    are safe to ignore for purposes of eliminating tail recursion,
    We abuse a pointer as a boolean variable because that is
    what SafeRecurse wants.
*/
{
    int         i;
    block       *dest;
    block       *safe;

    if( blk->class & BLOCK_MARKED ) return( NULL );
    blk->class |= BLOCK_MARKED;
    safe = NULL;
    if( SafePath( blk->ins.hd.next ) ) {
        safe = blk;
        for( i = 0; i < blk->targets; i++ ) {
            dest = blk->edge[ i ].destination.u.blk;
            if( SafeRecurse( SafeBlock, dest ) == NULL ) {
                safe = NULL;
                break;
            }
        }
    }
    blk->class &= ~BLOCK_MARKED;
    return( safe );
}

static bool ScaryOperand( name *var )
/************************************
    return TRUE if the operand given has any properties which
    might make elimination of tail recursion impossible
*/
{
    if( var != NULL ) {
        // if anything has had its address taken we chicken out
        if( var->n.class == N_TEMP ) {
            return( var->v.usage & USE_ADDRESS );
        }
    }
    return( FALSE );
}

static bool ScaryConditions( void )
/**********************************
    Traverse the current function and return
    TRUE if there are any conditions present which
    would scare us out of doing tail recursion
    elimination, such as vars which have had
    their addresses taken...
*/
{
    block       *blk;
    instruction *ins;
    int         i;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            for( i = 0; i < ins->num_operands; i++ ) {
                if( ScaryOperand( ins->operands[ i ] ) ) return( TRUE );
            }
            if( ScaryOperand( ins->result ) ) return( TRUE );
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    return( FALSE );
}

static bool     OkayToTransCall( block *blk, instruction *call_ins )
/*******************************************************************
    Check to see if it is okay to turn this call instruction
    into a jump to eliminate tail recursion.  To see if this
    is safe, we have to follow all paths of execution to the
    return block and make sure that nothing important happens
    along these.
*/
{
    sym_handle  label;
    block       *dest;
    int         i;
    instruction *parm;
    instruction *ins;
    bool        ok;

    label = AskForLblSym( CurrProc->label );
    if( call_ins->operands[ CALL_OP_ADDR ]->v.symbol != label ) {
        return( FALSE );
    }

    // check to make sure length of parm lists are the same
    parm = _TR_LINK( call_ins );
    for( ins = _PROC_LINK( CurrProc ); ins != NULL; ins = _TR_LINK( ins ) ) {
        if( parm == NULL ) return( FALSE );
        parm = _TR_LINK( parm );
    }
    if( parm != NULL ) return( FALSE );

    // check to see if all paths are hazard-free from the
    //  call ins to the return block
    ok = FALSE;
    ReturnValue = call_ins->result;
    blk->class |= BLOCK_MARKED;
    // if the call is in the return block, then there are no
    // paths out of this routine which do not go through it
    // (except perhaps stuff calling aborts routines) so don't
    // bother - better running out of stack than an infinite loop
    // (besides - certain codegen stuff needs a RET block)
    if( ( ( blk->class & RETURN ) == EMPTY ) &&
            SafePath( call_ins->head.next ) ) {
        ok = TRUE;
        for( i = 0; i < blk->targets; i++ ) {
            dest = blk->edge[ i ].destination.u.blk;
            if( SafeBlock( dest ) == NULL ) {
                ok = FALSE;
                break;
            }
        }
    }
    blk->class &= ~BLOCK_MARKED;
    return( ok );
}

extern void     TRAddParm( instruction *call_ins, instruction *parm_ins )
/************************************************************************
    Add another instruction to the list of parameters for the given call
    instruction.
*/
{
    // if either is NULL we've had an error and should refrain from GP faulting
    if( call_ins == NULL || parm_ins == NULL ) return;
    _TR_LINK( parm_ins ) = (void *)_TR_LINK( call_ins );
    _TR_LINK( call_ins ) = (void *)parm_ins;
}

extern void     TRDeclareParm( instruction *parm_ins )
/*****************************************************
    Declare another parm for the current procedure. This parm
    is added to a linked list hanging off of CurrProc.
*/
{
    if( parm_ins == NULL ) return;
    _TR_LINK( parm_ins ) = (void *)_PROC_LINK( CurrProc );
    _PROC_LINK( CurrProc ) = (void *)parm_ins;
}

extern bool     TailRecursion( void )
/************************************
    Eliminate any tail recursion. We assume that, for any call in
    the function, we have a linked list of the parameters to that
    call which was built up while creating the call instruction from
    the tree, through the use of TRAddParm.
    To do the deed, we scramble through the instructions backwards
    following all possible paths of execution. If, on any path, we
    encounter an instruction which might make tail recursion impossible
    to eliminate, we give up. If we encounter a call to ourselves
    before we encounter a useful instruction, we replace the call with
    some code which moves the parameters into the appropriate spots
    and jumps to the block just after the prologue.
*/
{
    block       *blk;
    instruction *ins;
    instruction *next;
    bool        changed;

    changed = FALSE;
    if( _IsntModel( NO_OPTIMIZATION ) &&
        !ScaryConditions() && !BlockByBlock ) {
        blk = HeadBlock;
        while( blk != NULL ) {
            ins = blk->ins.hd.next;
            while( ins->head.opcode != OP_BLOCK ) {
                if( ins->head.opcode == OP_CALL ) {
                    if( OkayToTransCall( blk, ins ) ) {
                        DoTrans( blk, ins );
                        changed = TRUE;
                        break;
                    }
                }
                ins = ins->head.next;
            }
            blk = blk->next_block;
        }
    }
    // now reset our links and flags
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            _TR_LINK( ins ) = NULL;
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    for( ins = _PROC_LINK( CurrProc ); ins != NULL; ins = next ) {
        next = _TR_LINK( ins );
        _TR_LINK( ins ) = NULL;
    }
    return( changed );
}
