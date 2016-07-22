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


#ifdef _InRegAssgn

#define _UpdateLive( ins, conf, reg_name ) \
             if( ( _GBitOverlap( ins->head.live.out_of_block, \
                                 conf->id.out_of_block ) ) \
              || ( _LBitOverlap( ins->head.live.within_block, \
                                 conf->id.within_block ) ) ) { \
                 HW_TurnOn( ins->head.live.regs, reg_name->r.reg ); \
             }

#define _Equal( op1, op2 ) ContainedIn( op1, op2 )

#define _SuffixLoad( ins, x, y, c ) \
             SuffixIns( ins, MakeMove( x, y, c ) ); \
             AddSegment( (ins)->head.next )

#define _PrefixLoad( ins, x, y, c ) \
             PrefixIns( ins, MakeMove( x, y, c ) ); \
             AddSegment( (ins)->head.prev )

#define _SuffixStore( ins, x, y, c ) \
             SuffixIns( ins, MakeMove( x, y, c ) ); \
             AddSegment( (ins)->head.next )

#define _PrefixStore( ins, x, y, c ) \
             PrefixIns( ins, MakeMove( x, y, c ) ); \
             AddSegment( (ins)->head.prev )

#define _ReplaceOpnd( tree, ins, i, reg_name, _1 ) \
             DelSegOp( ins, i ); \
             ins->operands[i] \
                = FindReg( tree, ins->operands[i], reg_name ); \
             FixGenEntry( ins );

#define _ReplaceIdxOpnd( tree, ins, i, reg_name ) \
             ins->operands[i] = \
                ReplIndex(ins,tree,ins->operands[i],reg_name); \
             ins->head.state = INS_NEEDS_WORK

#define _ReplaceResult( tree, ins, reg_name, _1 ) \
             DelSegRes( ins ); \
             ins->result = FindReg( tree, ins->result, reg_name ); \
             FixGenEntry( ins );

#define _ReplaceIdxResult( tree, ins, reg_name ) \
             ins->result = ReplIndex( ins, tree, ins->result, reg_name ); \
             ins->head.state = INS_NEEDS_WORK

#define _UpdateCost( _1, _2 )

#else

#define _UpdateLive( _1, _2, _3 )

#define _Equal( op1, op2 ) ( ( (op1)->n.class==N_TEMP ? DeAlias(op1) : (op1) ) \
                            == (op2) )

#define _SuffixLoad( _1, _2, _3, class )   \
             block_cost += Save.load_cost[ class ];

#define _PrefixLoad( _1, _2, _3, class )   \
             block_cost += Save.load_cost[ class ];

#define _SuffixStore( _1, _2, _3, class )  \
             block_cost += Save.store_cost[ class ];

#define _PrefixStore( _1, _2, _3, class )  \
             block_cost += Save.store_cost[ class ];

#define _ReplaceOpnd( _1, ins, i, _2, class ) \
             block_save += Save.use_save[ class ]; \
             if( i == 0 \
              && ins->head.opcode < FIRST_CONDITION \
              && ins->result != NULL \
              && ins->result->n.class == N_REGISTER ) { \
                 block_save += Save.use_save[ class ]; \
             }

#define _ReplaceResult( _1, ins, _2, class ) \
            if( ( ins->head.opcode==OP_MOV || ins->head.opcode==OP_CONVERT ) \
             && ins->result->n.class == N_TEMP \
             && ins->operands[ 0 ]->n.class == N_TEMP \
             && ins->result->t.location != NO_LOCATION \
             && ins->result->t.location \
                     == ins->operands[ 0 ]->t.location ) { \
                block_cost+=Save.load_cost[ class ]; \
            } else { \
                block_save+=Save.def_save[ class ]; \
                if( ins->num_operands != 0 \
                 && ins->operands[ 0 ]->n.class == N_REGISTER ) { \
                    block_save+=Save.use_save[ class ]; \
                } \
            }

#define _ReplaceIdxResult( _1, _2, _3 ) \
             block_save += Save.index_save;

#define _ReplaceIdxOpnd( _1, _2, _3, _4 ) \
             block_save += Save.index_save;

#define _UpdateCost( blk, reg_name ) \
             save += Weight( block_save, blk ); \
             cost += Weight( block_cost, blk ); \
             block_save = 0; \
             block_cost = 0

