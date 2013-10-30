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
* Description:  Loop unrolling optimizations. Contains lots of obsolete
*               and/or nonfunctional code. Currently doesn't work at all
*               because other optimizations munge loops into a form this
*               module doesn't expect.
*
****************************************************************************/


#include "cgstd.h"
#include <assert.h>
#include "coderep.h"
#include "indvars.h"
#include "cgmem.h"
#include "cfloat.h"
#include "zoiks.h"
#include "cgaux.h"
#include "data.h"
#include "makeins.h"
#include "dumpio.h"

extern  block           *MakeBlock(code_lbl *,block_num);
extern  instruction     *DupInstrs(instruction*,instruction*,instruction*,induction*,signed_32);
extern  void            MoveEdge(block_edge*,block*);
extern  void            PointEdge(block_edge*,block*);
extern  bool            AnalyseLoop(induction*,bool*,instruction**,block**);
extern  name            *DeAlias(name*);
extern  name            *TempOffset(name *,type_length ,type_class_def );
extern  name            *AllocTemp(type_class_def );
extern  name            *SAllocIndex(name *,name *,type_length ,type_class_def ,type_length );
extern  name            *AllocS32Const(signed_32 );
extern  void            SuffixIns(instruction *,instruction *);
extern  name            *ScaleIndex(name *,name *,type_length ,type_class_def ,type_length ,int ,i_flags );
extern  void            PrefixIns(instruction *,instruction *);
extern  bool            InvariantOp(name *);
extern  induction       *FindIndVar( name *);
extern  void            RemoveInputEdge( block_edge * );
extern  void            SuffixPreHeader( instruction * );
extern  block           *NewBlock( code_lbl *, bool );
extern  void            MarkLoop( void );
extern  void            UnMarkLoop( void );
extern  void            MarkInvariants( void );
extern  void            UnMarkInvariants( void );
extern  instruction     *DupIns( instruction *, instruction *, name *, signed_32 );
extern  void            RemoveBlock( block * );
extern  void            FlipCond( instruction * );
extern  void            RevCond( instruction * );
extern  int             CountIns( block *);
extern  void            MoveDownLoop( block * );
extern  block           *ReGenBlock( block *, code_lbl * );
extern  void            MakeJumpBlock( block *, block_edge * );
extern  void            URBlip( void );

extern type_class_def   Signed[];
extern block            *PreHead;
extern block            *Head;
extern induction        *IndVarList;
extern block            *Loop;

#define Assert( x )     { if( !(x) ) Zoiks( ZOIKS_113 ); }

typedef struct loop_condition {
    byte        opcode;
    induction   *induction;
    name        *invariant;
    block       *exit_edge;
    block       *loop_edge;
    instruction *compare_ins;
    block       *compare_block;
    bool        clean;                  // do we need something to clean up after us?
    bool        complete;               // did we unroll the loop completely
} loop_condition;


extern  void    FixBlockIds( void )
/**********************************
    Fix up the block_id field of temps.
*/
{
    block_num   id;
    block       *blk;
    name        *temp;

    id = 0;
    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        if( temp->t.u.block_id == NO_BLOCK_ID ) {
            temp->t.temp_flags |= VISITED;
        } else {
            temp->t.temp_flags &= ~VISITED;
        }
    }
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        ++id;
        for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
            if( temp->t.temp_flags & VISITED ) continue;
            if( temp->t.u.block_id != blk->id ) continue;
            temp->t.temp_flags |= VISITED;
            temp->t.u.block_id = id;
        }
        blk->id = id;
    }
    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        temp->t.temp_flags &= ~VISITED;
    }
}

extern  block   *DupBlock( block *blk )
/**************************************
    Create a copy of the given block and all the instructions in it.
    This copy is not linked in anywhere. It has room for as many edges
    as blk has targets, but the number of targets is set to 0. There
    are no inputs to this block. The block class is the same as blk.
*/
{
    block       *copy;

    copy = MakeBlock( AskForNewLabel(), blk->targets );
    copy->class = ( blk->class & ~( LOOP_HEADER | ITERATIONS_KNOWN ) );
    copy->id = NO_BLOCK_ID;
    copy->depth = blk->depth;
    copy->gen_id = blk->gen_id;
    copy->ins.hd.line_num = 0;
    copy->next_block = NULL;
    copy->prev_block = NULL;
    DupInstrs( (instruction *)&copy->ins, blk->ins.hd.next, blk->ins.hd.prev, NULL, 0 );
    return( copy );
}

