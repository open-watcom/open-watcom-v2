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
* Description:  Generate object code from symbolic instructions.
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "cgauxinf.h"
#include "zoiks.h"
#include "data.h"
#include "objout.h"
#include "objprof.h"
#include "object.h"
#include "ocentry.h"
#include "encode.h"
#include "edge.h"
#include "redefby.h"
#include "targetin.h"
#include "opttell.h"
#include "makeblk.h"
#include "nullprop.h"
#include "feprotos.h"


/* block flag usage                                                 */
/*                                                                  */
/* BLK_BLOCK_VISITED is used in the sense of placed                 */
/*                                                                  */

extern  void            Zoiks( int );

static  source_line_number      DumpLineNum( source_line_number n,
                                             source_line_number last,
                                             bool label_line ) {
/*************************************************************************/

    if( _IsModel( NUMBERS ) ) {
        if( n > 0 && n != last ) {
            last = n;
            CodeLineNumber( n, label_line );
        }
    }
    return( last );
}


void    GenObject( void )
/***********************/
{
    block               *blk;
    block               *next_blk;
    instruction         *ins;
    source_line_number  last_line;
    block_num           targets;
    block_num           i;
    segment_id          old;
    label_handle        lbl;
    unsigned            align;
    fe_attr             attr;

    old = SetOP( AskCodeSeg() );
    InitZeroPage();
    last_line = 0;
    attr = FEAttr( AskForLblSym( CurrProc->label ) );
    for( blk = HeadBlock; blk != NULL; blk = next_blk ) {
        if( blk->label != CurrProc->label && blk->label != NULL ) {
            last_line = DumpLineNum( blk->ins.hd.line_num, last_line, true );
            if( _IsBlkAttr( blk, BLK_ITERATIONS_KNOWN ) && blk->iterations >= 10 ) {
                align = DepthAlign( DEEP_LOOP_ALIGN );
            } else {
                align = DepthAlign( blk->depth );
            }
            CodeLabel( blk->label, align );
            if( (blk->edge[0].flags & BLOCK_LABEL_DIES) && BlocksUnTrimmed ) {
                TellCondemnedLabel( blk->label );
            }
        }
        StartBlockProfiling( blk );
        InitStackDepth( blk );
        next_blk = blk->next_block;
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            if( ins->head.opcode == OP_NOP
              && ( (ins->flags.nop_flags & NOP_SOURCE_QUEUE) || ins->flags.nop_flags == NOP_DBGINFO )) // an end block
            {
                last_line = DumpLineNum(ins->head.line_num, last_line, true);
            } else {
                last_line = DumpLineNum(ins->head.line_num, last_line, false);
            }
            if( attr & FE_NAKED ) {
                // don't want to generate anything except calls to pragma's for
                // naked functions
                if( ins->head.opcode == OP_CALL ) {
                    if( FindAuxInfo( ins->operands[CALL_OP_ADDR], CALL_BYTES ) != NULL ) {
                        GenObjCode( ins );
                    }
                }
            } else {
                GenObjCode( ins );
            }
        }
        EndBlockProfiling();
        if( _IsBlkAttr( blk, BLK_JUMP | BLK_BIG_JUMP ) ) {
            if( BlockByBlock
             || next_blk == NULL
             || blk->edge[0].destination.u.lbl != next_blk->label ) {
                // watch out for orphan blocks (no inputs/targets)
                if( blk->targets > 0 ) {
                    GenJumpLabel( blk->edge[0].destination.u.lbl );
                }
            }
        } else if( _IsBlkAttr( blk, BLK_RETURN ) ) {
            FiniZeroPage();
            GenEpilog();
        } else if( _IsBlkAttr( blk, BLK_CALL_LABEL ) ) {
            GenCallLabel( blk->edge[0].destination.u.blk );
            if( BlockByBlock ) {
                if( next_blk == NULL ) {
                    GenJumpLabel( blk->v.next->label );
                } else {
                    GenJumpLabel( next_blk->label );
                }
            }
        } else if( _IsBlkAttr( blk, BLK_LABEL_RETURN ) ) {
            GenLabelReturn();
        }
        if( !_IsBlkAttr( blk, BLK_LABEL_RETURN ) ) { /* maybe pointer to dead label */
            for( targets = blk->targets; targets-- > 0; ) {
                lbl = blk->edge[targets].destination.u.lbl;
                TellReachedLabel( lbl );
                if( (blk->edge[targets].flags & DEST_LABEL_DIES) && BlocksUnTrimmed ) {
                    TellCondemnedLabel( lbl );
                    for( i = targets; i-- > 0; ) {
                        if( blk->edge[i].destination.u.lbl == lbl ) {
                            blk->edge[i].flags &= ~DEST_LABEL_DIES;
                        }
                    }
                }
            }
        }
        if( BlocksUnTrimmed == false
          && blk->label != CurrProc->label && blk->label != NULL ) {
            TellCondemnedLabel( blk->label );
        }
        CurrBlock = blk;
        FreeBlock();
    }
    HeadBlock = blk;
    BlockList = blk;
    SetOP( old );
}


