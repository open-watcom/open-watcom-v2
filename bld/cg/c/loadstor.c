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
* Description:  Load/store calculations.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "conflict.h"
#include "opcodes.h"
#include "stackok.h"
#include "zoiks.h"

extern  conflict_node   *NameConflict(instruction*,name*);
extern  bool            NameIsConstant(name*);

extern    block *HeadBlock;
extern    bool  BlockByBlock;

#define REAL_REFERENCE  BLOCK_MARKED
#define NO_LOAD_STORE   BLOCK_VISITED
#define CONTAINS_CALL   LOOP_EXIT /* borrow. Only used during loop opts */


static  void    BitsOff( void )
/*****************************/
{
    block               *blk;

    blk = HeadBlock;
    while( blk != NULL ) {
        blk->class &= ~( CONTAINS_CALL | REAL_REFERENCE | NO_LOAD_STORE );
        blk = blk->next_block;
    }
}


static  bool    SameConf( name *op, instruction *ins, conflict_node *conf )
/*************************************************************************/
{
    if( op->n.class == N_INDEXED ) {
        if( NameConflict( ins, op->i.index ) == conf ) return( TRUE );
        if( HasTrueBase( op ) ) {
            if( NameConflict( ins, op->i.base ) == conf ) return( TRUE );
        }
    } else {
        if( NameConflict( ins, op ) == conf ) return( TRUE );
    }
    return( FALSE );
}


static  void    CheckRefs( conflict_node *conf, block *blk )
/***********************************************************
    mark block as REAL_REFERENCE if it contains a reference to conf.
    Also mark as CONTAINS_CALL if it does
*/
{
    int         i;
    instruction *ins;

    if( blk->class & ( BIG_LABEL | RETURN | BIG_JUMP ) ) {
        blk->class |= REAL_REFERENCE;
        return;
    }
    ins = blk->ins.hd.next;
    while( ins->head.opcode != OP_BLOCK ) {
        i = ins->num_operands;
        while( --i >= 0 ) {
            if( SameConf( ins->operands[ i ], ins, conf ) ) {
                blk->class |= REAL_REFERENCE;
                return;
            }
        }
        if( ins->result != NULL ) {
            if( SameConf( ins->result, ins, conf ) ) {
                blk->class |= REAL_REFERENCE;
                return;
            }
        }
        if( _OpIsCall( ins->head.opcode ) &&
           ! ( ( ins->flags.call_flags & CALL_WRITES_NO_MEMORY ) &&
               ( ins->flags.call_flags & CALL_READS_NO_MEMORY ) ) ) {
            blk->class |= CONTAINS_CALL;
        }
        ins = ins->head.next;
    }
}


static  void    LoadStoreIfCall( global_bit_set *id )
/****************************************************
    Turn on bits for need_load/need_store for conflict id in all blocks
    which have a call but no real reference to id.  This is sort of
    backwards, since it would cause a load at the start of the block and
    store at the end, but that will force stores in all ancestor blocks,
    and load in all successor blocks (done by PropagateLoadStoreBits).
    TurnOffLoadStoreBits will then turn off the bits we turned on in
    this block, and we achieve an optimial load/store scheme for
    cacheing a static.
*/
{
    block               *blk;
    data_flow_def       *flow;

    blk = HeadBlock;
    while( blk != NULL ) {
        if(( blk->class & CONTAINS_CALL ) && !( blk->class & REAL_REFERENCE )) {
            flow = blk->dataflow;
            _GBitTurnOn( flow->need_load, *id );
            _GBitTurnOn( flow->need_store, *id );
            _GBitTurnOn( flow->call_exempt, *id );
        }
        blk = blk->next_block;
    }
}


static  void    TurnOffLoadStoreBits( global_bit_set *id )
/*********************************************************
    If a block has need_load and need_store but never really references
    id, we can get rid of the load/store.
*/
{
    block               *blk;
    data_flow_def       *flow;

    blk = HeadBlock;
    while( blk != NULL ) {
        if( !( blk->class & REAL_REFERENCE ) ) {
            flow = blk->dataflow;
            if( _GBitOverlap( flow->need_load, *id )
             && _GBitOverlap( flow->need_store, *id ) ) {
                _GBitTurnOff( flow->need_load, *id );
                _GBitTurnOff( flow->need_store, *id );
            }
        }
        blk = blk->next_block;
    }
}


