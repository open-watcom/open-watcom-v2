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


#define GB_DEFINE_GBITNEXT
#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "data.h"
#include "feprotos.h"

extern  void            FindReferences( void );
extern  conflict_node   *AddConflictNode(name*);
extern  name            *DeAlias(name*);
extern  save_def        Weight( save_def value, block *blk );
extern  void            *SortList(void *,unsigned int,bool(*)(void *,void *));

static  void            PropagateConflicts( void );
static  void            LiveAnalysis( block *tail, global_bit_set memory_bits );
extern  void            SetInOut( block *blk );

static  bool            MoreUseInOtherTemps;


static  void    AddTempSave( name *op, block *blk )
/*************************************************/
{
    conflict_node *conf;

    if( op->n.class == N_INDEXED ) {
        op = op->i.index;
    }
    if( op->n.class != N_TEMP ) return;
    op = DeAlias( op );
    conf = op->v.conflict;
    if( conf == NULL ) return;
    conf->savings += Weight( 1, blk );
}


static  bool    AllocBefore( void *n1, void *n2 )
/***********************************************/
{
    name    *t1 = n1;
    name    *t2 = n2;

    /* const temps after all others */
    if( (t1->t.temp_flags & CONST_TEMP) && !(t2->t.temp_flags & CONST_TEMP) ) {
        return( false );
    }
    if( !(t1->t.temp_flags & CONST_TEMP) && (t2->t.temp_flags & CONST_TEMP) ) {
        return( true );
    }
    if( t1->v.conflict == NULL && t2->v.conflict != NULL ) {
        return( false );
    }
    if( t1->v.conflict != NULL ) {
        if( t2->v.conflict == NULL ) return( true );
        return( t1->v.conflict->savings > t2->v.conflict->savings );
    } else {
        return( t1->t.v.id > t2->t.v.id );
    }
}


static  void    RoughSortTemps( void )
/*************************************

    Do a real rough sort on the templist by savings in case
    we run out of global bits. This will help the register
    allocator do the right thing
*/
{
    name                *actual_name;
    name                *opnd;
    block               *blk;
    instruction         *ins;
    int                 i;

    for( opnd = Names[ N_TEMP ]; opnd != NULL; opnd = opnd->n.next_name ) {
        if( ( opnd->v.usage & ( USE_MEMORY|USE_ADDRESS ) ) ) {
            opnd->v.usage |= NEEDS_MEMORY | USE_MEMORY;
        } else if( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) {
            actual_name = DeAlias( opnd );
            if( actual_name->v.conflict == NULL ) {
                AddConflictNode( actual_name );
            }
        }
    }
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            for( i = 0; i < ins->num_operands; ++i ) {
                AddTempSave( ins->operands[i], blk );
            }
            if( ins->result != NULL ) {
                AddTempSave( ins->result, blk );
            }
        }
    }
    Names[ N_TEMP ] = SortList( Names[ N_TEMP ], offsetof( name, n.next_name ),
                                AllocBefore );
}


static  global_bit_set AssignGlobalBits( name_class_def list,
                             global_bit_set *bit, bool first_time )
/*****************************************************************/
{
    conflict_node       *conf;
    global_bit_set      all_used;
    name                *actual_name;
    name                *opnd;

    if( list == N_TEMP ) {
        _GBitInit( all_used, EMPTY );
        if( !MoreUseInOtherTemps ) return( all_used );
        MoreUseInOtherTemps = false;
    }
    _GBitInit( all_used, EMPTY );
    for( opnd = Names[ list ]; opnd != NULL; opnd = opnd->n.next_name ) {
        if( ( opnd->v.usage & ( USE_MEMORY | USE_ADDRESS ) ) ) {
            opnd->v.usage |= NEEDS_MEMORY | USE_MEMORY;
        } else if( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) {
            actual_name = opnd;
            if( list == N_TEMP ) {
                actual_name = DeAlias( actual_name );
            }
            if( _GBitEmpty( *bit ) && ( list == N_MEMORY ) ) {
                actual_name->v.usage |= NEEDS_MEMORY | USE_MEMORY;
            } else {
                conf = actual_name->v.conflict;
                if( conf == NULL && first_time ) {
                    conf = AddConflictNode( actual_name );
                }
                if( conf != NULL && !( conf->state & CONFLICT_ON_HOLD )
                 && _GBitEmpty( conf->id.out_of_block ) ) {
                    _GBitAssign( conf->id.out_of_block, *bit );
                    _GBitTurnOn( all_used, *bit );
                    _GBitNext( bit );
                    if( _GBitEmpty( conf->id.out_of_block ) ) {
                        conf->state |= CONFLICT_ON_HOLD;
                        MoreUseInOtherTemps = true;
                    }
                }
            }
        }
    }
    return( all_used );
}