#define COPY_PTR( x )   (x)->v.alter_ego

static  void    ClearCopyPtrs( block *tail )
/******************************************/
{
    block       *blk;

    blk = tail;
    while( blk != NULL ) {
        COPY_PTR( blk ) = NULL;
        blk = blk->u.loop;
    }
}

typedef struct  loop_abstract {
    block       *head;
    block       *tail;
} loop_abstract;

static  block   *DupLoop( block *tail, loop_abstract *loop )
/***********************************************************
    Make a copy of the given loop and return a pointer to its head.
    The blocks in the new loop will be linked together via blk->next_block,
    but they will not be linked into the function blocks. Edges going to
    blocks not in the loop will go to the same edges, others will go to the
    corresponding edge in the new loop.
*/
{
    block       *blk;
    block       *copy;
    block       *prev;
    block       *dest;
    block_num   i;
    block_edge  *edge;
    block       *old_header;

    URBlip();

    prev = copy = NULL;
    ClearCopyPtrs( tail );

    // make a copy of each of the blocks in the original loop
    for( blk = tail; blk != NULL; blk = blk->u.loop ) {
        if( ( blk->class & IGNORE ) != EMPTY ) continue;
        copy = DupBlock( blk );
        COPY_PTR( blk ) = copy;
        COPY_PTR( copy ) = blk;
        if( prev != NULL ) { // link this copy into the list of copied blocks
            prev->u.loop = copy;
            copy->next_block = prev;
            prev->prev_block = copy;
        } else {
            loop->tail = copy;
        }
        prev = copy;
    }
    loop->head = copy;
    if( copy != NULL ) {
        copy->u.loop = NULL; // terminate the list held in blk->u.loop
    }

    // now link the blocks together - for each edge, we point the
    // edge in the corresponding block to the same block if it is in the
    // loop, otherwise we use the copy of the block.
    old_header = Head;
    for( blk = tail; blk != NULL; blk = blk->u.loop ) {
        if( ( blk->class & IGNORE ) != EMPTY ) continue;
        copy = COPY_PTR( blk );
        for( i = 0; i < blk->targets; i++ ) {
            dest = blk->edge[ i ].destination.u.blk;
            if( ( dest->class & IN_LOOP ) != EMPTY ) {
                if( dest != old_header ) {
                    dest = COPY_PTR( dest );
                }
            }
            edge = &copy->edge[ i ];
            PointEdge( edge, dest );
            edge->flags &= ~ONE_ITER_EXIT;
            if( dest == old_header ) {
                edge->flags |= DEST_IS_HEADER;
            }
        }
    }
    return( COPY_PTR( tail ) );
}

static  void    MarkHeaderEdges( block *loop, block *head )
/**********************************************************
    Mark every edge in the loop which points to the given
    header as DEST_IS_HEADER.
*/
{
    block       *blk;
    block_num   i;
    block_edge  *edge;

    for( blk = loop; blk != NULL; blk = blk->u.loop ) {
        for( i = 0; i < blk->targets; i++ ) {
            edge = &blk->edge[ i ];
            if( edge->destination.u.blk == head ) {
                edge->flags |= DEST_IS_HEADER;
            }
        }
    }
}

static  void    RedirectHeaderEdges( block *loop, block *new_head )
/******************************************************************
    Run through the given loop replacing edges which point to the
    loop head with edges which point to new_head.
*/
{
    block       *blk;
    block_num   i;
    block_edge  *edge;

    for( blk = loop; blk != NULL; blk = blk->u.loop ) {
        for( i = 0; i < blk->targets; i++ ) {
            edge = &blk->edge[ i ];
            if( edge->flags & DEST_IS_HEADER ) {
                MoveEdge( edge, new_head );
                edge->source = blk;
            }
        }
    }
}

static  void    UnMarkHeaderEdges( block *loop )
/**********************************************/
{
    block       *blk;
    block_num   i;
    block_edge  *edge;

    for( blk = loop; blk != NULL; blk = blk->u.loop ) {
        for( i = 0; i < blk->targets; i++ ) {
            edge = &blk->edge[ i ];
            edge->flags &= ~DEST_IS_HEADER;
        }
    }
}


#define MAX_CODE_SIZE   0x20
#define UNROLL_MAX      0x20

