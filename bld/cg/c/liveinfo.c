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


#define _LBit_DEFINE_BITNEXT
#include "standard.h"
#include "coderep.h"
#include "opcodes.h"
#include "conflict.h"
#include "procdef.h"

extern  conflict_node   *FindConflictNode(name*,block*,instruction*);
extern  void            SuffixIns(instruction*,instruction*);
extern  void            PrefixIns(instruction*,instruction*);
extern  instruction     *MakeNop();
extern  void            Renumber(void);
extern  int             NumOperands(instruction*);

extern  block           *HeadBlock;
extern  conflict_node   *ConfList;
extern  proc_def        *CurrProc;
extern  global_bit_set  MemoryBits;
extern  bool            HaveLiveInfo;


static  void            GlobalConflictsFirst()
/*********************************************

    Run down the list of conflicts and rip out any which are
    USE_IN_ANOTHER_BLOCK and not CONFLICT_ON_HOLD (ones with global
    bits).  Make a new list out of these global-bit conflicts and, when
    we're done, create a new conflict list with these global-bit
    conflicts at the front, so that ExtendConflicts can have an early
    out.
*/
{
    conflict_node       **conf_owner;
    conflict_node       **list_owner;
    conflict_node       *list;
    conflict_node       *conf;

    conf_owner = &ConfList;
    list_owner = &list;
    for( ;; ) {
        conf = *conf_owner;
        if( conf == NULL ) break;
        if( !( conf->state & CONFLICT_ON_HOLD ) &&
             ( conf->name->v.usage & USE_IN_ANOTHER_BLOCK ) ) {
            *conf_owner = conf->next_conflict;
            *list_owner = conf;
            list_owner = &conf->next_conflict;
        } else {
            conf_owner = &conf->next_conflict;
        }
    }
    if( list_owner != &list ) {
        *list_owner = ConfList;
        ConfList = list;
    }
}


extern  void    MakeLiveInfo() {
/******************************/

    block               *blk;
    conflict_node       *first_global;
    bool                havelive;

    GlobalConflictsFirst();
    first_global = ConfList; // assumes conflicts get added at start of list
    havelive = HaveLiveInfo;
    HaveLiveInfo = FALSE;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        if( blk->ins.hd.prev->head.opcode != OP_NOP ) {
            PrefixIns( (instruction *)&blk->ins, MakeNop() );
        }
    }
    HaveLiveInfo = havelive;
    Renumber();
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        FlowConflicts( (instruction *)&blk->ins,
                       (instruction *)&blk->ins, blk );
        ExtendConflicts( blk, first_global );
    }
}


extern  void    LiveInfoUpdate() {
/*****************************/

    block       *blk;

    blk = HeadBlock;
    for(;;) {
        if( blk->ins.hd.next != (instruction *)&blk->ins ) {
            FlowConflicts( (instruction *)&blk->ins,
                           (instruction *)&blk->ins, blk );
        }
        blk = blk->next_block;
        if( blk == NULL ) break;
    }
}


extern  void    UpdateLive( instruction *first, instruction *last ) {
/*******************************************************************/

/* update the live information from 'first'.prev to 'last'.next inclusive*/

    instruction *ins;

    last = last->head.next;
    ins = last;
    while( ins->head.opcode != OP_BLOCK ) {
        ins = ins->head.next;
    }
    if( ins->head.next == ins->head.prev ) { /* 1 or 2 instructions*/
        FlowConflicts( ins, ins, _BLOCK( ins ) );
    } else {
        if( first->head.opcode != OP_BLOCK ) first = first->head.prev;
        FlowConflicts( first, last, _BLOCK( ins ) );
    }
}


static  void    ExtendConflicts( block *blk, conflict_node *first_global ) {
/**************************************************************************/

/*
 * Make sure that first & last pointers of extended conflicts point
 * to instructions that will never be replaced. (ie: OP_NOP)
 * Also, make sure there's a NOP at the end of the block to hold
 * live information
 */


    conflict_node       *conf;
    instruction         *last_ins;
    instruction         *first_ins;
    instruction         *new_ins;
    bool                havelive;
    data_flow_def       *flow;

    flow = blk->dataflow;
    for( conf = first_global; conf != NULL; conf = conf->next_conflict ) {
        if( conf->state & CONFLICT_ON_HOLD ) break;
        if( !( conf->name->v.usage & USE_IN_ANOTHER_BLOCK ) ) break;
        if( _GBitOverlap( conf->id.out_of_block, flow->out ) ) {
            last_ins = blk->ins.hd.prev;
            if( conf->ins_range.last == NULL
             || conf->ins_range.last->id <= last_ins->id ) {
                if( last_ins->head.opcode != OP_NOP ) {
                    new_ins = MakeNop();
                    havelive = HaveLiveInfo;
                    HaveLiveInfo = FALSE;
                    SuffixIns( last_ins, new_ins );
                    new_ins->head.live.regs = blk->ins.hd.live.regs;
                    new_ins->head.live.within_block
                        = blk->ins.hd.live.within_block;
                    new_ins->head.live.out_of_block
                        = blk->ins.hd.live.out_of_block;
                    HaveLiveInfo = havelive;
                    last_ins = new_ins;
                }
                conf->ins_range.last = last_ins;
                if( conf->ins_range.first == NULL ) {
                    conf->ins_range.first = last_ins;
                    conf->start_block = blk;
                }
            }
        }
        if( _GBitOverlap( conf->id.out_of_block, flow->in ) ) {
            first_ins = blk->ins.hd.next;
            if( conf->ins_range.first == NULL
             || conf->ins_range.first->id >= first_ins->id ) {
                if( first_ins->head.opcode != OP_NOP ) {
                    new_ins = MakeNop();
                    havelive = HaveLiveInfo;
                    HaveLiveInfo = FALSE;
                    PrefixIns( first_ins, new_ins );
                    new_ins->head.live.regs = blk->ins.hd.live.regs;
                    new_ins->head.live.within_block
                        = first_ins->head.live.within_block;
                    new_ins->head.live.out_of_block
                        = first_ins->head.live.out_of_block;
                    HaveLiveInfo = havelive;
                    first_ins = new_ins;
                }
                conf->ins_range.first = first_ins;
                conf->start_block = blk;
                if( conf->ins_range.last == NULL ) {
                    conf->ins_range.last = first_ins;
                }
            }
        }
    }
}


