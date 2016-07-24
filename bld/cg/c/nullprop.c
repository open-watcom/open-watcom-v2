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
* Description:  Propagate null pointer comparisons.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "cgdefs.h"
#include "data.h"
#include "stack.h"
#include "redefby.h"

extern void             KillCondBlk( block *blk, instruction *ins, int dest );
extern  bool            SideEffect( instruction * );
extern  bool            BlockTrim( void );

extern  void            ClearBlockBits( block_class mask )
/********************************************************/
{
    block               *blk;

    mask = ~mask;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->class &= mask;
    }
}

static  instruction     *CompareIns( block *blk )
/***********************************************/
{
    instruction         *last;

    if( blk->class & CONDITIONAL ) {
        for( last = blk->ins.hd.prev; last->head.opcode != OP_BLOCK; last = last->head.prev ) {
            if( _OpIsCompare( last->head.opcode ) ) {
                return( last );
            }
        }
    }
    return( NULL );
}

static  bool            IsZero( name *op )
/****************************************/
{
    return( op->n.class == N_CONSTANT && op->c.const_type == CONS_ABSOLUTE && op->c.lo.int_value == 0 );
}


typedef struct edge_entry {
    struct edge_entry   *next;
    block_edge          *edge;
} edge_entry;

typedef struct edge_stack {
    edge_entry          *top;
} edge_stack;

static  edge_stack      *InitStack( void )
/****************************************/
{
    edge_stack          *stk;

    stk = CGAlloc( sizeof( edge_stack ) );
    stk->top = NULL;
    return( stk );
}

static  bool            Empty( edge_stack *stk )
/**********************************************/
{
    return( stk->top == NULL );
}

static  void            Push( edge_stack *stk, block_edge *edge )
/***************************************************************/
{
    edge_entry          *new_entry;

    new_entry = CGAlloc( sizeof( edge_entry ) );
    new_entry->edge = edge;
    new_entry->next = stk->top;
    stk->top = new_entry;
}

static  block_edge      *Pop( edge_stack *stk )
/*********************************************/
{
    edge_entry          *top;
    block_edge          *edge;

    if( stk->top != NULL ) {
        top = stk->top;
        edge = top->edge;
        stk->top = top->next;
        CGFree( top );
        return( edge );
    }
    return( NULL );
}

static  void            FiniStack( edge_stack *stk )
/**************************************************/
{
    while( !Empty( stk ) ) {
        Pop( stk );
    }
    CGFree( stk );
}

static  bool            DereferencedBy( instruction *ins, name *ptr )
/********************************************************************
    Return true if the instruction dereferences the given name. Any use as
    an index (with op as index), providing the base is NULL, is considered a deref.
*/
{
    int                 i;
    name                *op;

    op = ins->result;
    for( i = 0; i <= ins->num_operands; i++ ) {
        if( op != NULL && op->n.class == N_INDEXED ) {
            if( ptr == op->i.index ) {
                if( op->i.base == NULL ) {
                    return( true );
                }
            }
        }
        op = ins->operands[i];
    }
    return( false );
}

static  void            PushTargets( edge_stack *stk, block *blk, bool forward )
/******************************************************************************/
{
    block_num           i;
    block_edge          *edge;

    if( forward ) {
        for( i = 0; i < blk->targets; i++ ) {
            Push( stk, &blk->edge[ i ] );
        }
    } else {
        for( edge = blk->input_edges; edge != NULL; edge = edge->next_source ) {
            Push( stk, edge );
        }
    }
}

static  instruction     *NextIns( instruction *ins, bool forward )
/****************************************************************/
{
    instruction         *next;

    if( forward ) {
        next = ins->head.next;
    } else {
        next = ins->head.prev;
    }
    return( next );
}

static  instruction     *FirstIns( block *blk, bool forward )
/***********************************************************/
{
    instruction         *first;

    if( forward ) {
        first = blk->ins.hd.next;
    } else {
        first = blk->ins.hd.prev;
    }
    return( first );
}

static  block           *EdgeBlock( block_edge *edge, bool forward )
/******************************************************************/
{
    if( forward ) {
        return( edge->destination.u.blk );
    } else {
        return( edge->source );
    }
}

static  bool            LastBlock( block *blk, bool forward )
/***********************************************************/
{
    if( forward ) {
        if( blk->class & RETURN ) return( true );
    } else {
        if( blk == HeadBlock ) return( true );
    }
    return( false );
}

