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
* Description:  Block creation and destruction functions.
*
****************************************************************************/


#include "cgstd.h"
#include <string.h>
#include "cgdefs.h"
#include "coderep.h"
#include "cgmem.h"
#include "cgaux.h"
#include "zoiks.h"
#include "makeins.h"
#include "data.h"
#include "feprotos.h"
#include "utils.h"

extern  void            TellBeginExecutions(void);
extern  void            FreeNames(void);
extern  int             AskDisplaySize(int);
extern  void            ReInitNames(void);
extern  type_class_def  CallState(aux_handle,type_def*,call_state*);
extern  name            *AllocMemory(pointer,type_length,cg_class,type_class_def);
extern  type_class_def  TypeClass(type_def*);
extern  byte            *Copy(void*,void*,uint);
extern  void            NamesCrossBlocks(void);
extern  void            RemoveInputEdge(block_edge*);
extern  pointer         SafeRecurse( pointer (* rtn)(), pointer arg );
extern  void            SaveToTargProc(void);
extern  void            RestoreFromTargProc(void);
extern  void            InitTargProc(void);


extern  block   *MakeBlock( label_handle label, block_num edges )
/***************************************************************/
{
    block       *blk;
    block_edge  *edge;
    block_num   i;

    blk = CGAlloc( sizeof( block ) + (edges-1)*sizeof( block_edge ) );
    blk->next_block = NULL;
    blk->prev_block = NULL;
    blk->label = label;
    blk->class = EMPTY;
    blk->ins.hd.next = (instruction *)&blk->ins;
    blk->ins.hd.prev = (instruction *)&blk->ins;
    blk->ins.hd.opcode = OP_BLOCK;
    HW_CAsgn( blk->ins.hd.live.regs, HW_EMPTY );
    _LBitInit( blk->ins.hd.live.within_block, EMPTY );
    _GBitInit( blk->ins.hd.live.out_of_block, EMPTY );
    blk->ins.blk = blk;
    blk->u.interval = NULL;
    blk->inputs = 0;
    blk->input_edges = NULL;
    blk->targets = 0;
    blk->dataflow = NULL;
    blk->cc = NULL;
    blk->loop_head = NULL;
    blk->unroll_count = 0;
    blk->stack_depth = 0;
    blk->depth = 0;
    _DBitInit( blk->dom.id, 0 );
    for( i = 0; i < edges; i++ ) {
        edge = &blk->edge[ i ];
        edge->source = blk;
    }
    return( blk );
}


extern  block   *NewBlock( label_handle label, bool label_dies )
/**************************************************************/
{
    block       *blk;

    blk = MakeBlock( label, 1 );
    if( label_dies ) {
        blk->edge[ 0 ].flags = BLOCK_LABEL_DIES;
    } else {
        blk->edge[ 0 ].flags = 0;
    }
    return( blk );
}


extern  void    FreeABlock( block * blk )
/***************************************/
{
    if( blk->targets <= 1 ) {
        CGFree( blk );
    } else {
        CGFree( blk );
    }
}


extern  void    FreeBlock( void )
/*******************************/
{
    while( CurrBlock->ins.hd.next != (instruction *)&CurrBlock->ins ) {
        FreeIns( CurrBlock->ins.hd.next );
    }
    if( CurrBlock->dataflow != NULL ) {
        CGFree( CurrBlock->dataflow );
    }
    FreeABlock( CurrBlock );
}


extern  void    EnLink( label_handle label, bool label_dies )
/***********************************************************/
{
    block       *blk;

    blk = NewBlock( label, label_dies );
    blk->ins.hd.line_num = SrcLine;
    CurrBlock = blk;
    SrcLine = 0;
}

extern  void    AddIns( instruction *ins )
/****************************************/
{
    if( HaveCurrBlock == FALSE ) {
        EnLink( AskForNewLabel(), TRUE );
        HaveCurrBlock = TRUE;
    }
    ins->head.next = (instruction *)&CurrBlock->ins;
    ins->head.prev = CurrBlock->ins.hd.prev;
    CurrBlock->ins.hd.prev->head.next = ins;
    CurrBlock->ins.hd.prev = ins;
    ins->head.line_num = SrcLine;
    _INS_NOT_BLOCK( ins );
    ins->id = ++ InsId;
    SrcLine = 0;
}