static  signed_32       UnrollCount( block *loop_tail, bool *clean, bool *complete )
/***********************************************************************************
    Figure out how many times we want to unroll the given loop.
*/
{
    signed_32   num_ins;
    signed_32   unroll_count;
    block       *blk;

    // check out this awesome heuristic...
    *complete = FALSE;
    *clean = FALSE;
    unroll_count = Head->unroll_count;
    if( unroll_count == 0 ) {
        if( _IsntModel( LOOP_UNROLLING ) ) return( 0 );
        if( OptForSize != 0 ) return( FALSE );
        num_ins = 0;
        blk = loop_tail;
        while( blk != NULL ) {
            if( blk->class & SELECT ) return( 0 );
            num_ins += CountIns( blk );
            blk = blk->u.loop;
        }
        if( Head->class & ITERATIONS_KNOWN ) {
            unroll_count = MAX_CODE_SIZE / num_ins;
            while( unroll_count ) {
                if( Head->iterations % ( unroll_count + 1 ) == 0 ) {
                    break;
                }
                unroll_count -= 1;
            }
            if( unroll_count >= ( Head->iterations - 1 ) ) {
                unroll_count = Head->iterations - 1;
                *complete = TRUE;
            }
        } else {
            // don't bother
            // unroll_count = MAX_CODE_SIZE / num_ins;
            // if( unroll_count > UNROLL_MAX ) unroll_count = UNROLL_MAX;
        }
    }
    return( unroll_count );
}

#if 0
static  bool    ReplaceName( name **pop, name *orig, name *new )
/***************************************************************
    Replace all occurrences of orig in *pop with new and return the new name.
*/
{
    name        *op;
    type_length offset;

    op = *pop;
    if( op == NULL ) return( FALSE );
    switch( op->n.class ) {
    case N_INDEXED:
        if( op->i.index == orig ) {
            *pop = ScaleIndex( new, op->i.base,
                               op->i.constant,
                               op->n.name_class, op->n.size,
                               op->i.scale, op->i.index_flags );
            return( TRUE );
        } else if( op->i.base == orig ) {
            *pop = ScaleIndex( op->i.index, new, op->i.constant,
                               op->n.name_class, op->n.size,
                               op->i.scale, op->i.index_flags );
        }
        break;
    case N_TEMP:
        op = DeAlias( op );
        if( op == orig ) {
            offset = (*pop)->v.offset - op->v.offset;
            *pop = TempOffset( new, offset, (*pop)->n.name_class );
            return( TRUE );
        }
        break;
    default:
        if( op == orig ) {
            *pop = new;
            return( TRUE );
        }
    }
    return( FALSE );
}

static  void    ReplaceInductionVars( block *loop, instruction *ins_list,
                                       signed_32 scale )
/*******************************************************
    Replace all occurrences of an induction var in the loop with a new
    temp, and add an instruction to initialize that temp onto the ins_list.
    By the time we get here, everything should be either dead or a basic
    induction var. Scale should be the ordinal of the iteration we are on,
    starting at 1 and incrementing each time this is called on a copy of a
    loop.
*/
{
    induction   *ind;
    name        *temp;
    name        *var;
    instruction *ins;
    instruction *new_ins;
    block       *blk;
    int         i;
    signed_32   adjust;

    for( ind = IndVarList; ind != NULL; ind = ind->next ) {
        if( _IsV( ind, IV_DEAD ) ) continue;
        var = ind->name;
        temp = AllocTemp( var->n.name_class );
        adjust = scale * ind->plus;
        new_ins = MakeBinary( OP_ADD, var, AllocS32Const( adjust ),
                            temp, temp->n.name_class );
        for( blk = loop; blk != NULL; blk = blk->u.loop ) {
            for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
                ReplaceName( &ins->result, var, temp );
                for( i = 0; i < ins->num_operands; i++ ) {
                    ReplaceName( &ins->operands[i], var, temp );
                }
            }
        }
        /* have to add this after we run the list replacing vars */
        PrefixIns( ins_list, new_ins );
        new_ins = MakeMove( temp, var, temp->n.name_class );
        SuffixIns( loop->ins.hd.prev, new_ins );
    }
}

extern  void    DumpLoop( block *loop )
{
    block_edge  *edge;
    block_num   i;

    DumpLiteral( "Block\t\tBlock->u.loop\tBlock->loop_head" );
    DumpNL();
    DumpNL();
    while( loop != NULL ) {
        DumpPtr( loop );
        DumpLiteral( "\t\t" );
        DumpPtr( loop->u.loop );
        DumpLiteral( "\t\t" );
        DumpPtr( loop->loop_head );
        DumpNL();
        DumpLiteral( "\tInputs: " );
        for( edge = loop->input_edges; edge != NULL; edge = edge->next_source ) {
            DumpPtr( edge->source );
            DumpChar( ' ' );
        }
        DumpNL();
        DumpLiteral( "\tDest: " );
        edge = &loop->edge[ 0 ];
        for( i = 0; i < loop->targets; i++ ) {
            DumpPtr( edge->destination.u.blk );
            DumpChar( ' ' );
            edge++;
        }
        DumpNL();
        loop = loop->u.loop;
    }
}
#endif