static  void    FlowConflicts( instruction *first,
                               instruction *last, block *blk ) {
/**************************************************************/


/* Scan through instructions backwards in the block*/
/* Mark each instruction with the set of names live*/
/* from the assignment of the previous instruction to the*/
/* assignment of the current instruction*/

    instruction         *ins;
    name                *opnd;
    conflict_node       *conf;
    int                 i;
    opcode_defs         opcode;
    name_set            alive;

    alive.regs          = last->head.live.regs;
    alive.out_of_block  = last->head.live.out_of_block;
    alive.within_block  = last->head.live.within_block;
#if _TARGET & _TARG_AXP
    if( blk == HeadBlock ) {
        HW_TurnOn( alive.regs, CurrProc->state.parm.used );
    }
#endif
    ins = last;
    for(;;) {

        /*   The operands of the current instruction are live in*/
        /*   previous instructions*/

        opcode = ins->head.opcode;
        i = 0;
        switch( opcode ) {
        case OP_SUB:
        case OP_XOR:
        case OP_MOD:
        case OP_DIV:
            if( ins->operands[ 1 ] != ins->operands[ 0 ] ) break;
            if( ins->operands[ 1 ]->n.class != N_REGISTER ) break;
            i = NumOperands( ins ); /* ignore the register operands */
            break;
        }
        if( opcode != OP_BLOCK ) {
            while( i < ins->num_operands ) {
                opnd = ins->operands[ i ];
                if( opnd->n.class == N_INDEXED ) {
                    opnd = opnd->i.index;
                }
                conf = FindConflictNode( opnd, blk, ins );
                NowAlive( opnd, conf, &alive, blk );
                ++ i;
            }
        }
        if( ins->head.opcode == OP_CALL
         || ins->head.opcode == OP_CALL_INDIRECT ) {
          /* all memory names are operands of a call instruction*/
            _GBitTurnOn( alive.out_of_block, MemoryBits );
        }

        /*   Move information into current instruction*/
        /*   (This information indicates what is live between*/
        /*   previous instruction & this instruction)*/

        ins->head.live.regs = alive.regs;
        HW_TurnOn( ins->head.live.regs, CurrProc->state.unalterable );

        ins->head.live.out_of_block = alive.out_of_block;
        ins->head.live.within_block = alive.within_block;
        ins = ins->head.prev;
        if( ins == first ) break;

        /*   Since the result is redefined by the current instruction,*/
        /*   its previous value is not live in previous instructions*/
        /*   Same goes for a zapped register.*/

        HW_TurnOff( alive.regs, ins->zap->reg );
        opnd = ins->result;
        if( opnd != NULL ) {
            if( opnd->n.class == N_INDEXED ) {
                conf = FindConflictNode( opnd->i.index, blk, ins );
                NowAlive( opnd->i.index, conf, &alive, blk );
            } else {
                conf = FindConflictNode( opnd, blk, ins );
                NowDead( opnd, conf, &alive, blk );
            }
        }
    }
}


extern  void    NowAlive( name *opnd, conflict_node *conf,
                          name_set *alive, block *blk ) {

    if( opnd->n.class == N_REGISTER ) {
        HW_TurnOn( alive->regs, opnd->r.reg );
    } else if( conf != NULL ) {
        if( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) {
            _GBitTurnOn( alive->out_of_block, conf->id.out_of_block );
        } else {
            if( _LBitEmpty( conf->id.within_block ) ) {
                AssignBit( conf, blk );
            }
            _LBitTurnOn( alive->within_block, conf->id.within_block );
        }
    }
}


extern  void    NowDead( name *opnd, conflict_node *conf,
                         name_set *alive, block *blk ) {

    if( opnd->n.class == N_REGISTER ) {
        HW_TurnOff( alive->regs, opnd->r.reg );
    } else if( conf != NULL ) {
        if( opnd->v.usage & USE_IN_ANOTHER_BLOCK ) {
            if( opnd->n.class != N_TEMP || opnd->t.alias == opnd ) {
                _GBitTurnOff( alive->out_of_block, conf->id.out_of_block );
            }
        } else {
            if( _LBitEmpty( conf->id.within_block ) ) {
                AssignBit( conf, blk );
            }
            if( opnd->n.class != N_TEMP || opnd->t.alias == opnd ) {
                _LBitTurnOff( alive->within_block, conf->id.within_block );
            }
        }
    }
}


static  void    AssignBit( conflict_node *conf, block *blk ) {
/************************************************************/

    local_bit_set     bit;

    if( _LBitEmpty( blk->available_bit ) ) {
        conf->state |= CONFLICT_ON_HOLD;
    } else if( ( conf->state & CONFLICT_ON_HOLD ) == EMPTY ) {
        _LBitFirst( bit );
        for(;;) {
            if( _LBitOverlap( blk->available_bit, bit ) ) break;
            _LBitNext( &bit );
        }
        _LBitAssign( conf->id.within_block, bit );
        _LBitTurnOff( blk->available_bit, bit );
    }
}
