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
* Description:  Clean up basic blocks in a routine.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "data.h"
#include "makeins.h"

extern  void            TellScrapLabel( code_lbl * );
extern  void            FreeABlock( block * );
extern  instruction_id  Renumber( void );
extern  void            RemoveEdge( block_edge * );

/* forward declarations */
extern  void    RemoveInputEdge( block_edge *edge );
extern  bool    BlockTrim( void );

static  instruction     *FindOneCond( block *blk )
/************************************************/
{
    instruction *ins;
    instruction *cond;

    cond = NULL;
    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
         if( _OpIsCondition( ins->head.opcode ) ) {
             if( cond != NULL ) return( NULL );
             cond = ins;
         }
    }
    return( cond );
}


static  void    UnMarkBlocks( void )
/**********************************/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->class &= ~BLOCK_VISITED;
    }
}


static  void    MarkReachableBlocks( void )
/*****************************************/
/* Written NON-Recursively for a very good reason. (stack blew up)*/
{
    block       *blk;
    block       *son;
    bool        change;
    block_num   i;

    for( ;; ) {
        change = FALSE;
        for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
            if( blk->class & ( BIG_LABEL | BLOCK_VISITED | SELECT ) ) {
                blk->class |= BLOCK_VISITED;
                for( i = blk->targets; i-- > 0; ) {
                    son = blk->edge[ i ].destination.u.blk;
                    if( ( son->class & BLOCK_VISITED ) == EMPTY ) {
                        son->class |= BLOCK_VISITED;
                        change = TRUE;
                    }
                }
            }
        }
        if( change == FALSE ) break;
    }
}


extern  int     CountIns( block *blk )
/************************************/
{
    int         num_instrs;
    instruction *ins;

    num_instrs = 0;
    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        num_instrs++;
    }
    return( num_instrs );
}


static  bool    FindBlock( block *target )
/****************************************/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk == target ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


extern  void    RemoveBlock( block *blk )
/***************************************/
{
    block_num   i;
    unsigned    last_line;
    block       *chk;
    block       *next;
    instruction *next_ins;

    if( blk->prev_block != NULL ) {
        blk->prev_block->next_block = blk->next_block;
    }
    if( blk->next_block != NULL ) {
        blk->next_block->prev_block = blk->prev_block;
    }
    for( i = 0; i < blk->targets; ++i ) {
        /* block may have already been removed by dead code removal*/
        if( FindBlock( blk->edge[ i ].destination.u.blk ) ) {
            RemoveInputEdge( & blk->edge[ i ] );
        }
    }
    last_line = blk->ins.hd.line_num;
    for( ;; ) {
        next_ins = blk->ins.hd.next;
        if( next_ins == (instruction *)&blk->ins ) break;
        if( next_ins->head.line_num != 0 ) {
            last_line = next_ins->head.line_num;
        }
        FreeIns( next_ins );
    }
    /*
        Move the last line number from the block being deleted to the head
        of the next block in source order, if that block doesn't already
        have a line number on it.
    */
    if( blk->next_block != NULL && blk->next_block->gen_id == (blk->gen_id + 1) ) {
        /* quick check to see if following block is next one in src order */
        next = blk->next_block;
    } else {
        next = NULL;
        for( chk = HeadBlock; chk != NULL; chk = chk->next_block ) {
            if( (chk != blk)
                && (chk->gen_id > blk->gen_id)
                && (next == NULL || next->gen_id > chk->gen_id) ) {
                next = chk;
            }
        }
    }
    if( next != NULL && next->ins.hd.line_num == 0 ) {
        next->ins.hd.line_num = last_line;
    }
    if( HeadBlock == blk ) {
        HeadBlock = blk->next_block;
    }
    if( BlockList == blk ) {
        BlockList = blk->prev_block;
        if( BlockList == NULL ) {
            BlockList = HeadBlock;
        }
    }
    TellScrapLabel( blk->label );
    if( blk->dataflow != NULL ) {
        CGFree( blk->dataflow );
    }
    FreeABlock( blk );
}


extern  void    RemoveInputEdge( block_edge *edge )
/*************************************************/
{
    block       *dest;
    block_edge  *prev;

    if( ( edge->flags & DEST_IS_BLOCK ) == EMPTY ) return;
    dest = edge->destination.u.blk;
    dest->inputs --;
    prev = dest->input_edges;
    if( prev == edge ) {
        dest->input_edges = edge->next_source;
    } else {
        while( prev->next_source != edge ) {
            prev = prev->next_source;
        }
        prev->next_source = edge->next_source;
    }
}