static  void    LinkBlocks( block *first, block *second )
{
    first->next_block = second;
    second->prev_block = first;
}

static  void    ChainTwoLoops( loop_abstract *first, loop_abstract *last )
/*************************************************************************
    Given two detached copies of a loop, link their blocks
    together and join them via block->u.loop.
*/
{
    LinkBlocks( first->tail, last->head );
    last->head->u.loop = first->tail;
}

static  block   *DoUnroll( block *tail, signed_32 reps, bool replace_vars )
/**************************************************************************
    Unroll the given loop (this is the tail block, and loop is connected
    through blk->u.loop to the head, in which blk->u.loop == NULL) reps
    times (there will be reps + 1 copies of the loop body) and replace induction
    vars with new temps if replace_vars == TRUE
    All of the copies made will be connected through blk->u.loop, and a
    pointer to the tail of the new super-loop will be returned.
*/
{
    loop_abstract       *new_loops;
    loop_abstract       *curr;
    loop_abstract       *next;
    loop_abstract       *first;
    loop_abstract       *last;
    block               *next_block;
    signed_32           i;
    signed_32           size;

    replace_vars = replace_vars;
    size = sizeof( loop_abstract ) * reps;

    // allocate an array of these abstract loop thingies
    new_loops = CGAlloc( size );
    first = &new_loops[ 0 ];
    last = &new_loops[ reps - 1 ];

    // create the actual copies - they will be independant of each other
    for( i = 0; i < reps; i++ ) {
        curr = &new_loops[ i ];
        DupLoop( tail, curr );
    }

    // want last copy to jump to original loop - we do it here because
    // the last copy is going to get linked (via blk->u.loop) into the
    // entire loop after the next pass over the array of copies
    RedirectHeaderEdges( last->tail, Head );

    // also, we make the original loop jump to the first of the duplicates
    RedirectHeaderEdges( tail, first->head );

    // now we need to make each of the loops chain to the next chap in line
    // we also take this opportunity to link the various copies together via
    // blk->next_block and blk->u.loop
    for( i = 1; i < reps; i++ ) {
        curr = &new_loops[ i - 1 ];
        next = &new_loops[   i   ];
        RedirectHeaderEdges( curr->tail, next->head );
        ChainTwoLoops( curr, next );
    }

    // now we just have to ram the entire thing into the block list
    // for this function and link them to the original via blk->u.loop
    next_block = tail->next_block;
    LinkBlocks( tail, first->head );
    LinkBlocks( last->tail, next_block );
    first->head->u.loop = tail;

    next_block = last->tail;
    CGFree( new_loops );

    // and return the tail of the new super-loop
    return( next_block );
}

