/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Manage basic block edges.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "utils.h"
#include "edge.h"
#include "procdef.h"
#include "makeblk.h"
#include "insutil.h"
#include "unroll.h"


void    PointEdge( block_edge *edge, block *new_dest )
/*****************************************************
 * Make edge point to block new_dest
 */
{
    /*
     * hook edge into new destination's input list
     */
    edge->next_source = new_dest->input_edges;
    new_dest->input_edges = edge;
    new_dest->inputs++;
    /*
     * point edge at new block
     */
    edge->destination.u.blk = new_dest;
}

void    RemoveEdge( block_edge *edge )
/*************************************
 * Remove the given edge from it's block.
 */
{
    block_edge  *curr;
    block_edge  **owner;

    /*
     * unhook edge from its old destination's input list
     */
    if( edge->flags & BEF_DEST_IS_BLOCK ) {
        owner = &edge->destination.u.blk->input_edges;
        for( ;; ) {
            curr = *owner;
            if( curr == edge )
                break;
            owner = &(*owner)->next_source;
        }
        *owner = curr->next_source;
        edge->destination.u.blk->inputs--;
    }
    edge->source->targets--;
}

void    MoveEdge( block_edge *edge, block *new_dest )
/****************************************************
 * Move edge to point to block new_dest
 */
{
    RemoveEdge( edge );
    edge->flags = BEF_DEST_IS_BLOCK;
    edge->source->targets++;
    PointEdge( edge, new_dest );
}

block   *SplitBlock( block *blk, instruction *ins )
/**************************************************
 * Split a block in two before the given instruction. The first block
 * will simply jump to the second block, which will receive all the
 * edges from the previous block.
 */
{
    block       *new_blk;
    block_edge  *edge;
    instruction *next;
    block_num   i;
    block_num   targets;

    targets = blk->targets;
    new_blk = MakeBlockCopy( targets, blk, targets );
    new_blk->next_block = blk->next_block;
    new_blk->prev_block = blk;
    blk->next_block = new_blk;
    new_blk->next_block->prev_block = new_blk;
    _MarkBlkAttrClr( blk, BLK_CONDITIONAL | BLK_RETURN | BLK_SELECT | BLK_LOOP_EXIT | BLK_UNKNOWN_DESTINATION );
    _MarkBlkAttrSet( blk, BLK_JUMP );
    _MarkBlkAttrClr( new_blk, BLK_LOOP_HEADER );
    new_blk->inputs = 0;
    new_blk->input_edges = NULL;
    new_blk->blk_id = BLK_ID_NONE;
    blk->targets = 1;
    edge = &blk->edge[0];
    edge->flags = BEF_DEST_IS_BLOCK;
    edge->source->targets++;
    PointEdge( edge, new_blk );
    for( i = 0; i < targets; ++i ) {
        new_blk->edge[i].source = new_blk;
    }
    for( ; ins->head.opcode != OP_BLOCK; ins = next ) {
        next = ins->head.next;
        RemoveIns( ins );
        SuffixIns( new_blk->ins.head.prev, ins );
    }
    FixBlockIds();
    return( new_blk );
}

edge_stack      *EdgeStackInit( void )
/************************************/
{
    edge_stack          *stk;

    stk = CGAlloc( sizeof( edge_stack ) );
    stk->top = NULL;
    return( stk );
}

bool            EdgeStackEmpty( edge_stack *stk )
/***********************************************/
{
    return( stk->top == NULL );
}

void            EdgeStackPush( edge_stack *stk, block_edge *edge )
/****************************************************************/
{
    edge_entry          *new_entry;

    new_entry = CGAlloc( sizeof( edge_entry ) );
    new_entry->edge = edge;
    new_entry->next = stk->top;
    stk->top = new_entry;
}

block_edge      *EdgeStackPop( edge_stack *stk )
/**********************************************/
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

void            EdgeStackFini( edge_stack *stk )
/**********************************************/
{
    while( !EdgeStackEmpty( stk ) ) {
        EdgeStackPop( stk );
    }
    CGFree( stk );
}
