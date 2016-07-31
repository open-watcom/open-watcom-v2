/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "zoiks.h"
#include "cgmem.h"
#include "data.h"
#include "makeins.h"
#include "namelist.h"
#include "nullprop.h"
#include "flowsave.h"
#include "rgtbl.h"
#include "score.h"
#include "insutil.h"


extern  hw_reg_set      PushRegs[];

extern  uint_32         CountBits( uint_32 );

static  block           *blockArray[_DBit_SIZE];
static  int             bitCount;
static  hw_reg_set      flowedRegs;

#if 0
static block *FindDominatorBlock( dom_bit_set *dom, bool post )
/**************************************************************
    Returns pointer to the block with the given dominator set. If
    post == true, then looks at post_dominator set instead.
*/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( post ) {
            if( _DBitSame( *dom, blk->dom.post_dominator ) ) return( blk );
        } else {
            if( _DBitSame( *dom, blk->dom.dominator ) ) return( blk );
        }
    }
    return( NULL );
}
#endif

static bool OpRefsReg( name *op, hw_reg_set reg )
/***********************************************/
{
    switch( op->n.class ) {
    case N_REGISTER:
        if( HW_Ovlap( op->r.reg, reg ) ) return( true );
        break;
    case N_INDEXED:
        assert( op->i.index->n.class == N_REGISTER );
        if( HW_Ovlap( op->i.index->r.reg, reg ) ) return( true );
        break;
    }
    return( false );
}

static bool BlockUses( block *blk, hw_reg_set reg )
/*************************************************/
{
    instruction *ins;
    int         i;

    for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
        for( i = 0; i < ins->num_operands; i++ ) {
            if( OpRefsReg( ins->operands[i], reg ) ) return( true );
        }
        if( ins->result != NULL && ins->head.opcode != OP_NOP ) {
            if( OpRefsReg( ins->result, reg ) ) return( true );
        }
        if( ins->head.opcode != OP_NOP && HW_Ovlap( ins->zap->reg, reg ) ) return( true );
    }
    return( false );
}

static bool     InLoop( block *blk )
/**********************************/
{
    if( blk->loop_head != NULL )
        return( true );
    if( _IsBlkAttr( blk, BLK_LOOP_HEADER ) )
        return( true );
    return( false );
}

#if 1
static void DoFix( block *blk )
/*****************************/
{
    block_num   i;

    if( _IsBlkVisited( blk ) )
        return;
    if( _IsBlkAttr( blk, BLK_RETURN ) )
        _Zoiks( ZOIKS_140 );
    blk->stack_depth += WORD_SIZE;
    _MarkBlkVisited( blk );
    for( i = 0; i < blk->targets; i++ ) {
        DoFix( blk->edge[i].destination.u.blk );
    }
}

static void FixStackDepth( block *save, block *restore )
/*******************************************************
    Flow down our flow-graph noodling the stack-depth
    variable for each block to take into account the
    additional register pushed on. NOTE: we don't change
    the stack_depth for the save block itself
    since AdjustStackDepth will take care of that.
*/
{
    block_num   i;

    _MarkBlkAllUnVisited();
    _MarkBlkVisited( restore );
    _MarkBlkVisited( save );
    if( save != restore ) {
        restore->stack_depth += WORD_SIZE;
        for( i = 0; i < save->targets; i++ ) {
            DoFix( save->edge[i].destination.u.blk );
        }
    }
}
#endif

static int CountBlocks( void )
/****************************/
{
    block               *blk;
    int                 i;

    i = 0;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        i++;
    }
    return( i );
}

static int CountRegs( hw_reg_set regs )
/*************************************/
{
    hw_reg_set          *curr;
    int                 count;

    count = 0;
    for( curr = PushRegs; !HW_CEqual( *curr, HW_EMPTY ); ++curr ) {
        if( HW_Ovlap( *curr, regs ) ) {
            count++;
        }
    }
    return( count );
}

static void InitBlockArray( void )
/********************************/
{
    block               *blk;
    int                 i;

    i = 0;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        blockArray[i++] = blk;
    }
}

static void DoCountBits( a_bit_set set )
/**************************************/
{
    bitCount += CountBits( set );
}

static int CountDomBits( dom_bit_set *dbits )
/*******************************************/
{
    bitCount = 0;
    _DBitIter( DoCountBits, *dbits );
    return( bitCount );
}

typedef struct {
    hw_reg_set          reg;                    // actual register bit
    dom_bit_set         dom_usage;              // dominator set for all uses/references
    dom_bit_set         post_dom_usage;         // post-dominator "
    block               *save;                  // block we have picked to save reg in
    block               *restore;               // ditto for restore
} reg_flow_info;

static void GetRegUsage( reg_flow_info *info )
/********************************************/
{
    block       *blk;

    _DBitInit( info->dom_usage, ~0U );
    _DBitInit( info->post_dom_usage, ~0U );
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( BlockUses( blk, info->reg ) ) {
            _DBitIntersect( info->dom_usage, blk->dom.dominator );
            _DBitIntersect( info->post_dom_usage, blk->dom.post_dominator );
        }
    }
}

