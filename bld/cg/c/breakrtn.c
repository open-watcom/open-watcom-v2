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
* Description:  Handle break and return statements.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "data.h"
#include "makeblk.h"


typedef struct  edge_list {
        block_edge              *edge;
        struct edge_list        *next;
        label_handle            lbl;
        block_num               gen_id;
} edge_list;

extern  void            RemoveInputEdge(block_edge*);

static  block           *Curr;
static  block           *Break;
static  edge_list       *BranchOuts;
static  block           *Tail = { NULL };
static  bool            HaveBreak = { false };


static  void            FreeBranchOuts( void )
/********************************************/
{
    edge_list   *next;
    edge_list   *curr;

    for( curr = BranchOuts; curr != NULL; curr = next ) {
        next = curr->next;
        CGFree( curr );
    }
}


extern  bool            CreateBreak( void )
/*****************************************/
{
    block       *blk;
    block       *break_blk;
    block       *back_break_blk;
    block       *exit_blk;
    block_edge  *edge;
    block_edge  *next_edge;
    block_num   targets;
    int         pending;
    edge_list   *exit_edge;

    if( HeadBlock == NULL ) {
        return( false );
    }
    if( _IsntModel( FORTRAN_ALIASING ) ) {
        return( false );
    }
    if( !FixReturns() ) {
        return( false );
    }
    FixEdges();
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        _MarkBlkUnVisited( blk );
    }
/*
    Run through the blocks and find a place (break_blk) where no previous
    blocks are branched to from later blocks. IE: there are no backward branches
    around break_blk.
*/

    break_blk = NULL;
    back_break_blk = NULL;
    pending = 0;
    BranchOuts = NULL;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( AskIfReachedLabel( blk->label ) && blk != HeadBlock )
            break;
        if( (blk->edge[0].flags & BLOCK_LABEL_DIES) == 0 && blk != HeadBlock ) {
            _MarkBlkVisited( blk );
            ++pending;
        } else if( pending == 0 ) {
            break_blk = blk;
        }
        edge = &blk->edge[ 0 ];
        for( targets = blk->targets; targets > 0; --targets ) {
            if( edge->flags & DEST_IS_BLOCK ) {
                if( edge->flags & DEST_LABEL_DIES ) {
                    if( _IsBlkVisited( edge->destination.u.blk ) ) {
                        _MarkBlkUnVisited( edge->destination.u.blk );
                        if( --pending == 0 ) {
                            back_break_blk = blk->next_block;
                        }
                    }
                }
            }
            ++edge;
        }
    }
    /* clean up the BLOCK_VISITED flags */
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        _MarkBlkUnVisited( blk );
    }
    if( back_break_blk != NULL ) {
        break_blk = back_break_blk; /* always better to break on a back edge */
    }
    if( break_blk == NULL ) {
        break_blk = HeadBlock;
    }
    if( break_blk == HeadBlock ) {
        break_blk = break_blk->next_block;
    }
    if( break_blk == NULL ) {
        UnFixEdges();
        return( false );
    }

/*
    create a new block and link it after break_blk. Point Break to the rest
    of the blocks and unhook them from the block list.
*/
    Break = break_blk;
    Curr = CurrBlock;
    Tail = BlockList;
    exit_blk = NewBlock( NULL, false );
    exit_blk->gen_id = BlockList->gen_id + 1;
    exit_blk->id = BlockList->id + 1;
    exit_blk->ins.hd.line_num = 0;
    BlockList = exit_blk;
    exit_blk->prev_block = break_blk->prev_block;
    exit_blk->next_block = NULL;
    _SetBlkAttr( exit_blk, UNKNOWN_DESTINATION );
    break_blk->prev_block->next_block = exit_blk;
    break_blk->prev_block = NULL;

/*
    run throuch all the blocks before break_blk, and create a 'BranchOut' for
    and edge that goes to a block after break_blk
*/
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        edge = &blk->edge[ 0 ];
        for( targets = blk->targets; targets > 0; --targets ) {
            if( (edge->flags & DEST_IS_BLOCK) == 0 || edge->destination.u.blk->gen_id >= break_blk->gen_id ) {
                exit_edge = CGAlloc( sizeof( edge_list ) );
                exit_edge->edge = edge;
                exit_edge->next = BranchOuts;
                exit_edge->gen_id = blk->gen_id;
                BranchOuts = exit_edge;
            }
            ++edge;
        }
    }