static  bool    TractableCond( loop_condition *cond )
/****************************************************
    To be a nice conditional exit (one we can munge) we need a comparison
    between an induction variable and a loop-invariant expression. If these
    conditions are present, we fill in the fields and return TRUE. If we
    return FALSE, the values of cond are guaranteed to be irrelevant junk.
*/
{
    bool        ok;
    induction   *tmp;
    signed_32   plus;
//    bool        exit_true;
    byte        opcode;
    name        *n;
    instruction *ins;
    block       *blk;


    ins = cond->compare_ins;
    blk = cond->compare_block;
    ok = FALSE;
//    exit_true = FALSE;
    MarkInvariants();
    if( !InvariantOp( ins->operands[ 1 ] ) ) {
        n = ins->operands[ 0 ];
        ins->operands[ 0 ] = ins->operands[ 1 ];
        ins->operands[ 1 ] = n;
        RevCond( ins );
    }
    if( InvariantOp( ins->operands[ 1 ] ) ) {
        tmp = FindIndVar( ins->operands[ 0 ] );
        if( tmp != NULL ) {
            cond->induction = tmp;
            cond->invariant = ins->operands[ 1 ];
            ok = TRUE;
        }
    }
    UnMarkInvariants();
    if( !ok ) return( FALSE );
    if( _IsV( cond->induction, IV_DEAD ) ) return( FALSE );
    if( ( blk->edge[ 0 ].destination.u.blk->class & IN_LOOP ) != EMPTY ) {
        cond->exit_edge = blk->edge[ 1 ].destination.u.blk;
        cond->loop_edge = blk->edge[ 0 ].destination.u.blk;
        if( _TrueIndex( ins ) == 1 ) {
            // want loop to continue executing if condition TRUE
            FlipCond( ins );
            _SetBlockIndex( ins, 0, 1 );
        }
    } else {
        cond->exit_edge = blk->edge[ 0 ].destination.u.blk;
        cond->loop_edge = blk->edge[ 1 ].destination.u.blk;
        if( _TrueIndex( ins ) == 0 ) {
            // want loop to continue executing if condition TRUE
            FlipCond( ins );
            _SetBlockIndex( ins, 1, 0 );
        }
    }
    plus = cond->induction->plus;
    opcode = ins->head.opcode;
    switch( opcode ) {
    case OP_CMP_NOT_EQUAL:
        if( plus < 0 ) {
            cond->opcode = OP_CMP_GREATER;
        } else {
            cond->opcode = OP_CMP_LESS;
        }
        break;
    case OP_CMP_GREATER:
    case OP_CMP_GREATER_EQUAL:
        cond->opcode = opcode;
        if( plus >= 0 ) ok = FALSE;
        break;
    case OP_CMP_LESS:
    case OP_CMP_LESS_EQUAL:
        cond->opcode = opcode;
        if( plus <= 0 ) ok = FALSE;
        break;
    case OP_CMP_EQUAL:
    case OP_BIT_TEST_TRUE:
    case OP_BIT_TEST_FALSE:
        ok = FALSE;
        break;
    default:
        Zoiks( ZOIKS_113 );
    }
    return( ok );
}

extern  block   *AddBlocks( block *insertion_point, block *block_list )
/**********************************************************************
    Insert the list of blocks after the given insertion point.
*/
{
    block       *last;

    for( last = block_list; last->next_block != NULL; ) {
        last = last->next_block;
    }
    last->next_block = insertion_point->next_block;
    if( last->next_block != NULL ) {
        last->next_block->prev_block = last;
    }
    block_list->prev_block = insertion_point;
    insertion_point->next_block = block_list;
    return( last );
}

extern  void    RemoveIns( instruction *ins )
/********************************************
    Remove the ins from the instruction ring. Does not take
    into account live info or anything else like that.
*/
{
    instruction *next;
    instruction *prev;

    next = ins->head.next;
    prev = ins->head.prev;
    next->head.prev = prev;
    prev->head.next = next;
    ins->head.next = NULL;
    ins->head.prev = NULL;
}

static  block   *MakeNonConditional( block *butt, block_edge *edge )
/*******************************************************************
    If butt is conditional, create a new block which is
    in between the conditional block and the head of the
    current loop. We need a nonconditional block so that
    we can append instructions to it when hoisting the
    condition to the top of the loop.
*/
{
    block       *blk;

    if( ( butt->class & CONDITIONAL ) != EMPTY ) {
        blk = MakeBlock( AskForNewLabel(), 1 );
        blk->class = JUMP | IN_LOOP;
        blk->id = NO_BLOCK_ID;
        blk->gen_id = butt->gen_id;
        blk->ins.hd.line_num = 0;
        blk->next_block = butt->next_block;
        if( blk->next_block != NULL ) {
            blk->next_block->prev_block = blk;
        }
        blk->prev_block = butt;
        butt->next_block = blk;
        blk->loop_head = Head;
        PointEdge( &blk->edge[ 0 ], edge->destination.u.blk );
        MoveEdge( edge, blk );
        UnMarkLoop();
        MarkLoop();
        return( blk );
    }
    return( butt );
}

static  int     ExitEdges( block *head )
/***************************************
    Return the number of edges in the loop with the given head which
    exit the loop.
*/
{
    int         count;
    block       *blk;

    count = 0;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk->loop_head == head || blk == head ) {
            if( blk->class & LOOP_EXIT ) count++;
        }
    }
    return( count );
}