#endif

    class = opnd->n.name_class;
    blk = conf->start_block;
    ins = conf->ins_range.first;
    last = false;
    if( _LBitOverlap( conf->ins_range.first->head.live.within_block,
                      conf->id.within_block ) ) {
        flows_in = true;
    } else {
        flows_in = false;
    }
    for(;;) {
        flow = blk->dataflow;
/*  for each block in conflict range */
        final_defn = NULL;
/*  the very last definition */
        last_defn = NULL;
/*  the previous reaching definition */
        first_use = NULL;
/*  the very first use */
        for(;;) {
/*  for each instruction in conflict range */
            if( ins->head.opcode == OP_BLOCK ) break;
            next = ins->head.next;
/*  reload volatile names after calls */
            if( _OpIsCall( ins->head.opcode ) ) {
                if( ( ( opnd->v.usage & USE_ADDRESS )
                   || ( ( opnd->v.usage & NEEDS_MEMORY )
                        && _Isnt( conf, CST_OK_ACROSS_CALLS ) ) )
                        && !_GBitOverlap( conf->id.out_of_block, flow->call_exempt ) ) {
                    if( (ins->flags.call_flags & CALL_WRITES_NO_MEMORY) == 0
                     || ( opnd->n.class == N_TEMP
                       && ( opnd->v.usage & USE_ADDRESS ) ) ) {
                        _SuffixLoad( ins, opnd, reg_name, class );
                    }
                    if( ( last_defn != NULL
                       || ( ( opnd->v.usage & USE_IN_ANOTHER_BLOCK )
                          && _GBitOverlap( conf->id.out_of_block, flow->in ) ) )
                     && ( ( (ins->flags.call_flags & CALL_WRITES_NO_MEMORY) == 0 )
                       || ( (ins->flags.call_flags & CALL_READS_NO_MEMORY) == 0 )
                       || ( opnd->n.class == N_TEMP
                            && ( opnd->v.usage & USE_ADDRESS ) ) ) ) {
                        last_defn = NULL;
                        temp_ins = ins->head.prev;
                        _PrefixStore( ins, reg_name, opnd, class );
                        if( first_use == NULL ) {
                            first_use = temp_ins->head.next;
                        }
                    }
                }
                if( first_use == NULL ) {
                    first_use = ins;
                }
            }
            for( i = ins->num_operands; i-- > 0; ) {
                if( _Equal( ins->operands[i], opnd ) ) {
                    if( first_use == NULL ) {
                        first_use = ins;
                    }
                    _ReplaceOpnd( tree, ins, i, reg_name, class );
                } else if( ins->operands[i]->n.class == N_INDEXED
                     && _Equal( ins->operands[i]->i.index, opnd ) ) {
                    if( first_use == NULL ) {
                        first_use = ins;
                    }
                    _ReplaceIdxOpnd( tree, ins, i, reg_name );
                }
            }
            if( ins->result != NULL ) {
                if( _Equal( ins->result, opnd ) ) {
                    last_defn = ins;
                    final_defn = ins;
                    _ReplaceResult( tree, ins, reg_name, class );
                } else if( ins->result->n.class == N_INDEXED
                     && _Equal( ins->result->i.index, opnd ) ) {
                    _ReplaceIdxResult( tree, ins, reg_name );
                }
            }
            _UpdateLive( ins, conf, reg_name );
            ins = next;
            last = ins->head.prev == conf->ins_range.last;
            if( last ) {
                break;
            }
        }
        _UpdateLive( ins, conf, reg_name );
        if( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) {
#ifdef _InRegAssgn
                if( first_use == NULL ) {
                    first_use = conf->ins_range.first;
                    _INS_NOT_BLOCK( first_use );
                    _INS_NOT_BLOCK( blk->ins.hd.next );
                    if( first_use->id < blk->ins.hd.next->id ) {
                        first_use = blk->ins.hd.next;
                    }
                }
                if( final_defn == NULL ) {
                    final_defn = conf->ins_range.last;
                    _INS_NOT_BLOCK( final_defn );
                    _INS_NOT_BLOCK( blk->ins.hd.prev );
                    if( final_defn->id > blk->ins.hd.prev->id ) {
                        final_defn = blk->ins.hd.prev;
                        if( ( blk->class & CONDITIONAL )
                          || ( blk->class & SELECT ) ) {
                            final_defn = blk->ins.hd.next;
                            while( !_OpIsJump( final_defn->head.opcode ) ) {
                                final_defn = final_defn->head.next;
                            }
                            final_defn = final_defn->head.prev;
                        } else {
                            while( final_defn->head.opcode == OP_NOP ) {
                                final_defn = final_defn->head.prev;
                            }
                        }
                    }
                }
#endif
            if( (instruction *)&blk->ins == blk->ins.hd.next ) {
                if( _GBitOverlap( conf->id.out_of_block, flow->need_store ) ) {
                    _SuffixStore( (instruction *)&blk->ins, reg_name, opnd, class );
                }
                if( _GBitOverlap( conf->id.out_of_block, flow->need_load ) ) {
                    _SuffixLoad( (instruction *)&blk->ins, opnd, reg_name, class );
                }
            } else {
                if( _GBitOverlap( conf->id.out_of_block, flow->need_store ) ) {
                    _SuffixStore( final_defn, reg_name, opnd, class );
#ifndef _InRegAssgn
                    final_defn = final_defn;
#endif
                }
                if( _GBitOverlap( conf->id.out_of_block, flow->need_load ) ) {
#ifdef _InRegAssgn
                    if( first_use->head.prev == final_defn->head.next ) {
                        if( first_use->head.prev->head.opcode != OP_BLOCK ) {
                            first_use = first_use->head.prev;
                        }
                    }
#endif
                    _PrefixLoad( first_use, opnd, reg_name, class );
                }
            }
        } else if( opnd->v.usage & ( NEEDS_MEMORY | USE_ADDRESS ) ) {
            if( last_defn != NULL ) {
                _SuffixStore( last_defn, reg_name, opnd, class );
/*  } else { it was already stored after its last definition */
            }
            if( flows_in ) {
                _PrefixLoad( conf->ins_range.first, opnd, reg_name, class );
            }
        }
        _UpdateCost( blk, opnd );
        if( last ) break;
        blk = blk->next_block;
        ins = blk->ins.hd.next;
    }