enum {
    BLOCK_DEREFS,
    BLOCK_REDEFS,
    BLOCK_NOTHING
};

typedef struct goofy_struct_so_we_can_use_saferecurse {
    block       *blk;
    instruction *ins;
    name        *op;
    bool        forward;
} parm_struct;

static  void            *DominatingDeref( parm_struct *parms );

static  int             BlockSearch( block *blk, instruction *ins, name *op, bool forward )
/******************************************************************************************
    Search the block for something interesting - either a dereference or a
    redefinition of the given operand.
*/
{
    instruction         *curr;

    blk = blk;
    for( curr = ins; curr->head.opcode != OP_BLOCK; curr = NextIns( curr, forward ) ) {
        // if we are going forward, the dereferences take precedence over the
        // redefinitions, the reverse is true when backpeddling
        if( forward ) {
            if( DereferencedBy( curr, op ) )
                return( BLOCK_DEREFS );
            if( _IsReDefinedBy( curr, op ) )
                return( BLOCK_REDEFS );
#if 0
            if( curr->head.opcode == OP_MOV &&
                curr->operands[ 0 ] == op &&
                curr->result != op ) {
                parm_struct parms;
                // we see mov t1 -> t2, and are trying to see if t1 has dominating
                // derefs from this path on - this is true if t2 has dominating
                // derefs from this path on - so we recurse (Yikes!)
                parms.blk = blk;
                parms.ins = curr;
                parms.op = curr->result;
                parms.forward = true;
                if( SafeRecurseCG( (func_sr)DominatingDeref, &parms ) != NULL ) {
                    return( BLOCK_DEREFS );
                }
            }
#endif
        } else {
            if( _IsReDefinedBy( curr, op ) )
                return( BLOCK_REDEFS );
            if( DereferencedBy( curr, op ) ) {
                return( BLOCK_DEREFS );
            }
        }
    }
    return( BLOCK_NOTHING );
}

static  void            *DominatingDeref( parm_struct *parms )
/*************************************************************
    Return true if the given instruction is dominated by a dereference of op. This is not a true
    dominator in the sense of the dragon book, but a dominator in the sense that every path from
    the instruction given to the return encounters a dereference of op (if forward) or every
    path from the first instruction in the function to the instruction given encounters a deref
    of op (if backwards or forwards = false ).
*/
{
    block_edge          *edge;
    edge_stack          *stk;
    int                 result;
    bool                dominated;
    block               *blk;


    // check instructions from ins to end of block
    // also check that op is USE_IN_ANOTHER_BLOCK
    result = BlockSearch( parms->blk, NextIns( parms->ins, parms->forward ), parms->op, parms->forward );
    switch( result ) {
    case BLOCK_DEREFS:
        return( NOT_NULL );
    case BLOCK_REDEFS:
        return( NULL );
    }
    if( LastBlock( parms->blk, parms->forward ) ||
        ( parms->op->v.usage & USE_IN_ANOTHER_BLOCK ) == EMPTY ) return( NULL );
    stk = InitStack();
    PushTargets( stk, parms->blk, parms->forward );
    for( dominated = true; dominated; ) {
        if( Empty( stk ) ) break;
        edge = Pop( stk );
        blk = EdgeBlock( edge, parms->forward );
        if( blk->class & BLOCK_VISITED ) continue;
        result = BlockSearch( blk, FirstIns( blk, parms->forward ), parms->op, parms->forward );
        switch( result ) {
        case BLOCK_DEREFS:
            // continue on with the next block in the old depth-first search
            // this path is ok and we do not need to push any targets
            break;
        case BLOCK_REDEFS:
            // one path encountered a redefinition before it got a deref, so
            // the compare is not dominated by a dereference
            dominated = false;
            break;
        default:
            PushTargets( stk, blk, parms->forward );
            if( LastBlock( blk, parms->forward ) ) {
                // we have hit either the return or head of the function without
                // finding a deref along this particular path, so the compare is
                // not dominated by a dereference.
                dominated = false;
            }
        }
        blk->class |= BLOCK_VISITED;
    }
    FiniStack( stk );
    return( dominated ? NOT_NULL : NULL );
}