/*
    now, point all 'BranchOuts' at the new 'exit' block, saving their original
    labels in the 'lbl' field of the exit_list
*/

    for( exit_edge = BranchOuts; exit_edge != NULL; exit_edge = exit_edge->next ) {
        edge = exit_edge->edge;
        if( edge->flags & DEST_IS_BLOCK ) {
            exit_edge->lbl = edge->destination.u.blk->label;
            RemoveInputEdge( edge );
        } else {
            exit_edge->lbl = edge->destination.u.lbl;
        }
        edge->destination.u.blk = exit_blk;
        edge->flags |= DEST_IS_BLOCK;
        edge->next_source = exit_blk->input_edges;
        exit_blk->input_edges = edge;
        exit_blk->inputs++;
    }

    if( exit_blk->inputs == 0 ) {
        BlockList = exit_blk->prev_block;
        BlockList->next_block = NULL;
        exit_blk->prev_block = NULL;
        FreeABlock( exit_blk );
    }

    _MarkBlkAttr( HeadBlock, BIG_LABEL );
    HaveBreak = true;
/*
    change any branches to HeadBlock from a block after break_blk into
    a label (~DEST_IS_BLOCK) branch.
*/
    for( edge = HeadBlock->input_edges; edge != NULL; edge = next_edge ) {
        next_edge = edge->next_source;
        if( edge->source->gen_id >= break_blk->gen_id ) {
            RemoveInputEdge( edge );
            edge->destination.u.lbl = edge->destination.u.blk->label;
            edge->flags &= ~DEST_IS_BLOCK;
        }
    }
/*
    Now, set up a new HeadBlock, and redirect all unknowns branches to it.
    Known branches may still go to the old HeadBlock. This is so that
    HeadBlock will not be a loop header. The loop optimizer will
    screw up if it is.
*/
    blk = NewBlock( NULL, false );
    blk->input_edges = NULL;
    blk->inputs = 0;
    blk->label = HeadBlock->label;
    blk->ins.hd.line_num = HeadBlock->ins.hd.line_num;
    HeadBlock->ins.hd.line_num = 0;
    blk->gen_id = 0;
    blk->id = 0;
    HeadBlock->label = AskForNewLabel();
    blk->targets = 1;
    _SetBlkAttr( blk, BIG_LABEL | JUMP );
    _MarkBlkAttrNot( HeadBlock, BIG_LABEL );
    edge = &blk->edge[ 0 ];
    edge->flags = DEST_IS_BLOCK;
    edge->destination.u.blk = HeadBlock;
    edge->source = blk;
    edge->next_source = HeadBlock->input_edges;
    HeadBlock->input_edges = edge;
    HeadBlock->inputs++;
    HeadBlock->prev_block = blk;
    blk->prev_block = NULL;
    blk->next_block = HeadBlock;
    HeadBlock = blk;
    return( true );
}


extern  void            RemoveBreak( void )
/*****************************************/
{
    HeadBlock = Break;
    CurrBlock = Curr;
    BlockList = Tail;
    Tail = NULL;
    HaveBreak = false;
    UnFixEdges();
}


extern  void            FixBreak( void )
/**************************************/
{
    block       *blk;
    edge_list   *exit_edge;

    for( exit_edge = BranchOuts; exit_edge != NULL; exit_edge = exit_edge->next ) {
        for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
            if( blk->gen_id == exit_edge->gen_id ) {
                RemoveInputEdge( exit_edge->edge );
                exit_edge->edge->destination.u.lbl = exit_edge->lbl;
                exit_edge->edge->flags &= ~DEST_IS_BLOCK;
                break;
            }
        }
    }
    UnFixEdges();
    FreeBranchOuts();
}

extern  block           *TailBlocks( void )
/*****************************************/
{
    return( Tail );
}

extern  bool            BreakExists( void )
/*****************************************/
{
    return( HaveBreak );
}