static  bool    GenId( block *blk, block *next ) {
/************************************************/

    return( blk->gen_id > next->gen_id );
}


static  void    BlocksSortedBy( bool (*bigger)( block *, block * ) )
/******************************************************************/
{
    block       *blk;
    block       *next;
    bool        change;

    for( change = ( HeadBlock->next_block != NULL ); change; ) {
        change = false;
        for( blk = HeadBlock->next_block; (next = blk->next_block) != NULL; blk = next ) {
            if( bigger( blk, next ) ) {
                blk->prev_block->next_block = next;
                if( next->next_block != NULL ) {
                    next->next_block->prev_block = blk;
                }
                blk->next_block = next->next_block;
                if( blk->prev_block == HeadBlock ) {
                    HeadBlock->next_block = next;
                }
                next->prev_block = blk->prev_block;
                blk->prev_block = next;
                next->next_block = blk;
                if( BlockList == next ) {
                    BlockList = blk;
                }
                change = true;
            }
        }
    }
}


typedef struct {
    block       *first;
    block       *last;
} block_queue;

static  void    BQInit( block_queue *q ) {
/****************************************/
    q->first = NULL;
    q->last = NULL;
}

static  bool    BQEmpty( block_queue *q ) {
/********************************************/
    return( q->first == NULL );
}

static  void    BQAdd( block_queue *q, block *element ) {
/*******************************************************/
    if( BQEmpty( q ) ) {
        element->next_block = NULL;
        element->prev_block = NULL;
        q->first = element;
        q->last = element;
    } else {
        element->prev_block = q->last;
        element->next_block = NULL;
        q->last->next_block = element;
        q->last = element;
    }
}

static block    *BQFirst( block_queue *q ) {
/******************************************/

    return( q->first );
}

static block    *BQNext( block_queue *q, block *curr ) {
/******************************************************/

    q = q;
    return( curr->next_block );
}

static  block   *BQRemove( block_queue *q, block *blk )
/*******************************************************/
{
    if( blk == NULL )
        blk = q->first;
    if( blk->prev_block )
        blk->prev_block->next_block = blk->next_block;
    if( blk->next_block )
        blk->next_block->prev_block = blk->prev_block;
    if( q->first == blk )
        q->first = blk->next_block;
    if( q->last  == blk )
        q->last = blk->prev_block;
    return( blk );
}

static  block_edge *FindLoopBackEdge( block *blk ) {
/**************************************************/
    block_edge          *edge;
    block_num           i;

    for( i = 0; i < blk->targets; i++ ) {
        edge = &blk->edge[i];
        if( edge->destination.u.blk == blk->loop_head )
            return( edge );
        if( edge->destination.u.blk == blk ) {
            return( edge );
        }
    }
    return( NULL );
}

static  instruction     *FindCondition( block *blk ) {
/****************************************************/

    instruction         *cond;

    for( cond = blk->ins.hd.prev; cond->head.opcode != OP_BLOCK; cond = cond->head.prev ) {
        if( _OpIsCondition( cond->head.opcode ) ) {
            return( cond );
        }
    }
    // _Zoiks( ZOIKS_XXX );
    return( NULL );
}

#define NOT_TAKEN       -1
#define DNA             0
#define TAKEN           1

static  int     PointerHeuristic( block *blk, instruction *cond ) {
/*****************************************************************/

    int                 prediction;

    blk = blk;
    prediction = DNA;
    if( _IsPointer( cond->type_class ) ) {
        switch( cond->head.opcode ) {
        case OP_CMP_EQUAL:
            prediction = NOT_TAKEN;
            break;
        case OP_CMP_NOT_EQUAL:
            prediction = TAKEN;
            break;
        }
    }
    return( prediction );
}