static bool PairOk( block *save, block *restore, reg_flow_info *info, int curr_reg )
/**********************************************************************************/
{
    int                 i;

    if( !_DBitOverlap( save->dom.id, info[curr_reg].dom_usage ) ) return( false );
    if( !_DBitOverlap( restore->dom.id, info[curr_reg].post_dom_usage ) ) return( false );
    if( !_DBitOverlap( save->dom.id, restore->dom.dominator ) ) return( false );
    if( !_DBitOverlap( restore->dom.id, save->dom.post_dominator ) ) return( false );
    if( _IsBlkAttr( restore, BLK_CONDITIONAL | BLK_SELECT ) )
        return( false );
    if( InLoop( save ) || InLoop( restore ) ) return( false );
    for( i = 0; i < curr_reg; i++ ) {
        // now, either our save/restore must dominate/postdominate info[i]'s
        // or info[i]'s save/restore must dominate/postdominate ours
        if( info[i].save == NULL || info[i].restore == NULL ) continue;
        if( info[i].save == save && info[i].restore != restore ) return( false );
        if( info[i].save != save && info[i].restore == restore ) return( false );
        if( _DBitOverlap( info[i].save->dom.dominator, save->dom.id ) ) {
            // save dominates info[i].save - now check that restore postdominate
            if( _DBitOverlap( info[i].restore->dom.post_dominator, restore->dom.id ) ) continue;
        } else if( _DBitOverlap( info[i].save->dom.id, save->dom.dominator ) ) {
            // info[i].save dominates save, now make sure info[i].restore postdom's restore
            if( _DBitOverlap( info[i].restore->dom.id, restore->dom.post_dominator ) ) continue;
        }
        return( false );
    }
    return( true );
}

void FlowSave( hw_reg_set *preg )
/*******************************/
{
    int                 score;
    int                 i, j;
    int                 best;
    int                 num_blocks;
    int                 num_regs;
    int                 curr_reg;
    hw_reg_set          *curr_push;
    reg_flow_info       *reg_info;
    block               *save;
    block               *restore;
    instruction         *ins;
    type_class_def      reg_type;

    HW_CAsgn( flowedRegs, HW_EMPTY );
    if( _IsntModel( FLOW_REG_SAVES ) ) return;
    if( !HaveDominatorInfo ) return;
    // we can't do this if we have push's which are 'live' at the end of a block
    // - this flag is set when we see a push being generated for a call in a different
    //   block
#if _TARGET & _TARG_INTEL
    if( CurrProc->targ.never_sp_frame ) return;
#endif
    num_regs = CountRegs( *preg );
    if( num_regs == 0 ) return;
    reg_info = CGAlloc( num_regs * sizeof( reg_flow_info ) );
    num_blocks = CountBlocks();
    InitBlockArray();
    curr_push = PushRegs;
    for( curr_reg = 0; curr_reg < num_regs; curr_reg++ ) {
        while( !HW_Ovlap( *curr_push, *preg ) ) curr_push++;
        HW_Asgn( reg_info[curr_reg].reg, *curr_push );
        reg_info[curr_reg].save = NULL;
        reg_info[curr_reg].restore = NULL;
    #if _TARGET & _TARG_INTEL
        if( HW_COvlap( *curr_push, HW_BP ) ) continue;  // don't mess with BP - it's magical
    #endif
        GetRegUsage( &reg_info[curr_reg] );
        best = 0;
        for( i = 0; i < num_blocks; i++ ) {
            for( j = 0; j < num_blocks; j++ ) {
                if( PairOk( blockArray[i], blockArray[j], &reg_info[0], curr_reg ) ) {
                    // we use the number of blocks dominated by the save block plus
                    // the number of blocks post-dominated by the restore block as a
                    // rough metric for determining how much we like a given (valid)
                    // pair of blocks - the more blocks dominated, the further 'in'
                    // we have pushed the save, which should be good
                    score =  CountDomBits( &blockArray[i]->dom.dominator );
                    score += CountDomBits( &blockArray[j]->dom.post_dominator );
                    if( score > best ) {
                        best = score;
                        reg_info[curr_reg].save    = blockArray[i];
                        reg_info[curr_reg].restore = blockArray[j];
                    }
                }
            }
        }
        // so now we know where we are going to save and restore the register
        // emit the instructions to do so, and remove reg from the set to push
        // in the normal prolog sequence
        save = reg_info[curr_reg].save;
        restore = reg_info[curr_reg].restore;
        if( ( save != NULL && save != HeadBlock ) && ( restore != NULL && !_IsBlkAttr( restore, BLK_RETURN ) ) ) {
            reg_type = WD;
        #if _TARGET & _TARG_INTEL
            if( IsSegReg( reg_info[curr_reg].reg ) ) {
                reg_type = U2;
            }
        #endif
            ins = MakeUnary( OP_PUSH, AllocRegName( reg_info[curr_reg].reg ), NULL, reg_type );
            ResetGenEntry( ins );
            PrefixIns( save->ins.hd.next, ins );
            ins = MakeUnary( OP_POP, NULL, AllocRegName( reg_info[curr_reg].reg ), reg_type );
            ins->num_operands = 0;
            ResetGenEntry( ins );
            SuffixIns( restore->ins.hd.prev, ins );
            HW_TurnOff( *preg, reg_info[curr_reg].reg );
            HW_TurnOn( flowedRegs, reg_info[curr_reg].reg );
            FixStackDepth( save, restore );
        }
        curr_push++;
    }
    CGFree( reg_info );
}

void FlowRestore( hw_reg_set *preg )
/**********************************/
{
    HW_TurnOff( *preg, flowedRegs );
}
