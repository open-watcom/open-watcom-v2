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
* Description:  Variable/memory usage analysis.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "cgmem.h"
#include "hwreg.h"
#include "data.h"

extern  name            *DeAlias(name*);


static  void    Use( name *op, block *blk, var_usage usage )
/***********************************************************
    Mark "op" as used in "blk".  assume that if it hasn't already been
    defined in this block, that it must be USE_IN_ANOTHER_BLOCK.  "usage"
    may be set to USE_ADDRESS indicating that we are using the address
    of the name.
*/
{
    if( op->n.class == N_CONSTANT && op->c.const_type == CONS_TEMP_ADDR ) {
        Use( (name*)op->c.value, blk, USE_ADDRESS );
        return;
    }
    if( op->n.class == N_TEMP ) {
        op = DeAlias( op );
    } else if( op->n.class != N_MEMORY ) {
        return;
    }
    if( op->v.block_usage & DEF_WITHIN_BLOCK ) {
        op->v.block_usage |= USE_WITHIN_BLOCK;
    } else {
        op->v.block_usage |= USE_IN_ANOTHER_BLOCK;
        if( op->v.conflict != NULL ) {
            _GBitTurnOn( blk->dataflow->use, op->v.conflict->id.out_of_block );
        }
    }
    op->v.block_usage |= usage;
}


static  bool    CoveringDefinitions( name *op )
/**********************************************
    If we are only defining a piece of "op", we check all of its aliases
    to see if they have been defined in this block as well.  Only if all
    the definitions cause the entire name to be redefined do we mark the
    master symbol table entry for the name as "DEF_WITHIN_BLOCK" and
    flip on its bit in dataflo->def.
*/
{
    name        *alias;
    int         i;
    uint        loc;
    bool        covered[MAX_POSSIBLE_REG+1];

    if( op->n.size > MAX_POSSIBLE_REG ) return( FALSE );
    i = op->n.size;
    while( --i >= 0 ) {
        covered[ i ] = FALSE;
    }
    alias = op->t.alias;
    while( alias != op ) {
        if( alias->v.block_usage & DEF_WITHIN_BLOCK ) {
            loc = alias->v.offset - op->v.offset;
            i = alias->n.size;
            while( --i >= 0 ) {
                if( loc < sizeof( covered ) ) covered[ loc ] = TRUE;
                ++loc;
            }
        }
        alias = alias->t.alias;
    }
    i = op->n.size;
    while( --i >= 0 ) {
        if( covered[ i ] == FALSE ) return( FALSE );
    }
    return( TRUE );
}


static  void    Define( name *op, block *blk )
/*********************************************
    Mark "op" as defined within "blk".
*/
{
    name        *actual;

    actual = op;
    actual->v.block_usage |= DEF_WITHIN_BLOCK;
    if( op->n.class == N_TEMP ) op = DeAlias( op );
    if( op->v.block_usage & USE_IN_ANOTHER_BLOCK ) return;
    if( actual != op && CoveringDefinitions( op ) == FALSE ) return;
    op->v.block_usage |= DEF_WITHIN_BLOCK;
    if( op->v.conflict == NULL ) return;
    _GBitTurnOn( blk->dataflow->def, op->v.conflict->id.out_of_block );
}


static  void    UseDefGlobals( block *blk )
/******************************************
    If a call instruction is encountered, all N_MEMORY names (visible
    outside this procedure), could be both used and defined by the call.
*/
{
    conflict_node       *conf;
    var_usage           usage;

    conf = ConfList;
    while( conf != NULL ) {
        if( conf->name->n.class == N_MEMORY ) {
            usage = conf->name->v.block_usage;
            Use( conf->name, blk, EMPTY );
            Define( conf->name, blk );
            _GBitTurnOn( blk->dataflow->def, conf->id.out_of_block );
            if( ( usage & DEF_WITHIN_BLOCK ) == 0 ) {
                _GBitTurnOn( blk->dataflow->use, conf->id.out_of_block );
            }
        }
        conf = conf->next_conflict;
    }
}


static void TransferBlockUsage( name *op )
/*****************************************
    Never have both USE_WITHIN_BLOCK and USE_IN_ANOTHER_BLOCK set.
*/
{
    op->v.usage |= op->v.block_usage;
    if( op->v.usage & USE_IN_ANOTHER_BLOCK ) {
        op->v.usage &= ~ USE_WITHIN_BLOCK;
    }
    op->v.block_usage = 0;
}