extern  void    GenBlock( block_class class, int targets )
/********************************************************/
{
    block       *new;
    block_edge  *edge;
    instruction *ins;

    NamesCrossBlocks();
    if( HeadBlock == NULL ) {
        HeadBlock = CurrBlock;
        CurrBlock->id = 1;
        CurrBlock->gen_id = 1;
    } else {
        BlockList->next_block = CurrBlock;
        CurrBlock->id = BlockList->id + 1;
        CurrBlock->gen_id = BlockList->gen_id + 1;
    }
    if( SrcLine != 0 ) {
        /* Add an instruction to carry the line number for the block ending
           opcode (the AddIns code puts in line number automatically). */
        ins = MakeNop();
        ins->flags.nop_flags |= NOP_SOURCE_QUEUE;
        AddIns( ins );
    }
    CurrBlock->prev_block = BlockList;
    BlockList = CurrBlock;
    CurrBlock->next_block = NULL;
    if( targets > 1 ) {
        new = CGAlloc( sizeof( block ) + (targets-1) * sizeof( block_edge ) );
        Copy( CurrBlock, new, sizeof( block ) );
        if( CurrBlock->ins.hd.next == (instruction *)&CurrBlock->ins ) {
            new->ins.hd.next = (instruction *)&new->ins;
            new->ins.hd.prev = (instruction *)&new->ins;
        } else {
            new->ins.hd.next->head.prev = (instruction *)&new->ins;
            new->ins.hd.prev->head.next = (instruction *)&new->ins;
        }
        new->ins.blk = new;

        /*   Move all references to CurrBlock*/

        if( HeadBlock == CurrBlock ) {
            HeadBlock = new;
        }
        if( BlockList == CurrBlock ) {
            BlockList = new;
        }
        if( new->prev_block != NULL ) {
            new->prev_block->next_block = new;
        }
        if( new->next_block != NULL ) {
            new->next_block->prev_block = new;
        }
        edge = new->input_edges;
        while( edge != NULL ) {
            edge->destination = new;
            edge = edge->next_source;
        }
        CGFree( CurrBlock );
        CurrBlock = new;
    }
    CurrBlock->class &= BIG_LABEL;   /* the only one that sticks*/
    CurrBlock->class |= class;
    while( --targets >= 1 ) {
        CurrBlock->edge[ targets ].flags = 0;
    }
}


extern  block   *ReGenBlock( block *blk, label_handle lbl )
/*********************************************************/
{
    block       *new;
    block_edge  *edge;
    int         targets;

    targets = blk->targets + 1;
    new = CGAlloc( sizeof( block ) + (targets-1) * sizeof( block_edge ) );
    Copy( blk, new, sizeof( block ) + ( targets - 2 ) * sizeof( block_edge ) );
    new->edge[ targets-1 ].destination = lbl;
    new->edge[ targets-1 ].flags = 0;
    new->targets = targets;

    /*   Move all references to blk*/

    if( blk->ins.hd.next == (instruction *)&blk->ins ) {
        new->ins.hd.next = (instruction *)&new->ins;
        new->ins.hd.prev = (instruction *)&new->ins;
    } else {
        blk->ins.hd.next->head.prev = (instruction *)&new->ins;
        blk->ins.hd.prev->head.next = (instruction *)&new->ins;
    }
    while( --targets >= 0 ) {
        new->edge[ targets ].source = new;
    }
    new->ins.blk = new;
    if( HeadBlock == blk ) {
        HeadBlock = new;
    }
    if( BlockList == blk ) {
        BlockList = new;
    }
    if( new->prev_block != NULL ) {
        new->prev_block->next_block = new;
    }
    if( new->next_block != NULL ) {
        new->next_block->prev_block = new;
    }
    edge = new->input_edges;
    while( edge != NULL ) {
        edge->destination = new;
        edge = edge->next_source;
    }
    FreeABlock( blk );
    return( new );
}


extern  type_class_def  InitCallState( type_def *tipe )
/*****************************************************/
{
    name        *name;
    sym_handle  sym;
    pointer     aux;

    sym = AskForLblSym( CurrProc->label );
    name = AllocMemory( sym, 0, CG_FE, TypeClass( tipe ) );
    name->v.usage |= USE_MEMORY;        /* so not put in conflict graph*/
    aux = FEAuxInfo( sym, AUX_LOOKUP );
    return( CallState( aux, tipe, &CurrProc->state ) );
}