static  int     OpcodeHeuristic( block *blk, instruction *cond ) {
/****************************************************************/

    int                 prediction;
//    name                *op1;
    name                *op2;

    blk = blk;
    prediction = DNA;
//    op1 = cond->operands[0];
    op2 = cond->operands[1];
    switch( cond->head.opcode ) {
    case OP_CMP_NOT_EQUAL:
        if( _IsFloating( cond->type_class ) ) {
            prediction = TAKEN;
        }
        break;
    case OP_CMP_EQUAL:
        if( _IsFloating( cond->type_class ) ) {
            prediction = NOT_TAKEN;
        }
        break;
    case OP_CMP_LESS:
    case OP_CMP_LESS_EQUAL:
        if( op2->n.class == N_CONSTANT ) {
            if( op2->c.const_type == CONS_ABSOLUTE ) {
                if( op2->c.lo.int_value == 0 ) {
                    prediction = NOT_TAKEN;
                }
            }
        }
        break;
    case OP_CMP_GREATER:
    case OP_CMP_GREATER_EQUAL:
        if( op2->n.class == N_CONSTANT ) {
            if( op2->c.const_type == CONS_ABSOLUTE ) {
                if( op2->c.lo.int_value == 0 ) {
                    prediction = TAKEN;
                }
            }
        }
        break;
    default:
        prediction = DNA;
    }
    return( prediction );
}

static  int     Want( instruction *cond, int index ) {
/*****************************************************
    We know which block we want, or rather its index in the
    list of edges of the basic block, so this tells us
    if the conditional will be TAKEN or NOT_TAKEN to get
    to that block.
*/

    int         prediction;

    prediction = TAKEN;
    if( _TrueIndex( cond ) == 0 ) {
        if( index == 1 ) {
            prediction = NOT_TAKEN;
        }
    } else {
        if( index == 0 ) {
            prediction = NOT_TAKEN;
        }
    }
    return( prediction );
}

static  bool    BlockContainsCall( block *blk ) {
/***********************************************/

    instruction         *ins;

    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        if( _OpIsCall( ins->head.opcode ) ) {
            return( true );
        }
    }
    return( false );
}

static  void    PushTargets( void *stack, block *blk ) {
/******************************************************/

    block_num   i;

    for( i = 0; i < blk->targets; i++ ) {
        EdgeStackPush( stack, &blk->edge[i] );
    }
}

typedef enum {
    ABORT = 0,          // Abort this particular path
    CONTINUE,           // Continue on as normal
    STOP,               // Stop the entire flood down
} flood_decision;

typedef struct flood_info {
    bool        post_dominates;
    block       *dominator;
} flood_info;

typedef flood_decision (*flood_func)( block *, flood_info * );

static  void    FloodDown( block *from, flood_func func, void *parm ) {
/*********************************************************************/

    void                *stack;
    block               *dest;
    flood_decision      decision;
    block_edge          *edge;

    ClearBlocksBits( BLK_FLOODED );
    stack = EdgeStackInit();
    PushTargets( stack, from );
    _MarkBlkAttr( from, BLK_FLOODED );
    while( !EdgeStackEmpty( stack ) ) {
        edge = EdgeStackPop( stack );
        dest = edge->destination.u.blk;
        if( _IsBlkAttr( dest, BLK_FLOODED ) )
            continue;
        decision = func( dest, parm );
        if( decision == ABORT )
            continue;
        if( decision == STOP )
            break;
        _MarkBlkAttr( dest, BLK_FLOODED );
        PushTargets( stack, dest );
    }
    EdgeStackFini( stack );
}

static  flood_decision PDFloodFunc( block *blk, flood_info *info ) {
/******************************************************************/

    if( blk == info->dominator )
        return( ABORT );
    if( _IsBlkAttr( blk, BLK_RETURN ) ) {
        info->post_dominates = false;
        return( STOP );
    }
    return( CONTINUE );
}

static  bool    PostDominates( block *dominator, block *blk ) {
/**************************************************************
    Return true if dominator post-dominates blk.
    To determine this, we just flood down aborting whenever we hit
    previously encountered blocks or the dominator. If we hit a
    RETURN block, we return false.
*/

    flood_info          info;

    info.post_dominates = true;
    info.dominator = dominator;
    FloodDown( blk, PDFloodFunc, (void *)&info );
    return( info.post_dominates );
}

static  bool    CallApplies( block *src, block *dst ) {
/*****************************************************/

    src = dst;
    if( BlockContainsCall( dst ) ) {
        if( !PostDominates( dst, src ) ) {
            return( true );
        }
    }
    return( false );
}