extern  void    MoveHead( block *old, block *new )
/*************************************************

    We're eliminating a loop header, so move it the the new
    block and point all loop_head pointers to the new block.
*/
{
    block       *blk;

    if( !( old->class & LOOP_HEADER ) ) return;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk->loop_head == old ) {
            blk->loop_head = new;
        }
    }
    new->class |= old->class & (LOOP_HEADER|ITERATIONS_KNOWN);
    old->class &= ~( LOOP_HEADER | ITERATIONS_KNOWN );
    new->iterations = old->iterations;
    new->loop_head = old->loop_head;
    old->loop_head = new;
}


static  bool    Retarget( block *blk )
/************************************/
{
    block_edge  *edge;
    block_edge  *next;
    block       *target;
    bool        success;

    success = TRUE;                    /* assume can get rid of block*/
    target = blk->edge[ 0 ].destination.u.blk;
    edge = blk->input_edges;
    blk->input_edges = NULL;
    while( edge != NULL ) {
        next = edge->next_source;
        if( edge->source->class & ( SELECT | LABEL_RETURN ) ) {
            success = FALSE;   /* let the optimizer do it later on*/
            edge->next_source = blk->input_edges;
            blk->input_edges = edge;
        } else {
            edge->destination.u.blk = target;
            edge->next_source = target->input_edges;
            target->input_edges = edge;
            target->inputs++;
            blk->inputs--;
        }
        edge = next;
    }
    if( success ) {
        MoveHead( blk, target );
        RemoveBlock( blk );
    }
    return( success );
}


static  void    JoinBlocks( block *jump, block *target )
/******************************************************/
{
    block_edge          *edge;
    source_line_number  line_num;
    code_lbl            *label;
    instruction         *nop;

    /*  To get here, 'target' is only entered from 'jump'*/
    /*  Thus, the only input edge to 'target' is from jump, and can be tossed*/

    /* keep the label from jump in case it's referenced in a SELECT block*/

    label = target->label;
    target->label = jump->label;
    if( jump->class & BIG_LABEL ) target->class |= BIG_LABEL;
    jump->label = label;
    line_num = target->ins.hd.line_num;
    target->ins.hd.line_num = jump->ins.hd.line_num;

    /*  Move the inputs to 'jump' to be inputs to 'target'*/

    target->inputs = jump->inputs;
    edge = jump->input_edges;
    target->input_edges = edge;
    for( ; edge != NULL; edge = edge->next_source ) {
        edge->destination.u.blk = target;    /* was 'jump' before*/
    }

    /*  Now join the instruction streams*/

    nop = jump->ins.hd.prev;
    if( nop->head.opcode == OP_NOP ) {
        if( nop->flags.nop_flags & NOP_SOURCE_QUEUE ) {
            /* this nop is only here to hold source info so we just
             * attach the source info to the next instruction and
             * nuke this nop so that it can't inhibit optimization */
             if( target->ins.hd.next->head.line_num == 0 ) {
                 target->ins.hd.next->head.line_num = nop->head.line_num;
             }
             FreeIns( nop );
        }
    }

    if( jump->ins.hd.next != (instruction *)&jump->ins ) {
        if( line_num != 0 ) {
            jump->ins.hd.prev->head.line_num = line_num;
        }
        jump->ins.hd.prev->head.next = target->ins.hd.next;
        target->ins.hd.next->head.prev = jump->ins.hd.prev;
        target->ins.hd.next = jump->ins.hd.next;
        target->ins.hd.next->head.prev = (instruction *)&target->ins;
        jump->ins.hd.next = (instruction *)&jump->ins;/* so RemoveBlock won't*/
        jump->ins.hd.prev = (instruction *)&jump->ins;/* free the instr list*/
    }

    jump->inputs = 0;
    jump->targets = 0;
    MoveHead( jump, target );
    RemoveBlock( jump );
}