extern  void    AddTarget( label_handle dest, bool dest_label_dies )
/******************************************************************/
/*   Don't handle expression jumps yet*/
{
    block_edge  *edge;

    edge = &CurrBlock->edge[  CurrBlock->targets++  ];
    edge->source = CurrBlock;
    edge->destination = dest;
    edge->next_source = NULL;
    if( dest_label_dies ) {
        edge->flags |= DEST_LABEL_DIES;
    }
}


extern  block   *FindBlockWithLbl( label_handle label )
/*****************************************************/
{
    block       *blk;

    blk = HeadBlock;
    for( ;; ) {
        if( blk == NULL ) return( NULL );
        if( blk->label == label ) return( blk );
        blk = blk->next_block;
    }
}


extern  void    FixEdges( void )
/******************************/
{
    block       *blk;
    block       *dest;
    int         targets;
    block_edge  *edge;

    blk = HeadBlock;
    while( blk != NULL ) {
        targets = blk->targets;
        if( ( blk->class & BIG_JUMP ) == 0 ) {
            while( --targets >= 0 ) {
              edge = &blk->edge[  targets  ];
              dest = FindBlockWithLbl( edge->destination );
              if( dest != NULL ) {
                  edge->flags |= DEST_IS_BLOCK;
                  edge->destination = dest;
                  edge->next_source = edge->destination->input_edges;
                  edge->destination->input_edges = edge;
                  edge->destination->inputs++;
              }
            }
        }
        blk = blk->next_block;
    }
}


static label_handle LinkReturnsParms[ 2 ];

static  pointer  LinkReturns( void )
/**********************************/
{
    block               *blk;
    int                 i;
    bool                found;
    label_handle        link_to;
    label_handle        to_search;

    link_to = LinkReturnsParms[ 0 ];
    to_search = LinkReturnsParms[ 1 ];
    blk = FindBlockWithLbl( to_search );
    found = FALSE;
    if( blk == NULL ) return( (pointer)FALSE );
    if( blk->class & BLOCK_VISITED ) return( (pointer)TRUE );
    if( blk->class & LABEL_RETURN ) {
        i = blk->targets;
        for( ;; ) {
            if( --i < 0 ) {
                blk = ReGenBlock( blk, link_to );
                break;
            }
            if( blk->edge[ i ].destination == link_to ) {
                break; /* kick out ... already linked */
            }
        }
        found = TRUE;
    } else {
        blk->class |= BLOCK_VISITED;
        if( blk->class & CALL_LABEL ) {
            if( blk->next_block == NULL ) return( (pointer)FALSE );
            LinkReturnsParms[ 0 ] = link_to;
            LinkReturnsParms[ 1 ] = blk->next_block->label;
            if( SafeRecurse( LinkReturns, NULL ) == (pointer)FALSE ) {
                return( (pointer)FALSE );
            }
        } else {
            i = blk->targets;
            while( --i >= 0 ) {
                LinkReturnsParms[ 0 ] = link_to;
                LinkReturnsParms[ 1 ] = blk->edge[ i ].destination;
                if( SafeRecurse( LinkReturns, NULL ) == (pointer)FALSE ) {
                    return( (pointer)FALSE );
                }
            }
        }
    }
    return( (pointer)TRUE );
}

extern  bool        FixReturns( void )
/************************************/
/* link all LABEL_RETURN blocks to any CALL_LABEL block they could*/
/* have been invoked from*/
{
    block       *blk;
    block       *other_blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        if( blk->class & CALL_LABEL ) {
            blk->class |= BLOCK_VISITED;
            if( blk->next_block == NULL ) return( FALSE );
            blk->next_block->class |= RETURNED_TO;
            LinkReturnsParms[ 0 ] = blk->next_block->label;
            LinkReturnsParms[ 1 ] = blk->edge[ 0 ].destination;
            if( !LinkReturns() ) {
                return( FALSE );
            }
            other_blk = HeadBlock;
            while( other_blk != NULL ) {
                other_blk->class &= ~BLOCK_VISITED;
                other_blk = other_blk->next_block;
            }
        }
        blk = blk->next_block;
    }
    return( TRUE );
}


