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
* Description:  Instruction data manipulation routines.
*
****************************************************************************/


#include "standard.h"
#include "coderep.h"
#include "conflict.h"
#include "opcodes.h"
#include "pattern.h"
#include "procdef.h"
#include "zoiks.h"
#include "makeins.h"

typedef enum {
        CB_FOR_INS1             = 0x01,
        CB_FOR_INS2             = 0x02,
        CB_NONE                 = 0x00
} conflict_bits;

typedef struct  conflict_info {
        conflict_node   *conf;
        conflict_bits   flags;
} conflict_info;

extern    block         *HeadBlock;
extern    conflict_node *ConfList;
extern    bool          HaveLiveInfo;

extern  void            UpdateLive(instruction*,instruction*);
extern  name            *DeAlias(name*);
extern  int             CountIns(block*);
extern  instruction_id  Renumber( void );

#define MAX_CONF_INFO   (2*( MAX_OPS_PER_INS+1 )+1)
static  int             CurrInfo;
static  conflict_info   ConflictInfo[ MAX_CONF_INFO ];


static void RenumFrom( instruction *ins ) {
/*****************************************/

    block               *blk;
    instruction_id      id;

    if( ins->head.opcode == OP_BLOCK ) {
        Renumber();
    } else {
        id = ins->id;
        ins = ins->head.next;
        for( ;; ) {
            while( ins->head.opcode != OP_BLOCK ) {
                ++id;
                if( ins->id > id ) return;
                ins->id = id;
                ins = ins->head.next;
            }
            blk = _BLOCK( ins )->next_block;
            if( blk == NULL ) break;
            ins = blk->ins.hd.next;
        }
    }
}


static  conflict_info   *AddConfInfo( conflict_node *conf ) {
/***********************************************************/

    conflict_info       *info;

    if( conf->state & CONF_VISITED ) {
        info = &ConflictInfo[ 0 ];
        for( info = &ConflictInfo[ 0 ];; ++info ) {
            if( info->conf == conf ) return( info );
        }
    } else {
        info = &ConflictInfo[ CurrInfo++ ];
        info->conf = conf;
        conf->state |= CONF_VISITED;
    }
    return( info );
}


static  void    FindNameConf( name *name, instruction *ins,
                              instruction *other, conflict_bits for_which ) {
/***********************************************************************/

    conflict_node       *conf;
    conflict_info       *info;

    if( name->n.class == N_INDEXED ) {
        name = name->i.index;
    }
    if( name->n.class == N_TEMP ) {
        name = DeAlias( name );
    } else if( name->n.class != N_MEMORY ) {
        return;
    }
    conf = name->v.conflict;
    info = NULL;
    while( conf != NULL && info == NULL ) {
        if( conf->ins_range.first == ins
         || conf->ins_range.first == other
         || conf->ins_range.last == ins
         || conf->ins_range.last == other ) {
            info = AddConfInfo( conf );
            info->flags |= for_which;
        }
        conf = conf->next_for_name;
    }
}


static  void    FindAllConflicts( instruction *ins,
                                 instruction *other, conflict_bits for_which ) {
/***************************************************************************/

    int         i;

    i = ins->num_operands;
    while( --i >= 0 ) {
        FindNameConf( ins->operands[ i ], ins, other, for_which );
    }
    if( ins->result != NULL ) {
        FindNameConf( ins->result, ins, other, for_which );
    }
}


static  void    MakeConflictInfo( instruction *ins1, instruction *ins2 ) {
/************************************************************************/

    int                 i;
    conflict_info       *info;

    for( i = 0; i < MAX_CONF_INFO; ++i ) {
        ConflictInfo[ i ].conf = NULL;
        ConflictInfo[ i ].flags = CB_NONE;
    }
    CurrInfo = 0;
    FindAllConflicts( ins1, ins2, CB_FOR_INS1 );
    FindAllConflicts( ins2, ins1, CB_FOR_INS2 );
    for( info = ConflictInfo; info->conf != NULL; ++info ) {
        info->conf->state &= ~CONF_VISITED;
    }
}


