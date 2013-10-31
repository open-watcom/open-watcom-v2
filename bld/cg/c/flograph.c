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


#include "cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "zoiks.h"
#include "stack.h"
#include "data.h"

extern  instruction_id  Renumber( void );
extern  block           *NewBlock(code_lbl *,bool);
extern  bool            FloodForward( block *, bool (*)( block *, void * ), void * );

static  void            NewInterval( block *blk, int level );

static    interval_def  *Intervals;


static  void    Irreducable( void )
/*********************************/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->class &= ~(ITERATIONS_KNOWN+LOOP_HEADER);
        blk->loop_head = NULL;
        blk->depth = 1;
    }
}


static  void    NoBlocksToSelf( void )
/************************************/
{
    block       *blk;
    block       *new_blk;
    block_edge  *edge;
    block_edge  *new_edge;
    block_edge  *curr;
    block_edge  **owner;
    block_edge  *next;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( edge = blk->input_edges; edge != NULL; edge = next ) {
            next = edge->next_source;
            if( edge->source == blk ) {
                new_blk = NewBlock( AskForNewLabel(), TRUE );
                /* set up new block to look like it was generated after blk*/
                new_blk->class = JUMP;
                new_blk->gen_id = blk->gen_id;
                new_blk->ins.hd.line_num = blk->ins.hd.line_num;
                new_blk->next_block = blk->next_block;
                new_blk->prev_block = blk;
                new_blk->targets++;
                new_blk->inputs++;
                new_blk->input_edges = edge;
                /* link it after blk*/
                blk->next_block = new_blk;
                if( new_blk->next_block != NULL ) {
                    new_blk->next_block->prev_block = new_blk;
                }
                /* retarget edge to point to new block*/
                edge->destination.u.blk = new_blk;
                edge->flags &= ~DEST_LABEL_DIES;
                /* set new block to jump from new_blk to blk*/
                new_edge = &new_blk->edge[ 0 ];
                new_edge->flags |= DEST_IS_BLOCK;
                new_edge->destination.u.blk = blk;
                new_edge->source = new_blk;
                /* replace edge with new_edge in blk's input edge list*/
                owner = &blk->input_edges;
                for( ;; ) {
                    curr = *owner;
                    if( curr == edge ) break;
                    owner = &(*owner)->next_source;
                }
                *owner = new_edge;
                new_edge->next_source = curr->next_source;
                /* edge is now the only input to new_blk*/
                edge->next_source = NULL;
            }
        }
    }
}


static  void    ReturnsToBottom( void )
/*************************************/
/* moving return blocks to the bottom*/
{
    block       *curr;
    block       *prev;
    block       *last;

    for( last = HeadBlock; last->next_block != NULL; ) {
        last = last->next_block;
    }
    for( curr = last->prev_block; curr != NULL; curr = prev ) {
        prev = curr->prev_block;
        if( curr->class & RETURN ) {
            if( prev != NULL ) {
                prev->next_block = curr->next_block;
            }
            curr->next_block->prev_block = prev;
            last->next_block = curr;
            curr->prev_block = last;
            curr->next_block = NULL;
            last = curr;
        }
    }
    BlockList = last;
}

static  void *MarkVisited( block *blk )
/*************************************/
{
    block_num   i;

    blk->class |= BLOCK_VISITED;
    for( i = 0; i < blk->targets; ++i ) {
        if( blk->edge[ i ].destination.u.blk->class & BLOCK_VISITED ) continue;
        SafeRecurseCG( (func_sr)MarkVisited, blk->edge[ i ].destination.u.blk );
    }
    blk->prev_block = BlockList;
    BlockList = blk;
    return NULL;
}

static  bool    DepthFirstSearch( void )
/**************************************/
{
    block       *blk;
    bool        reducible;

    BlockList = NULL;
    MarkVisited( HeadBlock );
    reducible = TRUE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( ( blk->class & BLOCK_VISITED ) == 0 ) {
            blk->prev_block = BlockList;
            BlockList = blk;
            reducible = FALSE;
        }
        blk->class &= ~BLOCK_VISITED;
    }
    return( reducible );
}


static  void    RestoreLinks( void )
/**********************************/
{
    block       *blk;
    block       *prev;

    prev = NULL;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->prev_block = prev;
        prev = blk;
    }
    BlockList = prev;
}