extern  void    UnFixEdges( void )
/********************************/
{
    block       *blk;
    int         targets;
    block_edge  *edge;

    blk = HeadBlock;
    while( blk != NULL ) {
        if( ( blk->class & BIG_JUMP ) == EMPTY ) {
            targets = blk->targets;
            while( --targets >= 0 ) {
                edge = &blk->edge[  targets  ];
                if( edge->flags & DEST_IS_BLOCK ) {
                    RemoveInputEdge( edge );
                    edge->destination = edge->destination->label;
                    edge->flags &= ~DEST_IS_BLOCK;
                }
            }
        }
        blk = blk->next_block;
    }
}


extern  void    AddAnIns( block *blk, instruction *ins )
/******************************************************/
{
    block       *curr_block;

    curr_block = CurrBlock;
    CurrBlock = blk;
    AddIns( ins );
    CurrBlock = curr_block;
}


extern  bool    BlkTooBig( void )
/*******************************/
{
    label_handle        blk;

    if( !HaveCurrBlock ) return( FALSE );
    if( CurrBlock == NULL ) return( FALSE );
    if( CurrBlock->ins.hd.next == (instruction *)&CurrBlock->ins ) return( FALSE );
    _INS_NOT_BLOCK( CurrBlock->ins.hd.next );
    if( (InsId - CurrBlock->ins.hd.next->id) < INS_PER_BLOCK ) return( FALSE );
    if( CurrBlock->targets != 0 ) return( FALSE );
    blk = AskForNewLabel();
    GenBlock( JUMP, 1 );
    AddTarget( blk, FALSE );
    EnLink( blk, TRUE );
    return( TRUE );
}


extern  void    NewProc( int level )
/**********************************/
{
    proc_def    *new;

    if( CurrProc != NULL ) {
        SaveToTargProc();
        CurrProc->head_block = HeadBlock;
        CurrProc->tail_block = BlockList;
        CurrProc->curr_block = CurrBlock;
        CurrProc->lasttemp = LastTemp;
        CurrProc->dummy_index = DummyIndex;
        CurrProc->names[ N_CONSTANT ] = Names[ N_CONSTANT ];
        CurrProc->names[ N_MEMORY   ] = Names[ N_MEMORY   ];
        CurrProc->names[ N_TEMP     ] = Names[ N_TEMP     ];
        CurrProc->names[ N_REGISTER ] = Names[ N_REGISTER ];
        CurrProc->names[ N_INDEXED  ] = Names[ N_INDEXED  ];
        CurrProc->block_by_block = BlockByBlock;
        CurrProc->ins_id = InsId;
        CurrProc->untrimmed = BlocksUnTrimmed;
    }
    HaveCurrBlock = TRUE;
    BlocksUnTrimmed = TRUE;
    MaxStack = 0;
    InsId = 0;
    HeadBlock = NULL;
    BlockList = NULL;
    ReInitNames();
    new = CGAlloc( sizeof( proc_def ) );
    memset( new, 0, sizeof( proc_def ) );
    new->next_proc = CurrProc;
    CurrProc = new;
    new->frame_index = NULL;
    new->lex_level = level;
    new->parms.size = 0;
    new->parms.base = 0;
    new->locals.size = 0;
    new->locals.base = AskDisplaySize( level );
    new->prolog_state = 0;
    new->parms_list = NULL;
    InitTargProc();
}


extern  void    FreeProc( void )
/******************************/
{
    proc_def    *oldproc;

    for( ;; ) {
        CurrBlock = HeadBlock;
        if( CurrBlock == NULL ) break;
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
            Names[ N_CONSTANT ] = CurrProc->names[ N_CONSTANT ];
            Names[ N_MEMORY   ] = CurrProc->names[ N_MEMORY   ];
            Names[ N_TEMP     ] = CurrProc->names[ N_TEMP     ];
            Names[ N_REGISTER ] = CurrProc->names[ N_REGISTER ];
            Names[ N_INDEXED  ] = CurrProc->names[ N_INDEXED  ];
            LastTemp = CurrProc->lasttemp;
            DummyIndex = CurrProc->dummy_index;
            BlockByBlock = CurrProc->block_by_block;
            BlocksUnTrimmed = CurrProc->untrimmed;
        } else {
            InsId = 0;
            BlockByBlock = FALSE;
        }
    }
}