extern  bool    CanHoist( block *head )
/**************************************
    Figure out if we can hoist the condition of a loop to the top of
    the loop. Basically, we can do this as long as there are no
    if's between the head and the comparison instruction.
*/
{
    block       *curr;
    block_edge  *edge;

    /* can't handle crap which isn't either a jump block or a conditional going to our head */
    for( edge = head->input_edges; edge != NULL; edge = edge->next_source ) {
        if( ( edge->source->class & ( JUMP | CONDITIONAL ) ) == EMPTY ) return( FALSE );
    }

    curr = head;
    while( curr != NULL ) {
        if( curr->class & LOOP_EXIT ) return( ExitEdges( head ) == 1 );
        if( curr->targets > 1 ) break;
        curr = curr->edge[ 0 ].destination.u.blk;
        if( curr == head ) break;
    }
    return( FALSE );
}

extern  void    HoistCondition( block *head )
/********************************************
    We want to knock stuff off the top of a loop until the condition
    which controls exit from the loop is the first statement in the
    loop. To do this, we remove each instruction and append it to the
    prehead, while also appending a copy of the instruction to the
    loop butts. Note that this could violate the limit on instructions
    per block, but I'm not checking at the moment. You may only call
    this routine if the above function returned TRUE.
*/
{
    block_edge  *edge;
    block       *blk;
    instruction *ins, *next;

    for( edge = head->input_edges; edge != NULL; edge = edge->next_source ) {
        if( edge->source->targets > 1 ) {
            MakeNonConditional( edge->source, edge );
        }
    }

    for( blk = head; blk != NULL; blk = blk->edge[ 0 ].destination.u.blk ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
            if( _OpIsCondition( ins->head.opcode ) ) {
                RemoveIns( ins );
                SuffixIns( head->ins.hd.prev, ins );
                return;
            }
            for( edge = head->input_edges; edge != NULL; edge = edge->next_source ) {
                DupIns( edge->source->ins.hd.prev, ins, NULL, 0 );
            }
            next = ins->head.next;
            FreeIns( ins );
        }
    }
    // should never reach here because we must have a conditional statement
    Zoiks( ZOIKS_113 );
}

#if 0
extern  void    HoistCondition( block **head, block *prehead )
/*************************************************************
    Munge the loop and prehead so that the condition is the first statement in
    the loop. This only works for a loop which has no conditional blocks in it
    other than the exit condition. To do this, we hoist each instruction from
    the head and make a copy appended to both the prehead and butt (my quaint
    notation for the block which jumps to the head and is not the prehead). We
    return TRUE if we were able to hoist the condition to the top, FALSE otherwise.
*/
{
    block       *butt;
    block_edge  *butt_edge;
    instruction *ins;
    instruction *next;
    instruction *butt_ins;
    instruction *prehead_ins;
    block       *blk;

    for( butt_edge = (*head)->input_edges; butt_edge != NULL; butt_edge = butt_edge->next_source ) {
        if( butt_edge->source != prehead ) break;
    }
    if( butt_edge == NULL ) return( FALSE );
    butt = MakeNonConditional( butt_edge->source, butt_edge );
    butt_ins = butt->ins.hd.prev;
    prehead_ins = prehead->ins.hd.prev;
    blk = *head;
    // it should be either a simple jump or our (1 and only) cond. exit
    if( ( blk->class & JUMP ) == EMPTY &&
        ( blk->class & CONDITIONAL ) == EMPTY ) return( FALSE );
    // the new head should have an input from prehead and one from the butt
    // any more and we have to bail out
    if( blk->inputs > 2 ) return( FALSE );
    // transfer all instructions to prehead/butt
    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = next ) {
        if( _OpIsCondition( ins->head.opcode ) ) {
            return( ( blk->class & LOOP_EXIT ) != EMPTY );
        }
        butt_ins = DupIns( butt_ins, ins, NULL, 0 );
        next = ins->head.next;
        RemoveIns( ins );
        SuffixIns( prehead_ins, ins );
        prehead_ins = ins;
    }
    return( FALSE );
}
#endif

static  void    MarkLoopHeader( block *loop, block *header )
/***********************************************************
    Mark the entire loop as having header as it's loop_head.
*/
{
    while( loop != NULL ) {
        if( ( loop->class & LOOP_HEADER ) != EMPTY ) {
            loop->loop_head = PreHead->loop_head;
            if( ( PreHead->class & LOOP_HEADER ) != EMPTY ) {
                loop->loop_head = PreHead;
            }
            break;
        }
        loop->loop_head = header;
        loop = loop->u.loop;
    }
    Assert( loop != NULL );
}