extern  void    PrefixInsRenum( instruction *ins, instruction *pref, bool renum ) {
/*****************************************************************/

    conflict_info       *info;
    conflict_node       *conf;
    block               *blk;
    instruction         *next;

/*   Link the new instruction into the instruction ring*/
/*   If renum = true, assign id and renumber. renum can be false only */
/*   if you're going to call Renumber() manually. */

    _INS_NOT_BLOCK( pref );
    pref->head.prev = ins->head.prev;
    pref->head.prev->head.next = pref;
    pref->head.next = ins;
    ins->head.prev = pref;
    if( ins->head.opcode != OP_BLOCK ) {
        pref->head.line_num = ins->head.line_num;
        ins->head.line_num = 0;

        pref->stk_entry = ins->stk_entry;
        pref->stk_exit = ins->stk_entry;
        // pref->s.stk_extra = ins->s.stk_extra;
        pref->sequence = ins->sequence;

        pref->id = ins->id;
    } else {
        pref->head.line_num = 0;

        if ( renum ) {
             /*
             * Oops. There is no id in OP_BLOCK and assigned id will be invalid.
             * This condition happens sometimes in loop optimizer.
             * NOTE: this case can be a bug. It means that we're trying to add
             * last instruction to the block. But other code can expect that
             * last instruction is NOP, but it'll not.
             * Currently we'll try to find next instruction with valid id.
             */
            next = ins;
            for (;;) {
                blk = _BLOCK( next )->next_block;
                if( blk == NULL ) {
                    Zoiks( ZOIKS_141 );
                    break;
                }
                next = blk->ins.hd.next;
                if ( next->head.opcode != OP_BLOCK ) break;
            }
            pref->id = next->id;
        }
    }

    if ( renum ) {
        RenumFrom( pref );
    }

    if( HaveLiveInfo ) {

/*      move the first/last pointers of any relevant conflict nodes */

        if( ins->head.opcode != OP_BLOCK ) {
            MakeConflictInfo( pref, ins );
            for( info = ConflictInfo; info->conf != NULL; ++info ) {
                if( info->flags & CB_FOR_INS1 ) {
                    conf = info->conf;
                    if( !( info->flags & CB_FOR_INS2 ) ) {
                        if( conf->ins_range.last == ins ) {
                            conf->ins_range.last = pref;
                        }
                    }
                    if( conf->ins_range.first == ins ) {
                        conf->ins_range.first = pref;
                    }
                }
            }
        }

    /*   Set up the state for both instructions*/

        ins->head.state = INS_NEEDS_WORK;
        pref->head.state = INS_NEEDS_WORK;

    /*   Add live & conflict information for the new instruction*/

        UpdateLive( pref, ins );
    }
}


extern  void    PrefixIns( instruction *ins, instruction *pref ) {
/****************************************************************/

    PrefixInsRenum( ins, pref, TRUE );
}


extern  void    SuffixIns( instruction *ins, instruction *suff ) {
/****************************************************************/

/*   Link the new instruction into the instruction ring*/

    conflict_info       *info;
    conflict_node       *conf;

    suff->head.next = ins->head.next;
    suff->head.next->head.prev = suff;
    suff->head.prev = ins;
    ins->head.next = suff;
    suff->head.line_num = 0;
    _INS_NOT_BLOCK( suff );
    /*
     * A little dirty, but check of OP_BLOCK can be skipped - in this case
     * we'll just Renumber() everything from scratch.
     */
    /* _INS_NOT_BLOCK( ins ); */
    suff->id = ins->id;
    if( ins->head.opcode == OP_NOP ) {
        /* get rid of the little bugger so it doesn't mess up our optimizing */
        if( ins->flags.nop_flags & NOP_SOURCE_QUEUE ) {
            /* transfer the line_num info to the next instruction */
            suff->head.line_num = ins->head.line_num;
            ins = ins->head.prev;
            FreeIns( ins->head.next );
        }
    }
    if( ins->head.opcode != OP_BLOCK ) {
        suff->stk_entry = ins->stk_exit;
        suff->stk_exit = ins->stk_exit;
        // suff->s.stk_extra = ins->s.stk_extra;
        suff->sequence = ins->sequence;
    }
    RenumFrom( ins );
    if( HaveLiveInfo ) {

/*      move the first/last pointers of any relevant conflict nodes */

        if( ins->head.opcode != OP_BLOCK ) {
            MakeConflictInfo( ins, suff );
            for( info = ConflictInfo; info->conf != NULL; ++info ) {
                if( info->flags & CB_FOR_INS2 ) {
                    conf = info->conf;
                    if( !( info->flags & CB_FOR_INS1 ) ) {
                        if( conf->ins_range.first == ins ) {
                            conf->ins_range.first = suff;
                        }
                    }
                    if( conf->ins_range.last == ins ) {
                        conf->ins_range.last = suff;
                    }
                }
            }
        }

    /*   Set up the state for both instructions*/

        ins->head.state = INS_NEEDS_WORK;
        suff->head.state = INS_NEEDS_WORK;
        UpdateLive( ins, suff );
    }
}