static  int     CallHeuristic( block *blk, instruction *cond ) {
/**************************************************************/

    int         prediction;

    prediction = DNA;
    if( CallApplies( blk, blk->edge[0].destination.u.blk ) ) {
        if( !CallApplies( blk, blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 1 );
        }
    } else {
        if( CallApplies( blk, blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 0 );
        }
    }
    return( prediction );
}

static  bool    LoopApplies( block *blk ) {
/*****************************************/

    if( _IsBlkAttr( blk, BLK_LOOP_HEADER ) )
        return( true );
    if( _IsBlkAttr( blk, BLK_JUMP ) ) {
        if( _IsBlkAttr( blk->edge[0].destination.u.blk, BLK_LOOP_HEADER ) ) {
            return( true );
        }
    }
    return( false );
}

static  int     LoopHeuristic( block *blk, instruction *cond ) {
/**************************************************************/

    int         prediction;

    prediction = DNA;
    if( LoopApplies( blk->edge[0].destination.u.blk ) ) {
        if( !LoopApplies( blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 0 );
        }
    } else {
        if( LoopApplies( blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 1 );
        }
    }
    return( prediction );
}

static  bool    GuardApplies( block *blk, block *dst, name *reg ) {
/*****************************************************************/

    instruction *ins;
    int         i;

    for( ins = dst->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        for( i = 0; i < ins->num_operands; i++ ) {
            // this could be beefed up to take into account aggragates
            if( ins->operands[i] == reg ) {
                return( !PostDominates( dst, blk ) );
            }
            if( _IsReDefinedBy( ins, reg ) ) {
                return( false );
            }
        }
    }
    return( false );
}

static  int     TryGuard( block *blk, instruction *cond, name *reg ) {
/********************************************************************/

    int         prediction;

    prediction = DNA;
    if( GuardApplies( blk, blk->edge[0].destination.u.blk, reg ) ) {
        if( !GuardApplies( blk, blk->edge[1].destination.u.blk, reg ) ) {
            prediction = Want( cond, 0 );
        }
    } else {
        if( GuardApplies( blk, blk->edge[1].destination.u.blk, reg ) ) {
            prediction = Want( cond, 1 );
        }
    }
    return( prediction );
}

static  int     GuardHeuristic( block *blk, instruction *cond ) {
/***************************************************************/

    name        *op1;
    name        *op2;
    int         prediction;

    op1 = cond->operands[0];
    op2 = cond->operands[1];
    prediction = DNA;
    if( op1->n.class == N_REGISTER ) {
        prediction = TryGuard( blk, cond, op1 );
    }
    if( prediction == DNA ) {
        if( op2->n.class == N_REGISTER ) {
            prediction = TryGuard( blk, cond, op2 );
        }
    }
    return( DNA );
}

static  bool    StoreApplies( block *blk, block *next ) {
/*******************************************************/

    instruction *ins;

    if( PostDominates( next, blk ) )
        return( false );
    for( ins = next->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        if( ins->result == NULL )
            continue;
        switch( ins->result->n.class ) {
        case N_MEMORY:
        case N_INDEXED:
        // case N_TEMP:
            return( true );
        }
    }
    return( false );
}

static  int     StoreHeuristic( block *blk, instruction *cond ) {
/***************************************************************/

    int         prediction;

    prediction = DNA;
    if( StoreApplies( blk, blk->edge[0].destination.u.blk ) ) {
        if( !StoreApplies( blk, blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 1 );
        }
    } else {
        if( StoreApplies( blk, blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 0 );
        }
    }
    return( prediction );
}

static  bool    ReturnApplies( block *blk ) {
/*******************************************/

    if( _IsBlkAttr( blk, BLK_RETURN ) )
        return( true );
    if( _IsBlkAttr( blk, BLK_JUMP ) ) {
        if( _IsBlkAttr( blk->edge[0].destination.u.blk, BLK_RETURN ) ) {
            return( true );
        }
    }
    return( false );
}

static  int     ReturnHeuristic( block *blk, instruction *cond ) {
/****************************************************************/

    int         prediction;

    prediction = DNA;
    if( ReturnApplies( blk->edge[0].destination.u.blk ) ) {
        if( !ReturnApplies( blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 1 );
        }
    } else {
        if( ReturnApplies( blk->edge[1].destination.u.blk ) ) {
            prediction = Want( cond, 0 );
        }
    }
    return( prediction );
}

typedef int (*bp_heuristic)( block *, instruction * );

static bp_heuristic     Heuristics[] = {
    PointerHeuristic,
    CallHeuristic,
    OpcodeHeuristic,
    ReturnHeuristic,
    StoreHeuristic,
    LoopHeuristic,
    GuardHeuristic,
};

