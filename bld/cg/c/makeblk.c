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
* Description:  Block creation and destruction functions.
*
****************************************************************************/


#include "_cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "cgauxinf.h"
#include "zoiks.h"
#include "makeins.h"
#include "data.h"
#include "utils.h"
#include "stackcg.h"
#include "makeaddr.h"
#include "namelist.h"
#include "targetin.h"
#include "opttell.h"
#include "makeblk.h"
#include "typemap.h"
#include "blktrim.h"
#include "bgcall.h"
#include "edge.h"
#include "feprotos.h"


#define BLOCK_SIZE(n)   (sizeof( block ) + (n - 1) * sizeof( block_edge ))

block   *MakeBlock( label_handle label, block_num targets )
/*********************************************************/
{
    block       *blk;
    block_num   i;

    blk = CGAlloc( BLOCK_SIZE( targets ) );
    blk->next_block = NULL;
    blk->prev_block = NULL;
    blk->label = label;
    blk->class = 0;
    blk->ins.head.line_num = 0;
    blk->ins.head.next = (instruction *)&blk->ins;
    blk->ins.head.prev = (instruction *)&blk->ins;
    blk->ins.head.opcode = OP_BLOCK;
    HW_CAsgn( blk->ins.head.live.regs, HW_EMPTY );
    _LBitInit( blk->ins.head.live.within_block, EMPTY );
    _GBitInit( blk->ins.head.live.out_of_block, EMPTY );
    blk->ins.blk = blk;
    blk->u.interval = NULL;
    blk->inputs = 0;
    blk->input_edges = NULL;
    blk->targets = 0;
    blk->dataflow = NULL;
    blk->u1.cc = NULL;
    blk->loop_head = NULL;
    blk->unroll_count = 0;
    blk->stack_depth = 0;
    blk->depth = 0;
    _DBitInit( blk->dom.id, 0U );
    for( i = 0; i < targets; i++ ) {
        blk->edge[i].source = blk;
    }
    blk->blk_id = BLK_ID_NONE;
    blk->gen_blk_id = BLK_ID_NONE;
    return( blk );
}


block   *MakeBlockCopy( block_num targets, block *src, block_num src_targets )
{
    block       *blk;

    blk = CGAlloc( BLOCK_SIZE( targets ) );
    Copy( src, blk, BLOCK_SIZE( src_targets ) );
    return( blk );
}


block   *NewBlock( label_handle label, bool label_dies )
/******************************************************/
{
    block       *blk;

    blk = MakeBlock( label, 1 );
    blk->edge[0].flags = ( label_dies ) ? BEF_BLOCK_LABEL_DIES : BEF_NONE;
    return( blk );
}


void    FreeABlock( block * blk )
/*******************************/
{
    if( blk->targets <= 1 ) {
        CGFree( blk );
    } else {
        CGFree( blk );
    }
}


void    FreeBlock( void )
/***********************/
{
    while( CurrBlock->ins.head.next != (instruction *)&CurrBlock->ins ) {
        FreeIns( CurrBlock->ins.head.next );
    }
    if( CurrBlock->dataflow != NULL ) {
        CGFree( CurrBlock->dataflow );
    }
    FreeABlock( CurrBlock );
}


void    EnLink( label_handle label, bool label_dies )
/***************************************************/
{
    block       *blk;

    blk = NewBlock( label, label_dies );
    blk->ins.head.line_num = SrcLine;
    CurrBlock = blk;
    SrcLine = 0;
}

void    AddIns( instruction *ins )
/********************************/
{
    if( !HaveCurrBlock ) {
        EnLink( AskForNewLabel(), true );
        HaveCurrBlock = true;
    }
    ins->head.next = (instruction *)&CurrBlock->ins;
    ins->head.prev = CurrBlock->ins.head.prev;
    CurrBlock->ins.head.prev->head.next = ins;
    CurrBlock->ins.head.prev = ins;
    ins->head.line_num = SrcLine;
    _INS_NOT_BLOCK( ins );
    ins->id = ++ InsId;
    SrcLine = 0;
}