static void TransferOneTempBlockUsage( name *op )
/************************************************
    see TransferTempBlockUsage ^
*/
{
    name        *alias;

    if( op->n.class == N_CONSTANT && op->c.const_type == CONS_TEMP_ADDR ) {
        TransferOneTempBlockUsage( op->c.value );
    } else if( op->n.class == N_INDEXED ) {
        TransferOneTempBlockUsage( op->i.index );
        if( HasTrueBase( op ) ) {
            TransferOneTempBlockUsage( op->i.base );
        }
    } else if ( op->n.class == N_TEMP ) {
        alias = op->t.alias;
        for( ;; ) {
            TransferBlockUsage( alias );
            if( alias == op ) break;
            alias = alias->t.alias;
        }
    }
}


static void TransferTempBlockUsage( block *blk )
/***********************************************
    Traverse the block "blk", and for each variable referenced by an
    instruction within block, Make sure that USE_WITHIN_BLOCK and
    USE_IN_OTHER_BLOCK are not on simultaneously for each variable and
    all of its aliases.  We traverse the block rather than Names[N_TEMP]
    since running down the list of temps could be very expensive (if
    long).  Blocks on the other hand are always reasonably short
*/
{
    instruction *ins;
    int         i;

    ins = blk->ins.hd.next;
    while( ins->head.opcode != OP_BLOCK ) {
        i = ins->num_operands;
        while( --i >= 0 ) {
            TransferOneTempBlockUsage( ins->operands[ i ] );
        }
        if( ins->result != NULL ) {
            TransferOneTempBlockUsage( ins->result );
        }
        ins = ins->head.next;
    }
}


static  void    TransferAllMemBlockUsage( void )
/***********************************************
    Like TransferTempBlockUsage.
*/
{
    name        *mem;

    for( mem = Names[N_MEMORY]; mem != NULL; mem = mem->n.next_name ) {
        TransferBlockUsage( mem );
    }
}


static void TransferOneMemBlockUsage( name *op )
/***********************************************
    see TransferMemBlockUsage ^
*/
{
    if( op->n.class == N_INDEXED ) {
        TransferOneMemBlockUsage( op->i.index );
        if( HasTrueBase( op ) ) {
            TransferOneMemBlockUsage( op->i.base );
        }
    } else if ( op->n.class == N_MEMORY ) {
        TransferBlockUsage( op );
    }
}


static  void    TransferMemBlockUsage( block *blk )
/**************************************************
    Like TransferTempBlockUsage.
*/
{
    instruction *ins;
    int         i;

    ins = blk->ins.hd.next;
    while( ins->head.opcode != OP_BLOCK ) {
        i = ins->num_operands;
        while( --i >= 0 ) {
            TransferOneMemBlockUsage( ins->operands[ i ] );
        }
        if( ins->result != NULL ) {
            TransferOneMemBlockUsage( ins->result );
        }
        ins = ins->head.next;
    }
}


static void TransferOneTempFlag( name *t )
/****************************************/
{
    name        *alias;
    var_usage   usage;

    if( t->n.class == N_TEMP ) {
        t = DeAlias( t );
        alias = t;
        usage = 0;
        do {
            usage |= alias->v.usage;
            alias = alias->t.alias;
        } while( alias != t );
        alias = t;
        do {
            alias->v.usage |= usage;
            alias = alias->t.alias;
        } while( alias != t );
    } else if( t->n.class == N_INDEXED ) {
        TransferOneTempFlag( t->i.index );
        if( HasTrueBase( t ) ) {
            TransferOneTempFlag( t->i.base );
        }
    }
}


extern  void    TransferTempFlags( void )
/****************************************
    Run through the program and for each variable reference, transfer
    the usage flags from each variable to all of its aliases.  (if we
    use the address of T1, we're effectively using the address of T1+1
    as well, etc).  We could traverse Names[N_TEMP] but it could be much
    longer than the block list.
*/
{
    block       *blk;
    instruction *ins;
    int         i;

    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            i = ins->num_operands;
            while( --i >= 0 ) {
                TransferOneTempFlag( ins->operands[ i ] );
            }
            if( ins->result != NULL ) {
                TransferOneTempFlag( ins->result );
            }
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    blk = HeadBlock;
    while( blk != NULL ) {
        TransferTempBlockUsage( blk );
        blk = blk->next_block;
    }
}