static  block   *Predictor( block *blk ) {
/*****************************************
    Given a conditional block, return the most likely
    successor of this block.
*/
    block_edge          *edge;
    bp_heuristic        ptr;
    int                 i;
    int                 prediction;
    instruction         *cond;

    edge = FindLoopBackEdge( blk );
    if( edge == NULL ) {
        cond = FindCondition( blk );
        for( i = 0; i < sizeof(Heuristics) / sizeof(Heuristics[0]); i++ ) {
            ptr = Heuristics[i];
            prediction = ptr( blk, cond );
            switch( prediction ) {
            case TAKEN:
                return( blk->edge[_TrueIndex( cond )].destination.u.blk );
            case NOT_TAKEN:
                return( blk->edge[_FalseIndex( cond )].destination.u.blk );
            }
        }
    }
    /* what the hell, pick one at random */
    return( blk->edge[0].destination.u.blk );
}

static  block   *BestFollower( block_queue *unplaced, block *blk ) {
/******************************************************************/

    block       *best;
    block       *curr;
    block_num   i;

    best = NULL;
    switch( blk->class & (BLK_RETURN | BLK_JUMP | BLK_CONDITIONAL | BLK_SELECT | BLK_CALL_LABEL) ) {
    case BLK_RETURN:
    case BLK_JUMP:
    case BLK_SELECT:
        for( i = 0; i < blk->targets; i++ ) {
            best = blk->edge[i].destination.u.blk;
            if( !_IsBlkVisited( best ) ) {
                return( best );
            }
        }
        best = NULL;
        break;
    case BLK_CONDITIONAL:
        /*
         * If exactly one of the followers has already been placed,
         * then the other one is obviously the best candidate. Otherwise,
         * we run branch prediction if neither is placed, or return NULL
         * if both have been placed.
         */
        #define _Munge( a, b )  ( ( (a) << 8 ) + (b) )
        switch( _Munge( _IsBlkVisited( blk->edge[0].destination.u.blk ), _IsBlkVisited( blk->edge[1].destination.u.blk ) ) ) {
        case _Munge( 0, 0 ):
            /* get some branch prediction going here */
            best = Predictor( blk );
            break;
        case _Munge( 1, 0 ):
            best = blk->edge[1].destination.u.blk;
            break;
        case _Munge( 0, 1 ):
            best = blk->edge[0].destination.u.blk;
            break;
        case _Munge( 1, 1 ):
            best = NULL;
            break;
        }
        break;
    case BLK_CALL_LABEL:
        for( curr = BQFirst( unplaced ); curr != NULL; curr = BQNext( unplaced, curr ) ) {
            if( curr->gen_id == ( blk->gen_id + 1 ) ) {
                best = curr;
                break;
            }
        }
        assert( best != NULL );
        assert( (best->class & BLK_RETURNED_TO) != 0 );
        break;
    }
    return( best );
}

void    SortBlocks( void )
/************************/
{
    block_queue unplaced;
    block_queue placed;
    block       *curr;
    block       *next;
//    block       *ret_block;

    MarkBlkAllUnVisited();
    BlocksSortedBy( GenId );
    if( _IsModel( NO_OPTIMIZATION ) )
        return;
    if( _IsntModel( BRANCH_PREDICTION ) )
        return;
    if( OptForSize > 50 )
        return;
    // we can't screw about with the placement of the return
    // block when we are outputting records which mark the start
    // of the epilog etc...
    if( _IsModel( DBG_LOCALS ) )
        return;
    BQInit( &unplaced );
    BQInit( &placed );
    for( curr = HeadBlock; curr != NULL; curr = next ) {
        next = curr->next_block;
        BQAdd( &unplaced, curr );
        if( _IsBlkAttr( curr, BLK_RETURNED_TO ) ) {
            // blocks which are returned to by a call_label routine
            // should not be placed because they are special cased in
            // BestFollower to come out directly after the CALL_LABEL
            // block
            _MarkBlkVisited( curr );
        }
//        ret_block = curr;
    }
    while( !BQEmpty( &unplaced ) ) {
        curr = BQRemove( &unplaced, NULL );
        if( _IsBlkVisited( curr ) )
            continue;
        for( ;; ) {
            BQAdd( &placed, curr );
            _MarkBlkVisited( curr );
            curr = BestFollower( &unplaced, curr );
            if( curr == NULL )
                break;
            BQRemove( &unplaced, curr );
        }
    }
    HeadBlock = placed.first;
    MarkBlkAllUnVisited();
}