void    GenBlock( block_class class, block_num targets )
/******************************************************/
{
    block       *new_blk;
    block_edge  *edge;
    instruction *ins;

    NamesCrossBlocks();
    if( HeadBlock == NULL ) {
        HeadBlock = CurrBlock;
        CurrBlock->blk_id = 1;
        CurrBlock->gen_blk_id = 1;
    } else {
        BlockList->next_block = CurrBlock;
        CurrBlock->blk_id = BlockList->blk_id + 1;
        CurrBlock->gen_blk_id = BlockList->gen_blk_id + 1;
    }
    if( SrcLine != 0 ) {
        /*
         * Add an instruction to carry the line number for the block ending
         * opcode (the AddIns code puts in line number automatically).
         */
        ins = MakeNop();
        ins->flags.u.nop_flags |= NOP_SOURCE_QUEUE;
        AddIns( ins );
    }
    CurrBlock->prev_block = BlockList;
    BlockList = CurrBlock;
    CurrBlock->next_block = NULL;
    if( targets > 1 ) {
        new_blk = MakeBlockCopy( targets, CurrBlock, 1 );
        if( CurrBlock->ins.head.next == (instruction *)&CurrBlock->ins ) {
            new_blk->ins.head.next = (instruction *)&new_blk->ins;
            new_blk->ins.head.prev = (instruction *)&new_blk->ins;
        } else {
            new_blk->ins.head.next->head.prev = (instruction *)&new_blk->ins;
            new_blk->ins.head.prev->head.next = (instruction *)&new_blk->ins;
        }
        new_blk->ins.blk = new_blk;
        /*
         * Move all references to CurrBlock
         */
        if( HeadBlock == CurrBlock ) {
            HeadBlock = new_blk;
        }
        if( BlockList == CurrBlock ) {
            BlockList = new_blk;
        }
        if( new_blk->prev_block != NULL ) {
            new_blk->prev_block->next_block = new_blk;
        }
        if( new_blk->next_block != NULL ) {
            new_blk->next_block->prev_block = new_blk;
        }
        for( edge = new_blk->input_edges; edge != NULL; edge = edge->next_source ) {
            edge->destination.u.blk = new_blk;
        }
        CGFree( CurrBlock );
        CurrBlock = new_blk;
    }
    if( _IsBlkAttr( CurrBlock, BLK_BIG_LABEL ) )        /* the only one that sticks*/
        class |= BLK_BIG_LABEL;
    CurrBlock->class = class;
    while( targets-- > 1 ) {
        CurrBlock->edge[targets].flags = BEF_NONE;
    }
}


block   *ReGenBlock( block *blk, label_handle lbl )
/*************************************************/
{
    block       *new_blk;
    block_edge  *edge;
    block_num   targets;

    targets = blk->targets + 1;
    new_blk = MakeBlockCopy( targets, blk, targets - 1 );
    new_blk->edge[targets - 1].destination.u.lbl = lbl;
    new_blk->edge[targets - 1].flags = BEF_NONE;
    new_blk->targets = targets;
    /*
     * Move all references to blk
     */
    if( blk->ins.head.next == (instruction *)&blk->ins ) {
        new_blk->ins.head.next = (instruction *)&new_blk->ins;
        new_blk->ins.head.prev = (instruction *)&new_blk->ins;
    } else {
        blk->ins.head.next->head.prev = (instruction *)&new_blk->ins;
        blk->ins.head.prev->head.next = (instruction *)&new_blk->ins;
    }
    while( targets-- > 0 ) {
        new_blk->edge[targets].source = new_blk;
    }
    new_blk->ins.blk = new_blk;
    if( HeadBlock == blk ) {
        HeadBlock = new_blk;
    }
    if( BlockList == blk ) {
        BlockList = new_blk;
    }
    if( new_blk->prev_block != NULL ) {
        new_blk->prev_block->next_block = new_blk;
    }
    if( new_blk->next_block != NULL ) {
        new_blk->next_block->prev_block = new_blk;
    }
    for( edge = new_blk->input_edges; edge != NULL; edge = edge->next_source ) {
        edge->destination.u.blk = new_blk;
    }
    FreeABlock( blk );
    return( new_blk );
}


type_class_def  InitCallState( const type_def *tipe )
/***************************************************/
{
    name            *name;
    cg_sym_handle   sym;
    aux_handle      aux;

    sym = AskForLblSym( CurrProc->label );
    name = AllocMemory( sym, 0, CG_FE, TypeClass( tipe ) );
    name->v.usage |= USE_MEMORY;        /* so not put in conflict graph*/
    aux = FEAuxInfo( sym, FEINF_AUX_LOOKUP );
    return( CallState( aux, tipe, &CurrProc->state ) );
}