static  void    FixLinks( void )
/******************************/
{
    block       *blk;
    block       *prev;
    block_num   id;

    prev = NULL;
    id = 0;
    for( blk = BlockList; blk != NULL; blk = blk->next_block ) {
        blk->next_block = blk->prev_block;
        blk->prev_block = prev;
        blk->id = ++id;
        prev = blk;
    }
    HeadBlock = BlockList;
    BlockList = prev;
}


static  interval_def    *IntervalNo( block *blk, int level )
/**********************************************************/
{
    interval_def        *curr;

    curr = blk->u.interval;
    while( -- level >= 0 ) {
        curr = curr->parent;
    }
    return( curr );
}


static  bool    FindIntervals( void )
/***********************************/
{
    block               *blk;
    block_edge          *edge;
    interval_def        *curr;
    interval_def        *prev_int;
    interval_def        *test;
    int                 level;
    int                 num;
    int                 prev_num;
    bool                add;

    num = 0;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        ++num;
        curr = CGAlloc( sizeof( interval_def ) );
        curr->link = Intervals;
        Intervals = curr;
        curr->parent = NULL;
        curr->sub_int = NULL;
        curr->next_sub_int = NULL;
        curr->first_block = blk;
        curr->last_block = blk;
        curr->level = 0;
        blk->depth = 0;
        blk->u.interval = curr;
    }
    level = 1;
    for( ;; ) {
        prev_num = num;
        num = 1;                       /* at least one node at new level*/
        NewInterval( HeadBlock, level );
        for( blk = HeadBlock->next_block; blk != NULL; blk = blk->next_block ) {
            curr = IntervalNo( blk, level - 1 );
            if( curr->parent == NULL ) {
                prev_int = NULL;
                edge = blk->input_edges;
                add = FALSE;
                for( ;; ) {
                    test = IntervalNo( edge->source, level - 1 );
                                                /* guess - internal edge*/
                    if( test != curr ) {
                                                /* guess - lower level head*/
                        test = test->parent;
                        if( test == NULL ) {
                            add = TRUE;
                            break;
                        }
                                                /* guess - no other predecessor*/
                        if( prev_int == NULL ) {
                            prev_int = test;
                        } else {
                                                /* guess - different predecessor*/
                            if( test != prev_int ) {
                                add = TRUE;
                                break;
                            }
                        }
                    }
                    edge = edge->next_source;
                    if( edge == NULL ) break;
                }
                if( add == FALSE ) {
                    curr->parent = prev_int;
                    prev_int->last_block = curr->last_block;
                    for( test = prev_int->sub_int; test->next_sub_int != NULL; ) {
                        test = test->next_sub_int;
                    }
                    test->next_sub_int = curr;
                } else {                        /* admit - create a new interval*/
                    NewInterval( blk, level );
                    num ++;
                }
            }
        }
        ++ level;
        if( num == prev_num || num == 1 ) break;
    }
    return( num == 1 );
}


static  void    ReorderBlocks( void )
/***********************************/
/*   Reorder blocks according to the interval ordering*/
/*   This allows each interval to be identified as a continuous range of blocks*/
{
    interval_def        *curr;
    block               *last_block;
    block               *next_block;

    last_block = HeadBlock;
    curr = HeadBlock->u.interval;
    for( ;; ) {
        for( ;; ) {
            if( curr->next_sub_int != NULL ) break;
            curr = curr->parent;
            if( curr == NULL )break;
            curr->last_block = last_block;
        }
        if( curr == NULL )break;
        for( curr = curr->next_sub_int; curr->level > 0; ) {
            curr = curr->sub_int;
        }
        next_block = curr->first_block;
        next_block->prev_block = last_block;
        last_block->next_block = next_block;
        next_block->id = last_block->id + 1;
        last_block = next_block;
    }
    last_block->next_block = NULL;
    BlockList = last_block;
}


static  void    EdgeLevels( void )
/********************************/
{
    block               *blk;
    block_edge          *edge;
    interval_def        *interval;
    block_num           id;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( edge = blk->input_edges; edge != NULL; edge = edge->next_source ) {
            id = edge->source->id;
            interval = blk->u.interval;
            for( ;; ) {
                if( id >= interval->first_block->id
                    && id <= interval->last_block->id ) break;
                edge->join_level = interval->level;
                interval = interval->parent;
                if( interval == NULL ) break;
            }
        }
    }
}