static  bool    SameTarget( block *blk )
/**************************************/
{
    instruction *ins;
    block       *targ1, *targ2;

    targ1 = blk->edge[ 0 ].destination.u.blk;
    targ2 = blk->edge[ 1 ].destination.u.blk;
    if( targ1 != targ2 ) return( FALSE );
    if( ( targ1->class | targ2->class ) & UNKNOWN_DESTINATION ) return( FALSE );
    blk->class &= ~CONDITIONAL;
    blk->class |= JUMP;
    RemoveEdge( &blk->edge[1] );
    ins = blk->ins.hd.prev;
    while( !_OpIsCondition( ins->head.opcode ) ) {
        ins = ins->head.prev;
    }
    FreeIns( ins );
    return( TRUE );
}


static  bool    DoBlockTrim( void )
/*********************************/
{
    block       *blk;
    block       *next;
    block       *target;
    instruction *ins;
    bool        change;
    bool        any_change = FALSE;
    block_num   block_id;

    for( ;; ) {
        change = FALSE;
        MarkReachableBlocks();
        for( blk = HeadBlock->next_block; blk != NULL; blk = next ) {
            next = blk->next_block;
            if( !( blk->class & ( UNKNOWN_DESTINATION | BLOCK_VISITED ) ) ) {
                while( blk->input_edges != NULL ) {
                    RemoveInputEdge( blk->input_edges );
                }
                RemoveBlock( blk );
                change = TRUE;
            } else if( blk->class & CONDITIONAL ) {
                change |= SameTarget( blk );
            } else if( blk->class & JUMP ) {
                target = blk->edge[ 0 ].destination.u.blk;
                if( target != blk && !(target->class & UNKNOWN_DESTINATION) ) {
                    for( ins = blk->ins.hd.next; ins->head.opcode == OP_NOP; ins = ins->head.next ) {
                        if( ins->flags.nop_flags & (NOP_DBGINFO|NOP_DBGINFO_START) ) {
                            break;
                        }
                    }
                    if( ins->head.opcode == OP_BLOCK ) { /* was an empty block*/
                        if( ( blk->class & BIG_LABEL ) == 0 ) {
                            change |= Retarget( blk );
                        }
                    } else if( target->inputs == 1
                          && ( target->class & BIG_LABEL ) == 0
                          && ( CountIns(blk)+CountIns(target) ) <= INS_PER_BLOCK
                          ) {
                        if( !( blk->class & ( RETURNED_TO | BIG_LABEL ) )
                           && !( target->class & CALL_LABEL ) ) {
                            JoinBlocks( blk, target );
                            change = TRUE;
                        }
                    }
                }
            }
        }
        UnMarkBlocks();
        if( change == FALSE ) break;
        BlocksUnTrimmed = FALSE;
        any_change = TRUE;
    }
    block_id = 1;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->id = block_id++;
    }
    return( any_change );
}

extern void KillCondBlk( block *blk, instruction *ins, int dest )
/***************************************************************/
// Assume blk is a conditional with compare ins
// Make dest the destination and delete the unused edge
// Change blk to a JMP to dest edge
{
    block_edge  *edge;
    block       *dest_blk;

    RemoveInputEdge( &blk->edge[ 0 ] );
    RemoveInputEdge( &blk->edge[ 1 ] );
    blk->class &= ~CONDITIONAL;
    blk->class |= JUMP;
    blk->targets = 1;
    dest_blk = blk->edge[dest].destination.u.blk;
    edge = &blk->edge[0];
    edge->flags = blk->edge[dest].flags;
    edge->source = blk;
    edge->destination.u.blk = dest_blk;
    edge->next_source = dest_blk->input_edges;
    dest_blk->input_edges = edge;
    dest_blk->inputs++;
    FreeIns( ins );
}

extern  bool    DeadBlocks( void )
/********************************/
{
    block       *blk;
    instruction *ins;
    int         dest;
    bool        change;

    change = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk->class & CONDITIONAL ) {
            ins = FindOneCond( blk );
            if( ins == NULL ) continue;
            if( ins->result != NULL ) continue;
            dest = _TrueIndex( ins );
            if( dest != _FalseIndex( ins ) ) continue;
            KillCondBlk( blk, ins, dest );
            change = TRUE;
        }
    }
    if( change ) {
        BlockTrim();
        return( TRUE );
    }
    return( FALSE );
}


extern  bool    BlockTrim( void )
/*******************************/
{
    bool    change = FALSE;

    if( ( CurrProc->state.attr & ROUTINE_WANTS_DEBUGGING ) == 0 ) {
        change = DoBlockTrim();
        Renumber();
    }
    return( change );
}