/*
    Here's my candidate for weeny comment of the year - BBB Jan '94

    To unroll a nice loop which looks like:

        while( i < n ) {
            LoopBody();
            i += constant;
        }

    ( reps - 1 ) times ( so we get reps total repetitions of
    LoopBody inside our unrolled loop) we produce code which looks
    like the following:

    +---------------------------+
    |                           |
    |         PreHeader         |       Has "add n, -reps -> new_temp"
    |                           |       instruction added to it
    +---------------------------+
                  |
                 \|/
    +---------------------------+
    |                           |       If comparison is unsigned,
    |         SignCheck         |--+    we check to make sure that n and
    |                           |  |    ( n - reps ) have the same sign.
    +---------------------------+  |    ( If not - goto the normal loop below )
                  |                |
                 \|/               |
    +---------------------------+  |
    |                           |  |    This is something which looks like:
    |         BigCond           |--+     if ( i COMPARISON new_temp ) then
    |                           |  |            goto unrolled body
    +---------------------------+  |     else
                  |                |            goto littleCond below
                 \|/               |
    +---------------------------+  |
  \ |                           |  |    This is just reps copies of the
 +-+|         Unrolled Body     |  |    loop body with the conditional
 |/ |                           |  |    statement deleted of course.
 |  +---------------------------+  |
 |                |                |
 |               \|/               |
 |  +---------------------------+  |
 |  |                           |  |    A replication of the bigcond above -
 +--|         BigCond           |  |    as long as we are throwing code size
    |                           |  |    to the wind, might as well have fun
    +---------------------------+  |
                  |                |
                 \|/               |
    +---------------------------+/ |
    |                           |--+
    |         LittleCond        |\      What follows is just a guarded copy
    |                           |--+    of the original loop, to take care of
    +---------------------------+  |    any slop left over (n % reps iterations)
                  |                |
                 \|/               |
    +---------------------------+  |
  \ |                           |  |    The LittleCond blocks look like
 +-+|         LoopBody          |  |
 |/ |                           |  |            if i COMPARISON n then
 |  +---------------------------+  |                    goto LoopBody
 |                |                |            else
 |               \|/               |                    goto exit block
 |  +---------------------------+  |
 |  |                           |  |
 +--|         LittleCond        |  |
    |                           |  |
    +---------------------------+  |
                  |                |
                 \|/               |
    +---------------------------+  |
    |                           |/ |    We only unroll in this fun manner if
    |         Exit Block        |--+    we only had one conditional exit from the
    |                           |\      loop, so this guy is well-defined
    +---------------------------+
*/