static  void    NewInterval( block *blk, int level )
/**************************************************/
{
    interval_def        *prev;
    interval_def        *new;

    prev = IntervalNo( blk, level - 1 );
    new = CGAlloc( sizeof( interval_def ) );
    new->link = Intervals;
    Intervals = new;
    new->sub_int = prev;
    new->next_sub_int = NULL;
    new->level = level;
    new->parent = NULL;
    new->first_block = blk;
    new->last_block = blk;
    prev->parent = new;
}


static  void    NestingDepth( void )
/**********************************/
{
    block               *blk;
    int                 level;
    interval_def        *interval;
    block_edge          *edge;
    block               *target;
    block_num           i;
    bool                change;

    for( interval = BlockList->u.interval->parent; interval->parent != NULL; interval = interval->parent ) {
        level = interval->level - 1;
        /* borrow 'next_block'*/
        /* identify all back edges at this level*/
        for( blk = BlockList; blk != NULL; blk = blk->prev_block ) {
            blk->next_block = NULL;
            for( i = blk->targets; i-- > 0; ) {
                edge = &blk->edge[ i ];
                target = edge->destination.u.blk;
                if( target->id <= blk->id ) {     /* if back edge*/
                    if( edge->join_level == level ) {
                        blk->next_block = target;
                        if( blk->loop_head == NULL ) {
                            blk->loop_head = target;
                        }
                        blk->depth++;
                        break;
                    }
                }
            }
        }
        for( ;; ) {
            change = FALSE;
            for( blk = BlockList; blk != NULL; blk = blk->prev_block ) {
                if( blk->next_block == NULL ) {
                    for( i = blk->targets; i-- > 0; ) {
                        edge = & blk->edge[ i ];
                        if( edge->join_level <= level ) {
                            target = edge->destination.u.blk->next_block;
                            if( target != NULL ) {
                                blk->depth ++;
                                blk->next_block = target;  /* store head in node*/
                                if( blk->loop_head == NULL && blk != target ) {
                                    blk->loop_head = target;
                                }
                                target->class |= LOOP_HEADER;
                                change = TRUE;
                                break;
                            }
                        }
                    }
                }
            }
            if( change == FALSE ) break;
        }
    }

/*   Restore 'next_block'*/

    HeadBlock = NULL;
    for( blk = BlockList; blk != NULL; blk = blk->prev_block ) {
        blk->next_block = HeadBlock;
        HeadBlock = blk;
    }
}

static  void    KillIntervals( void )
/***********************************/
{
    interval_def        *next;
    block               *blk;

    for( ; Intervals != NULL; Intervals = next ) {
        next = Intervals->link;
        CGFree( Intervals );
    }
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blk->u.interval = NULL;
    }
}

static bool functionDegenerate;

static  bool    FlowDone( block *curr, void *parm )
/*************************************************/
{
    if( curr == (block *)parm ) {
        functionDegenerate = TRUE;
        return( FALSE );
    }
    return( TRUE );
}

static  bool    Degenerate( void )
/********************************/
{
    block       *blk;
    block_edge  *edge;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( ( blk->class & LOOP_HEADER ) == EMPTY ) continue;
        for( edge = blk->input_edges; edge != NULL; edge = edge->next_source ) {
            if( ( edge->source->class & SELECT ) != EMPTY ) {
                functionDegenerate = FALSE;
                FloodForward( edge->source, FlowDone, blk );
                if( functionDegenerate ) return( TRUE );
            }
        }
    }
    return( FALSE );
}

extern  void    MakeFlowGraph( void )
/***********************************/
{
    Irreducable();
    if( CurrProc->state.attr & ROUTINE_WANTS_DEBUGGING ) {
        return;
    }
    NoBlocksToSelf();
    Intervals = NULL;
    if( HeadBlock != NULL ) {
        if( !DepthFirstSearch() ) {
            Irreducable();
            RestoreLinks();
            Renumber();
            return;
        }
        FixLinks();
        if( FindIntervals() ) {
            ReorderBlocks();
            EdgeLevels();
            NestingDepth();
            if( Degenerate() ) {
                Irreducable();
            }
       } else {
            /* irreducable flow graph. repair prev_block links*/
            Irreducable();
        }
        KillIntervals();
        ReturnsToBottom();
        Renumber();
    }
}