static  block   *GetBlockPointer( block *blk, instruction *ins ) {
/****************************************************************/

    if( blk != NULL ) return( blk );
    while( ins->head.opcode != OP_BLOCK ) {
        ins = ins->head.next;
    }
    return( _BLOCK( ins ) );
}


extern  void    ReplIns( instruction *ins, instruction *new ) {
/*************************************************************/

    block               *blk;
    conflict_info       *info;
    conflict_node       *conf;


/*   Move conflict information to the new instruction*/

/*   Link the new instruction into the instruction ring*/
/*   (It will be removed by FreeIns() )*/

    new->head.next = ins->head.next;
    new->head.next->head.prev = new;
    new->head.prev = ins;
    ins->head.next = new;
    new->stk_entry = ins->stk_entry;
    new->stk_exit = ins->stk_exit;
    // new->s.stk_extra = ins->s.stk_extra;
    new->sequence = ins->sequence;

    _INS_NOT_BLOCK( new );
    _INS_NOT_BLOCK( ins );
    new->head.line_num = ins->head.line_num;
    new->id = ins->id;
    new->head.state = INS_NEEDS_WORK;
    blk = NULL;
    if( HaveLiveInfo ) {

/*  Copy the live analysis information from ins to new*/

        new->head.live.regs         = ins->head.live.regs;
        new->head.live.within_block = ins->head.live.within_block;
        new->head.live.out_of_block = ins->head.live.out_of_block;

/*      move the first/last pointers of any relevant conflict nodes */

/*
 * !!! BUG BUG !!!
 * Moving/changing egde of conflict with CONFLICT_ON_HOLD flag set
 * to non-conflicting instruction can cause famous bug 352. If new instruction
 * should be also replaced, codegen will not notice that it's a part of
 * any conflict and will not update egde. Event worse, if replaced instruction
 * is replaced again, this case will be completely missed by conflict manager.
 * At least, compiler can Zoiks now.
 *
 * This problem may also exist in other instruction modification functions.
 * It must be fixed in more correct way if somebody ever understand
 * how all this stuff works.
 */
        MakeConflictInfo( ins, new );
        for( info = ConflictInfo; info->conf != NULL; ++info ) {
            if( info->flags & CB_FOR_INS1 ) {
                conf = info->conf;
                if( conf->ins_range.first == ins ) {
                    conf->ins_range.first = new;
                }
                if( conf->ins_range.last == ins ) {
                    conf->ins_range.last = new;
                }
                if( info->flags & CB_FOR_INS2 ) continue;
                if( conf->ins_range.first == new ) {
                    blk = GetBlockPointer( blk, ins );
                    conf->ins_range.first = blk->ins.hd.prev;
                }
                if( conf->ins_range.last == new ) {
                    blk = GetBlockPointer( blk, ins );
                    conf->ins_range.last = blk->ins.hd.next; /* 89-08-16 */
                    if( blk->ins.hd.next == ins ) {
                        // oops - grasping for a straw and caught hold of
                        // ins (which is about to be deleted)
                        conf->ins_range.last = blk->ins.hd.next->head.next;
                    }
                }
            }
        }
    }

/*  Free up the old instruction */

    FreeIns( ins );

/*  if we wrecked a conflicts first or last pointer */

    if( blk != NULL ) { /* moved here 89-08-16 */
        UpdateLive( blk->ins.hd.next, blk->ins.hd.prev );
    }
}


extern  instruction_id  Renumber( void )
/**************************************/
{
    block               *blk;
    instruction         *ins;
    instruction_id      id;
    instruction_id      increment;
    unsigned_32         number_instrs;

    number_instrs = 0;
    blk = HeadBlock;
    while( blk != NULL ) {
        number_instrs += CountIns( blk );
        blk = blk->next_block;
    }
    if( number_instrs == 0 ) {
        increment = 1;
    } else if( number_instrs > MAX_INS_ID ) {
        _Zoiks( ZOIKS_069 );
        increment = 1;
    } else {
        increment = 10; // BBB - tired of stupid looking ins id's
    }
    id = 0;
    blk = HeadBlock;
    while( blk != NULL ) {
        ins = blk->ins.hd.next;
        while( ins->head.opcode != OP_BLOCK ) {
            ins->id = id;
            id += increment;
            ins = ins->head.next;
        }
        blk = blk->next_block;
    }
    return( id );
}


extern  void            ClearInsBits( instruction_flags mask ) {
/**************************************************************/

    block               *blk;
    instruction         *ins;

    mask = ~mask;
    for( blk = HeadBlock; blk != NULL; blk = blk->next_block ) {
        for( ins = blk->ins.hd.next; ins->head.opcode != OP_BLOCK; ins = ins->head.next ) {
            ins->ins_flags &= mask;
        }
    }
}