void    AddTarget( label_handle dest, bool dest_label_dies )
/***********************************************************
 * Don't handle expression jumps yet
 */
{
    block_edge  *edge;

    edge = &CurrBlock->edge[CurrBlock->targets++];
    edge->source = CurrBlock;
    edge->destination.u.lbl = dest;
    edge->next_source = NULL;
    if( dest_label_dies ) {
        edge->flags |= BEF_DEST_LABEL_DIES;
    }
}


block   *FindBlockWithLbl( label_handle label )
/*********************************************/
{
    block       *blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk->label == label ) {
            break;
        }
    }
    return( blk );
}


void    FixEdges( void )
/**********************/
{
    block       *blk;
    block       *dest;
    block_num   i;
    block_edge  *edge;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( !_IsBlkAttr( blk, BLK_BIG_JUMP ) ) {
            for( i = blk->targets; i-- > 0; ) {
                edge = &blk->edge[i];
                dest = FindBlockWithLbl( edge->destination.u.lbl );
                if( dest != NULL ) {
                    edge->flags |= BEF_DEST_IS_BLOCK;
                    PointEdge( edge, dest );
                }
            }
        }
    }
}


static label_handle LinkReturnsParms[2];

static void *LinkReturns( void *arg )
/***********************************/
{
    block               *blk;
    block_num           i;
//    bool                found;
    label_handle        link_to;
    label_handle        to_search;

    /* unused parameters */ (void)arg;

    link_to = LinkReturnsParms[0];
    to_search = LinkReturnsParms[1];
    blk = FindBlockWithLbl( to_search );
//    found = false;
    if( blk == NULL )
        return( TO_SR_VALUE( false ) );
    if( _IsBlkVisited( blk ) )
        return( TO_SR_VALUE( true ) );
    if( _IsBlkAttr( blk, BLK_LABEL_RETURN ) ) {
        for( i = blk->targets; i-- > 0; ) {
            if( blk->edge[i].destination.u.lbl == link_to ) {
                /*
                 * kick out ... already linked
                 */
                return( TO_SR_VALUE( true ) );
            }
        }
        blk = ReGenBlock( blk, link_to );
//        found = true;
    } else {
        _MarkBlkVisited( blk );
        if( _IsBlkAttr( blk, BLK_CALL_LABEL ) ) {
            if( blk->next_block == NULL )
                return( TO_SR_VALUE( false ) );
            LinkReturnsParms[0] = link_to;
            LinkReturnsParms[1] = blk->next_block->label;
            if( !FROM_SR_VALUE( SafeRecurseCG( LinkReturns, NULL ), bool ) ) {
                return( TO_SR_VALUE( false ) );
            }
        } else {
            for( i = blk->targets; i-- > 0; ) {
                LinkReturnsParms[0] = link_to;
                LinkReturnsParms[1] = blk->edge[i].destination.u.lbl;
                if( !FROM_SR_VALUE( SafeRecurseCG( LinkReturns, NULL ), bool ) ) {
                    return( TO_SR_VALUE( false ) );
                }
            }
        }
    }
    return( TO_SR_VALUE( true ) );
}

bool        FixReturns( void )
/*****************************
 * link all LABEL_RETURN blocks to any CALL_LABEL block they could
 * have been invoked from
 */
{
    block       *blk;
    block       *other_blk;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( _IsBlkAttr( blk, BLK_CALL_LABEL ) ) {
            _MarkBlkVisited( blk );
            if( blk->next_block == NULL )
                return( false );
            _MarkBlkAttrSet( blk, BLK_RETURNED_TO );
            LinkReturnsParms[0] = blk->next_block->label;
            LinkReturnsParms[1] = blk->edge[0].destination.u.lbl;
            if( !FROM_SR_VALUE( LinkReturns( NULL ), bool ) ) {
                return( false );
            }
            for( other_blk = HeadBlock; other_blk != NULL; other_blk = other_blk->next_block ) {
                _MarkBlkUnVisited( other_blk );
            }
        }
    }
    return( true );
}