extern  void            FloodDown( block *blk, block_class bits )
/***************************************************************/
{
    edge_stack          *stk;
    block_edge          *edge;

    stk = InitStack();
    for(;;) {
        if( ( blk->class & bits ) != EMPTY ) {
            blk->class |= bits;
            PushTargets( stk, blk, true );
        }
        if( Empty( stk ) ) break;
        edge = Pop( stk );
        blk = edge->destination.u.blk;
    }
    FiniStack( stk );
}

static  bool            BlockSideEffect( block *blk )
/***************************************************/
{
    instruction         *ins;

    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        if( _OpIsCall( ins->head.opcode ) ) return( true );
        if( SideEffect( ins ) ) return( true );
    }
    return( false );
}

static  bool            EdgeHasSideEffect( block *blk, instruction *cmp, bool cmp_result )
/****************************************************************************************/
{
    edge_stack          *stk;
    block               *elim;
    block               *taken;
    bool                side_effect;
    block_edge          *edge;

    if( cmp_result ) {
        taken = blk->edge[ _TrueIndex( cmp ) ].destination.u.blk;
        elim = blk->edge[ _FalseIndex( cmp ) ].destination.u.blk;
    } else {
        taken = blk->edge[ _FalseIndex( cmp ) ].destination.u.blk;
        elim = blk->edge[ _TrueIndex( cmp ) ].destination.u.blk;
    }
    ClearBlockBits( BLOCK_VISITED );
    FloodDown( taken, BLOCK_VISITED );
    stk = InitStack();

    /*
     * Flood fill forward looking for a path which has a side effect on it.
     * Paths are killed when we hit a block already seen or which is in
     * the other edge of the graph (the edge which is taken).
     */
    for( side_effect = false; !side_effect; ) {
        if( ( elim->class & BLOCK_VISITED ) == EMPTY ) {
            if( BlockSideEffect( elim ) ) {
                side_effect = true;
                break;
            }
            // should add something here which cuts out if we see a
            // deref of the temp we are searching for - a call to
            // BlockSearch with the appropriate parms should do
            elim->class |= BLOCK_VISITED;
            PushTargets( stk, elim, true );
        }
        if( Empty( stk ) ) break;
        edge = Pop( stk );
        elim = edge->destination.u.blk;
    }
    FiniStack( stk );
    ClearBlockBits( BLOCK_VISITED );
    return( side_effect );
}

static  bool            NullProp( block *blk )
/********************************************/
{
    instruction         *cmp;
    name                **ptr;
    parm_struct         parms;
    int                 dest_index;

    cmp = CompareIns( blk );
    if( cmp == NULL ) return( false );
    switch( cmp->head.opcode ) {
    case OP_CMP_EQUAL:
        dest_index = _FalseIndex( cmp );
        break;
    case OP_CMP_NOT_EQUAL:
        dest_index = _TrueIndex( cmp );
        break;
    default:
        return( false );
    }
    if( IsZero( cmp->operands[ 0 ] ) ) {
        ptr = &cmp->operands[ 1 ];
    } else if( IsZero( cmp->operands[ 1 ] ) ) {
        ptr = &cmp->operands[ 0 ];
    } else {
        return( false );
    }
    parms.blk = blk;
    parms.ins = cmp;
    parms.op = *ptr;
    parms.forward = true;
    ClearBlockBits( BLOCK_VISITED );
    if( DominatingDeref( &parms ) != NULL ) {
        if( !EdgeHasSideEffect( blk, cmp, cmp->head.opcode == OP_CMP_NOT_EQUAL ) ) {
            // only nuke the edge if the code we are removing
            // does not have a side effect or if the dominators are before the compare
            KillCondBlk( blk, cmp, dest_index );
            return( true );
        }
    }
    parms.forward = false;
    ClearBlockBits( BLOCK_VISITED );
    if( DominatingDeref( &parms ) != NULL ) {
        KillCondBlk( blk, cmp, dest_index );
        return( true );
    }
    return( false );
}

extern  void            PropNullInfo( void )
/*******************************************
    Use pointer dereferences as information to enable folding of
    pointer comparisons versus NULL.
*/
{
    block               *blk;
    bool                change;

    if( _IsModel( NO_OPTIMIZATION ) ) return;
    if( _IsModel( NULL_DEREF_OK ) ) return;
    change = false;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        change |= NullProp( blk );
    }
    ClearBlockBits( BLOCK_VISITED );
    if( change ){
        BlockTrim();
    }
}
