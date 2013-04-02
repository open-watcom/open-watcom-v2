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


#include "standard.h"
#include "cgdefs.h"
#include "coderep.h"
#include "opcodes.h"
#include "model.h"
#include "procdef.h"
#include "cgaux.h"
#include "zoiks.h"
#include "feprotos.h"

extern  seg_id          AskCodeSeg( void );
extern  seg_id          SetOP(seg_id);
extern  void            CodeLabel(label_handle,unsigned);
extern  void            GenObjCode(instruction*);
extern  void            GenJumpLabel(pointer);
extern  void            GenEpilog( void );
extern  void            GenCallLabel(pointer);
extern  void            GenLabelReturn( void );
extern  void            TellCondemnedLabel(label_handle);
extern  void            FreeBlock( void );
extern  void            CodeLineNum(cg_linenum,bool);
extern  void            InitZeroPage( void );
extern  void            FiniZeroPage( void );
extern  void            TellReachedLabel(label_handle);
extern  unsigned        DepthAlign( unsigned );
extern  void            InitStackDepth(block*);
extern  block           *FindBlockWithLbl( label_handle label );
extern  void            Zoiks( int );
extern  void            ClearBlockBits( block_class );
extern  bool            ReDefinedBy( instruction *, name * );
extern  pointer         FindAuxInfo( name *, aux_class );
extern  void            StartBlockProfiling( block *blk );
extern  void            EndBlockProfiling( void );

extern  void            *EdgeStackInit( void );
extern  void            EdgeStackFini( void * );
extern  bool            EdgeStackEmpty( void * );
extern  void            EdgeStackPush( void *, block_edge * );
extern  block_edge      *EdgeStackPop( void * );

extern    block         *HeadBlock;
extern    block         *CurrBlock;
extern    block         *BlockList;
extern    proc_def      *CurrProc;
extern    bool          BlocksUnTrimmed;
extern    bool          BlockByBlock;
extern    byte          OptForSize;

static  source_line_number      DumpLineNum( source_line_number n,
                                             source_line_number last,
                                             bool label_line ) {
/*************************************************************************/

    if( _IsModel( NUMBERS ) ) {
        if( n > 0 && n != last ) {
            last = n;
            CodeLineNum( n, label_line );
        }
    }
    return( last );
}


extern  void    GenObject( void )
/*******************************/
{
    block               *blk;
    block               *next_blk;
    instruction         *ins;
    source_line_number  last_line;
    int                 targets;
    int                 i;
    seg_id              old;
    label_handle        lbl;
    unsigned            align;
    fe_attr             attr;

    old = SetOP( AskCodeSeg() );
    InitZeroPage();
    last_line = 0;
    attr = FEAttr( AskForLblSym( CurrProc->label ) );
    blk = HeadBlock;
    while( blk != NULL ) {
        if( blk->label != CurrProc->label && blk->label != NULL ) {
            last_line = DumpLineNum( blk->ins.hd.line_num, last_line, TRUE );
            if( ( blk->class & ITERATIONS_KNOWN ) && blk->iterations >= 10 ) {
                align = DepthAlign( DEEP_LOOP_ALIGN );
            } else {
                align = DepthAlign( blk->depth );
            }
            CodeLabel( blk->label, align );
            if( ( blk->edge[ 0 ].flags & BLOCK_LABEL_DIES ) != 0
              && BlocksUnTrimmed ) {
                TellCondemnedLabel( blk->label );
            }
        }
        StartBlockProfiling( blk );
        InitStackDepth( blk );
        ins = blk->ins.hd.next;
        next_blk = blk->next_block;
        while( ins->head.opcode != OP_BLOCK ) {
            if( ins->head.opcode == OP_NOP
              &&( (ins->flags.nop_flags & NOP_SOURCE_QUEUE )
                ||(ins->flags.nop_flags == NOP_DBGINFO   ))) // an end block
            {
                last_line = DumpLineNum(ins->head.line_num, last_line, TRUE);
            } else {
                last_line = DumpLineNum(ins->head.line_num, last_line, FALSE);
            }
            if( attr & FE_NAKED ) {
                // don't want to generate anything except calls to pragma's for
                // naked functions
                if( ins->head.opcode == OP_CALL ) {
                    if( FindAuxInfo( ins->operands[ CALL_OP_ADDR ], CALL_BYTES ) != NULL ) {
                        GenObjCode( ins );
                    }
                }
            } else {
                GenObjCode( ins );
            }
            ins = ins->head.next;
        }
        EndBlockProfiling();
        if( blk->class & ( JUMP | BIG_JUMP ) ) {
            if( BlockByBlock
             || next_blk == NULL
             || blk->edge[ 0 ].destination != next_blk->label ) {
                // watch out for orphan blocks (no inputs/targets)
                if( blk->targets > 0 ) {
                    GenJumpLabel( blk->edge[ 0 ].destination );
                }
            }
        } else if( blk->class & RETURN ) {
            FiniZeroPage();
            GenEpilog();
        } else if( blk->class & CALL_LABEL ) {
            GenCallLabel( blk->edge[ 0 ].destination );
            if( BlockByBlock ) {
                if( next_blk == NULL ) {
                    GenJumpLabel( blk->v.next->label );
                } else {
                    GenJumpLabel( next_blk->label );
                }
            }
        } else if( blk->class & LABEL_RETURN ) {
            GenLabelReturn();
        }
        if( !( blk->class & LABEL_RETURN ) ) { /* maybe pointer to dead label */
            targets = blk->targets;
            while( --targets >= 0 ) {
                lbl = blk->edge[ targets ].destination;
                TellReachedLabel( lbl );
                if( ( blk->edge[ targets ].flags & DEST_LABEL_DIES ) != 0
                  && BlocksUnTrimmed ) {
                    TellCondemnedLabel( lbl );
                    i = targets;
                    while( --i >= 0 ) {
                        if( blk->edge[ i ].destination == lbl ) {
                            blk->edge[ i ].flags &= ~DEST_LABEL_DIES;
                        }
                    }
                }
            }
        }
        if( BlocksUnTrimmed == FALSE
         && blk->label != CurrProc->label && blk->label != NULL ) {
            TellCondemnedLabel( blk->label );
        }
        CurrBlock = blk;
        FreeBlock();
        blk = next_blk;
    }
    HeadBlock = blk;
    BlockList = blk;
    SetOP( old );
}