static  void    MakeWorldGoAround( block *loop, loop_abstract *cleanup_copy, loop_condition *cond, signed_32 reps )
/******************************************************************************************************************
    This functions lays out the code as given above, creating and linking in condition
    blocks as needed. It then sorts the blocks into the given order, to make sure that nothing
    screws up our lovely flow of control.
*/
{
    name                *temp;
    instruction         *add;
    name                *modifier;
    type_class_def      comp_type;
    block               *new;
    instruction         *ins;
    unsigned_32         high_bit;

    comp_type = cond->compare_ins->type_class;
    temp = AllocTemp( comp_type );
    modifier = AllocS32Const( -1 * reps * cond->induction->plus );
    add = MakeBinary( OP_ADD, cond->invariant, modifier, temp, comp_type );
    SuffixPreHeader( add );

    // add a piece of code to check and make sure n and ( n - reps ) have the same sign
    if( cond->complete == FALSE && Signed[ comp_type ] != comp_type ) {
        new = MakeBlock( AskForNewLabel(), 2 );
        new->class = CONDITIONAL;
        new->loop_head = PreHead->loop_head;
        new->next_block = NULL;
        new->prev_block = NULL;
        new->input_edges = NULL;
        new->id = NO_BLOCK_ID;
        new->gen_id = PreHead->gen_id;
        new->ins.hd.line_num = 0;
        temp = AllocTemp( comp_type );
        ins = MakeBinary( OP_XOR, add->result, cond->invariant, temp, comp_type );
        SuffixIns( new->ins.hd.prev, ins );
        high_bit = 1 << ( ( 8 * TypeClassSize[ comp_type ] ) - 1 );
        ins = MakeCondition( OP_BIT_TEST_TRUE, temp, AllocS32Const( high_bit ), 0, 1, comp_type );
        SuffixIns( new->ins.hd.prev, ins );
        PointEdge( &new->edge[ 0 ], cleanup_copy->head );
        PointEdge( &new->edge[ 1 ], loop->loop_head );
        MoveEdge( &PreHead->edge[ 0 ], new );
        AddBlocks( PreHead, new );
    }

    // now munge Head so that it looks more like we want it to, and make a copy which we can
    // then attach to our butt
    ins = Head->ins.hd.prev;
    ins->head.opcode = cond->opcode;
    ins->operands[ 0 ] = cond->induction->name;
    ins->operands[ 1 ] = add->result;
    _SetBlockIndex( ins, 0, 1 );
    if( cond->complete ) {
        block_edge      *edge;

        // we have completely unrolled the loop - so behead it
        MarkHeaderEdges( loop, Head );
        RedirectHeaderEdges( loop, cond->exit_edge );
        edge = &Head->edge[ 0 ];
        if( ( edge->destination.u.blk->class & IN_LOOP ) == EMPTY ) {
            edge = &Head->edge[ 1 ];
        }
        FreeIns( ins );
        MakeJumpBlock( Head, edge );
        MarkLoopHeader( loop, Head->loop_head );
        Head->class &= ~(LOOP_HEADER | ITERATIONS_KNOWN);
    } else {
        MoveEdge( &Head->edge[ 0 ], cond->loop_edge );
        if( cond->clean ) {
            MoveEdge( &Head->edge[ 1 ], cond->exit_edge );
        } else {
            MoveEdge( &Head->edge[ 1 ], cleanup_copy->head );
        }
        if( ( loop->class & JUMP ) != EMPTY ) {
            if( loop->edge[ 0 ].destination.u.blk == Head ) {
                block   *blk;

                Head->class &= ~LOOP_HEADER;
                blk = DupBlock( Head );
                AddBlocks( loop, blk );
                MoveEdge( &loop->edge[ 0 ], blk );
                PointEdge( &blk->edge[ 0 ], Head->edge[ 0 ].destination.u.blk );
                PointEdge( &blk->edge[ 1 ], Head->edge[ 1 ].destination.u.blk );
                blk->u.loop = loop;
                for( ;; ) {
                    if( loop->u.loop == Head ) {
                        loop->u.loop = NULL;
                        loop->class |= LOOP_HEADER;
                        loop->loop_head = Head->loop_head;
                        MarkLoopHeader( blk, loop );
                        Head = loop;
                        break;
                    }
                    loop = loop->u.loop;
                }
            }
        }
    }
    // won't make copy now until I have this all worked out
    // the cleanup stuff should all be pointing in the right place by now
}

extern  bool    Hoisted( block *head, instruction *compare )
{
#if 0
    if( CanHoist( head ) ) {
        HoistCondition( head );
        return( TRUE );
    }
#else
    if( head->class & CONDITIONAL ) {
        if( _OpIsCondition( head->ins.hd.next->head.opcode ) &&
                compare == head->ins.hd.next ) return( TRUE );
    }
#endif
    return( FALSE );
}

extern  bool    UnRoll()
/***********************
    Unroll the given loop n times.
*/
{
    loop_condition      cond;
    block               *last;
    signed_32           unroll_count;
    bool                one_cond;
    loop_abstract       cleanup_copy;

    if( Head->class & DONT_UNROLL ) return( FALSE );
    unroll_count = UnrollCount( Loop, &cond.clean, &cond.complete );
    if( unroll_count <= 0 ) return( FALSE );
    AnalyseLoop( NULL, &one_cond, &cond.compare_ins, &cond.compare_block );
    if( one_cond &&
        Hoisted( Head, cond.compare_ins ) &&
        TractableCond( &cond ) ) {
        MarkHeaderEdges( Loop, Head );
        DupLoop( Loop, &cleanup_copy );
        RedirectHeaderEdges( cleanup_copy.tail, cleanup_copy.head );
        cleanup_copy.head->class |= LOOP_HEADER | DONT_UNROLL;
        MarkLoopHeader( cleanup_copy.tail, cleanup_copy.head );
        Head->class |= IGNORE;
        last = DoUnroll( Loop, unroll_count, FALSE );
        Head->class &= ~IGNORE;
        MarkLoopHeader( last, Head );
        MakeWorldGoAround( last, &cleanup_copy, &cond, unroll_count );
        cleanup_copy.head->loop_head = Head->loop_head;
        AddBlocks( last, cleanup_copy.head );
        UnMarkHeaderEdges( last );
        // MoveDownLoop( cond_blk );
    } else if( Head->unroll_count > 0 ) {
        last = DoUnroll( Loop, Head->unroll_count, FALSE );
        MarkLoopHeader( last, Head );
    }
    Head->class |= DONT_UNROLL;
    FixBlockIds();
    ClearCopyPtrs( Loop );
    return( TRUE );
}