static  void    PropagateLoadStoreBits( block *start, global_bit_set *id )
/*************************************************************************
    Make sure that ancestors of need_load blocks do a store at the end
    and successors of need_store blocks do a load at the beginning.  This
    will be a conservative estimate, fixed up by TurnOffLoadStoreBits
*/
{
    data_flow_def       *source_dat;
    data_flow_def       *blk_dat;
    bool                change;
    block_edge          *edge;
    block               *blk;

    for( ;; ) {
        change = FALSE;
        blk = start;
        while( blk != NULL ) {
            blk_dat = blk->dataflow;
            edge = blk->input_edges;
            while( edge != NULL ) {
                source_dat = edge->source->dataflow;
                if( _GBitOverlap( source_dat->out, *id ) &&
                    _GBitOverlap( blk_dat->in, *id ) ) {
                    /* NB: there are 3 IFs to minimize the & of iterations*/
                    if( _GBitOverlap( source_dat->need_store, *id ) ) {
                        change |= !_GBitOverlap( blk_dat->need_load, *id );
                        _GBitTurnOn( blk_dat->need_load, *id );
                    }
                    if( _GBitOverlap( blk_dat->need_load, *id ) ) {
                        change |= !_GBitOverlap( source_dat->need_store, *id );
                        _GBitTurnOn( source_dat->need_store, *id );
                    }
                    if( _GBitOverlap( source_dat->need_store, *id ) ) {
                        change |= !_GBitOverlap( blk_dat->need_load, *id );
                        _GBitTurnOn( blk_dat->need_load, *id );
                    }
                }
                edge = edge->next_source;
            }
            blk = blk->next_block;
        }
        if( change == FALSE ) break;
    }
}


static  void    CalculateLoadStore( conflict_node *conf )
/********************************************************
    If we are going to cache a global variable in a register, we have a
    few problems, which are resolved in this routine.  If a subroutine
    is called, we must store the register into memory just before the
    subroutine call, and load the new value afterward.  We must also
    store before leaving the range of the conflict node, and load it if
    the conflict range is entered from without.  This routine uses an
    iterative algorithm to determine which blocks will need to have
    loads/stores at the beginning/end because of this. All blocks outside
    the range of the conflict are marked as "need load" and "need store".
    All blocks which don't reference the conflict variable, yet have a
    call are marked as "need load" and "need store". An iterative algorithm
    is used to ensure consistency in the state of the variable/register.
    "need load" requires "need store" in all ancestor blocks. "need store"
    requires "need load" in all successor blocks. Once this is done,
    we optimize by "painting" regions of the blocks which don't reference
    the variable at all. Internal "need load"/"need store" attributes
    are turned off in these regions.
*/
{
    global_bit_set      id;
    block               *blk;
    data_flow_def       *flow;

    BitsOff();
    blk = HeadBlock;
    if( blk != NULL ) {
        blk->class |= BIG_LABEL;
    }
    _GBitAssign( id, conf->id.out_of_block );
    /* turn on bits before the conflict range */
    while( blk != NULL ) {
        if( blk == conf->start_block ) break;
        _GBitTurnOn( blk->dataflow->need_load, id );
        _GBitTurnOn( blk->dataflow->need_store, id );
        blk->class |= REAL_REFERENCE;
        blk = blk->next_block;
    }
    /* turn on bits in the conflict range */
    while( blk != NULL ) {
        flow = blk->dataflow;
        CheckRefs( conf, blk );
        if( _GBitOverlap( flow->in, id ) && ( blk->class & BIG_LABEL ) ) {
            _GBitTurnOn( flow->need_load, id );
        } else {
            _GBitTurnOff( flow->need_load, id );
        }
        if( _GBitOverlap( flow->out, id )
         && ( blk->class & ( RETURN | BIG_JUMP ) ) ) {
            _GBitTurnOn( flow->need_store, id );
        } else {
            _GBitTurnOff( flow->need_store, id );
        }
        if( blk->ins.hd.prev != (instruction *)&blk->ins ) {
            _INS_NOT_BLOCK( blk->ins.hd.prev );
            _INS_NOT_BLOCK( conf->ins_range.last );
            if( blk->ins.hd.prev->id >= conf->ins_range.last->id) break;
        }
        blk = blk->next_block;
    }
    /* turn on bits after the conflict range */
    while( blk != NULL ) {
        flow = blk->dataflow;
        blk = blk->next_block;
        if( blk == NULL ) break;
        blk->class |= REAL_REFERENCE;
        _GBitTurnOn( flow->need_load, id );
        _GBitTurnOn( flow->need_store, id );
    }
    LoadStoreIfCall( &id );
    PropagateLoadStoreBits( conf->start_block, &id );
    TurnOffLoadStoreBits( &id );
    if( NameIsConstant( conf->name ) ) {
        blk = HeadBlock;
        id = conf->id.out_of_block;
        while( blk != NULL ) {
            flow = blk->dataflow;
            _GBitTurnOff( flow->need_store, id );
            blk = blk->next_block;
        }
    }
    BitsOff();
}


extern  void    CalcLoadStore( conflict_node *conf )
/***************************************************
    see below
*/
{
    if( BlockByBlock == FALSE ) {
        CalculateLoadStore( conf );
    }
}