static  bool    GenId( block *blk, block *next ) {
/************************************************/

    return( blk->gen_id > next->gen_id );
}


extern  void    BlocksSortedBy( bool (*bigger)( block *, block * ) ) {
/********************************************************************/
    block       *blk;
    block       *next;
    bool        change;

    for(;;) {
        blk = HeadBlock->next_block;
        if( blk == NULL ) break;
        change = FALSE;
        for(;;) {
            next = blk->next_block;
            if( next == NULL ) break;
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
                change = TRUE;
            }
            blk = next;
        }
        if( change == FALSE ) break;
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

static  block   *BQRemove( block_queue *q, block *blk ) {
/*******************************************************/
    if( blk == NULL ) blk = q->first;
    if( blk->prev_block ) blk->prev_block->next_block = blk->next_block;
    if( blk->next_block ) blk->next_block->prev_block = blk->prev_block;
    if( q->first == blk ) q->first = blk->next_block;
    if( q->last  == blk ) q->last = blk->prev_block;
    return( blk );
}

static  block_edge *FindLoopBackEdge( block *blk ) {
/**************************************************/
    block_edge          *edge;
    block_num           i;

    for( i = 0; i < blk->targets; i++ ) {
        edge = &blk->edge[ i ];
        if( edge->destination == blk->loop_head ) return( edge );
        if( edge->destination == blk ) return( edge );
    }
    return( NULL );
}

static  instruction     *FindCondition( block *blk ) {
/****************************************************/

    instruction         *cond;

    cond = blk->ins.hd.prev;
    while( !_OpIsCondition( cond->head.opcode ) ) {
        if( cond->head.opcode == OP_BLOCK ) {
            // _Zoiks( ZOIKS_XXX );
            return( NULL );
        }
        cond = cond->head.prev;
    }
    return( cond );
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
    name                *op1;
    name                *op2;

    blk = blk;
    prediction = DNA;
    op1 = cond->operands[ 0 ];
    op2 = cond->operands[ 1 ];
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
                if( op2->c.int_value == 0 ) {
                    prediction = NOT_TAKEN;
                }
            }
        }
        break;
    case OP_CMP_GREATER:
    case OP_CMP_GREATER_EQUAL:
        if( op2->n.class == N_CONSTANT ) {
            if( op2->c.const_type == CONS_ABSOLUTE ) {
                if( op2->c.int_value == 0 ) {
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
        if( _OpIsCall( ins->head.opcode ) ) return( TRUE );
    }
    return( FALSE );
}

static  void    PushTargets( void *stack, block *blk ) {
/******************************************************/

    block_num   i;

    for( i = 0; i < blk->targets; i++ ) {
        EdgeStackPush( stack, &blk->edge[ i ] );
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

    ClearBlockBits( FLOODED );
    stack = EdgeStackInit();
    PushTargets( stack, from );
    from->class |= FLOODED;
    while( !EdgeStackEmpty( stack ) ) {
        edge = EdgeStackPop( stack );
        dest = edge->destination;
        if( ( dest->class & FLOODED ) != EMPTY ) continue;
        decision = func( dest, parm );
        if( decision == ABORT ) continue;
        if( decision == STOP ) break;
        dest->class |= FLOODED;
        PushTargets( stack, dest );
    }
    EdgeStackFini( stack );
}

static  flood_decision PDFloodFunc( block *blk, flood_info *info ) {
/******************************************************************/

    if( blk == info->dominator ) return( ABORT );
    if( ( blk->class & RETURN ) != EMPTY ) {
        info->post_dominates = FALSE;
        return( STOP );
    }
    return( CONTINUE );
}

static  bool    PostDominates( block *dominator, block *blk ) {
/**************************************************************
    Return TRUE if dominator post-dominates blk.
    To determine this, we just flood down aborting whenever we hit
    previously encountered blocks or the dominator. If we hit a
    RETURN block, we return FALSE.
*/

    flood_info          info;

    info.post_dominates = TRUE;
    info.dominator = dominator;
    FloodDown( blk, PDFloodFunc, (void *)&info );
    return( info.post_dominates );
}

static  bool    CallApplies( block *src, block *dst ) {
/*****************************************************/

    src = dst;
    if( BlockContainsCall( dst ) ) {
        if( !PostDominates( dst, src ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static  int     CallHeuristic( block *blk, instruction *cond ) {
/**************************************************************/

    int         prediction;

    prediction = DNA;
    if( CallApplies( blk, blk->edge[ 0 ].destination ) ) {
        if( !CallApplies( blk, blk->edge[ 1 ].destination ) ) {
            prediction = Want( cond, 1 );
        }
    } else {
        if( CallApplies( blk, blk->edge[ 1 ].destination ) ) {
            prediction = Want( cond, 0 );
        }
    }
    return( prediction );
}

static  bool    LoopApplies( block *blk ) {
/*****************************************/

    if( ( blk->class & LOOP_HEADER ) != EMPTY ) return( TRUE );
    if( ( blk->class & JUMP ) != EMPTY ) {
        if( ( blk->edge[ 0 ].destination->class & LOOP_HEADER ) != EMPTY ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static  int     LoopHeuristic( block *blk, instruction *cond ) {
/**************************************************************/

    int         prediction;

    prediction = DNA;
    if( LoopApplies( blk->edge[ 0 ].destination ) ) {
        if( !LoopApplies( blk->edge[ 1 ].destination ) ) {
            prediction = Want( cond, 0 );
        }
    } else {
        if( LoopApplies( blk->edge[ 1 ].destination ) ) {
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
            if( ins->operands[ i ] == reg ) {
                return( !PostDominates( dst, blk ) );
            }
            if( ReDefinedBy( ins, reg ) ) {
                return( FALSE );
            }
        }
    }
    return( FALSE );
}

static  int     TryGuard( block *blk, instruction *cond, name *reg ) {
/********************************************************************/

    int         prediction;

    prediction = DNA;
    if( GuardApplies( blk, blk->edge[ 0 ].destination, reg ) ) {
        if( !GuardApplies( blk, blk->edge[ 1 ].destination, reg ) ) {
            prediction = Want( cond, 0 );
        }
    } else {
        if( GuardApplies( blk, blk->edge[ 1 ].destination, reg ) ) {
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

    op1 = cond->operands[ 0 ];
    op2 = cond->operands[ 1 ];
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

    if( PostDominates( next, blk ) ) return( FALSE );
    for( ins = next->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        if( ins->result == NULL ) continue;
        switch( ins->result->n.class ) {
        case N_MEMORY:
        case N_INDEXED:
        // case N_TEMP:
            return( TRUE );
        }
    }
    return( FALSE );
}

static  int     StoreHeuristic( block *blk, instruction *cond ) {
/***************************************************************/

    int         prediction;

    prediction = DNA;
    if( StoreApplies( blk, blk->edge[ 0 ].destination ) ) {
        if( !StoreApplies( blk, blk->edge[ 1 ].destination ) ) {
            prediction = Want( cond, 1 );
        }
    } else {
        if( StoreApplies( blk, blk->edge[ 1 ].destination ) ) {
            prediction = Want( cond, 0 );
        }
    }
    return( prediction );
}

static  bool    ReturnApplies( block *blk ) {
/*******************************************/

    if( ( blk->class & RETURN ) != EMPTY ) return( TRUE );
    if( ( blk->class & JUMP ) != EMPTY ) {
        if( ( blk->edge[ 0 ].destination->class & RETURN ) != EMPTY ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static  int     ReturnHeuristic( block *blk, instruction *cond ) {
/****************************************************************/

    int         prediction;

    prediction = DNA;
    if( ReturnApplies( blk->edge[ 0 ].destination ) ) {
        if( !ReturnApplies( blk->edge[ 1 ].destination ) ) {
            prediction = Want( cond, 1 );
        }
    } else {
        if( ReturnApplies( blk->edge[ 1 ].destination ) ) {
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
            ptr = Heuristics[ i ];
            prediction = ptr( blk, cond );
            switch( prediction ) {
            case TAKEN:
                return( blk->edge[ _TrueIndex( cond ) ].destination );
            case NOT_TAKEN:
                return( blk->edge[ _FalseIndex( cond ) ].destination );
            }
        }
    }
    /* what the hell, pick one at random */
    return( blk->edge[ 0 ].destination );
}

#define _Placed( x )    (((x)->class&BLOCK_VISITED)!=EMPTY)
#define _MarkPlaced( x )((x)->class|=BLOCK_VISITED)

static  block   *BestFollower( block_queue *unplaced, block *blk ) {
/******************************************************************/

    block       *best;
    block       *curr;
    block_num   i;

    best = NULL;
    switch( ( blk->class & (RETURN|JUMP|CONDITIONAL|SELECT|CALL_LABEL) ) ) {
    case RETURN:
    case JUMP:
    case SELECT:
        for( i = 0; i < blk->targets; i++ ) {
            best = blk->edge[ i ].destination;
            if( !_Placed( best ) ) return( best );
        }
        best = NULL;
        break;
    case CONDITIONAL:
        /*
         * If exactly one of the followers has already been placed,
         * then the other one is obviously the best candidate. Otherwise,
         * we run branch prediction if neither is placed, or return NULL
         * if both have been placed.
         */
        #define _Munge( a, b )  ( ( (a) << 8 ) + (b) )
        switch( _Munge( _Placed( blk->edge[ 0 ].destination ),
                        _Placed( blk->edge[ 1 ].destination ) ) ) {
        case _Munge( 0, 0 ):
            /* get some branch prediction going here */
            best = Predictor( blk );
            break;
        case _Munge( 1, 0 ):
            best = blk->edge[ 1 ].destination;
            break;
        case _Munge( 0, 1 ):
            best = blk->edge[ 0 ].destination;
            break;
        case _Munge( 1, 1 ):
            best = NULL;
            break;
        }
        break;
    case CALL_LABEL:
        for( curr = BQFirst( unplaced ); curr != NULL; curr = BQNext( unplaced, curr ) ) {
            if( curr->gen_id == ( blk->gen_id + 1 ) ) {
                best = curr;
                break;
            }
        }
        assert( best != NULL );
        assert( ( best->class & RETURNED_TO ) != EMPTY );
        break;
    }
    return( best );
}

extern  void    SortBlocks( void )
/********************************/
{
    block_queue unplaced;
    block_queue placed;
    block       *curr;
    block       *next;
    block       *ret_block;

    ClearBlockBits( BLOCK_VISITED );
    BlocksSortedBy( GenId );
    if( _IsModel( NO_OPTIMIZATION ) ) return;
    if( _IsntModel( BRANCH_PREDICTION ) ) return;
    if( OptForSize > 50 ) return;
    // we can't screw about with the placement of the return
    // block when we are outputting records which mark the start
    // of the epilog etc...
    if( _IsModel( DBG_LOCALS ) ) return;
    BQInit( &unplaced );
    BQInit( &placed );
    for( curr = HeadBlock; curr != NULL; curr = next ) {
        next = curr->next_block;
        BQAdd( &unplaced, curr );
        if( ( curr->class & RETURNED_TO ) != EMPTY ) {
            // blocks which are returned to by a call_label routine
            // should not be placed because they are special cased in
            // BestFollower to come out directly after the CALL_LABEL
            // block
            _MarkPlaced( curr );
        }
        ret_block = curr;
    }
    while( !BQEmpty( &unplaced ) ) {
        curr = BQRemove( &unplaced, NULL );
        if( _Placed( curr ) ) continue;
        for( ;; ) {
            BQAdd( &placed, curr );
            _MarkPlaced( curr );
            curr = BestFollower( &unplaced, curr );
            if( curr == NULL ) break;
            BQRemove( &unplaced, curr );
        }
    }
    HeadBlock = placed.first;
    ClearBlockBits( BLOCK_VISITED );
}