static  void    TransferMemoryFlags( void )
/******************************************
    If we use the address of FOOBAR, we're effectively using the address
    of FOOBAR+1 as well
*/
{
    name        *m;
    name        *same_sym;
    var_usage   usage;

    m = Names[  N_MEMORY  ];
    while( m != NULL ) {
        m->m.memory_flags = MF_EMPTY;
        m = m->n.next_name;
    }

    m = Names[  N_MEMORY  ];
    while( m != NULL ) {
        if( ( m->m.memory_flags & MF_VISITED ) == 0 ) {
            if( m->m.same_sym != NULL ) {
                // union together mem flags from all symbols that
                // are same and set all v.usage's to this union
                usage = m->v.usage;
                same_sym = m->m.same_sym;
                while( same_sym != m ) {
                    usage |= same_sym->v.usage;
                    same_sym = same_sym->m.same_sym;
                }
                m->v.usage = usage;
                same_sym = m->m.same_sym;
                while( same_sym != m ) {
                    same_sym->v.usage |= usage;
                    same_sym = same_sym->m.same_sym;
                    same_sym->m.memory_flags |= MF_VISITED;
                }
            }
            m->m.memory_flags |= MF_VISITED;
        }
        m = m->n.next_name;
    }
}


static  void    SearchDefUse( void )
/***********************************
    see FindReferences ^
*/
{
    block       *blk;
    instruction *ins;
    name        *name;
    int         i;
    bool        touched_non_op;

    blk = HeadBlock;
    for( ;; ) {
        touched_non_op = FALSE;
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            if( ( ins->head.opcode == OP_CALL
               || ins->head.opcode == OP_CALL_INDIRECT )
             && ( ( ins->flags.call_flags & CALL_READS_NO_MEMORY ) == 0
/*21-nov-90*/  || ( ins->flags.call_flags & CALL_WRITES_NO_MEMORY ) == 0 ) ) {
                UseDefGlobals( blk );
                touched_non_op = TRUE;
            }
            i = ins->num_operands;
            while( -- i >= 0 ) {
                name = ins->operands[ i ];
                if( name->n.class == N_INDEXED ) {
                    Use( name->i.index, blk, EMPTY );
                    if( HasTrueBase( name ) ) {
                        Use( name->i.base, blk, USE_ADDRESS );
                    }
                } else {
                    if( ins->head.opcode == OP_LA && i == 0 ) {
                        Use( name, blk, USE_ADDRESS );
                    } else {
                        Use( name, blk, EMPTY );
                    }
                }
            }
            name = ins->result;
            if( name != NULL ) {
                if( name->n.class == N_INDEXED ) {
                    Use( name->i.index, blk, EMPTY );
                    if( HasTrueBase( name ) ) {
                        Use( name->i.base, blk, USE_ADDRESS );
                    }
                } else if( name->n.class == N_MEMORY ) {

                    /*   Any static data defined can be used in another block*/

                    name->v.usage |= USE_IN_ANOTHER_BLOCK;
                    Define( name, blk );
                } else if( name->n.class == N_TEMP ) {
                    Define( name, blk );
                }
            }
            ins = ins->head.next;
        }
        /* in/out/def/use ignored if BlockByBlock so don't worry about it */
        if( touched_non_op && !BlockByBlock ) {
            TransferAllMemBlockUsage();
        } else {
            TransferMemBlockUsage( blk );
        }
        TransferTempBlockUsage( blk );
        blk = blk->next_block;
        if( blk == NULL ) break;
    }
    TransferTempFlags();
    TransferMemoryFlags();
}


extern  void    FindReferences( void )
/*************************************
    Traverse the blocks an allocate a data_flow_def for each one if it
    is needed.  Then calculate which variables are USE_WITHIN_BLOCK,
    USE_IN_OTHER_BLOCK, DEF_IN_BLOCK and turn on their bits in the
    dataflo->def and dataflo->use sets.  Note that USE_WITHIN_BLOCK
    means that a variable is used before it is defined.
    DEF_WITHIN_BLOCK means that the entire variable is redefined by the
    block, so that no definitition of it in a previous block could flow
    through this block and be used in a subsequent block.  USE_IN_ANOTHER
    block means that a variable is used in more than one block.
*/
{
    block       *curr;

    curr = HeadBlock;
    while( curr != NULL ) {
        if( curr->dataflow == NULL ) {
            curr->dataflow = CGAlloc( sizeof( data_flow_def ) );
        }
        _GBitInit( curr->dataflow->def         , EMPTY );
        _GBitInit( curr->dataflow->use         , EMPTY );
        _GBitInit( curr->dataflow->in          , EMPTY );
        _GBitInit( curr->dataflow->out         , EMPTY );
        _GBitInit( curr->dataflow->call_exempt , EMPTY );
        _GBitInit( curr->dataflow->need_load   , EMPTY );
        _GBitInit( curr->dataflow->need_store  , EMPTY );
        curr = curr->next_block;
    }
    SearchDefUse();
}