void    UnFixEdges( void )
/************************/
{
    block       *blk;
    block_num   i;
    block_edge  *edge;

    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( !_IsBlkAttr( blk, BLK_BIG_JUMP ) ) {
            for( i = blk->targets; i-- > 0; ) {
                edge = &blk->edge[i];
                if( edge->flags & BEF_DEST_IS_BLOCK ) {
                    RemoveInputEdge( edge );
                    edge->destination.u.lbl = edge->destination.u.blk->label;
                    edge->flags &= ~BEF_DEST_IS_BLOCK;
                }
            }
        }
    }
}


void    AddAnIns( block *blk, instruction *ins )
/**********************************************/
{
    block       *curr_block;

    curr_block = CurrBlock;
    CurrBlock = blk;
    AddIns( ins );
    CurrBlock = curr_block;
}


bool    BlkTooBig( void )
/***********************/
{
    label_handle    blk;

    if( !HaveCurrBlock )
        return( false );
    if( CurrBlock == NULL )
        return( false );
    if( CurrBlock->ins.head.next == (instruction *)&CurrBlock->ins )
        return( false );
    _INS_NOT_BLOCK( CurrBlock->ins.head.next );
    if( (InsId - CurrBlock->ins.head.next->id) < INS_PER_BLOCK )
        return( false );
    if( CurrBlock->targets != 0 )
        return( false );
    blk = AskForNewLabel();
    GenBlock( BLK_JUMP, 1 );
    AddTarget( blk, false );
    EnLink( blk, true );
    return( true );
}


void    NewProc( level_depth level )
/**********************************/
{
    proc_def        *new_proc;
    name_class_def  class;

    if( CurrProc != NULL ) {
        SaveToTargProc();
        CurrProc->head_block = HeadBlock;
        CurrProc->tail_block = BlockList;
        CurrProc->curr_block = CurrBlock;
        CurrProc->lasttemp = LastTemp;
        CurrProc->dummy_index = DummyIndex;
        for( class = 0; class < N_CLASS_MAX; class++ ) {
            CurrProc->names[class] = Names[class];
        }
        CurrProc->block_by_block = BlockByBlock;
        CurrProc->ins_id = InsId;
        CurrProc->untrimmed = BlocksUnTrimmed;
    }
    HaveCurrBlock = true;
    BlocksUnTrimmed = true;
    MaxStack = 0;
    InsId = 0;
    HeadBlock = NULL;
    BlockList = NULL;
    ReInitNames();
    new_proc = CGAlloc( sizeof( proc_def ) );
    memset( new_proc, 0, sizeof( proc_def ) );
    new_proc->next_proc = CurrProc;
    CurrProc = new_proc;
    new_proc->frame_index = NULL;
    new_proc->lex_level = level;
    new_proc->parms.size = 0;
    new_proc->parms.base = 0;
    new_proc->locals.size = 0;
    new_proc->locals.base = AskDisplaySize( level );
    new_proc->prolog_state = 0;
    new_proc->parms_list = NULL;
    InitTargProc();
}


void    FreeProc( void )
/**********************/
{
    proc_def        *oldproc;
    name_class_def  class;

    for( CurrBlock = HeadBlock; CurrBlock != NULL; CurrBlock = HeadBlock ) {
        HeadBlock = CurrBlock->next_block;
        FreeBlock();
    }
    BlockList = NULL;
    FreeNames();
    TellBeginExecutions();
    oldproc = CurrProc;
    if( oldproc != NULL ) {
        CurrProc = CurrProc->next_proc;
        if( oldproc->state.parm.table ) {
            CGFree( oldproc->state.parm.table );
        }
        CGFree( oldproc );
        if( CurrProc != NULL ) {
            RestoreFromTargProc();
            InsId = CurrProc->ins_id;
            HeadBlock = CurrProc->head_block;
            BlockList = CurrProc->tail_block;
            CurrBlock = CurrProc->curr_block;
            for( class = 0; class < N_CLASS_MAX; class++ ) {
                Names[class] = CurrProc->names[class];
            }
            LastTemp = CurrProc->lasttemp;
            DummyIndex = CurrProc->dummy_index;
            BlockByBlock = CurrProc->block_by_block;
            BlocksUnTrimmed = CurrProc->untrimmed;
        } else {
            InsId = 0;
            BlockByBlock = false;
        }
    }
}