static  void    CheckGlobals( void )
/**********************************/
{
    name        *op;

#define FORCE_MEM (USE_MEMORY|USE_ADDRESS)
    if( _IsntModel( RELAX_ALIAS ) ) {
        for( op = Names[N_MEMORY]; op != NULL; op = op->n.next_name ) {
            if( op->m.memory_type == CG_FE ) {
                if( ( op->v.usage & FORCE_MEM ) != FORCE_MEM ) {
                    if( FEAttr( op->v.symbol ) & ( FE_VISIBLE | FE_GLOBAL ) ) {
                        op->v.usage |= FORCE_MEM;
                    }
                }
            }
        }
    }
}


extern  void    MakeConflicts( void )
/***********************************/
{
    global_bit_set     bit;

    CheckGlobals();
    MoreUseInOtherTemps = true;
    _GBitFirst( bit );
    if( !BlockByBlock ) {
        RoughSortTemps();
    }
    AssignGlobalBits( N_TEMP, &bit, true );
    MemoryBits = AssignGlobalBits( N_MEMORY, &bit, true );
    PropagateConflicts();
}


extern  bool    MoreConflicts( void )
/***********************************/
{
    global_bit_set     bit;

    _GBitFirst( bit );
    _GBitInit( MemoryBits, EMPTY );
    bit = AssignGlobalBits( N_TEMP, &bit, false );
    if( !_GBitEmpty( bit ) ) {
        PropagateConflicts();
        return( true );
    } else {
        return( false );
    }
}



static  void    PropagateConflicts( void )
/****************************************/
{
    block       *blk;

    /*   Assign global bits to temporaries first in case we run out of*/
    /*   bits.*/
    /*   (When we run out of bits, everything } else { is forced to memory)*/

    FindReferences();
    if( BlockByBlock == false ) {
        LiveAnalysis( HeadBlock, MemoryBits );
    }
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        SetInOut( blk );
    }
}


extern  void    SetInOut( block *blk )
/************************************/
{
    if( BlockByBlock ) {
        blk->dataflow->in  = blk->dataflow->use;
        _GBitTurnOn( blk->dataflow->in, blk->dataflow->def );
        _GBitAssign( blk->dataflow->out, blk->dataflow->in );
    }
    if( blk->class & RETURN ) {
        HW_TurnOn( blk->ins.hd.live.regs, CurrProc->state.return_reg );
    }
    _LBitInit( blk->ins.hd.live.within_block, EMPTY );
    _GBitAssign( blk->ins.hd.live.out_of_block, blk->dataflow->out );
    _LBitInit( blk->available_bit, ~EMPTY );
}


static  void    LiveAnalysis( block *tail, global_bit_set memory_bits )
/*********************************************************************/
{
    block               *blk;
    block               *target;
    data_flow_def       *data;
    global_bit_set      new;
    block_num           i;
    bool                change;


    /*   Want tail of block list, to do inside-out traversal of*/
    /*   depth first spanning tree*/

    while( tail->next_block != NULL ) {
        tail = tail->next_block;
    }
    for( ;; ) {
        change = false;
        for( blk = tail; blk != NULL; blk = blk->prev_block ) {
            data = blk->dataflow;

            /*   The OUT set of a return block includes any globals*/
            /*   defined within the procedure*/

            if( blk->class & ( RETURN | LABEL_RETURN ) ) {
                new = memory_bits;
            } else {
                _GBitInit( new, EMPTY );
            }
            if( ( blk->class & BIG_JUMP ) == EMPTY ) {
                for( i = blk->targets; i-- > 0; ) {
                    target = blk->edge[ i ].destination.u.blk;

                    /*   new OUT = union of successors' IN*/

                    _GBitTurnOn( new, target->dataflow->in );
                }
            }
            if( !_GBitSame( data->out, new ) ) {
                data->out = new;
                change = true;
            }

            /*   new IN == ( new OUT - DEF ) union USE*/

            _GBitTurnOff( new, data->def );
            _GBitTurnOn( new, data->use );
            if( !_GBitSame( data->in, new ) ) {
                _GBitAssign( data->in, new );
                change = true;
            }
        }
        if( change == false ) break;
    }
}
